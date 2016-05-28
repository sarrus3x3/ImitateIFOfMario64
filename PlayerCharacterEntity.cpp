#include "PlayerCharacterEntity.h"
#include "State.h"
#include "AnimationManager.h"
#include "VirtualController.h"

// #### �R���X�g���N�^ ####
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
	// AnimationManager �̃C���X�^���X�𐶐�
	m_pAnimMgr = new AnimationManager();

	// �����X�e�[�g��ݒ�
	ChangeState( Running::Instance() );
};

void PlayerCharacterEntity::ChangeState( State *pNewState )
{
	if(m_pCurrentState!=NULL) m_pCurrentState->Exit(this);

	m_pPreviousState = m_pCurrentState; // �O�̃X�e�[�g���L��
	
	m_pCurrentState = pNewState;

	m_pCurrentState->Enter(this);

};

void PlayerCharacterEntity::Update(double time_elapsed)
{
	m_dTimeElapsed = time_elapsed;
	m_dStopWatchCounter += m_dTimeElapsed;

	// �ڒn����
	// �ʒu���n���ʈȉ��@���� ���x�������� �̏ꍇ
	if( !m_bTouchGroundFlg && m_vPos.y < 0 && m_vVelocity.y < 0 )
	{
		// y�������x�� 0 �ɂ���B
		m_vVelocity.y = 0;

		// y�����ʒu�� 0 �ɂ���
		m_vPos.y = 0;

		// Heading��y�������� 0 �ɂ���
		m_vHeading.y = 0;
		m_vHeading = m_vHeading.normalize();

		// �ڒnflg��ON�ɂ���
		m_bTouchGroundFlg = true;
	}

	Vector3D SteeringForce;

	// State �̍X�V����y�тɎ��s
	m_pCurrentState->StateTranceDetect(this);

	// State �� Execute ���\�b�h��Entity�̓����͂��v�Z
	PhysicalQuantityVariation PhyVar;
	m_pCurrentState->Calculate(this, PhyVar);
	
	// �^���������ɏ]���A���x�E�ʒu���X�V
	SteeringForce = PhyVar.Force;
	Vector3D acceleration = SteeringForce / m_dMass;

	// ���x���X�V
	m_vVelocity += time_elapsed * acceleration ;

	// ����^���ɂ�鑬�x�x�N�g���̉�]
	if (m_vVelocity.toVector2D().sqlen() > 0.00000001)
	{
		m_vVelocity += time_elapsed * m_vVelocity.len() * PhyVar.DstVar;
	}

	// �ʒu���X�V
	m_vPos += m_vVelocity * time_elapsed;

	// Entity�̌����𑬓x��������X�V
	if (m_vVelocity.toVector2D().sqlen() > 0.00000001)
	{
		Vector2D Head2D = m_vVelocity.toVector2D().normalize();
		Vector2D Side2D = Head2D.side();
		m_vHeading = Head2D.toVector3D();
		m_vSide    = Side2D.toVector3D();
	}
	else
	{
		// ����^���ɂ�鑬�x�x�N�g���̉�]
		m_vHeading += time_elapsed * PhyVar.DstVar;
		m_vHeading = m_vHeading.normalize(); // �ċK�i��
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
	// m_pAnimUniqueInfoContainer �̐���
	m_pAnimUniqueInfoContainer = new AnimUniqueInfo[m_iAnimIDMax];

	// �S�ẴA�j���[�V�����ɂ��āAAnimUniqueInfo ���`���Ă���
	AnimUniqueInfo *pAnimUnq;

	// --------- NoAnim --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[NoAnim];
	pAnimUnq->init();

	// --------- Standing --------- 
	// Jumping�̃��[�V�����̗����G��؂�o���ė��p
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
	pAnimUnq->m_fUniquePlayPitch = (float)(14.0/12.0); // Running �̃A�j���[�V�����Ɠ��������邽�߁A�ŗL�̍Đ��s�b�`���`
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
	pAnimUnq->m_fAnimEndTime      = 5.75f; // ���P�F���яオ������A�Z���^�[�ʒu��������Ԃ̈ʒu�ɂȂ�^�C�~���O

	// --------- Jump_Ascent --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jump_Ascent];
	pAnimUnq->init();
	pAnimUnq->m_CurAttachedMotion   = 1;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 5.75f; // ���P
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
	pAnimUnq->m_fAnimEndTime        = 31.35f; // ���Q�F���S�ɒ��n����O�A�Z���^�[�ʒu��������Ԃ̈ʒu�ɂȂ�^�C�~���O
	pAnimUnq->m_bCorrectionToCenter = true;
	pAnimUnq->m_vFixCenterPosLocal.set( 0.0, 8.0, 0.0 );

	// --------- Jump_Landing --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jump_Landing];
	pAnimUnq->init();
	pAnimUnq->m_CurAttachedMotion   = 1;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 31.35f; // ���Q
	pAnimUnq->m_fAnimEndTime        = 40.0f;
	
	// --------- Jump_Landing_Short --------- 
	// �W�����v��̒��n�i�Z���^�[�ʒu�������Ƃ�������Ƃ���܂ŁB
	// �W�����v���n������o���̃A�j���[�V�����Ŏg�p���邽�߁j
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jump_Landing_Short];
	pAnimUnq->init();
	pAnimUnq->m_CurAttachedMotion   = 1;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 31.35f; // ���Q
	pAnimUnq->m_fAnimEndTime        = 33.35f;  // �d�S����ԒႢ�^�C�~���O�{�������߂Ɏ���Ă݂�

	// --------- DBG_HairUp --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[DBG_HairUp];
	pAnimUnq->init();
	pAnimUnq->m_CurAttachedMotion = 5; 


};