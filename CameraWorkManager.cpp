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
	m_TrgtHight(20.0)
{

	m_dSqDstCamToTrgtDef = m_dDstCamToTrgtDef * m_dDstCamToTrgtDef;
	m_vFinalCamPos = Vector3D(0,m_dCamHight,0);
	m_TargetPos    = Vector3D(0,0,0);
	
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
	m_scroll -= (16.0/100) * (double)(GetMouseWheelRotVol());
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
	getKeyInput();

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
	

	// m_dHead と m_dTilt から、カメラの位置を計算する
	Vector2D Hedding2D( 0, 1 );
	Hedding2D = Hedding2D.rot(m_dHead); // 回転
	Vector2D toCamPos2D = ( -m_dCamDistFrmFP * cos(m_dTilt) ) * Hedding2D.normalize();
	Vector3D toCamPos3D( toCamPos2D.x, m_dCamDistFrmFP*sin(m_dTilt), toCamPos2D.y );
	
	// カメラ状態を更新
	m_vFinalCamPos = m_TargetPos.toVector2D().toVector3D() + toCamPos3D;
	m_vFinalTargetPos = m_TargetPos.toVector2D().toVector3D();

	m_vFinalTargetPos.y += m_TrgtHight;
	m_vFinalCamPos.y    += m_TrgtHight;
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

void CameraWorkManager::setCamera()
{
	// DXlibでカメラの位置を設定
	SetCameraPositionAndTarget_UpVecY( m_vFinalCamPos.toVECTOR(), m_vFinalTargetPos.toVECTOR() );
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
	default:
		break;
	}
	return str; // これで動くのか...？
};