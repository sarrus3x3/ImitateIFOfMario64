#include "PlayerCharacterEntity.h"
#include "State.h"
#include "AnimationManager.h"
#include "VirtualController.h"
#include "CameraWorkManager.h"
#include "SolidObjects.h"

#include <cassert>

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
	// AnimationManager �̃C���X�^���X�𐶐�
	m_pAnimMgr = new AnimationManager();

	// �����X�e�[�g��ݒ�
	//ChangeState( Running::Instance() );
	ChangeState( SurfaceMove::Instance() );

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
	m_lGameStepCounter++;
	m_dTimeElapsed = time_elapsed;
	m_dStopWatchCounter += m_dTimeElapsed;

	// #### �ڒn����
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
	
	// #### ���x���X�V
	if( PhyVar.UseVelVar )
	{
		m_vVelocity += time_elapsed * PhyVar.VelVar ;
	}
	else
	{
		// �^���������ɏ]���X�V
		SteeringForce = PhyVar.Force;
		Vector3D acceleration = SteeringForce / m_dMass;
		m_vVelocity += time_elapsed * acceleration ;
	}

	// #### �ʒu���X�V
	if( PhyVar.UsePosVar )
	{
		m_vPos += time_elapsed * PhyVar.PosVar ;
	}
	else
	{
		m_vPos += m_vVelocity * time_elapsed;
	}

	// #### Entity�̌������X�V

	if( PhyVar.UseHeading )
	{ // �����𒼐ڐݒ肷��ꍇ
		m_vHeading = PhyVar.Heading;
	}
	else if (m_vVelocity.toVector2D().sqlen() > 0.00000001)
	{ // Entity�̌����𑬓x��������X�V
		Vector2D Head2D = m_vVelocity.toVector2D().normalize(); // �L�����N�^�����ʁ�xz���ʂ̏ꍇ����̎���
		m_vHeading = Head2D.toVector3D();
	}

	// m_vHeading ���Am_vSide ���X�V
	m_vSide = VCross( m_vHeading.toVECTOR(), m_vUpper.toVECTOR() );

};

void PlayerCharacterEntity::Render()
{
	m_pCurrentState->Render(this);
	
	m_pAnimMgr->Play(this);
};

// �X�e�B�b�N�X������i�s�����̌v�Z�����̕ύX
//#define SAVE_ANGLE_VARIATION

Vector3D PlayerCharacterEntity::calcMovementDirFromStick()
{
	static Vector3D vStickTiltFromCam;
	static LONGLONG LastGameStep=0;

	// �ۑ����Ă���LastGameStep�̒l�ƁA���݂̃Q�[���X�e�b�v�����r���A�Čv�Z�v�ۂ𔻒肷��B
	if( LastGameStep == m_lGameStepCounter )
	{
		return vStickTiltFromCam;
	}

	// ##### �X�e�B�b�N�̌X���̕�������AEntity�̈ړ��������v�Z����
	// �E���[���h���W���g�����Ƃɂ�鐸�x�򉻂�h�����߁A�v�Z��Entity�̃��[�J�����W�ōs���悤�ɏC��
	
	// ���݂̃J�����̃r���[�s���ޔ��iEntity�̐i�s�w�����������߂�v�Z�ŁA�r���[�s���Entity�̃��[�J�����W�ł̂��̂ɐݒ肷�邽�߁j
	MATRIX MSaveViewMat = GetCameraViewMatrix();

	// �J�����̃r���[�s���Entity�̃��[�J�����W�ł̂��̂ɐݒ�
	SetCameraViewMatrix( CameraWorkManager::Instance()->m_MViewLocal );

	static const Vector3D vPosOrign = Vector3D( 0,0,0 );


// ��ʏ�̃X�e�B�b�N�̌X�����L�����N�^���ʂɓ��e���A�i�s���������肷�����
#ifndef SAVE_ANGLE_VARIATION

	// �X�N���[�����Entity�ʒu���v�Z
	Vector3D EntiPosForScreen = ConvWorldPosToScreenPos( vPosOrign.toVECTOR() );
	assert( EntiPosForScreen.z >= 0.0 && EntiPosForScreen.z <= 1.0 );

	// �X�N���[����ɃX�e�B�b�N�̌X�������̈ʒu�����߂�
	Vector3D vStickTile = vPosOrign;
	vStickTile.x =  m_pVirCntrl->m_vStickL.x;
	vStickTile.y = -m_pVirCntrl->m_vStickL.y;
	Vector3D StickTiltPosForScreen = EntiPosForScreen + vStickTile;

	// �X�e�B�b�N�̌X�������̈ʒuEntity�̒n���ʂɓ��e���邵���ʒu�����߂�
	StickTiltPosForScreen.z = 0.0;
	Vector3D BgnPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	StickTiltPosForScreen.z = 1.0;
	Vector3D EndPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	Vector3D vCrossPos;
	int rtn = calcCrossPointWithXZPlane( BgnPos, EndPos, vCrossPos ); // ����́A�����ɂ̓L�����N�^�����ʂł͂Ȃ��̂ŁA�W�����v����ƕs���m�B�C�����K�v��
	vCrossPos.y = 0; // ���ՂȑΏ��@

	// ���Ǘ͂̌���
	Vector3D vSteeringForceDir = ( vCrossPos ).normalize();
	if( rtn < 0 )
	{ // �J�����̌����̔��Α��Œn���ʂƌ����ꍇ
		vSteeringForceDir *= -1; // ���Ό����ɐݒ�
	}

#endif

// �X�e�B�b�N�̏�����̂݃L�����N�^���ʂ֓��e���������ƍ����āA
// �X�e�B�b�N�̌X�����L�����N�^���ʂ֍��̕ϊ����Đi�s���������肷�����
#ifdef SAVE_ANGLE_VARIATION
	// 


#endif


	// �J�����̃r���[�s������ɖ߂�
	SetCameraViewMatrix( MSaveViewMat );

	// �傫���̓I���W�i���̃X�e�B�b�N�̌X�����g�p
	vStickTiltFromCam = m_pVirCntrl->m_vStickL.toVector3D().len() * vSteeringForceDir;

	// �v�Z���̃Q�[���X�e�b�v����ۑ�
	LastGameStep = m_lGameStepCounter;

	// �f�o�b�N�p�ɋL��
	DBG_m_vStickPos = 10 * vStickTiltFromCam + Pos();

	return vStickTiltFromCam;

};

void PlayerCharacterEntity::DBG_renderMovementDirFromStick()
{
	// �f�o�b�N�p
	// �iEntity���ʏ�ɓ��e�����j�X�e�B�b�N�̌X���̈ʒu��`��
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
	// m_pAnimUniqueInfoContainer �̐���
	m_pAnimUniqueInfoContainer = new AnimUniqueInfo[m_iAnimIDMax];

	// �S�ẴA�j���[�V�����ɂ��āAAnimUniqueInfo ���`���Ă���
	AnimUniqueInfo *pAnimUnq;

	// --------- NoAnim --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[NoAnim];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "NoAnim"; 


	// --------- Standing --------- 
	// Jumping�̃��[�V�����̗����G��؂�o���ė��p
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
	pAnimUnq->m_fUniquePlayPitch = (float)(14.0/12.0); // Running �̃A�j���[�V�����Ɠ��������邽�߁A�ŗL�̍Đ��s�b�`���`
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
	pAnimUnq->m_fAnimEndTime      = 5.75f; // ���P�F���яオ������A�Z���^�[�ʒu��������Ԃ̈ʒu�ɂȂ�^�C�~���O

	// --------- Jump_Ascent --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Jump_Ascent];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Jump_Ascent"; 
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
	pAnimUnq->m_sAnimName = "Jump_Descent"; 
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
	pAnimUnq->m_sAnimName = "Jump_Landing"; 
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
	pAnimUnq->m_sAnimName = "Jump_Landing_Short"; 
	pAnimUnq->m_CurAttachedMotion   = 1;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 31.35f; // ���Q
	pAnimUnq->m_fAnimEndTime        = 33.35f;  // �d�S����ԒႢ�^�C�~���O�{�������߂Ɏ���Ă݂�

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