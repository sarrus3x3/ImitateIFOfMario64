#include <cassert>
#include <vector>

#include "Vector3D.h"
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
	if( m_iTexturesHandle<0 )
	{
		DrawPolygon3D( m_pVertex, m_PolygonNum, DX_NONE_GRAPH , FALSE ) ;
	}
	else
	{
		DrawPolygon3D( m_pVertex, m_PolygonNum, m_iTexturesHandle, FALSE ) ;
	}
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
		ObjectTypeID ObjectType     // オブジェクトタイプ
		) :
	m_bOutward( Outward ),
	m_eObjectType( ObjectType )
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
	m_pRawVertexNrm = new Vector3D[VectexNum];

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

			// ##### m_pRawVertexNrm
			// 三角形１
			m_pRawVertexNrm[ 6*suffix+0 ] = NorBL;
			m_pRawVertexNrm[ 6*suffix+1 ] = NorBR;
			m_pRawVertexNrm[ 6*suffix+2 ] = NorTL;
										  
			// 三角形２					  
			m_pRawVertexNrm[ 6*suffix+3 ] = NorBR;
			m_pRawVertexNrm[ 6*suffix+4 ] = NorTL;
			m_pRawVertexNrm[ 6*suffix+5 ] = NorTR;

		}
	}

	// color と使わない要素を代入する
	COLOR_U8 DifColor = GetColorU8( 255, 255, 255, 0 );
	COLOR_U8 SpcColor = GetColorU8( 255, 255, 255, 0 );
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
	// 表面が外向きの場合は、zbufを無効にしない
	if( !m_bOutward )
	{
		// Zバッファ OFF
		SetUseZBuffer3D( FALSE );
		SetWriteZBuffer3D( FALSE );
	}

	// オブジェクトタイプで描画方法を使い分け
	switch( m_eObjectType )
	{
	case OBJECT_SKYDOME: // スカイドーム（パノラマ球）: エミッシブ光のみ
		// ライトの影響を受けないように設定
		SetMaterialUseVertDifColor( FALSE ) ; // 頂点データのディフューズカラーを使用しないようにする
		SetMaterialUseVertSpcColor( FALSE ) ; // 頂点データのスペキュラカラーを使用しないようにする
		SetMaterialParam( m_Material ) ; // マテリアルの設定を有効
		break;
	case OBJECT_NOSPECULAR: //  光沢なし : スペキュラOFF
		SetMaterialUseVertSpcColor( FALSE ) ; // 頂点データのスペキュラカラーを使用しないようにする
		break;
	}

	// 描画実施
	/*
	if(m_bOutward)
	{
		DrawPolygon3D( m_pVertex, m_iPolygonNum, DX_NONE_GRAPH , FALSE ) ;
	}
	else
	{
		DrawPolygon3D( m_pVertex, m_iPolygonNum, m_iTexturesHandle, FALSE ) ;
	}
	*/
	DrawPolygon3D( m_pVertex, m_iPolygonNum, m_iTexturesHandle, FALSE ) ;


	// 変更した描画方法を元に戻す
	switch( m_eObjectType )
	{
	case OBJECT_SKYDOME: // スカイドーム（パノラマ球）: エミッシブ光のみ
		// マテリアル設定を元に戻す
		SetMaterialUseVertDifColor( TRUE ) ;
		SetMaterialUseVertSpcColor( TRUE ) ;
		SetMaterialParam( m_MaterialDefault ) ;
		break;
	case OBJECT_NOSPECULAR: //  光沢なし : スペキュラOFF
		SetMaterialUseVertSpcColor( TRUE ) ;
		break;
	}

	if( !m_bOutward )
	{
		// Zバッファ有効化
		SetUseZBuffer3D( TRUE );
		SetWriteZBuffer3D( TRUE );
	}

	/*
	// デバック
	int VectexNum = m_iPolygonNum * 3;
	for( int i=0; i<VectexNum; i++ )
	{
		DrawPixel3D( m_pVertex[i].pos, GetColor(255, 0, 0 ) );
	};
	*/

};

void TextureSphere3D::resetVertex()
{
	int VectexNum = m_iPolygonNum * 3;

	// m_pVertex.pos を m_pRawVertexPos で上書き
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos  = m_pRawVertexPos[i].toVECTOR();
		m_pVertex[i].norm = m_pRawVertexNrm[i].toVECTOR();
	}
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

void TextureSphere3D::rotEuler( double angX, double angZ, double angY )
{
	// 2018/01/09 コンパイルエラー（未初期化変数の使用）になるため、ロジック修正。
	// オイラー角で回転行列を計算
	bool inited = false;
	MATRIX RotMut = MGetIdent();
	if( angY!=0 ) RotMut = MMult( MGetRotY( (float)angY ), RotMut );
	if( angZ!=0 ) RotMut = MMult( MGetRotZ( (float)angZ ), RotMut );
	if( angX!=0 ) RotMut = MMult( MGetRotX( (float)angX ), RotMut );

	// 回転行列を作用
	int VectexNum = m_iPolygonNum * 3;
	for( int i=0; i<VectexNum; i++ )
	{
		m_pRawVertexPos[i] = VTransformSR( m_pRawVertexPos[i].toVECTOR(), RotMut );
		m_pRawVertexNrm[i] = VTransformSR( m_pRawVertexNrm[i].toVECTOR(), RotMut ); // 法線ベクトルも変換が必要になる
	}

};

void TextureSphere3D::MatTransVertex( const MATRIX &Mat )
{
	int VectexNum = m_iPolygonNum * 3;
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos  = VTransform( m_pVertex[i].pos, Mat );
		m_pVertex[i].norm = VTransformSR( m_pVertex[i].norm, Mat ); // 法線ベクトルも変換が必要になる
	}
};

// ###############################################
// ########## class GroundGrid
// ###############################################

GroundGrid::GroundGrid( 
		double GridRange,	// グリッド範囲
		int    GridNum,		// グリッド数
		int    Color		// グリッドの色
		) : m_iColor( Color )
{
	m_dGridRange = GridRange;
	m_iGridNum = GridNum;

	m_iGridLinesNum = GridNum+1;
	m_pVertiGrid = new LINE[m_iGridLinesNum];
	m_pHorizGrid = new LINE[m_iGridLinesNum];

	double GridSize = (2*GridRange) / (double)GridNum;

	// m_pVertiGrid と m_pHorizGrid を生成
	for( int i=0; i<=m_iGridNum; i++ )
	{
		m_pHorizGrid[i].from = Vector2D( -m_dGridRange, GridSize*i-m_dGridRange ).toVector3D();
		m_pHorizGrid[i].to   = Vector2D(  m_dGridRange, GridSize*i-m_dGridRange ).toVector3D();
		m_pVertiGrid[i].from = Vector2D( GridSize*i-m_dGridRange, -m_dGridRange ).toVector3D();
		m_pVertiGrid[i].to   = Vector2D( GridSize*i-m_dGridRange,  m_dGridRange ).toVector3D();	
	}

	m_iMaxVectorNum = 2 * 2 * m_iGridLinesNum;

};

void GroundGrid::Render()
{
	for( int i=0; i<=m_iGridNum; i++ )
	{
		DrawLine3D( m_pHorizGrid[i].from.toVECTOR(), m_pHorizGrid[i].to.toVECTOR(), m_iColor );
		DrawLine3D( m_pVertiGrid[i].from.toVECTOR(), m_pVertiGrid[i].to.toVECTOR(), m_iColor );
	}
};

// 頂点を直接編集
Vector3D& GroundGrid::editVertexes( int VectorIndex )
{
	assert( VectorIndex < m_iMaxVectorNum );

	int DirFlg = VectorIndex / (2*m_iGridLinesNum);
	int Rest = VectorIndex % (2*m_iGridLinesNum);
	int LineIndex = Rest / 2;
	int Edge = Rest % 2;

	LINE* pLineArray;
	if( DirFlg==0 )
	{
		pLineArray = m_pVertiGrid;
	}
	else
	{
		pLineArray = m_pHorizGrid;
	}

	if( Edge==0 )
	{
		return (pLineArray[LineIndex].to);
	}
	else
	{
		return (pLineArray[LineIndex].from);
	}

}

// ###############################################
// ########## class BasePlaneFigure
// ###############################################

// コンストラクタ
BasePlaneFigure::BasePlaneFigure( 
		int       PolygonNum,       // 分割数（頂点数）
		COLOR_F   EmissivColor,     // オブジェクトの色（自己発光色）
		bool      UseZBuffer        // Zバッファを使用するか？
		) : m_bUseZBuffer(UseZBuffer), m_iPolygonNum(PolygonNum)
{
	
	// Vectexのメモリを確保
	int VectexNum = m_iPolygonNum*3;
	m_pVertex       = new VERTEX3D[VectexNum];
	m_pRawVertexPos = new Vector3D[VectexNum];
	m_pRawVertexNrm = new Vector3D[VectexNum];
	
	// color と使わない要素を代入する
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos  = m_pRawVertexPos[i].toVECTOR();
		m_pVertex[i].norm = m_pRawVertexNrm[i].toVECTOR();
		m_pVertex[i].dif  = GetColorU8( 0,0,0,0); //DifColor;
		m_pVertex[i].spc  = GetColorU8( 0,0,0,0); //SpcColor;
		m_pVertex[i].u    = 0.0f;
		m_pVertex[i].v    = 0.0f;
		m_pVertex[i].su   = 0.0f;
		m_pVertex[i].sv   = 0.0f;
	}

	// マテリアルパラメータの初期化
	m_Material.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Emissive = EmissivColor ;
	m_Material.Power    = 0.0f ;

	m_MaterialDefault.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Emissive = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Power    = 20.0f ;

};


void BasePlaneFigure::resetVertex()
{
	int VectexNum = m_iPolygonNum * 3;

	// m_pVertex.pos を m_pRawVertexPos で上書き
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos  = m_pRawVertexPos[i].toVECTOR();
		m_pVertex[i].norm = m_pRawVertexNrm[i].toVECTOR();
	}
};

void BasePlaneFigure::Render()
{
	if( !m_bUseZBuffer )
	{
		// Zバッファ OFF
		SetUseZBuffer3D( FALSE );
		SetWriteZBuffer3D( FALSE );
	}

	// ライトの影響を受けないように設定
	SetMaterialUseVertDifColor( FALSE ) ; // 頂点データのディフューズカラーを使用しないようにする
	SetMaterialUseVertSpcColor( FALSE ) ; // 頂点データのスペキュラカラーを使用しないようにする
	SetMaterialParam( m_Material ) ; // マテリアルの設定を有効

	DrawPolygon3D( m_pVertex, m_iPolygonNum, DX_NONE_GRAPH, FALSE ) ;

	// マテリアル設定を元に戻す
	SetMaterialUseVertDifColor( TRUE ) ;
	SetMaterialUseVertSpcColor( TRUE ) ;
	SetMaterialParam( m_MaterialDefault ) ;

	if( !m_bUseZBuffer )
	{
		// Zバッファ有効化
		SetUseZBuffer3D( TRUE );
		SetWriteZBuffer3D( TRUE );
	}


};

void BasePlaneFigure::MatTransVertex( const MATRIX &Mat )
{
	int VectexNum = m_iPolygonNum * 3;
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos  = VTransform( m_pVertex[i].pos, Mat );
		m_pVertex[i].norm = VTransformSR( m_pVertex[i].norm, Mat ); // 法線ベクトルも変換が必要になる
	}
};



// ###############################################
// ########## class PlaneRing
// ###############################################
PlaneRing::PlaneRing( 
		double    Radius,           // 輪の半径（内径）
		double    Width,			// 輪の幅 
		int       DivNum            // 分割数
		) : 
	BasePlaneFigure( DivNum*2, GetColorF( 1.0f, 0.0f, 0.0f, 0.0f ), false ),
	m_vCntPos( Vector3D(0,0,0) )
{
	// #### 先ずは、単位円柱をのVertexを生成し、それをシフト、スケーリングする
	
	// 分割数から正多角形の中心角を計算
	double CenterAng = 2*DX_PI_F/((double)DivNum);
	
	Vector3D V3D0B( 0,  0,0 );
	Vector3D V3D0T( 0,1.0,0 );
	Vector3D Nrm( 0.0, 1.0, 0.0 );

	for( int i=0; i<DivNum; i++ )
	{
		int j=(i+1)%DivNum; // 次の添字（円順対応）
		Vector2D V2D1( cos(CenterAng*i), sin(CenterAng*i) );
		Vector2D V2D2( cos(CenterAng*j), sin(CenterAng*j) );
		
		Vector3D V3D1i = ( Radius       *V2D1).toVector3D();
		Vector3D V3D1o = ((Radius+Width)*V2D1).toVector3D();
		Vector3D V3D2i = ( Radius       *V2D2).toVector3D();
		Vector3D V3D2o = ((Radius+Width)*V2D2).toVector3D();


		// 三角形１
		m_pRawVertexPos[ 6*i+3*0+0 ] = V3D1o;
		m_pRawVertexPos[ 6*i+3*0+1 ] = V3D2o;
		m_pRawVertexPos[ 6*i+3*0+2 ] = V3D1i;
		
		// 三角形２
		m_pRawVertexPos[ 6*i+3*1+0 ] = V3D1i;
		m_pRawVertexPos[ 6*i+3*1+1 ] = V3D2i;
		m_pRawVertexPos[ 6*i+3*1+2 ] = V3D2o;

		for( j=0; j<6; j++ )
		{
			m_pRawVertexNrm[ 6*i+j ] = Nrm;
		}

	}

	resetVertex();

};

void PlaneRing::setCenterPos( Vector3D CntPos )
{
	int VectexNum = m_iPolygonNum * 3;

	// 中心位置をシフト
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos = (m_pRawVertexPos[i]+CntPos).toVECTOR();
	}
};


// ###############################################
// ########## class PlaneConvexFill
// ###############################################

// コンストラクタ
PlaneConvexFill::PlaneConvexFill( 
		Vector2D  *pVertex2D,       // 凸形図形の輪郭頂点の配列
		int       DivNum,           // 分割数（頂点数）
		COLOR_F   EmissivColor      // オブジェクトの色（自己発光色）
		) : BasePlaneFigure( DivNum - 2, EmissivColor, true )
{
	// 輪郭頂点情報から、ポリゴン情報を生成する。
	// 初めの要素 pVertex2D[0] を中心に放射状に三角形分割する

	// 法線方向ベクトル
	Vector3D vNrm = Vector3D( 0.0, 1.0, 0.0 );

	int c=0; //カウンタ
	for( int i=2; i<DivNum; i++ )
	{
		// 三角形
		m_pRawVertexPos[ 3*c+0 ] = pVertex2D[0].toVector3D();
		m_pRawVertexNrm[ 3*c+0 ] = vNrm;

		m_pRawVertexPos[ 3*c+1 ] = pVertex2D[i-1].toVector3D();
		m_pRawVertexNrm[ 3*c+1 ] = vNrm;

		m_pRawVertexPos[ 3*c+2 ] = pVertex2D[i].toVector3D();
		m_pRawVertexNrm[ 3*c+2 ] = vNrm;
		c++;

	}
	assert( c==m_iPolygonNum );

	resetVertex();

};


// ###############################################
// ########## class LineRing
// ###############################################


LineRing::LineRing(
		double    Radius,           // 輪の半径
		int       DivNum,           // 分割数
		unsigned int Color          // 線の色
		) :
	m_iColor( Color )
{
	// m_pVECTORs, m_pRawVertexPos のメモリを確保
	m_iVertexNum = DivNum;
	m_pVECTORs      = new VECTOR[m_iVertexNum];
	m_pRawVertexPos = new Vector3D[m_iVertexNum];

	// 分割数から正多角形の中心角を計算
	double CenterAng = 2*DX_PI_F/((double)m_iVertexNum);

	// m_pRawVertexPos を生成
	for( int i=0; i<m_iVertexNum; i++ )
	{
		Vector2D V2D( cos(CenterAng*i), sin(CenterAng*i) );
		m_pRawVertexPos[i] = (Radius*V2D).toVector3D();
	}
};

void LineRing::resetVertex()
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = m_pRawVertexPos[i].toVECTOR();
	}
}

void LineRing::Render()
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		int j=(i+1)%m_iVertexNum; // 次の添字（円順対応）
		DrawLine3D( m_pVECTORs[i], m_pVECTORs[j], m_iColor );
	}
};

void LineRing::MatTransVertex( const MATRIX &Mat )
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = VTransform( m_pVECTORs[i], Mat );
	}
};


// ###############################################
// ########## class LineSegment
// ###############################################


LineSegment::LineSegment(
		Vector3D bgn,  // 始点
		Vector3D end,  // 終点
		unsigned int Color          // 線の色
		) :
	m_iColor( Color )
{
	// m_pVECTORs, m_pRawVertexPos のメモリを確保
	m_iVertexNum = 2;
	m_pVECTORs      = new VECTOR[m_iVertexNum];
	m_pRawVertexPos = new Vector3D[m_iVertexNum];

	m_pRawVertexPos[0] = bgn;
	m_pRawVertexPos[1] = end;
};

void LineSegment::resetVertex()
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = m_pRawVertexPos[i].toVECTOR();
	}
}

void LineSegment::Render()
{
	DrawLine3D( m_pVECTORs[0], m_pVECTORs[1], m_iColor );
};

void LineSegment::MatTransVertex( const MATRIX &Mat )
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = VTransform( m_pVECTORs[i], Mat );
	}
};

// ###############################################
// ########## class LineFreeCycle
// ###############################################

LineFreeCycle::LineFreeCycle(
		int       DivNum,           // 分割数
		unsigned int Color          // 線の色
		) :
	m_iColor( Color )
{
	// m_pVECTORs, m_pRawVertexPos のメモリを確保
	m_iVertexNum = DivNum;
	m_pVECTORs      = new VECTOR[m_iVertexNum];
	m_pRawVertexPos = new Vector3D[m_iVertexNum];

	// 分割数から正多角形の中心角を計算
	double CenterAng = 2*DX_PI_F/((double)m_iVertexNum);

	// m_pRawVertexPos を生成
	for( int i=0; i<m_iVertexNum; i++ )
	{
		// 初期値は単位円になるようにする。
		Vector2D V2D( cos(CenterAng*i), sin(CenterAng*i) );
		m_pRawVertexPos[i] = (1.0*V2D).toVector3D();
	}
};

void LineFreeCycle::resetVertex()
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = m_pRawVertexPos[i].toVECTOR();
	}
}

void LineFreeCycle::Render()
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		int j=(i+1)%m_iVertexNum; // 次の添字（円順対応）
		DrawLine3D( m_pVECTORs[i], m_pVECTORs[j], m_iColor );
	}
};

void LineFreeCycle::MatTransVertex( const MATRIX &Mat )
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = VTransform( m_pVECTORs[i], Mat );
	}
};

// ###############################################
// ########## class Arrow3D
// ###############################################
// コンストラクタ
Arrow3D::Arrow3D( 
	double    ArrowHight, 
	double    ArrowWidth, 
	double    BodyWidth, 
	COLOR_F   EmissivColor
	) : m_dArwHight( ArrowHight ),
		m_dArwWidth( ArrowWidth ),
		m_dBdyWidth( BodyWidth )
{
	// ポリゴン配列を確保・初期化
	m_iPolygonNum = 3; // 1(矢印の矢の部分) + 2(矢印の棒の部分)

	// Vectexのメモリを確保
	int VectexNum = m_iPolygonNum*3;
	m_pVertex       = new VERTEX3D[VectexNum];
	
	// color と使わない要素を代入する
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].dif  = GetColorU8( 0,0,0,0); //DifColor;
		m_pVertex[i].spc  = GetColorU8( 0,0,0,0); //SpcColor;
		m_pVertex[i].u    = 0.0f;
		m_pVertex[i].v    = 0.0f;
		m_pVertex[i].su   = 0.0f;
		m_pVertex[i].sv   = 0.0f;
	}

	// マテリアルパラメータの初期化
	m_Material.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Emissive = EmissivColor ;
	m_Material.Power    = 0.0f ;

	m_MaterialDefault.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Emissive = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Power    = 20.0f ;

};

// 描画
void Arrow3D::Render( Vector3D vBgn, Vector3D vEnd, Vector3D vUpper )
{
	// ① 矢印を基本平面に変換するための座標変換行列を計算する

	// 各基底ベクトルを計算する
	Vector3D vHeading = (vEnd-vBgn).normalize();
	//Vector3D vSide = (vHeading * vUpper).normalize(); // やはりエラーになる
	Vector3D vSide = VCross( vHeading.toVECTOR() , vUpper.toVECTOR() );
	vSide = vSide.normalize();

	Vector3D vNewUpper = VCross( vSide.toVECTOR() , vHeading.toVECTOR() );

	// 座標変換行列を計算する
	MATRIX TransMat = MGetAxis1(
		vHeading.toVECTOR(),
		vNewUpper.toVECTOR(),
		vSide.toVECTOR(),
		vBgn.toVECTOR() );

	// ② 基本平面上で矢印の各頂点の座標を計算する
	double dArwLength = (vEnd-vBgn).len();

	Vector3D vArwT (  dArwLength            , 0,                0 );
	Vector3D vArwL (  dArwLength-m_dArwHight, 0, -0.5*m_dArwWidth );
	Vector3D vArwR (  dArwLength-m_dArwHight, 0,  0.5*m_dArwWidth );
	Vector3D vBdyTL(  dArwLength-m_dArwHight, 0, -0.5*m_dBdyWidth );
	Vector3D vBdyTR(  dArwLength-m_dArwHight, 0,  0.5*m_dBdyWidth );
	Vector3D vBdyBL(                       0, 0, -0.5*m_dBdyWidth );
	Vector3D vBdyBR(                       0, 0,  0.5*m_dBdyWidth );

	// ③ ②で計算した基本平面上の頂点座標を①で計算した座標変換行列でワールド座標に変換する（ポリゴンの頂点座標を更新する。）

	vArwT  = VTransform( vArwT .toVECTOR(), TransMat);
	vArwL  = VTransform( vArwL .toVECTOR(), TransMat);
	vArwR  = VTransform( vArwR .toVECTOR(), TransMat);
	vBdyTL = VTransform( vBdyTL.toVECTOR(), TransMat);
	vBdyTR = VTransform( vBdyTR.toVECTOR(), TransMat);
	vBdyBL = VTransform( vBdyBL.toVECTOR(), TransMat);
	vBdyBR = VTransform( vBdyBR.toVECTOR(), TransMat);

	m_pVertex[0].pos  = vArwT.toVECTOR();
	m_pVertex[1].pos  = vArwL.toVECTOR();
	m_pVertex[2].pos  = vArwR.toVECTOR();

	m_pVertex[3].pos  = vBdyTL.toVECTOR();
	m_pVertex[4].pos  = vBdyTR.toVECTOR();
	m_pVertex[5].pos  = vBdyBL.toVECTOR();

	m_pVertex[6].pos  = vBdyBL.toVECTOR();
	m_pVertex[7].pos  = vBdyBR.toVECTOR();
	m_pVertex[8].pos  = vBdyTR.toVECTOR();

	// ④ ポリゴンを描画する。

	// Zバッファ OFF
	//SetUseZBuffer3D( FALSE );
	//SetWriteZBuffer3D( FALSE );

	// ライトの影響を受けないように設定
	SetMaterialUseVertDifColor( FALSE ) ; // 頂点データのディフューズカラーを使用しないようにする
	SetMaterialUseVertSpcColor( FALSE ) ; // 頂点データのスペキュラカラーを使用しないようにする
	SetMaterialParam( m_Material ) ; // マテリアルの設定を有効

	DrawPolygon3D( m_pVertex, m_iPolygonNum, DX_NONE_GRAPH, FALSE ) ;

	// マテリアル設定を元に戻す
	SetMaterialUseVertDifColor( TRUE ) ;
	SetMaterialUseVertSpcColor( TRUE ) ;
	SetMaterialParam( m_MaterialDefault ) ;

	// Zバッファ有効化
	//SetUseZBuffer3D( TRUE );
	//SetWriteZBuffer3D( TRUE );

};

// ###############################################
// ########## class BasicFig_Base
// ###############################################

// コンストラクタ
// - 図形の特性に合わせて、様々な初期化方法になる。
//--------------------------
BasicFig_Base::BasicFig_Base( 
		int       PolygonNum,       // ポリゴン（三角形）数
		COLOR_U8  DifColor,         // 頂点ディフューズカラー
		COLOR_U8  SpcColor          // 球の頂点スペキュラカラー
		) : 
		m_iPolygonNum(PolygonNum)
{
	m_iVectexNum = 3* m_iPolygonNum ;
	
	//   - ポインタのメモリ確保。
	// Vectexのメモリを確保
	m_pVertex   = new VERTEX3D[m_iVectexNum];
	m_pOrgVertx = new Vector3D[m_iVectexNum];
	m_pOrgNorms = new Vector3D[m_iVectexNum];

	//   - m_pVertex へ色（＋α）の設定。
	// color と使わない要素を代入する
	for( int i=0; i<m_iVectexNum; i++ )
	{
		m_pVertex[i].pos  = m_pOrgVertx[i].toVECTOR();
		m_pVertex[i].norm = m_pOrgNorms[i].toVECTOR();
		m_pVertex[i].dif  = DifColor; //DifColor;
		m_pVertex[i].spc  = SpcColor; //SpcColor;
		m_pVertex[i].u    = 0.0f;
		m_pVertex[i].v    = 0.0f;
		m_pVertex[i].su   = 0.0f;
		m_pVertex[i].sv   = 0.0f;
	}

};

// 基本変形の実施
// - オリジナルのvertexに適用される（=図形のデフォルトの姿勢）
//--------------------------
void BasicFig_Base::setDefault( const MATRIX &Mat )
{
	for( int i=0; i<m_iVectexNum; i++ )
	{
		m_pOrgVertx[i]  = VTransform(   m_pOrgVertx[i].toVECTOR() , Mat );
		m_pOrgNorms[i]  = VTransformSR( m_pOrgNorms[i].toVECTOR() , Mat ); // 法線ベクトルも変換が必要になる
	}

	// * m_pVertex を更新（setMatrix を実行）。
	setMatrix(m_MTransMat);

};

// 座標変換行列の設定
// - 描画時に適用される変形
//--------------------------
void BasicFig_Base::setMatrix( const MATRIX &Mat )
{
	// 行列を m_MTransMat に保存
	m_MTransMat = Mat;

	// * m_pOrgVertx/m_pOrgNorms → m_pVertex へ代入。
	// * m_pVertex に TransMat の座標変換を実施
	for( int i=0; i<m_iVectexNum; i++ )
	{
		m_pVertex[i].pos   = VTransform(   m_pOrgVertx[i].toVECTOR() , m_MTransMat );
		m_pVertex[i].norm  = VTransformSR( m_pOrgNorms[i].toVECTOR() , m_MTransMat ); // 法線ベクトルも変換が必要になる
	}

};

// 描画
//--------------------------
void BasicFig_Base::Render() 
{
	// * 描画
	DrawPolygon3D( m_pVertex, m_iPolygonNum, DX_NONE_GRAPH, FALSE ) ;

};


// ###############################################
// ########## class BasicFig_Column : BasicFig_Base
// ###############################################

// コンストラクタ
BasicFig_Column::BasicFig_Column( 
		Vector3D  CenterPos,       // 底面の円形の中心位置
		double    Radius,          // 半径
		double    Hight,           // 円柱の高さ
		int       DivNum,          // 分割数（＝底面の円形の分割数）
		COLOR_U8  DifColor,        // 頂点ディフューズカラー
		COLOR_U8  SpcColor         // 球の頂点スペキュラカラー
		) :
		BasicFig_Base( DivNum*4, DifColor, SpcColor )
{
	// ポリゴン数：
	// DivNum * 4(上面・底辺・側面（２倍）) 

	// 後は前に作成した円柱クラスのロジックをコピー
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
		m_pOrgVertx[ 12*i+3*0+0 ]  = V3D0B;
		m_pOrgVertx[ 12*i+3*0+1 ]  = V3D1B;
		m_pOrgVertx[ 12*i+3*0+2 ]  = V3D2B;
		for( int k=0; k<3; k++) m_pOrgNorms[ 12*i+3*0+k ] = Vector3D( 0, -1.0, 0);

		// 側面１
		m_pOrgVertx[ 12*i+3*1+0 ]  = V3D1T;
		m_pOrgVertx[ 12*i+3*1+1 ]  = V3D1B;
		m_pOrgVertx[ 12*i+3*1+2 ]  = V3D2B;
		for( int k=0; k<3; k++) m_pOrgNorms[ 12*i+3*1+k ] = V3Dn;

		// 側面２
		m_pOrgVertx[ 12*i+3*2+0 ]  = V3D2B;
		m_pOrgVertx[ 12*i+3*2+1 ]  = V3D1T;
		m_pOrgVertx[ 12*i+3*2+2 ]  = V3D2T;
		for( int k=0; k<3; k++) m_pOrgNorms[ 12*i+3*2+k ] = V3Dn;

		// 上面
		m_pOrgVertx[ 12*i+3*3+0 ]  = V3D0T;
		m_pOrgVertx[ 12*i+3*3+1 ]  = V3D1T;
		m_pOrgVertx[ 12*i+3*3+2 ]  = V3D2T;
		for( int k=0; k<3; k++) m_pOrgNorms[ 12*i+3*3+k ] = Vector3D( 0, 1.0, 0);

	}
	
	// シフト、スケーリングする
	for( int i=0; i<m_iVectexNum; i++ )
	{
		Vector3D tmp = m_pOrgVertx[i];
		tmp.x *= Radius;
		tmp.z *= Radius;
		tmp.y *= Hight;
		tmp += CenterPos;
		m_pOrgVertx[i] = tmp;
	}

	// 行列の初期値（単位行列）の設定とm_pVertex の初期化
	// * 全ての基本図形クラスで実施が必要。
	setMatrix(MGetIdent());

};

// ###############################################
// ########## class BasicFig_Column : BasicFig_Base
// ###############################################

// コンストラクタ
BasicFig_Cone::BasicFig_Cone( 
		Vector3D  CenterPos,       // 底面の円形の中心位置
		double    Radius,          // 半径
		double    Hight,           // 円錐の高さ
		int       DivNum,          // 分割数（＝底面の円形の分割数）
		COLOR_U8  DifColor,        // 頂点ディフューズカラー
		COLOR_U8  SpcColor         // 球の頂点スペキュラカラー
		) :
		BasicFig_Base( DivNum*2, DifColor, SpcColor )
{
	// ポリゴン数：
	// DivNum * 2(底辺・側面) 

	// 後は前に作成した円柱クラスのロジックをコピー
	// 分割数から正多角形の中心角を計算
	double CenterAng = 2*DX_PI_F/((double)DivNum);
	
	Vector3D V3D0B( 0,  0,0 );
	Vector3D V3D0T( 0,1.0,0 );

	for( int i=0; i<DivNum; i++ )
	{
		int j=(i+1)%DivNum; // 次の添字（円順対応）
		Vector2D V2D1( cos(CenterAng*i), sin(CenterAng*i) );
		Vector2D V2D2( cos(CenterAng*j), sin(CenterAng*j) );
		
		Vector3D V3D1B = V2D1.toVector3D();
		Vector3D V3D2B = V2D2.toVector3D();

		// 底面
		m_pOrgVertx[ 6*i+3*0+0 ]  = V3D0B;
		m_pOrgVertx[ 6*i+3*0+1 ]  = V3D1B;
		m_pOrgVertx[ 6*i+3*0+2 ]  = V3D2B;
		for( int k=0; k<3; k++) m_pOrgNorms[ 6*i+3*0+k ] = Vector3D( 0, -1.0, 0);

		// 側面１
		m_pOrgVertx[ 6*i+3*1+0 ]  = V3D0T;
		m_pOrgVertx[ 6*i+3*1+1 ]  = V3D1B;
		m_pOrgVertx[ 6*i+3*1+2 ]  = V3D2B;

		// 法線方向は外積から計算する
		for( int k=0; k<3; k++) m_pOrgNorms[ 6*i+3*1+k ] = ((V3D2B-V3D0T)%(V3D1B-V3D0T)).normalize();

	}
	
	// シフト、スケーリングする
	for( int i=0; i<m_iVectexNum; i++ )
	{
		Vector3D tmp = m_pOrgVertx[i];
		tmp.x *= Radius;
		tmp.z *= Radius;
		tmp.y *= Hight;
		tmp += CenterPos;
		m_pOrgVertx[i] = tmp;
	}

	// 行列の初期値（単位行列）の設定とm_pVertex の初期化
	// * 全ての基本図形クラスで実施が必要。
	setMatrix(MGetIdent());

};

// ###############################################
// ########## class CoordinateAxisModel
// ###############################################

// コンストラクタ
CoordinateAxisModel::CoordinateAxisModel(
		double thickness,		// 座標軸の太さ（＝矢印大きさ）
		double axis_x_length,	// Ｘ軸の長さ
		double axis_y_length,	// Ｙ軸の長さ
		double axis_z_length	// Ｚ軸の長さ
		)
{
	// 各基本図形の寸法を決めるための計算。
	
	// * 軸の棒部分の半径     = thickness
	// * 軸の先端の円錐の半径 = thickness * 1.5
	// * 軸の先端の円錐の高さ = thickness * 2.0

	double scale_botom = 2.0; 
	double scale_hight = 4.0;

	// 基本図形のインスタンス化

	m_pAxisX_Bar = new BasicFig_Column( 
		Vector3D( 0, 0, 0 ),
		thickness,
		axis_x_length,
		32,
		GetColorU8( 255, 0, 0, 0 ),
		GetColorU8( 255, 0, 0, 0 )
		);

	m_pAxisY_Bar = new BasicFig_Column( 
		Vector3D( 0, 0, 0 ),
		thickness,
		axis_y_length,
		32,
		GetColorU8( 0, 255, 0, 0 ),
		GetColorU8( 0, 255, 0, 0 )
		);

	m_pAxisZ_Bar = new BasicFig_Column( 
		Vector3D( 0, 0, 0 ),
		thickness,
		axis_z_length,
		32,
		GetColorU8( 0, 0, 255, 0 ),
		GetColorU8( 0, 0, 255, 0 )
		);

	m_pAxisX_Tip = new BasicFig_Cone( 
		Vector3D( 0, 0, 0 ),
		scale_botom * thickness,
		scale_hight * thickness,
		32,
		GetColorU8( 255, 0, 0, 0 ),
		GetColorU8( 255, 0, 0, 0 )
		);

	m_pAxisY_Tip = new BasicFig_Cone( 
		Vector3D( 0, 0, 0 ),
		scale_botom * thickness,
		scale_hight * thickness,
		32,
		GetColorU8( 0, 255, 0, 0 ),
		GetColorU8( 0, 255, 0, 0 )
		);

	m_pAxisZ_Tip = new BasicFig_Cone( 
		Vector3D( 0, 0, 0 ),
		scale_botom * thickness,
		scale_hight * thickness,
		32,
		GetColorU8( 0, 0, 255, 0 ),
		GetColorU8( 0, 0, 255, 0 )
		);

	// 基本図形の位置向きの設定
	MATRIX Msft;
	MATRIX Mrot;
	MATRIX Mwrk;

	// 矢印の形を作ってから、軸の方向に倒す。
	// → 上方向へ軸の長さ分だけシフトから軸方向へ回転

	// Ｘ軸
	Msft = MGetTranslate( Vector3D( 0, axis_x_length, 0 ).toVECTOR() );
	Mrot = MGetRotZ( -DX_PI_F/2 ); // X軸向きは反対になるようにする

	Mwrk = MMult( Msft, Mrot );
	m_pAxisX_Tip->setDefault(Mwrk);
	m_pAxisX_Bar->setDefault(Mrot);

	// Ｙ軸
	Msft = MGetTranslate( Vector3D( 0, axis_y_length, 0 ).toVECTOR() );
	Mrot = MGetIdent();

	Mwrk = MMult( Msft, Mrot );
	m_pAxisY_Tip->setDefault(Mwrk);
	m_pAxisY_Bar->setDefault(Mrot);

	// Ｚ軸
	Msft = MGetTranslate( Vector3D( 0, axis_z_length, 0 ).toVECTOR() );
	Mrot = MGetRotX( DX_PI_F/2 );

	Mwrk = MMult( Msft, Mrot );
	m_pAxisZ_Tip->setDefault(Mwrk);
	m_pAxisZ_Bar->setDefault(Mrot);

};

void CoordinateAxisModel::setMatrix( const MATRIX &Mat )
{
	// 各基本図形をsetMatrixするだけ
	m_pAxisX_Tip->setMatrix( Mat );
	m_pAxisX_Bar->setMatrix( Mat );
	m_pAxisY_Tip->setMatrix( Mat );
	m_pAxisY_Bar->setMatrix( Mat );
	m_pAxisZ_Tip->setMatrix( Mat );
	m_pAxisZ_Bar->setMatrix( Mat );

};

void CoordinateAxisModel::Render()
{
	// 各基本図形をRenderするだけ
	m_pAxisX_Tip->Render();
	m_pAxisX_Bar->Render();
	m_pAxisY_Tip->Render();
	m_pAxisY_Bar->Render();
	m_pAxisZ_Tip->Render();
	m_pAxisZ_Bar->Render();

};

// 後は、このクラスの動作確認
// 表示可能か？
// 変形可能か？
// → 完了

// 円錐クラスの作成
// * 考える必要があるとすれば、法線ベクトルの計算をどうやってやるか。
// → 完了（多分）

// キャラクタローカル座標モデル（応用図形クラス）の作成
// → 21:09 完了。

// モデルをキャラクタのローカル座標に描画させる


// 今日は図書館に行かなくていいや...
// 記事作成に邁進したい......









