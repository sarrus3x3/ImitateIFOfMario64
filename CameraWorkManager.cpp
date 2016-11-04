#include <fstream> /// saveViewMatrix, loadViewMatrix で使用。

#include "DxLib.h"

#include "Vector3D.h"
#include "Vector2D.h"
#include "CameraWorkManager.h"

static const float ROTATE_SPEED = DX_PI_F/90;  //回転スピード

// コンストラクタ
CameraWorkManager::CameraWorkManager() :
	m_CurCamMode(RotateCamOnGazePoint),
	m_dTilt(0),
	m_dHead(0),
	m_dCamDistFrmFP(100.0),
	m_dCamHight(30.0),
	m_dDstCamToTrgtDef(30.0),
	m_TrgtHight(20.0),
	m_MViewLocal( MGetIdent() ),
	m_MViewWorld( MGetIdent() )
{

	m_dSqDstCamToTrgtDef = m_dDstCamToTrgtDef * m_dDstCamToTrgtDef;
	m_vFinalCamPos = Vector3D(0,m_dCamHight,0);
	m_TargetPos    = Vector3D(0,0,0);
	
	// カメラのビュー行列を外部ファイルから変数に読み込む
	loadViewMatrix();

}

CameraWorkManager* CameraWorkManager::Instance()
{
	static CameraWorkManager instance;
	return &instance;
};

void CameraWorkManager::getKeyInput()
{
	// キー入力による球体の移動
	static const double SPHERE_MOVE=3.0;
	if( CheckHitKey(KEY_INPUT_NUMPAD8)      > 0 )
	{ //左キーが押されていたら
		m_dTilt += ROTATE_SPEED;
	}
	else if( CheckHitKey(KEY_INPUT_NUMPAD2) > 0 )
	{ //右キーが押されていたら
		m_dTilt -= ROTATE_SPEED;
	}
	else if( CheckHitKey(KEY_INPUT_NUMPAD4) > 0 )
	{ //上キーが押されていたら
		m_dHead -= ROTATE_SPEED;
	}
	else if( CheckHitKey(KEY_INPUT_NUMPAD6) > 0 )
	{ //下キーが押されていたら
		m_dHead += ROTATE_SPEED;
	}
};

void CameraWorkManager::getMouseWheelInput( double timeslice )
{
	// C:\Users\kotaro MIHARA\Dropbox\Projects\GraphDraw_v0.0\GraphDraw_v0.0
	// DrawGraphMgr::update_scale() より流用
 
	static double m_scroll; // スクロール量
	static const double maxsc =  2.302585093;   // log(10)  - 倍率の上限値
	static const double minsc = -2.302585093;   // log(0.1) - 倍率の下限値

	// スクロールを滑らかにするため、物理モデルを導入
	static const double MASS           = 0.5;	// 質量
	static const double SPRING	       = 20;	// バネ定数
	static const double DAMPER         = 5;		// 減衰係数

	static double scrlloc = 0; // 位置
	static double scrlvel = 0; // 速度

	// m_scroll を更新
	int val = GetMouseWheelRotVol();
	m_scroll -= (16.0/100) * (double)(val);
	if(m_scroll>maxsc) m_scroll=maxsc; // 上限
	if(m_scroll<minsc) m_scroll=minsc; // 下限

	// モデルに従いForce(力)を計算
	double Force = SPRING * ( m_scroll - scrlloc );
	Force += -1 * ( DAMPER * scrlvel );

	// 物理量を更新
	scrlvel += ( timeslice / MASS ) * Force;
	scrlloc += timeslice * scrlvel;

	// m_viewscale の更新
	m_dCamDistFrmFP = 100*exp( scrlloc );

};

void CameraWorkManager::Update( double timeslice )
{
	switch (m_CurCamMode)
	{
	case RotateCamOnGazePoint:
		Update_RotateCamOnGazePoint( timeslice );
		break;

	case TrackingMovingTarget:
		Update_TrackingMovingTarget( timeslice );
		break;

	case SetSavedViewMatrix:
		Update_SetSavedViewMatrix();
		break;

	default:
		break;
	}
};

void CameraWorkManager::Update_RotateCamOnGazePoint( double timeslice )
{
	static const double DrugMoveScale=0.005; // ドラッグ量に対する視点の移動量のパラメータ

	static Vector2D ScreenDrugStartPoint;  // マウスでドラッグを開始したスクリーン上の座標を格納
	static Vector2D SavedCamDist;          // マウスでドラッグを開始したときの、m_dTilt、m_dHead を保持
	static bool     nowDruging=false;      // 今ドラッグ中か？

	// キー入力から、m_dTilt と m_dHead を更新
	//getKeyInput();

	// スクロール操作から、カメラと注視点の距離を更新
	getMouseWheelInput(timeslice);

	// マウスによるドラッグ操作により、m_dTilt と m_dHead を更新
	if( nowDruging )
	{ // 今ドラッグ中
		// ドラッグ終了したかを判定
		if( ( GetMouseInput() & MOUSE_INPUT_LEFT ) == 0 )
		{ // ドラッグ終了
			nowDruging = false; 
		}
		else
		{
			// 今のマウス位置を取得
			Point2D pos;
			GetMousePoint( &(pos.x) , &(pos.y) );
			Vector2D CurMousePos( (double)pos.x, (double)pos.y);
			
			// m_dTilt と m_dHead を更新
			Vector2D NewCamDist = -DrugMoveScale*(CurMousePos-ScreenDrugStartPoint)+SavedCamDist;
			m_dTilt = NewCamDist.y;
			m_dHead = NewCamDist.x;

		}
	}
	else
	{ // ドラッグ中でない
		// ドラッグ開始したかを判定
		int button;
		Point2D pos;
		if( GetMouseInputLog( &button, &pos.x, &pos.y, TRUE )==0
			&& ( button & MOUSE_INPUT_LEFT ) != 0 )
		{ // ドラッグが開始された
			// ドラッグ開始時の マウス座標を ScreenDrugStartPoint に保持
			ScreenDrugStartPoint = pos.toRealVector();

			// ドラッグ開始時の m_dTilt、m_dHead の値を SavedCamDist に保持
			SavedCamDist.y = m_dTilt;
			SavedCamDist.x = m_dHead;

			// ドラッグフラグを立てる
			nowDruging = true;
		}
	}
	

	// m_dHead と m_dTilt から、（GazingPointを原点とした）カメラの位置を計算する
	Vector3D Hedding3D( 1.0, 0, 0 );
	Hedding3D = VTransform( Hedding3D.toVECTOR(), MGetRotZ( (float)m_dTilt ) ); // もしかしたら、マイナスにしないと駄目かも
	Hedding3D = VTransform( Hedding3D.toVECTOR(), MGetRotY( -1*(float)m_dHead ) );
	Vector3D toCamPos3D = m_dCamDistFrmFP * Hedding3D ;

	// ### ビュー行列を生成を確認

	// カメラの座標変換行列のz軸向き（=カメラの向き）
	Vector3D vViewBaseZ = -1 * Hedding3D; 

	// カメラの座標変換行列のy軸向きを仮置き
	Vector3D vViewBaseY( 0.0, 1.0, 0.0 );

	// カメラの座標変換行列のx軸向き（= y × z ）
	Vector3D vViewBaseX = VCross( vViewBaseY.toVECTOR(), vViewBaseZ.toVECTOR() );
	vViewBaseX = vViewBaseX.normalize();

	// カメラの座標変換行列のy軸向きを再調整
	vViewBaseY = VCross( vViewBaseZ.toVECTOR(), vViewBaseX.toVECTOR() );

	// 基底を組み合わせてカメラ座標変換行列（Entityローカル座標）を生成
	MATRIX MCamTransMatLocal = 
		MGetAxis1(
		//MakeMatrixFromBaseVec( 
			vViewBaseX.toVECTOR(),
			vViewBaseY.toVECTOR(),
			vViewBaseZ.toVECTOR(),
			toCamPos3D.toVECTOR() );
	MCamTransMatLocal.m[3][1] += m_TrgtHight;

	// Entity位置だけシフトしたものがワールド座標でのカメラ座標変換行列
	MATRIX MCamTransMatWorld = MCamTransMatLocal;
	MCamTransMatWorld.m[3][0] += m_TargetPos.x;
	//MCamTransMatWorld.m[3][1] += m_TrgtHight; <- MCamTransMatLocal の段階でシフト
	MCamTransMatWorld.m[3][2] += m_TargetPos.z;

	// 逆行列がビュー行列
	m_MViewLocal = MInverse( MCamTransMatLocal );
	m_MViewWorld = MInverse( MCamTransMatWorld );
	
	// 背景パノラマ球の描画に使用しているため m_vFinalCamPos を参照している
	m_vFinalCamPos = VTransform( Vector3D( 0,0,0 ).toVECTOR(), MCamTransMatWorld );
};

void CameraWorkManager::Update_TrackingMovingTarget( double timeslice )
{
	// （地面に射影した時の）カメラと被写体の距離が
	//  DstCamToTrgt 以下になるようにカメラを被写体に引き寄せる
	if( (m_TargetPos-m_vFinalCamPos).toVector2D().sqlen() > m_dSqDstCamToTrgtDef )
	{
		Vector2D CamPos2D = (m_vFinalCamPos-m_TargetPos).toVector2D().normalize() * m_dDstCamToTrgtDef;
		CamPos2D += m_TargetPos.toVector2D();
		m_vFinalCamPos = CamPos2D.toVector3D();
	}

	// m_vFinalCamPos の高さを調整
	m_vFinalCamPos.y = m_TargetPos.y+m_dCamHight;

	// カメラ位置をモデルの高さに合せて調整
	m_vFinalTargetPos = m_TargetPos;
	m_vFinalTargetPos.y += m_TrgtHight;

};

void CameraWorkManager::Update_SetSavedViewMatrix()
{
	m_MViewWorld = m_MSaveViewWorld;
};

void CameraWorkManager::setCamera()
{
	/*
	// DXlibでカメラの位置を設定
	SetCameraPositionAndTarget_UpVecY( m_vFinalCamPos.toVECTOR(), m_vFinalTargetPos.toVECTOR() );
	*/

	// ビュー行列設定
	SetCameraViewMatrix( m_MViewWorld );
};

std::string CameraWorkManager::getCurCamModeName()
{
	std::string str;
	switch (m_CurCamMode)
	{
	case RotateCamOnGazePoint:
		str = "RotateCamOnGazePoint";
		break;
	case TrackingMovingTarget:
		str = "TrackingMovingTarget";
		break;
	case SetSavedViewMatrix:
		str = "SetSavedViewMatrix";
		break;

	default:
		break;
	}
	return str; // これで動くのか...？
};


// カメラのビュー行列を保存（外部ファイルへの書出し＆変数に保持）
void CameraWorkManager::saveViewMatrix( MATRIX mViewMat )
{
	m_MSaveViewWorld = mViewMat;

	std::ofstream ofs( "CameraWorkManager-SaveViewMatrix.txt" );

	for( int i=0; i<4; i++)
	{
		for( int j=0; j<4; j++ )
		{
			ofs << m_MSaveViewWorld.m[i][j] << std::endl;
		}
	}

};

// カメラのビュー行列を外部ファイルから変数に読み込む
void CameraWorkManager::loadViewMatrix()
{
	//ファイルの読み込み
	ifstream ifs("CameraWorkManager-SaveViewMatrix.txt");
	if(!ifs)
	{
		// ファイル読込に失敗（あるいはファイルそのものがなければ）スルー
		return ;
	}

	for( int i=0; i<4; i++)
	{
		for( int j=0; j<4; j++ )
		{
			//1行ずつ読み込む
			string str;
			getline(ifs,str);

			// 数値(float)に変換
			float tmp=stof(str); 
			m_MSaveViewWorld.m[i][j] = tmp;

		}
	}

};

