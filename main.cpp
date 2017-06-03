#include <cassert>

#include "DxLib.h"

// 基本ライブラリ
#include "utils.h"
#include "MyUtilities.h"
#include "Vector3D.h"
#include "Vector2D.h"

// ゲーム基盤系
#include "PrecisionTimer.h"
#include "VirtualController.h"
#include "PlayerCharacterEntity.h"

// 特化系？
#include "CameraWorkManager.h"
//#include "SolidObjects.h" // Scenery.h内でinclude済み
#include "Scenery.h" // 背景セット

// デバック用
#include "AnimationManager.h"

#include "DEMO_DashTurnBehavior.h"


// 2017/04/23
// 新PCからコミットテステス

// ########### 制御用 defin ###########

//#define MONUMENT_ON // モニュメント（円柱の密林）あり
#define GROUND_MESH_ON  // 地面の方眼模様あり
//#define FARFAR_AWAY // 遠景（宇宙レベル）で遠いオブジェクトを描画する場合
//#define FLOATING_DUNGEON  // 浮遊ダンジョンジオラマ
//#define ITEM_GETTING // アイテム収集演出


// ####################################


// 2016/03/16
// 歩きのモーション → 地面：アニメ＝8:20 ぐらいがちょうどいい。

// 2016/04/03
// ★ ジャンプ後に、地面にめり込んで行くバグが有る。
//    恐らく、着地時にHeadingが、下方向に向いているので、後の移動処理でHeadingの地中方向に動いているのでは？
//    あとは情報をいろいろ出力させてみる

//#define LIGHT_ON

static const float ROTATE_SPEED = DX_PI_F/90;//回転スピード

// #############################################################################
//                                   main
// #############################################################################

// プログラムは WinMain から始まります
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	// 背景色の設定
	//SetBackgroundColor( 135, 206, 235 ); // skyblue 87ceeb
	//SetBackgroundColor( 255, 255, 255 ); // skyblue 87ceeb
	
	if( ChangeWindowMode(TRUE) != DX_CHANGESCREEN_OK || DxLib_Init() == -1 ) return -1; //Windowモード
	//if( ChangeWindowMode(FALSE) != DX_CHANGESCREEN_OK || DxLib_Init() == -1 ) return -1; //全画面モード

	SetDrawScreen( DX_SCREEN_BACK );        //裏画面に設定

	SetUseZBuffer3D( TRUE ); // ZバッファON（Column描画に必要）
	SetWriteZBuffer3D( TRUE );

	SetCameraNearFar( 1.0f, 1500.0f ) ; // カメラの 手前クリップ距離と 奥クリップ距離を設定

	// ################## 地面の模様描画に使用する変数の準備 #######################

#ifdef MONUMENT_ON	
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
#endif

#ifdef GROUND_MESH_ON
	// 方眼メッシュのインスタンス化
	GroundGrid ObjGroundGrid( 1000.0, 100, GetColor( 255,255,255) );

#endif

#ifdef FARFAR_AWAY
	// 背景パノラマ球のインスタンス化
	TextureSphere3D ExperimentSphere(
		Vector3D( 0, 0, 0 ),
		500.0,
		false,
		32,
		32,
		LoadGraph( "Resize_PanoramaPict.jpg" ),
		//GetColorU8( 255, 255, 255, 0 ),
		//GetColorU8( 255, 255, 255, 0 ),
		TextureSphere3D::OBJECT_SKYDOME
		);
	ExperimentSphere.rotEuler( 10*ROTATE_SPEED, 0, 20*ROTATE_SPEED );

	// 木星実験
	TextureSphere3D BackgroundObjectJupiter(
		Vector3D( 1000000000, 0, 0),
		10000000.0,
		true,
		32,
		32,
		LoadGraph("JupiterSystemDiorama\\jupiter.jpg" ),
		//GetColorU8( 255, 255, 255, 0 ),
		//GetColorU8( 255, 255, 255, 0 ),
		TextureSphere3D::OBJECT_NOSPECULAR
		);
	
	// 回転
	BackgroundObjectJupiter.rotEuler( 10*ROTATE_SPEED, 0, 20*ROTATE_SPEED );
	BackgroundObjectJupiter.setCenterPos( Vector3D( 100000000, 0, 0) );
#endif

	// 木星ジオラマ
	JupiterSystemDiorama JupiterDioramaIns;

	// 少し姿勢を変える
	MATRIX TmpMat = MGetIdent();
	//MATRIX TmpMat = MGetRotZ( DX_PI_F/3 );

	// スケール行列を取得（ 実縮尺 : 100,000,000倍 ）
	TmpMat = MScale( TmpMat, 100000000.0 );

	// 木星系に対するワールド位置を設定（実縮尺で）
	TmpMat.m[3][0] = 5500000000.0;
	
	TmpMat = MMult( TmpMat, MGetRotX( DX_PI_F/8 ) );

	JupiterDioramaIns.m_mLocalToWorldMatrix = TmpMat;

	JupiterDioramaIns.setVertex();

	// ライト設定テスト
	//SetLightDifColor( GetColorF( 1.0, 1.0, 1.0, 1.0 ) );
	SetLightDirection( VGet( 1.0, -0.5, 0 ) ); // 木星（系）の見え方と、光の当たり方（=太陽の方向）がメチャクチャだけど、まぁ、いいか。

#ifdef FLOATING_DUNGEON
	// 浮遊ダンジョン
	FloatingDungeon Dungeon( 20.0, 200.0, "mapdef.bmp" );
#endif

	// ################## 変数の定義・初期化 #######################

	// ゲーム時間のカウンタ
	int GameWorldCounter=0;

	// タイマを生成
	PrecisionTimer timer;
	timer.Start();

	// 仮想コントローラーのインスタンス化
	VirtualController VController; 
	VController.CheckAndSetGamePadMode();

	// カメラモードを設定
	//CameraWorkManager::Instance()->setCameraMode( CameraWorkManager::TrackingMovingTarget );
	CameraWorkManager::Instance()->setCameraMode( CameraWorkManager::RotateCamOnGazePoint );
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

#ifdef ITEM_GETTING
	// みせかけアイテム回収機能実験
	SampleGameWorld::Initialize();
	SampleGameWorld GameWorldIns;

	GameWorldIns.SetItemsToWorld( 20.0, 5.0, "ItemPlaceDef.bmp" );
#endif

	// キャラクタローカル座標モデル描画テスト
	CoordinateAxisModel CharCordiModelTest( 0.5, 20, 20, 20 );

	// ライト関係パラメータ
	Vector3D LightPos;
	Vector3D LightHead;
    float OutAngle = 90.0f ;
    float InAngle = 45.0f ;
    float Range = 2000.0f ;
    float Atten0 = 0.0f ;
    float Atten1 = 0.0f ;
    float Atten2 = 0.0001f ;



#ifdef FARFAR_AWAY
	int Sx , Sy , Cb ;
    GetScreenState( &Sx , &Sy , &Cb ) ;
	int SubScreenHandle = MakeScreen( Sx, Sy, FALSE );
#endif

	// ################## メインループの開始 #######################
	while(!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen()){
		//ココに処理を書いていく

		// タイムスライスを取得
		double timeelaps = timer.TimeElapsed();

		// ゲーム時間のカウンタを更新
		GameWorldCounter++;

		// ################## コントローラーを更新 #######################
		VController.Update( timeelaps );

		// もし、Bボタン（Ctrlキー）が押されたら、カメラモードを変更
		if( VController.ButB.isNowPush() )
		{
			CurCameraMode = (CurCameraMode+1)%CameraWorkManager::m_iCameraModeMax;
			CameraWorkManager::Instance()->setCameraMode( (CameraWorkManager::CameraModeID)CurCameraMode );
		}

		//MV1SetRotationXYZ( ModelHandle, VGet( angleX, angleY, 0.0f ) ) ;

		// Xボタン（Xキー）が押されたら、物理演算モード変更
		if( VController.ButX.isNowPush() )
		{
			static int phystype = 0;
			phystype = (++phystype)%3 ;
			PCEnti.m_pAnimMgr->setAnimPhysicsType( (AnimationManager::PhysicsTypeID)phystype );

			// PHYSICS_NONE
			// PHYSICS_SELFMADE
			// PHYSICS_DXLIB

		}

		// Yボタン（yキー）が押されたら、ボーン表示／モデル表示切替
		if( VController.ButY.isNowPush() )
		{
			// 自動操作モードオン／オフ
			VController.AutoControlOnOff();

			/*
			PCEnti.m_pAnimMgr->ExpBoneOfPhysicsPart( !PCEnti.m_pAnimMgr->getCurBoneExpress() );
			MV1PhysicsResetState( PCEnti.m_pAnimMgr->DBG_getModelHandle() ); // 物理演算状態をリセット（これで発散するのが回避できる？）→ 上手くいかない
			*/
		}

		// アニメーション物理演算のタイムステップを制御する
		static double AnimPhysTimeElapsed = 0.001;
		if( CheckHitKey(KEY_INPUT_NUMPAD8)      > 0 ){ //左キーが押されていたら
			AnimPhysTimeElapsed += 0.001;
			PCEnti.m_pAnimMgr->m_pLeftHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
			PCEnti.m_pAnimMgr->m_pRightHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
		}
		else if( CheckHitKey(KEY_INPUT_NUMPAD2) > 0 ){ //右キーが押されていたら
			AnimPhysTimeElapsed -= 0.001;
			if( AnimPhysTimeElapsed < 0.0001 ) AnimPhysTimeElapsed = 0.0001;
			PCEnti.m_pAnimMgr->m_pLeftHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
			PCEnti.m_pAnimMgr->m_pRightHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
		}
		else if( CheckHitKey(KEY_INPUT_NUMPAD4) > 0 ){ //上キーが押されていたら
			AnimPhysTimeElapsed += 0.00001;
			PCEnti.m_pAnimMgr->m_pLeftHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
			PCEnti.m_pAnimMgr->m_pRightHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
		}
		else if( CheckHitKey(KEY_INPUT_NUMPAD6) > 0 ){ //下キーが押されていたら
			AnimPhysTimeElapsed -= 0.00001;
			if( AnimPhysTimeElapsed < 0.0001 ) AnimPhysTimeElapsed = 0.0001;
			PCEnti.m_pAnimMgr->m_pLeftHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
			PCEnti.m_pAnimMgr->m_pRightHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
		}

		// ################## EntityのUpdate #######################
		PCEnti.Update( timeelaps );

#ifdef ITEM_GETTING
		// ######### みせかけだけアイテム回収機能の実験 #########
		GameWorldIns.Update( timeelaps , PCEnti.Pos() );
#endif

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

#ifdef FARFAR_AWAY
		// サブ画面を描画対象に設定
		SetDrawScreen( SubScreenHandle );
		ClearDrawScreen(); // サブ画面をクリア

		CameraWorkManager::Instance()->setCamera();

		// 背景パノラマの描画
		ExperimentSphere.setCenterPos( CameraWorkManager::Instance()->getCamPos() ); // パノラマ球の中心位置をカメラ中心へ
		ExperimentSphere.Render();

		// 木星描画のためクリップ距離を変更
		SetCameraNearFar( 10000000.0f, 50000000000.0f ) ; // カメラの 手前クリップ距離と 奥クリップ距離を設定

		// 100000000.0f
		// 1,500,000,000.0f

		// 木星実験
		//BackgroundObjectJupiter.Render();

		// 木星ジオラマの描画
		JupiterDioramaIns.Update( 0.0001 );
		JupiterDioramaIns.setVertex();
		JupiterDioramaIns.Render();

		// 裏画面を描画対象にする
		SetDrawScreen(DX_SCREEN_BACK);

		// サブ画面のグラフィックを裏画面に透過処理つきで描画
		DrawGraph(0,0,SubScreenHandle,FALSE);

		// クリップ距離を戻す
		SetCameraNearFar( 1.0f, 1500.0f ) ; // カメラの 手前クリップ距離と 奥クリップ距離を設定
#endif
		// カメラ位置再設定
		CameraWorkManager::Instance()->setCamera();

		// ################## 地面の模様の描画 #######################
		

#ifdef MONUMENT_ON
		// 地面用の円盤を描画
		//GroundDisk.Render();
		GrateEarthGround.Render();

		// 円柱の密林を描画
		for( int i=0; i<MaxColumnsNum; i++ ){ ppColmunList[i]->Render(); }
#endif

#ifdef GROUND_MESH_ON
		// 地面の方眼模様を描画
		ObjGroundGrid.Render(); // 素晴らしい..
#endif

#ifdef FLOATING_DUNGEON
		// 浮遊ダンジョンジオラマの描画
		Dungeon.Render();
#endif

		// ################## Entityの描画 #######################
		PCEnti.Render();

#ifdef ITEM_GETTING
		// ######### みせかけだけアイテム回収機能の実験 #########
		GameWorldIns.Render();
#endif
		// キャラクタローカル座標モデル描画テスト

		// キャラクタローカル座標に合わせて描画するようにする
		MATRIX Mtns = MGetAxis1(	// キャラクタの姿勢から [ Head, Upper, Side ]
			PCEnti.Heading().toVECTOR(),
			PCEnti.Uppder().toVECTOR(),
			PCEnti.Side().toVECTOR(),
			PCEnti.Pos().toVECTOR()
			); 

		// ★はい！なんかおかしいね！→もう寝なさい！

		CharCordiModelTest.setMatrix(Mtns);
		CharCordiModelTest.Render();

		// ################## コントローラーを描画 #######################
		//VController.Render( Vector2D(25,25) );

		// ################## デバック情報を出力 #######################
		//行数
		int colmun= 10;
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

		// 現在のStateの描画
		DrawFormatString( 0, width*colmun, 0xffffff, "StateName:%s", PCEnti.DBG_getCurrentStateName().c_str() ); 
		colmun++;


		// アナログスティック状態とEntityの向きを描画
		PCEnti.DBG_renderStickTiltAndHeading( Vector2D( 400, 100 ) );

		// 現在のアニメーションの再生時間を出力
		DrawFormatString( 0, width*colmun, 0xffffff, "Current Animation Plya Time :%8f",PCEnti.m_pAnimMgr->CurPlayTime() ); 
		colmun++;

		// 再生中のアニメーション情報を出力
		DrawFormatString(0, width*colmun, 0xffffff, "Current  Anim Name:%s", PCEnti.m_pAnimMgr->getCurAnimName().c_str());
		colmun++;

		DrawFormatString(0, width*colmun, 0xffffff, "Previous Anim Name:%s", PCEnti.m_pAnimMgr->getPrvAnimName().c_str());
		colmun++;

		/*
		// キャラクタのローカル座標情報を出力
		DrawFormatString( 0, width*colmun, 0xffffff, "Head :%4f, %4f, %4f",PCEnti.Heading().x, PCEnti.Heading().y, PCEnti.Heading().z ); 
		colmun++;

		DrawFormatString( 0, width*colmun, 0xffffff, "Side :%4f, %4f, %4f",PCEnti.Side().x, PCEnti.Side().y, PCEnti.Side().z ); 
		colmun++;

		DrawFormatString( 0, width*colmun, 0xffffff, "Upper:%4f, %4f, %4f",PCEnti.Uppder().x, PCEnti.Uppder().y, PCEnti.Uppder().z ); 
		colmun++;
		*/

		// サブ状態の継続時間を出力
		//PCEnti.DBG_exp_OneEightyDegreeTurn_SubStateDurations( colmun );

		/*
		// Entityの移動レベルを表示
		DrawFormatString( 0, width*colmun, 0xffffff, "Entity Mode Level:%d",PCEnti.m_eMoveLevel ); 
		colmun++;

		// アナログスティックの傾き量
		DrawFormatString( 0, width*colmun, 0xffffff, "Analog Stick Tilt:%f",PCEnti.m_pVirCntrl->m_dStickL_len ); 
		colmun++;

		DrawFormatString( 0, width*colmun, 0xffffff, "pEntity->SpeedSq():%f",PCEnti.DBG_m_dDBG ); 
		colmun++;
		*/


		/*
		// 現在のアニメーション物理演算種別
		DrawFormatString( 0, width*colmun, 0xffffff, "AnimationPhysicsType:%d", PCEnti.m_pAnimMgr->getPhysicsType() ); 
		colmun++;

		// 現在のボーン表示／モデル表示
		DrawFormatString( 0, width*colmun, 0xffffff, "AnimationPhysics-CurBoneExpress:%d", PCEnti.m_pAnimMgr->getCurBoneExpress() ); 
		colmun++;


		DrawFormatString( 0, width*colmun, 0xffffff, 
			"AnimationPhysics-CurBoneExpress(DXlib):%d", 
			MV1GetFrameVisible( 
				PCEnti.m_pAnimMgr->DBG_getModelHandle(), 
				PCEnti.m_pAnimMgr->m_iLeftHair1FrameIndex 
				)
			); 
		colmun++;

		// アニメーション物理演算のタイムステップを表示
		DrawFormatString( 0, width*colmun, 0xffffff, "AnimPhysTimeElapsed:%f", AnimPhysTimeElapsed ); 
		colmun++;

		// アニメーション物理演算の処理時間を表示
		DrawFormatString( 0, width*colmun, 0xffffff, 
			"DBG_m_dAverageTimeForUpdate:%f", 
			PCEnti.m_pAnimMgr->m_pLeftHairPhysics->DBG_m_dAverageTimeForUpdate ); 
		colmun++;
		*/

		// ################## 退避させておいた物理情報の更新 #######################
		PCEnti.DBG_UpdateSavePhys();

	}

	DxLib_End() ;			// ＤＸライブラリ使用の終了処理
	return 0 ;				// ソフトの終了 
}

