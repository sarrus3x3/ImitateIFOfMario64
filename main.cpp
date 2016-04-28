#include <cassert>

#include "DxLib.h"
#include "utils.h"

#include "Vector3D.h"
#include "Vector2D.h"
#include "PrecisionTimer.h"
#include "VirtualController.h"
#include "PlayerCharacterEntity.h"

#include "CameraWorkManager.h"

// 2016/03/16
// 歩きのモーション → 地面：アニメ＝8:20 ぐらいがちょうどいい。

// 2016/04/03
// ★ ジャンプ後に、地面にめり込んで行くバグが有る。
//    恐らく、着地時にHeadingが、下方向に向いているので、後の移動処理でHeadingの地中方向に動いているのでは？
//    あとは情報をいろいろ出力させてみる

//#define LIGHT_ON

static const float ROTATE_SPEED = DX_PI_F/90;//回転スピード

// 円柱を描画するclass
class Column
{
private:
	VERTEX3D* m_pVertex; // ポリゴン集合を保持
	int m_PolygonNum;    // ポリゴン数

public:
	// コンストラクタ
	Column( 
		Vector3D  CenterPos,       // 底面の円形の中心位置
		double    Radius,          // 半径
		double    Hight,           // 円柱の高さ
		int       DivNum,          // 分割数（＝底面の円形の分割数）
		COLOR_U8  DifColor, // 頂点ディフューズカラー
		COLOR_U8  SpcColor  // 球の頂点スペキュラカラー
		);

	// 描画
	void Render();

};

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

// 細かいメッシュの平板
class FineMeshedFlatPlate
{
private:
	VERTEX3D* m_pVertex; // ポリゴン集合を保持
	int m_PolygonNum;    // ポリゴン数

public:
	// コンストラクタ
	FineMeshedFlatPlate( 
		Vector2D  BLCorner,        // 平板の（世界平面上の）左下頂点座標
		Vector2D  TRCorner,        // 平板の（世界平面上の）右上頂点座標
		int       DivNum,          // 分割数（縦・横この数で分割されるため、ポリゴン三角形数は DivNum * DivNum * 2 となる）
		COLOR_U8  DifColor,        // 頂点ディフューズカラー
		COLOR_U8  SpcColor         // 球の頂点スペキュラカラー
		);

	// 描画
	void Render();

};

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

// 直方体を描画するclass、画像貼り付けの実験
class ParallelBox3D
{
private:
	VERTEX3D* m_pVertex; // ポリゴン集合を保持
	int m_PolygonNum;    // ポリゴン数

	// テクスチャ画像のハンドル
	int m_iTexturesHandle;

public:
	// コンストラクタ
	ParallelBox3D( 
		Vector3D  SmallVertex,      // 立方体の頂点で、座標的に一番小さい
		Vector3D  LargeVertex,      // 立方体の頂点で、座標的に一番大きい
		COLOR_U8  DifColor, // 頂点ディフューズカラー
		COLOR_U8  SpcColor  // 球の頂点スペキュラカラー
		);

	// 描画
	void Render();

};
ParallelBox3D::ParallelBox3D( 
		Vector3D  SmallVertex,      // 立方体の頂点で、座標的に一番小さい
		Vector3D  LargeVertex,      // 立方体の頂点で、座標的に一番大きい
		COLOR_U8  DifColor, // 頂点ディフューズカラー
		COLOR_U8  SpcColor  // 球の頂点スペキュラカラー
		)
{
	// テクスチャ用の画像の読み込み
    m_iTexturesHandle = LoadGraph( "kirby_connect.bmp" ) ;

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



// ################## FPS測定用 #######################
class MeasureFPS
{
public:
	static const int SampleRate = 20; // FPS表示で平均をとる数

	std::vector<double> m_FrameTimeHistory;
	int m_iNextUpdateSlot;

	// コンストラクタ
	MeasureFPS():
		m_FrameTimeHistory(SampleRate,0.0),
		m_iNextUpdateSlot(0)
	{}

	// 更新と平均値の計算
	double Update( double NewFrameTime )
	{
		m_FrameTimeHistory[m_iNextUpdateSlot]=NewFrameTime;
		
		m_iNextUpdateSlot = (m_iNextUpdateSlot+1)%SampleRate;

		double sum=0;
		std::vector<double>::iterator it = m_FrameTimeHistory.begin();
		for( it; it!=m_FrameTimeHistory.end(); it++ )
		{
			sum += *it;
		}

		return sum / (double)SampleRate ;
	}

};



// #############################################################################
//                                   main
// #############################################################################

// プログラムは WinMain から始まります
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	VirtualController VController; // 仮想コントローラーのインスタンス化

	// 背景色の設定
	//SetBackgroundColor( 135, 206, 235 ); // skyblue 87ceeb
	
	if( ChangeWindowMode(TRUE) != DX_CHANGESCREEN_OK || DxLib_Init() == -1 ) return -1; //Windowモード
	//if( ChangeWindowMode(FALSE) != DX_CHANGESCREEN_OK || DxLib_Init() == -1 ) return -1; //全画面モード

	SetDrawScreen( DX_SCREEN_BACK );        //裏画面に設定

	SetUseZBuffer3D( TRUE ); // ZバッファON（Column描画に必要）
	SetWriteZBuffer3D( TRUE );

	SetCameraNearFar( 1.0f, 1500.0f ) ; // カメラの 手前クリップ距離と 奥クリップ距離を設定

	// ################## 地面の模様描画に使用する変数の準備 #######################
	
	// 円柱の密林を生成する
	int    MaxColumnsNum     = 250;
	double ColumnForestRange = 4*250.0;
	double AverageHight      = 20.0;
	Column** ppColmunList;

	ppColmunList = new Column*[MaxColumnsNum]; // メモリを確保

	vector<Vector2D> ColmunPoss(MaxColumnsNum);
	for( int i=0; i<MaxColumnsNum; i++ )
	{
		// 位置は、ColumnForestRange * ColumnForestRange の領域のランダム
		Vector2D Pos2D;
		Pos2D.x = ColumnForestRange * RandFloat();
		Pos2D.y = ColumnForestRange * RandFloat();
		ColmunPoss[i] = Pos2D;
	}

	for( int i=0; i<MaxColumnsNum; i++ )
	{
		Vector3D Pos3D = ColmunPoss[i].toVector3D();

		double columnhight = AverageHight*(1+0.25*RandGaussian());
		double columnradius = 2.5*(1+0.25*RandGaussian());

		// 負値になることを防ぐ
		columnhight  = max( columnhight,  0.1 );
		columnradius = max( columnradius, 0.1 );

		//int GlayLv = rand();
		int GlayLv = 254;
		COLOR_U8 col = GetColorU8( GlayLv%255, GlayLv%255, GlayLv%255, 0 );

		ppColmunList[i] = new Column( 
			Pos3D,
			columnradius,
			columnhight,
			16*(int)(columnradius+1), // これだと重くなる。半径の大きな円柱が負荷になるようだ
			//16, 
			col, 
			col ) ;

	}

	// 地面用の円盤
	/*
	Column GroundDisk( 
		Vector3D(0,-1.0,0),
		ColumnForestRange*3,
		1.0,
		64,
		GetColorU8( 255, 255, 255, 0 ),
		GetColorU8( 255, 255, 255, 0 )
		);
		*/

	FineMeshedFlatPlate GrateEarthGround(
		Vector2D( -ColumnForestRange, -ColumnForestRange),
		Vector2D(  ColumnForestRange,  ColumnForestRange),
		//250,
		1,
		GetColorU8( 255, 255, 255, 0 ),
		GetColorU8( 255, 255, 255, 0 )
		);

	ParallelBox3D ColorfullBox(
		Vector3D( 0,0,0),
		Vector3D( 20,20,20),
		GetColorU8( 255, 255, 255, 0 ),
		GetColorU8( 255, 255, 255, 0 )
		);

	// ################## 変数の定義・初期化 #######################

	// タイマを生成
	PrecisionTimer timer;
	timer.Start();

	// カメラモードを設定
	CameraWorkManager::Instance()->setCameraMode( CameraWorkManager::TrackingMovingTarget );
	//CameraWorkManager::Instance()->setCameraMode( CameraWorkManager::RotateCamOnGazePoint );
	int CurCameraMode = 0;

	// Entityのインスタンス化
	PlayerCharacterEntity PCEnti(
		Vector3D(0,0,0),
		Vector3D(0,0,0),
		Vector3D(1.0,0,0),
		Vector3D(0.0,0,1.0),
		10.0,
		5.0,
		&VController );	

	// FPS測定
	MeasureFPS InsMeasureFPS;


	// ライト関係パラメータ
	Vector3D LightPos;
	Vector3D LightHead;
    float OutAngle = 90.0f ;
    float InAngle = 45.0f ;
    float Range = 2000.0f ;
    float Atten0 = 0.0f ;
    float Atten1 = 0.0f ;
    float Atten2 = 0.0001f ;


	// ################## メインループの開始 #######################
	while(!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen()){
		//ココに処理を書いていく

		// タイムスライスを取得
		double timeelaps = timer.TimeElapsed();

		// ################## コントローラーを更新 #######################
		VController.Update();

		// もし、Bボタン（Ctrlキー）が押されたら、カメラモードを変更
		if( VController.ButB.isNowPush() )
		{
			CurCameraMode = (CurCameraMode+1)%CameraWorkManager::m_iCameraModeMax;
			CameraWorkManager::Instance()->setCameraMode( (CameraWorkManager::CameraModeID)CurCameraMode );
		}

		//MV1SetRotationXYZ( ModelHandle, VGet( angleX, angleY, 0.0f ) ) ;


		// ################## EntityのUpdate #######################
		PCEnti.Update( timeelaps );

		// ################## ライト（照明）の設定 #######################
#ifdef LIGHT_ON
		if( CheckHitKey( KEY_INPUT_L ) == 0 )
		{
			LightPos     = PCEnti.Pos();
			LightHead    = PCEnti.Heading();
			LightPos.y  += 10.0;
			LightHead.y -=  0.1;
			LightPos    += 3.0 * LightHead;
		}

		ChangeLightTypeSpot(
            LightPos.toVECTOR(),
            LightHead.toVECTOR(),
            OutAngle * DX_PI_F / 180.0f,
            InAngle * DX_PI_F / 180.0f,
            Range,
            Atten0,
            Atten1,
            Atten2 ) ;
#endif
		// ################## カメラ設定 #######################
		CameraWorkManager::Instance()->setTarget(PCEnti.Pos());
		CameraWorkManager::Instance()->Update( timeelaps );
		CameraWorkManager::Instance()->setCamera();

		// ################## 地面の模様の描画 #######################

		// 地面用の円盤を描画
		//GroundDisk.Render();
		GrateEarthGround.Render();

		// 円柱の密林を描画
		for( int i=0; i<MaxColumnsNum; i++ ){ ppColmunList[i]->Render(); }

		// テクスチャ実験用の箱を描画
		ColorfullBox.Render();

		// ################## Entityの描画 #######################
		PCEnti.Render();

		// ################## コントローラーを描画 #######################
		VController.Render( Vector2D(25,25) );

		// ################## デバック情報を出力 #######################
		//行数
		int colmun= 0;
		int width = 15;

		// m_FrameTimeHistoryを更新
		int AvFPS = (int)(1.0 / InsMeasureFPS.Update(timeelaps));

		// FPSを出力
		DrawFormatString( 0, width*colmun, 0xffffff, "FPS:%d", AvFPS ); 
		colmun++;

		// カメラモードを出力
		std::string sCamMode = CameraWorkManager::Instance()->getCurCamModeName();
		DrawFormatString( 0, width*colmun, 0xffffff, "CamMode:%s", sCamMode.c_str() ); 
		colmun++;

		// Entityの速度を表示
		DrawFormatString( 0, width*colmun, 0xffffff, "Entity Speed:%8f",PCEnti.Speed() ); 
		colmun++;

		// 少しsleep
		//Sleep( 100 );

	}

	DxLib_End() ;			// ＤＸライブラリ使用の終了処理
	return 0 ;				// ソフトの終了 
}

