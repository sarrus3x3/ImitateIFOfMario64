#include "PlayerCharacterEntity.h"
#include "State.h"
#include "AnimationManager.h"
#include "VirtualController.h"
#include "CameraWorkManager.h"
#include "SolidObjects.h"

#include <cassert>

// #### コンストラクタ ####
PlayerCharacterEntity::PlayerCharacterEntity(
		Vector3D vPos,
		Vector3D vVelocity,
		Vector3D vHeading,
		Vector3D vSide,
		double   dMass,
		double   dBoundingRadius,
		VirtualController* pVirCntrl
		) :
			m_vPos(vPos),
			m_vVelocity(vVelocity),
			m_vHeading(vHeading),
			m_vSide(vSide),
			m_vUpper(Vector3D(0,1,0)),
			m_dMass(dMass),
			m_dBoundingRadius(dBoundingRadius),
			m_pCurrentState(NULL),
			m_pVirCntrl(pVirCntrl),
			m_bJmpChrgFlg(false),
			m_dStopWatchCounter(0.0),
			m_bTouchGroundFlg(true),
			m_bJmpChrgUsageFlg(false),
			m_lGameStepCounter( 1 )
{
	// AnimationManager のインスタンスを生成
	m_pAnimMgr = new AnimationManager();

	// 初期ステートを設定
	//ChangeState( Running::Instance() );
	ChangeState( SurfaceMove::Instance() );

};

void PlayerCharacterEntity::ChangeState( State *pNewState )
{
	if(m_pCurrentState!=NULL) m_pCurrentState->Exit(this);

	m_pPreviousState = m_pCurrentState; // 前のステートを記憶
	
	m_pCurrentState = pNewState;

	m_pCurrentState->Enter(this);

};

void PlayerCharacterEntity::Update(double time_elapsed)
{
	m_lGameStepCounter++;
	m_dTimeElapsed = time_elapsed;
	m_dStopWatchCounter += m_dTimeElapsed;

	// #### 接地判定
	// 位置が地平面以下　かつ 速度が下方向 の場合
	if( !m_bTouchGroundFlg && m_vPos.y < 0 && m_vVelocity.y < 0 )
	{
		// y方向速度を 0 にする。
		m_vVelocity.y = 0;

		// y方向位置を 0 にする
		m_vPos.y = 0;

		// Headingのy軸方向を 0 にする
		m_vHeading.y = 0;
		m_vHeading = m_vHeading.normalize();

		// 接地flgをONにする
		m_bTouchGroundFlg = true;
	}

	Vector3D SteeringForce;

	// State の更新判定及びに実行
	m_pCurrentState->StateTranceDetect(this);

	// State の Execute メソッドでEntityの働く力を計算
	PhysicalQuantityVariation PhyVar;
	m_pCurrentState->Calculate(this, PhyVar);
	
	// #### 速度を更新
	if( PhyVar.UseVelVar )
	{
		m_vVelocity += time_elapsed * PhyVar.VelVar ;
	}
	else
	{
		// 運動方程式に従い更新
		SteeringForce = PhyVar.Force;
		Vector3D acceleration = SteeringForce / m_dMass;
		m_vVelocity += time_elapsed * acceleration ;
	}

	// #### 位置を更新
	if( PhyVar.UsePosVar )
	{
		m_vPos += time_elapsed * PhyVar.PosVar ;
	}
	else
	{
		m_vPos += m_vVelocity * time_elapsed;
	}

	// #### Entityの向きを更新

	if( PhyVar.UseHeading )
	{ // 向きを直接設定する場合
		m_vHeading = PhyVar.Heading;
	}
	else if (m_vVelocity.toVector2D().sqlen() > 0.00000001)
	{ // Entityの向きを速度方向から更新
		Vector2D Head2D = m_vVelocity.toVector2D().normalize(); // キャラクタ水平面＝xz平面の場合限定の実装
		m_vHeading = Head2D.toVector3D();
	}

	// m_vHeading より、m_vSide を更新
	m_vSide = VCross( m_vHeading.toVECTOR(), m_vUpper.toVECTOR() );

};

void PlayerCharacterEntity::Render()
{
	m_pCurrentState->Render(this);
	
	m_pAnimMgr->Play(this);
};

// スティック傾きから進行方向の計算方式の変更
//#define SAVE_ANGLE_VARIATION

Vector3D PlayerCharacterEntity::calcMovementDirFromStick()
{
	static Vector3D vStickTiltFromCam;
	static LONGLONG LastGameStep=0;

	// 保存してあるLastGameStepの値と、現在のゲームステップ数を比較し、再計算要否を判定する。
	if( LastGameStep == m_lGameStepCounter )
	{
		return vStickTiltFromCam;
	}

	// ##### スティックの傾きの方向から、Entityの移動方向を計算する
	// ・ワールド座標を使うことによる精度劣化を防ぐため、計算をEntityのローカル座標で行うように修正
	
	// 現在のカメラのビュー行列を退避（Entityの進行指示方向を求める計算で、ビュー行列をEntityのローカル座標でのものに設定するため）
	MATRIX MSaveViewMat = GetCameraViewMatrix();

	// カメラのビュー行列をEntityのローカル座標でのものに設定
	SetCameraViewMatrix( CameraWorkManager::Instance()->m_MViewLocal );

	static const Vector3D vPosOrign = Vector3D( 0,0,0 );


// 画面上のスティックの傾きをキャラクタ平面に投影し、進行方向を決定する方式
#ifndef SAVE_ANGLE_VARIATION

	// スクリーン上のEntity位置を計算
	Vector3D EntiPosForScreen = ConvWorldPosToScreenPos( vPosOrign.toVECTOR() );
	assert( EntiPosForScreen.z >= 0.0 && EntiPosForScreen.z <= 1.0 );

	// スクリーン上にスティックの傾き方向の位置を求める
	Vector3D vStickTile = vPosOrign;
	vStickTile.x =  m_pVirCntrl->m_vStickL.x;
	vStickTile.y = -m_pVirCntrl->m_vStickL.y;
	Vector3D StickTiltPosForScreen = EntiPosForScreen + vStickTile;

	// スティックの傾き方向の位置Entityの地平面に投影するした位置を求める
	StickTiltPosForScreen.z = 0.0;
	Vector3D BgnPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	StickTiltPosForScreen.z = 1.0;
	Vector3D EndPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	Vector3D vCrossPos;
	int rtn = calcCrossPointWithXZPlane( BgnPos, EndPos, vCrossPos ); // これは、厳密にはキャラクタ水平面ではないので、ジャンプすると不正確。修正が必要★
	vCrossPos.y = 0; // 安易な対処法

	// 操舵力の決定
	Vector3D vSteeringForceDir = ( vCrossPos ).normalize();
	if( rtn < 0 )
	{ // カメラの向きの反対側で地平面と交わる場合
		vSteeringForceDir *= -1; // 反対向きに設定
	}

#endif

// スティックの上方向のみキャラクタ平面へ投影した方向と合せて、
// スティックの傾きをキャラクタ平面へ剛体変換して進行方向を決定する方式
#ifdef SAVE_ANGLE_VARIATION
	// 


#endif


	// カメラのビュー行列を元に戻す
	SetCameraViewMatrix( MSaveViewMat );

	// 大きさはオリジナルのスティックの傾きを使用
	vStickTiltFromCam = m_pVirCntrl->m_vStickL.toVector3D().len() * vSteeringForceDir;

	// 計算時のゲームステップ数を保存
	LastGameStep = m_lGameStepCounter;

	// デバック用に記憶
	DBG_m_vStickPos = 10 * vStickTiltFromCam + Pos();

	return vStickTiltFromCam;

};

void PlayerCharacterEntity::DBG_renderMovementDirFromStick()
{
	// デバック用
	// （Entity平面上に投影した）スティックの傾きの位置を描画
	static PlaneRing RingIns( 
		0.6, 0.4, 16, 
		GetColorU8(255,   0,   0, 0 ),
		GetColorU8(255, 255, 255, 0 ) );
	RingIns.setCenterPos( DBG_m_vStickPos );
	RingIns.Render(); 

};




// ##### AnimUniqueInfoManager #####
PlayerCharacterEntity::AnimUniqueInfoManager* PlayerCharacterEntity::AnimUniqueInfoManager::Instance()
{
	static AnimUniqueInfoManager instance;
	return &instance;
}

PlayerCharacterEntity::AnimUniqueInfoManager::AnimUniqueInfoManager()
{
	// m_pAnimUniqueInfoContainer の生成
	m_pAnimUniqueInfoContainer = new AnimUniqueInfo[m_iAnimIDMax];

	// 全てのアニメーションについて、AnimUniqueInfo を定義していく
	AnimUniqueInfo *pAnimUnq;

	// --------- NoAnim --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[NoAnim];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "NoAnim"; 


	// --------- Standing --------- 
	// Jumpingのモーションの立ち絵を切り出して利用
	pAnimUnq = &m_pAnimUniqueInfoContainer[Standing];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Standing"; 
	pAnimUnq->m_CurAttachedMotion = 1; 
	pAnimUnq->m_bRepeatAnimation  = false;
	pAnimUnq->m_bCutPartAnimation = true;
	pAnimUnq->m_fAnimStartTime    = 0.0f;
	pAnimUnq->m_fAnimEndTime      = 0.0f;

	// --------- Walking --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Walking];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Walking"; 
	pAnimUnq->m_CurAttachedMotion = 2; 
	pAnimUnq->m_vPosShift = Vector3D( 0.0, 0.0, 4.75 );
	pAnimUnq->m_fUniquePlayPitch = (float)(14.0/12.0); // Running のアニメーションと同期させるため、固有の再生ピッチを定義
	pAnimUnq->m_fAnimStartTime   = 4.0f;
	pAnimUnq->m_fAnimInterval    = 14.0;

	// --------- Running --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Running];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Running"; 
	pAnimUnq->m_CurAttachedMotion = 0; 
	pAnimUnq->m_vPosShift = Vector3D( 0.0, 0.0, 4.5 );
	pAnimUnq->m_fAnimStartTime    = 20.0f;
	pAnimUnq->m_fAnimInterval     = 12.0;

	// --------- Jumping --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jumping];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Jumping"; 
	pAnimUnq->m_CurAttachedMotion = 1;
	pAnimUnq->m_bRepeatAnimation  = false;

	// --------- Jump_PreMotion --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jump_PreMotion];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Jump_PreMotion"; 
	pAnimUnq->m_CurAttachedMotion = 1;
	pAnimUnq->m_bRepeatAnimation  = false;
	pAnimUnq->m_bCutPartAnimation = true;
	pAnimUnq->m_fAnimStartTime    = 0.0f;
	pAnimUnq->m_fAnimEndTime      = 5.75f; // ※１：跳び上がった後、センター位置が立ち状態の位置になるタイミング

	// --------- Jump_Ascent --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jump_Ascent];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Jump_Ascent"; 
	pAnimUnq->m_CurAttachedMotion   = 1;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 5.75f; // ※１
	pAnimUnq->m_fAnimEndTime        = 20.0f; 
	pAnimUnq->m_bCorrectionToCenter = true;
	pAnimUnq->m_vFixCenterPosLocal.set( 0.0, 8.0, 0.0 );

	// --------- Jump_Descent --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jump_Descent];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Jump_Descent"; 
	pAnimUnq->m_CurAttachedMotion   = 1;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 20.0f;
	pAnimUnq->m_fAnimEndTime        = 31.35f; // ※２：完全に着地する前、センター位置が立ち状態の位置になるタイミング
	pAnimUnq->m_bCorrectionToCenter = true;
	pAnimUnq->m_vFixCenterPosLocal.set( 0.0, 8.0, 0.0 );

	// --------- Jump_Landing --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jump_Landing];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Jump_Landing"; 
	pAnimUnq->m_CurAttachedMotion   = 1;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 31.35f; // ※２
	pAnimUnq->m_fAnimEndTime        = 40.0f;
	
	// --------- Jump_Landing_Short --------- 
	// ジャンプ後の着地（センター位置がもっとも下がるところまで。
	// ジャンプ着地→走り出しのアニメーションで使用するため）
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jump_Landing_Short];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Jump_Landing_Short"; 
	pAnimUnq->m_CurAttachedMotion   = 1;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 31.35f; // ※２
	pAnimUnq->m_fAnimEndTime        = 33.35f;  // 重心が一番低いタイミング＋少し長めに取ってみる

	// --------- Breaking --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Breaking];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Breaking"; 
	pAnimUnq->m_CurAttachedMotion   = 4;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 0.0f; 
	//pAnimUnq->m_fAnimEndTime        = 10.0f; 
	pAnimUnq->m_fAnimEndTime        = 20.0f; 
	//pAnimUnq->m_fUniquePlayPitch = (float)1.8;

	// --------- Turning --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Turning];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Turning"; 
	pAnimUnq->m_CurAttachedMotion   = 4;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	//pAnimUnq->m_fAnimStartTime      = 10.0f; 
	pAnimUnq->m_fAnimStartTime      = 20.0f; 
	pAnimUnq->m_fAnimEndTime        = 41.0f; 
	//pAnimUnq->m_vPosShift = Vector3D( 0.0, 0.0, 1.0 );

	// --------- BreakAndTurn --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[BreakAndTurn];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "BreakAndTurn"; 
	pAnimUnq->m_CurAttachedMotion   = 4;

	// --------- BreakingAfter --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[BreakingAfter];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "BreakingAfter"; 
	pAnimUnq->m_CurAttachedMotion = 6;
	pAnimUnq->m_bRepeatAnimation  = false;
	pAnimUnq->m_fUniquePlayPitch = (float)2.0;

};