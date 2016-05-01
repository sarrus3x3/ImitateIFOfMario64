#include <cassert>

#include "SolidObjects.h"

// ###############################################
// ########## class Column
// ###############################################

Column::Column( 
		Vector3D  CenterPos,       // 底面の円形の中心位置
		double    Radius,          // 半径
		double    Hight,           // 円柱の高さ
		int       DivNum,          // 分割数（＝底面の円形の分割数）
		COLOR_U8  DifColor, // 頂点ディフューズカラー
		COLOR_U8  SpcColor  // 球の頂点スペキュラカラー
		)
{
	// Vertex を計算する

	m_PolygonNum = DivNum*4;

	// #### 先ずは、単位円柱をのVertexを生成し、それをシフト、スケーリングする

	// Vectexのメモリを確保 Vertex数：DivNum * 4(上面・底辺・側面（２倍）) * 3（１ポリゴンの頂点数）
	int VectexNum = DivNum*4*3;
	m_pVertex = new VERTEX3D[VectexNum];

	// 分割数から正多角形の中心角を計算
	double CenterAng = 2*DX_PI_F/((double)DivNum);
	
	Vector3D V3D0B( 0,  0,0 );
	Vector3D V3D0T( 0,1.0,0 );

	for( int i=0; i<DivNum; i++ )
	{
		int j=(i+1)%DivNum; // 次の添字（円順対応）
		Vector2D V2D1( cos(CenterAng*i), sin(CenterAng*i) );
		Vector2D V2D2( cos(CenterAng*j), sin(CenterAng*j) );
		Vector2D V2Dn( cos(CenterAng*((double)i+0.5)), sin(CenterAng*((double)i+0.5)) ); // 法線
		
		Vector3D V3D1B = V2D1.toVector3D();
		Vector3D V3D1T = V2D1.toVector3D(1.0);
		Vector3D V3D2B = V2D2.toVector3D();
		Vector3D V3D2T = V2D2.toVector3D(1.0);
		Vector3D V3Dn  = V2Dn.toVector3D();

		// 底面
		m_pVertex[ 12*i+3*0+0 ].pos  = V3D0B.toVECTOR();
		m_pVertex[ 12*i+3*0+1 ].pos  = V3D1B.toVECTOR();
		m_pVertex[ 12*i+3*0+2 ].pos  = V3D2B.toVECTOR();
		for( int k=0; k<3; k++) m_pVertex[ 12*i+3*0+k ].norm = Vector3D( 0, -1.0, 0).toVECTOR();

		// 側面１
		m_pVertex[ 12*i+3*1+0 ].pos  = V3D1T.toVECTOR();
		m_pVertex[ 12*i+3*1+1 ].pos  = V3D1B.toVECTOR();
		m_pVertex[ 12*i+3*1+2 ].pos  = V3D2B.toVECTOR();
		for( int k=0; k<3; k++) m_pVertex[ 12*i+3*1+k ].norm = V3Dn.toVECTOR();

		// 側面２
		m_pVertex[ 12*i+3*2+0 ].pos  = V3D2B.toVECTOR();
		m_pVertex[ 12*i+3*2+1 ].pos  = V3D1T.toVECTOR();
		m_pVertex[ 12*i+3*2+2 ].pos  = V3D2T.toVECTOR();
		for( int k=0; k<3; k++) m_pVertex[ 12*i+3*2+k ].norm = V3Dn.toVECTOR();

		// 上面
		m_pVertex[ 12*i+3*3+0 ].pos  = V3D0T.toVECTOR();
		m_pVertex[ 12*i+3*3+1 ].pos  = V3D1T.toVECTOR();
		m_pVertex[ 12*i+3*3+2 ].pos  = V3D2T.toVECTOR();
		for( int k=0; k<3; k++) m_pVertex[ 12*i+3*3+k ].norm = Vector3D( 0, 1.0, 0).toVECTOR();

	}

	// color と使わない要素を代入する
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].dif = DifColor;
		m_pVertex[i].spc = SpcColor;
		m_pVertex[i].u   = 0.0f;
		m_pVertex[i].v   = 0.0f;
		m_pVertex[i].su  = 0.0f;
		m_pVertex[i].sv  = 0.0f;
	}

	// シフト、スケーリングする
	for( int i=0; i<VectexNum; i++ )
	{
		Vector3D tmp = m_pVertex[i].pos;
		tmp.x *= Radius;
		tmp.z *= Radius;
		tmp.y *= Hight;
		tmp += CenterPos;
		m_pVertex[i].pos = tmp.toVECTOR();
	}

};

void Column::Render()
{
	DrawPolygon3D( m_pVertex, m_PolygonNum, DX_NONE_GRAPH, FALSE ) ;
};

// ###############################################
// ########## class FineMeshedFlatPlate
// ###############################################

FineMeshedFlatPlate::FineMeshedFlatPlate( 
		Vector2D  BLCorner,        // 平板の（世界平面上の）左下頂点座標
		Vector2D  TRCorner,        // 平板の（世界平面上の）右上頂点座標
		int       DivNum,          // 分割数（縦・横この数で分割されるため、ポリゴン三角形数は DivNum * DivNum * 2 となる）
		COLOR_U8  DifColor,        // 頂点ディフューズカラー
		COLOR_U8  SpcColor         // 球の頂点スペキュラカラー
		)
{
	// Vertex を計算する

	// ポリゴン三角形数だっけ？
	m_PolygonNum = DivNum*DivNum*2;

	// Vectexのメモリを確保 Vertex数：
	// DivNum*DivNum（縦横分割数）* 2（１四角を２三角形） * 3（１ポリゴンの頂点数）
	int VectexNum = DivNum*DivNum*2*3;
	m_pVertex = new VERTEX3D[VectexNum];

	double VertiDivWidth = (TRCorner.y - BLCorner.y)/(double)DivNum; // 横方向分割幅
	double HorizDivWidth = (TRCorner.x - BLCorner.x)/(double)DivNum; // 縦方向分割幅

	// 分初幅が負値でないことを確認
	assert ( VertiDivWidth>0 && "VertiDivWidth is negative.");
	assert ( HorizDivWidth>0 && "HorizDivWidth is negative.");

	// 縦横全てのメッシュに対し
	for( int i=0; i<DivNum; i++ ){
		for( int j=0; j<DivNum; j++ ){
			double x = (double)i;
			double y = (double)j;
			Vector2D PolyBL(BLCorner.x + HorizDivWidth*   x ,BLCorner.y + VertiDivWidth*   y );
			Vector2D PolyBR(BLCorner.x + HorizDivWidth*(x+1),BLCorner.y + VertiDivWidth*   y );
			Vector2D PolyTL(BLCorner.x + HorizDivWidth*   x ,BLCorner.y + VertiDivWidth*(y+1));
			Vector2D PolyTR(BLCorner.x + HorizDivWidth*(x+1),BLCorner.y + VertiDivWidth*(y+1));

			int suffix = DivNum*i + j; // 縦横通番

			// 三角形１
			m_pVertex[ 6*suffix+0 ].pos = PolyBL.toVector3D().toVECTOR();
			m_pVertex[ 6*suffix+1 ].pos = PolyBR.toVector3D().toVECTOR();
			m_pVertex[ 6*suffix+2 ].pos = PolyTL.toVector3D().toVECTOR();

			// 三角形２
			m_pVertex[ 6*suffix+3 ].pos = PolyBR.toVector3D().toVECTOR();
			m_pVertex[ 6*suffix+4 ].pos = PolyTL.toVector3D().toVECTOR();
			m_pVertex[ 6*suffix+5 ].pos = PolyTR.toVector3D().toVECTOR();

		}
	}

	// 法線ベクトルと、color と使わない要素を代入する
	Vector3D vNorm( 0, 1.0, 0 ); // 法線ベクトル、y軸上方向。
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].norm = vNorm.toVECTOR();
		m_pVertex[i].dif = DifColor;
		m_pVertex[i].spc = SpcColor;
		m_pVertex[i].u   = 0.0f;
		m_pVertex[i].v   = 0.0f;
		m_pVertex[i].su  = 0.0f;
		m_pVertex[i].sv  = 0.0f;
	}

};

void FineMeshedFlatPlate::Render()
{
	DrawPolygon3D( m_pVertex, m_PolygonNum, DX_NONE_GRAPH, FALSE ) ;
};

// ###############################################
// ########## class ParallelBox3D
// ###############################################

ParallelBox3D::ParallelBox3D( 
		Vector3D  SmallVertex,      // 立方体の頂点で、座標的に一番小さい
		Vector3D  LargeVertex,      // 立方体の頂点で、座標的に一番大きい
		int       TexturesHandle,   // テクスチャ画像のハンドル
		COLOR_U8  DifColor, // 頂点ディフューズカラー
		COLOR_U8  SpcColor  // 球の頂点スペキュラカラー
		)
{
	// テクスチャ用の画像の読み込み
    m_iTexturesHandle = TexturesHandle ;

	// Vertex を計算する
	m_PolygonNum = 6*2;

	// Vectexのメモリを確保 Vertex数：DivNum * 4(上面・底辺・側面（２倍）) * 3（１ポリゴンの頂点数）
	int VectexNum = m_PolygonNum*3;
	m_pVertex = new VERTEX3D[VectexNum];

	// #### 先ずは、立方体をのVertexを生成し、それをシフト、スケーリングする

	// color と使わない要素を代入する
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].dif = DifColor;
		m_pVertex[i].spc = SpcColor;
		m_pVertex[i].u   = 0.0f;
		m_pVertex[i].v   = 0.0f;
		m_pVertex[i].su  = 0.0f;
		m_pVertex[i].sv  = 0.0f;
	}

	// 各頂点を定義
	Vector3D V000( 0, 0, 0 );
	Vector3D V001( 0, 0, 1 );
	Vector3D V010( 0, 1, 0 );
	Vector3D V100( 1, 0, 0 );
	Vector3D V011( 0, 1, 1 );
	Vector3D V101( 1, 0, 1 );
	Vector3D V110( 1, 1, 0 );
	Vector3D V111( 1, 1, 1 );

	// 6面分定義
	int n=0;
	int save=0;

	save = n;
	m_pVertex[ n++ ].pos = V000.toVECTOR();
	m_pVertex[ n++ ].pos = V001.toVECTOR();
	m_pVertex[ n++ ].pos = V010.toVECTOR();
	m_pVertex[ n++ ].pos = V001.toVECTOR();
	m_pVertex[ n++ ].pos = V010.toVECTOR();
	m_pVertex[ n++ ].pos = V011.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D(-1,0,0).toVECTOR();

	save = n;
	m_pVertex[ n++ ].pos = V100.toVECTOR();
	m_pVertex[ n++ ].pos = V101.toVECTOR();
	m_pVertex[ n++ ].pos = V110.toVECTOR();
	m_pVertex[ n++ ].pos = V101.toVECTOR();
	m_pVertex[ n++ ].pos = V110.toVECTOR();
	m_pVertex[ n++ ].pos = V111.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D( 1,0,0).toVECTOR();

	save = n;
	m_pVertex[ n++ ].pos = V000.toVECTOR();
	m_pVertex[ n++ ].pos = V001.toVECTOR();
	m_pVertex[ n++ ].pos = V100.toVECTOR();
	m_pVertex[ n++ ].pos = V001.toVECTOR();
	m_pVertex[ n++ ].pos = V100.toVECTOR();
	m_pVertex[ n++ ].pos = V101.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D(0,-1,0).toVECTOR();
	
	save = n;
	m_pVertex[ n++ ].pos = V010.toVECTOR();
	m_pVertex[ n++ ].pos = V011.toVECTOR();
	m_pVertex[ n++ ].pos = V110.toVECTOR();
	m_pVertex[ n++ ].pos = V011.toVECTOR();
	m_pVertex[ n++ ].pos = V110.toVECTOR();
	m_pVertex[ n++ ].pos = V111.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D(0, 1,0).toVECTOR();

	save = n;
	m_pVertex[ n++ ].pos = V000.toVECTOR();
	m_pVertex[ n++ ].pos = V010.toVECTOR();
	m_pVertex[ n++ ].pos = V100.toVECTOR();
	m_pVertex[ n++ ].pos = V010.toVECTOR();
	m_pVertex[ n++ ].pos = V100.toVECTOR();
	m_pVertex[ n++ ].pos = V110.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D(0,0,-1).toVECTOR();
	
	save = n;
	m_pVertex[ n++ ].pos = V001.toVECTOR();
	m_pVertex[ n++ ].pos = V011.toVECTOR();
	m_pVertex[ n++ ].pos = V101.toVECTOR();
	m_pVertex[ n++ ].pos = V011.toVECTOR();
	m_pVertex[ n++ ].pos = V101.toVECTOR();
	m_pVertex[ n++ ].pos = V111.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D(0,0, 1).toVECTOR();

	// 画像貼り付け座標を設定
	for( int i=0; i<6; i++ )
	{
		m_pVertex[ 6*i+0 ].u = 0.0f;
		m_pVertex[ 6*i+0 ].v = 0.0f;
		m_pVertex[ 6*i+1 ].u = 0.0f;
		m_pVertex[ 6*i+1 ].v = 1.0f;
		m_pVertex[ 6*i+2 ].u = 1.0f;
		m_pVertex[ 6*i+2 ].v = 0.0f;
		m_pVertex[ 6*i+3 ].u = 0.0f;
		m_pVertex[ 6*i+3 ].v = 1.0f;
		m_pVertex[ 6*i+4 ].u = 1.0f;
		m_pVertex[ 6*i+4 ].v = 0.0f;
		m_pVertex[ 6*i+5 ].u = 1.0f;
		m_pVertex[ 6*i+5 ].v = 1.0f;
	}

	// シフト、スケーリングする
	Vector3D ScaleVec = LargeVertex - SmallVertex;
	for( int i=0; i<VectexNum; i++ )
	{
		Vector3D tmp = m_pVertex[i].pos;
		tmp.x *= ScaleVec.x;
		tmp.y *= ScaleVec.y;
		tmp.z *= ScaleVec.z; // 内積とも違うなにか
		tmp += SmallVertex;
		m_pVertex[i].pos = tmp.toVECTOR();
	}

};

void ParallelBox3D::Render()
{
	DrawPolygon3D( m_pVertex, m_PolygonNum, m_iTexturesHandle, FALSE ) ;
};

// ###############################################
// ########## class TextureSphere
// ###############################################

TextureSphere3D::TextureSphere3D( 
		Vector3D  CntPos,           // 球中心の位置
		double    Radius,           // 球の半径 
		bool      Outward,          // true:外向き、false:内向き
		int       DivNumLongi,      // 経度方向の分割数
		int       DivNumLati,       // 緯度方向の分割数
		int       TexturesHandle,   // テクスチャ画像のハンドル
		COLOR_U8  DifColor, // 頂点ディフューズカラー
		COLOR_U8  SpcColor  // 球の頂点スペキュラカラー
		)
{
	// テクスチャ用の画像の読み込み
    m_iTexturesHandle = TexturesHandle ;

	// 中心位置保存
	m_vCntPos = CntPos;

	// ポリゴン三角形数だっけ？
	m_iPolygonNum = DivNumLongi*DivNumLati*2;

	// Vectexのメモリを確保 Vertex数：
	int VectexNum = m_iPolygonNum * 3;
	m_pVertex       = new VERTEX3D[VectexNum];
	m_pRawVertexPos = new Vector3D[VectexNum];

	// 頂点位置と、トポロジ構造、法線ベクトル、テキスチャマッピングを計算する
	double LongiSlot = DX_TWO_PI / (double)DivNumLongi; // 緯度の分割幅
	double LatiSlot  = DX_PI / (double)DivNumLati;      // 緯度の分割幅
	double XTxSlot   = 1.0 / (double)DivNumLongi;       // テクスチャX軸方向の分割幅
	double YTxSlot   = 1.0 / (double)DivNumLati;        // テクスチャY軸方向の分割幅

	// 縦横全てのメッシュに対し
	for( int i=0; i<DivNumLongi; i++ ){
		for( int j=0; j<DivNumLati; j++ ){

			// 頂点位置の計算
			double LongiL = LongiSlot *  i;                 // 緯度L
			double LongiR = LongiSlot * (i+1);              // 緯度R
			double LatiB  = LatiSlot *  j    - (0.5*DX_PI); // 緯度B
			double LatiT  = LatiSlot * (j+1) - (0.5*DX_PI); // 緯度T
			Vector3D PolyBL( cos(LatiB)*cos(LongiL), sin(LatiB), cos(LatiB)*sin(LongiL) );
			Vector3D PolyBR( cos(LatiB)*cos(LongiR), sin(LatiB), cos(LatiB)*sin(LongiR) );
			Vector3D PolyTL( cos(LatiT)*cos(LongiL), sin(LatiT), cos(LatiT)*sin(LongiL) );
			Vector3D PolyTR( cos(LatiT)*cos(LongiR), sin(LatiT), cos(LatiT)*sin(LongiR) );
			PolyBL *= -Radius; // 半径を反映 + 向きを反転されるため-1をかける
			PolyBR *= -Radius;
			PolyTL *= -Radius;
			PolyTR *= -Radius;

			// テキスチャマッピング位置の計算
			double TxL = XTxSlot *  i;         
			double TxR = XTxSlot * (i+1);      
			double TxB = YTxSlot *  j;
			double TxT = YTxSlot * (j+1);
			Vector2D TxBL( TxL, TxB );
			Vector2D TxBR( TxR, TxB );
			Vector2D TxTL( TxL, TxT );
			Vector2D TxTR( TxR, TxT );

			// 法線ベクトルの計算
			double dist;
			if( Outward ) dist =  1.0;
			else          dist = -1.0;
			Vector3D NorBL = (dist * PolyBL).normalize();
			Vector3D NorBR = (dist * PolyBR).normalize();
			Vector3D NorTL = (dist * PolyTL).normalize();
			Vector3D NorTR = (dist * PolyTR).normalize();

			int suffix = DivNumLati*i + j; // 縦横通番

			// ##### m_pVertex
			// 三角形１
			//m_pVertex[ 6*suffix+0 ].pos = PolyBL.toVECTOR();
			m_pVertex[ 6*suffix+0 ].u   = (float)TxBL.x;
			m_pVertex[ 6*suffix+0 ].v   = (float)TxBL.y;
			m_pVertex[ 6*suffix+0 ].norm= NorBL.toVECTOR();
			//m_pVertex[ 6*suffix+1 ].pos = PolyBR.toVECTOR();
			m_pVertex[ 6*suffix+1 ].u   = (float)TxBR.x;
			m_pVertex[ 6*suffix+1 ].v   = (float)TxBR.y;
			m_pVertex[ 6*suffix+1 ].norm= NorBR.toVECTOR();
			//m_pVertex[ 6*suffix+2 ].pos = PolyTL.toVECTOR();
			m_pVertex[ 6*suffix+2 ].u   = (float)TxTL.x;
			m_pVertex[ 6*suffix+2 ].v   = (float)TxTL.y;
			m_pVertex[ 6*suffix+2 ].norm= NorTL.toVECTOR();

			// 三角形２
			//m_pVertex[ 6*suffix+3 ].pos = PolyBR.toVECTOR();
			m_pVertex[ 6*suffix+3 ].u   = (float)TxBR.x;
			m_pVertex[ 6*suffix+3 ].v   = (float)TxBR.y;
			m_pVertex[ 6*suffix+3 ].norm= NorBR.toVECTOR();
			//m_pVertex[ 6*suffix+4 ].pos = PolyTL.toVECTOR();
			m_pVertex[ 6*suffix+4 ].u   = (float)TxTL.x;
			m_pVertex[ 6*suffix+4 ].v   = (float)TxTL.y;
			m_pVertex[ 6*suffix+4 ].norm= NorTL.toVECTOR();
			//m_pVertex[ 6*suffix+5 ].pos = PolyTR.toVECTOR();
			m_pVertex[ 6*suffix+5 ].u   = (float)TxTR.x;
			m_pVertex[ 6*suffix+5 ].v   = (float)TxTR.y;
			m_pVertex[ 6*suffix+5 ].norm= NorTR.toVECTOR();

			// ##### m_pRawVertexPos
			// 三角形１
			m_pRawVertexPos[ 6*suffix+0 ] = PolyBL;
			m_pRawVertexPos[ 6*suffix+1 ] = PolyBR;
			m_pRawVertexPos[ 6*suffix+2 ] = PolyTL;

			// 三角形２
			m_pRawVertexPos[ 6*suffix+3 ] = PolyBR;
			m_pRawVertexPos[ 6*suffix+4 ] = PolyTL;
			m_pRawVertexPos[ 6*suffix+5 ] = PolyTR;

		}
	}

	// color と使わない要素を代入する
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].dif = DifColor;
		m_pVertex[i].spc = SpcColor;
		m_pVertex[i].su  = 0.0f;
		m_pVertex[i].sv  = 0.0f;
	}

	// 中心位置をシフト
	setCenterPos( m_vCntPos );

	// マテリアルパラメータの初期化
	m_Material.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Emissive = GetColorF( 1.0f, 1.0f, 1.0f, 1.0f ) ;
	m_Material.Power    = 0.0f ;

	m_MaterialDefault.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Emissive = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Power    = 20.0f ;
};

void TextureSphere3D::Render()
{
	// Zバッファ OFF
	SetUseZBuffer3D( FALSE );
	SetWriteZBuffer3D( FALSE );

	// ライトの影響を受けないように設定
	SetMaterialUseVertDifColor( FALSE ) ; // 頂点データのディフューズカラーを使用しないようにする
	SetMaterialUseVertSpcColor( FALSE ) ; // 頂点データのスペキュラカラーを使用しないようにする
	SetMaterialParam( m_Material ) ; // マテリアルの設定を有効

	DrawPolygon3D( m_pVertex, m_iPolygonNum, m_iTexturesHandle, FALSE ) ;
	//DrawPolygon3D( m_pVertex, m_iPolygonNum, DX_NONE_GRAPH , FALSE ) ;

	// マテリアル設定を元に戻す
	SetMaterialUseVertDifColor( TRUE ) ;
	SetMaterialUseVertSpcColor( TRUE ) ;
	SetMaterialParam( m_MaterialDefault ) ;

	// Zバッファ有効化
	SetUseZBuffer3D( TRUE );
	SetWriteZBuffer3D( TRUE );

	/*
	// デバック
	int VectexNum = m_iPolygonNum * 3;
	for( int i=0; i<VectexNum; i++ )
	{
		DrawPixel3D( m_pVertex[i].pos, GetColor(255, 0, 0 ) );
	};
	*/

	// vertex自体とpolygonはちゃんと生成されている。問題はテクスチャの設定か？
	// →添字間違い
	// まだ出ないか。
	// m_iTexturesHandle を初期化するのをわすれていた。これはひどい。
	// よし、成功


};

void TextureSphere3D::setCenterPos( Vector3D CntPos )
{
	int VectexNum = m_iPolygonNum * 3;

	// 中心位置をシフト
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos = (m_pRawVertexPos[i]+CntPos).toVECTOR();
	}
};