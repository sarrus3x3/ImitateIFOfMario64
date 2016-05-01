#include <cassert>

#include "DxLib.h"

// 基本ライブラリ
#include "utils.h"
#include "Vector3D.h"
#include "Vector2D.h"

// ゲーム基盤系
#include "PrecisionTimer.h"
#include "VirtualController.h"
#include "PlayerCharacterEntity.h"

// 特化系？
#include "CameraWorkManager.h"
#include "SolidObjects.h"

// 2016/03/16
// 歩きのモーション → 地面：アニメ＝8:20 ぐらいがちょうどいい。

// 2016/04/03
// ★ ジャンプ後に、地面にめり込んで行くバグが有る。
//    恐らく、着地時にHeadingが、下方向に向いているので、後の移動処理でHeadingの地中方向に動いているのでは？
//    あとは情報をいろいろ出力させてみる

//#define LIGHT_ON

static const float ROTATE_SPEED = DX_PI_F/90;//回転スピード


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
	/*
	ParallelBox3D ColorfullBox(
		Vector3D( 0,0,0),
		Vector3D( 20,20,20),
		LoadGraph( "kirby_connect.bmp" ),
		GetColorU8( 255, 255, 255, 0 ),
		GetColorU8( 255, 255, 255, 0 )
		);
		*/

	TextureSphere3D ExperimentSphere(
		Vector3D( 0,0,0),
		500.0,
		false,
		32,
		32,
		LoadGraph( "Resize_PanoramaPict.jpg" ),
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

		// 背景パノラマの描画
		ExperimentSphere.setCenterPos( CameraWorkManager::Instance()->getCamPos() ); // パノラマ球の中心位置をカメラ中心へ
		ExperimentSphere.Render();

		// 地面用の円盤を描画
		//GroundDisk.Render();
		GrateEarthGround.Render();

		// 円柱の密林を描画
		for( int i=0; i<MaxColumnsNum; i++ ){ ppColmunList[i]->Render(); }

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

