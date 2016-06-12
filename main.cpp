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
#include "SolidObjects.h"

// デバック用
#include "AnimationManager.h"


// ########### 制御用 defin ###########

//#define MONUMENT_ON // モニュメント（円柱の密林）あり

#define GROUND_MESH_ON  // 地面の方眼模様あり

#define FARFAR_AWAY // 遠景（宇宙レベル）で遠いオブジェクトを描画する場合


// 木星衛星系のジオラマ
class JupiterSystemDiorama
{
private:
	// #### TextureSphere3D型の惑星オブジェクト

	// 木星オブジェクト
	TextureSphere3D *m_pPrimalyObj;

	// 衛星オブジェクト（配列）
	std::vector<TextureSphere3D> m_cSatelliteObjList;

	// 衛星軌道の線輪オブジェクト（配列）
	std::vector<LineRing> m_cSatelliteOrbitalObjList;

	// #### 属性パラメータ ####

	// 主星半径
	double m_dPrimalyStarRadius;

	// 主星自転周期(rotation period)
	double m_dPrimalyRotationPeriod;

public:
	// 衛星数
	static const int m_iSatelliteNum = 4;

	// 衛星名（列挙型）
	enum SatelliteID
	{
		SATELLITE_IO       = 0,
		SATELLITE_EUROPA   = 1,
		SATELLITE_GANYMEDE = 2,
		SATELLITE_CALLISTO = 3
	};

private:
	// 衛星半径（配列）
	double *m_pSatelliteStarRadius;

	// 衛星軌道半径（配列）
	double *m_pSatelliteOrbitalRadius;

	// 衛星自転周期（単位：日）（配列）
	double *m_pSatelliteRotationPeriod;

	// 衛星公転周期（単位：日）（配列）
	double *m_pSatelliteOrbitalPeriod;

	// 主星自転速度
	double m_dPrimalyRotationSpeed;

	// 衛星自転速度（ラジアン／日）（配列）
	double *m_pSatelliteRotationSpeed;

	// 衛星公転速度（ラジアン／日）（配列）
	double *m_pSatelliteOrbitalSpeed;


	// #### 状態パラメータ ####

	// 主星自転角
	double m_dPrimalyRotateAngle;

	// 衛星自転角（配列）
	std::vector<double> m_dSatelliteRotateAngleList;

	// 衛星公転角（配列）
	std::vector<double> m_dSatelliteOrbitalAngleList;

public:
	// ワールド座標への変換行列
	MATRIX m_mLocalToWorldMatrix;

	// #### メソッド ####

	// コストラクタ
	JupiterSystemDiorama();

	// 衛星位置更新
	void Update( double TimeElapse );

	// オブジェクトの頂点情報位置計算
	//   Render()で描画する前に、
	//   軌道位置などの系状態と変換行列から各オブジェクトの頂点位置を計算する。
	//   Render()と分離しているのは、系状態・変換行列の更新ない時に処理軽減したいため。
	void setVertex();

	// 描画
	void Render();

	// 一回テストしてみたほうがいいでしょう。

};

// コンストラクタ（パチパチとデータを入力していく）
JupiterSystemDiorama::JupiterSystemDiorama() :
	m_mLocalToWorldMatrix( MGetIdent() ), // マトリクスの初期化
	m_dSatelliteRotateAngleList( m_iSatelliteNum, 0 ),
	m_dSatelliteOrbitalAngleList( m_iSatelliteNum, 0 ),
	m_dPrimalyRotateAngle( 0 )
{
	// ######### 各種変数の初期化処理 #########

	// #### 属性パラメータ ####
	// Desktop\素材\惑星テクスチャ\木星と衛星

	// 主星半径
	m_dPrimalyStarRadius = 8.93650000;

	// 主星自転周期(rotation period)
	m_dPrimalyRotationPeriod = 0.4135;

	// 衛星半径（配列）
	m_pSatelliteStarRadius = new double[m_iSatelliteNum];
	m_pSatelliteStarRadius[SATELLITE_IO      ] = 0.22766667;
	m_pSatelliteStarRadius[SATELLITE_EUROPA  ] = 0.19510000;
	m_pSatelliteStarRadius[SATELLITE_GANYMEDE] = 0.32890000;
	m_pSatelliteStarRadius[SATELLITE_CALLISTO] = 0.30128750;
	/*
	m_pSatelliteStarRadius[SATELLITE_IO      ] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_EUROPA  ] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_GANYMEDE] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_CALLISTO] = 3.0;
	*/

	// 衛星軌道半径（配列）
	m_pSatelliteOrbitalRadius = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalRadius[SATELLITE_IO      ] =  52.71250000;
	m_pSatelliteOrbitalRadius[SATELLITE_EUROPA  ] =  83.87925000;
	m_pSatelliteOrbitalRadius[SATELLITE_GANYMEDE] = 133.80150000;
	m_pSatelliteOrbitalRadius[SATELLITE_CALLISTO] = 235.33862500;

	// 衛星自転周期（配列）
	m_pSatelliteRotationPeriod = new double[m_iSatelliteNum];
	m_pSatelliteRotationPeriod[SATELLITE_IO      ] = 1.769137786;
	m_pSatelliteRotationPeriod[SATELLITE_EUROPA  ] = 3.551181041;
	m_pSatelliteRotationPeriod[SATELLITE_GANYMEDE] = 7.15455296;
	m_pSatelliteRotationPeriod[SATELLITE_CALLISTO] = 16.6890184;

	// 衛星公転周期（配列）（orbital period）
	m_pSatelliteOrbitalPeriod = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalPeriod[SATELLITE_IO      ] = 1.769137786;
	m_pSatelliteOrbitalPeriod[SATELLITE_EUROPA  ] = 3.551181041;
	m_pSatelliteOrbitalPeriod[SATELLITE_GANYMEDE] = 7.15455296;
	m_pSatelliteOrbitalPeriod[SATELLITE_CALLISTO] = 16.6890184;

	// #### TextureSphere3D型の惑星オブジェクト

	// 木星オブジェクト
	m_pPrimalyObj = new TextureSphere3D(
				Vector3D( 0, 0, 0),
				m_dPrimalyStarRadius,
				true,
				32,
				32,
				LoadGraph( "JupiterSystemDiorama\\jupiter.jpg" ),
				TextureSphere3D::OBJECT_NOSPECULAR
			);

	// 衛星オブジェクト（配列）
	//   配列で構築（仮確保）して、vector型のメンバに代入する方法をとる
	//   引数を持つクラスの配列の初期化方法
	//     http://brain.cc.kogakuin.ac.jp/~kanamaru/lecture/C++2/09/09-01.html
	TextureSphere3D Tmp[] = {
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_IO      ],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\0_io_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_EUROPA  ],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\1_europa_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_GANYMEDE],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\2_ganymede_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_CALLISTO],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\3_callisto_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			)
	};

	m_cSatelliteObjList.assign( &Tmp[0], &Tmp[m_iSatelliteNum] ); // vector型のメンバに代入

	// 衛星軌道の線輪オブジェクト（配列）
	LineRing Tmp2[] = {
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_IO      ],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_EUROPA  ],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_GANYMEDE],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_CALLISTO],
			32,
			GetColor( 255, 255, 255 )
			)
	};

	m_cSatelliteOrbitalObjList.assign( &Tmp2[0], &Tmp2[m_iSatelliteNum] ); // vector型のメンバに代入

	// ### 自転速度・公転速度を計算して格納
	
	// 主星
	m_dPrimalyRotationSpeed = 2*DX_PI_F/(double)m_dPrimalyRotationPeriod;

	// 衛星
	m_pSatelliteRotationSpeed = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalSpeed  = new double[m_iSatelliteNum];
	for( int i=0; i<m_iSatelliteNum; i++ )
	{
		m_pSatelliteRotationSpeed[i] = 2*DX_PI_F/(double)m_pSatelliteRotationPeriod[i];
		m_pSatelliteOrbitalSpeed[i]  = 2*DX_PI_F/(double)m_pSatelliteOrbitalPeriod[i];
	}

};

void JupiterSystemDiorama::setVertex()
{
	static MATRIX WorkMat;

	// #### 自転角、公転角から、ローカル座標における位置を計算

	m_pPrimalyObj->resetVertex();

	// 主星 - 自転
	WorkMat = MGetRotY( (float)m_dPrimalyRotateAngle );

	// 主星 - ワールド座標に変換
	WorkMat = MMult( WorkMat, m_mLocalToWorldMatrix );

	// Vertexに反映
	m_pPrimalyObj->MatTransVertex( WorkMat );


	// 衛星
	for( int i=0; i<m_iSatelliteNum; i++ )
	{ 
		m_cSatelliteObjList[i].resetVertex();

		// 自転
		WorkMat = MGetRotY( (float)m_dSatelliteRotateAngleList[i] );

		// 公転
		Vector2D OrbitalPos2D( 1, 0 );
		OrbitalPos2D = m_pSatelliteOrbitalRadius[i] * OrbitalPos2D.rot(m_dSatelliteOrbitalAngleList[i]);
		//Vector3D OrbitalPos3D = OrbitalPos2D.toVector3D();
		
		WorkMat.m[3][0] = OrbitalPos2D.x;
		WorkMat.m[3][2] = OrbitalPos2D.y;

		// ワールド座標に変換
		WorkMat = MMult( WorkMat, m_mLocalToWorldMatrix ); // 座標変換行列を合成するときは、先に作用させたい行列を MMult の左側に置くこと。

		// Vertexに反映
		m_cSatelliteObjList[i].MatTransVertex( WorkMat );

		// 軌道線
		m_cSatelliteOrbitalObjList[i].resetVertex();
		m_cSatelliteOrbitalObjList[i].MatTransVertex( m_mLocalToWorldMatrix );

	}

};

void JupiterSystemDiorama::Update( double TimeElapse )
{
	// 自転角、公転角を更新
	m_dPrimalyRotateAngle += TimeElapse*m_dPrimalyRotationSpeed;

	for( int i=0; i<m_iSatelliteNum; i++ )
	{
		m_dSatelliteRotateAngleList[i]  += TimeElapse*m_pSatelliteRotationSpeed[i];
		m_dSatelliteOrbitalAngleList[i] -= TimeElapse*m_pSatelliteOrbitalSpeed[i];
	}
};

void JupiterSystemDiorama::Render()
{
	// 各オブジェクトの描画メソッドを呼ぶだけ

	// 主星
	m_pPrimalyObj->Render();

	// 衛星
	for( int i=0; i<m_iSatelliteNum; i++ )
	{ 
		m_cSatelliteObjList[i].Render();

		// 軌道を表示
		m_cSatelliteOrbitalObjList[i].Render();

	}
	
};

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
	VirtualController VController; // 仮想コントローラーのインスタンス化

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
	GroundGrid ObjGroundGrid( 1000.0, 100 );

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
	SetLightDirection( VGet( 1.0, 0, 0 ) );

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

		// ################## コントローラーを更新 #######################
		VController.Update();

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
			PCEnti.m_pAnimMgr->ExpBoneOfPhysicsPart( !PCEnti.m_pAnimMgr->getCurBoneExpress() );
			MV1PhysicsResetState( PCEnti.m_pAnimMgr->DBG_getModelHandle() ); // 物理演算状態をリセット（これで発散するのが回避できる？）→ 上手くいかない
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

	}

	DxLib_End() ;			// ＤＸライブラリ使用の終了処理
	return 0 ;				// ソフトの終了 
}

