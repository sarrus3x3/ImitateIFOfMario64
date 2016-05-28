#include "PlayerCharacterEntity.h"
#include "State.h"
#include "AnimationManager.h"
#include "VirtualController.h"

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
			m_dMass(dMass),
			m_dBoundingRadius(dBoundingRadius),
			m_pCurrentState(NULL),
			m_pVirCntrl(pVirCntrl),
			m_bJmpChrgFlg(false),
			m_dStopWatchCounter(0.0),
			m_bTouchGroundFlg(true),
			m_bJmpChrgUsageFlg(false)
{
	// AnimationManager のインスタンスを生成
	m_pAnimMgr = new AnimationManager();

	// 初期ステートを設定
	ChangeState( Running::Instance() );
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
	m_dTimeElapsed = time_elapsed;
	m_dStopWatchCounter += m_dTimeElapsed;

	// 接地判定
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
	
	// 運動方程式に従い、速度・位置を更新
	SteeringForce = PhyVar.Force;
	Vector3D acceleration = SteeringForce / m_dMass;

	// 速度を更新
	m_vVelocity += time_elapsed * acceleration ;

	// 旋回運動による速度ベクトルの回転
	if (m_vVelocity.toVector2D().sqlen() > 0.00000001)
	{
		m_vVelocity += time_elapsed * m_vVelocity.len() * PhyVar.DstVar;
	}

	// 位置を更新
	m_vPos += m_vVelocity * time_elapsed;

	// Entityの向きを速度方向から更新
	if (m_vVelocity.toVector2D().sqlen() > 0.00000001)
	{
		Vector2D Head2D = m_vVelocity.toVector2D().normalize();
		Vector2D Side2D = Head2D.side();
		m_vHeading = Head2D.toVector3D();
		m_vSide    = Side2D.toVector3D();
	}
	else
	{
		// 旋回運動による速度ベクトルの回転
		m_vHeading += time_elapsed * PhyVar.DstVar;
		m_vHeading = m_vHeading.normalize(); // 再規格化
		m_vSide = m_vHeading.toVector2D().side().toVector3D();
	}


};

void PlayerCharacterEntity::Render()
{
	m_pCurrentState->Render(this);
	
	m_pAnimMgr->Play(this);
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

	// --------- Standing --------- 
	// Jumpingのモーションの立ち絵を切り出して利用
	pAnimUnq = &m_pAnimUniqueInfoContainer[Standing];
	pAnimUnq->init();
	pAnimUnq->m_CurAttachedMotion = 1; 
	pAnimUnq->m_bRepeatAnimation  = false;
	pAnimUnq->m_bCutPartAnimation = true;
	pAnimUnq->m_fAnimStartTime    = 0.0f;
	pAnimUnq->m_fAnimEndTime      = 0.0f;

	// --------- Walking --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Walking];
	pAnimUnq->init();
	pAnimUnq->m_CurAttachedMotion = 2; 
	pAnimUnq->m_vPosShift = Vector3D( 0.0, 0.0, -4.75 );
	pAnimUnq->m_fUniquePlayPitch = (float)(14.0/12.0); // Running のアニメーションと同期させるため、固有の再生ピッチを定義
	pAnimUnq->m_fAnimInterval = 14.0;

	// --------- Running --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Running];
	pAnimUnq->init();
	pAnimUnq->m_CurAttachedMotion = 0; 
	pAnimUnq->m_vPosShift = Vector3D( 0.0, 0.0, -4.5 );
	pAnimUnq->m_fAnimStartTime    = 20.0f;
	pAnimUnq->m_fAnimInterval = 12.0;

	// --------- Jumping --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jumping];
	pAnimUnq->init();
	pAnimUnq->m_CurAttachedMotion = 1;
	pAnimUnq->m_bRepeatAnimation  = false;

	// --------- Jump_PreMotion --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jump_PreMotion];
	pAnimUnq->init();
	pAnimUnq->m_CurAttachedMotion = 1;
	pAnimUnq->m_bRepeatAnimation  = false;
	pAnimUnq->m_bCutPartAnimation = true;
	pAnimUnq->m_fAnimStartTime    = 0.0f;
	pAnimUnq->m_fAnimEndTime      = 5.75f; // ※１：跳び上がった後、センター位置が立ち状態の位置になるタイミング

	// --------- Jump_Ascent --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jump_Ascent];
	pAnimUnq->init();
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
	pAnimUnq->m_CurAttachedMotion   = 1;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 31.35f; // ※２
	pAnimUnq->m_fAnimEndTime        = 33.35f;  // 重心が一番低いタイミング＋少し長めに取ってみる

	// --------- DBG_HairUp --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[DBG_HairUp];
	pAnimUnq->init();
	pAnimUnq->m_CurAttachedMotion = 5; 


};