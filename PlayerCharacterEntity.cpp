#include "PlayerCharacterEntity.h"
#include "State.h"
#include "AnimationManager.h"
#include "VirtualController.h"
#include "CameraWorkManager.h"
#include "SolidObjects.h"
#include "MyUtilities.h" // �f�o�b�N�`��̂��߂̃c�[���g�p

#include <cassert>

// ## �ÓI�����o�̒�`
//const double PlayerCharacterEntity::m_dConfigScaling = 9.0; // SuperMario64HD�̃R���t�B�O���ڐA����Ƃ��̃X�P�[�����O�ϐ�
// 2018/01/07 �`���[�j���O�B
const double PlayerCharacterEntity::m_dConfigScaling = 12.0; // SuperMario64HD�̃R���t�B�O���ڐA����Ƃ��̃X�P�[�����O�ϐ�

// �����Œ�`���āA�ʂ̃\�[�X�t�@�C������ǂݍ��߂�񂾂낤���H

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
	//m_vSide = VCross( m_vHeading.toVECTOR(), m_vUpper.toVECTOR() );
	m_vSide = m_vHeading % m_vUpper ;

	//assert(m_vVelocity.y);
	assert(m_vVelocity.len() < 1000.0);

};

void PlayerCharacterEntity::Render()
{
	m_pCurrentState->Render(this);
	m_pAnimMgr->Play(this);
};

Vector3D PlayerCharacterEntity::MoveInput()
{

#define SAVE_ANGLE_VARIATION // �X�e�B�b�N�X������i�s�����̌v�Z�����̕ύX

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

	static const Vector3D vPosOrign      = Vector3D( 0.0,  0.0, 0.0 );
	static const Vector3D vDirStickUpper = Vector3D( 0.0, -1.0, 0.0 ); // �X�e�B�b�N���W�H�ɂ����������By�����������W�n�ł��邱�Ƃɒ��ӁB
	static const Vector3D vBaseY         = Vector3D( 0.0,  1.0, 0.0 );

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

	// �傫���̓I���W�i���̃X�e�B�b�N�̌X�����g�p
	vStickTiltFromCam = m_pVirCntrl->m_vStickL.toVector3D().len() * vSteeringForceDir;

#endif

// �X�e�B�b�N�̏�����̂݃L�����N�^���ʂ֓��e���������ƍ����āA
// �X�e�B�b�N�̌X�����L�����N�^���ʂ֍��̕ϊ����Đi�s���������肷�����
#ifdef SAVE_ANGLE_VARIATION

	// �X�N���[�����Entity�ʒu���v�Z
	Vector3D EntiPosForScreen = ConvWorldPosToScreenPos(vPosOrign.toVECTOR());
	assert(EntiPosForScreen.z >= 0.0 && EntiPosForScreen.z <= 1.0);

	// ## ��ʏ�������L�����N�^���ʂɓ��e�����������v�Z����

	// ��ʒ����ʒu���L�����N�^���ʂɓ��e�������W���v�Z����
	Vector3D vScreenCntPrj = convScreenPosToXZPlane(EntiPosForScreen);

	// ��ʏ�̏�����x�N�g�����L�����N�^���ʂɓ��e�������W���v�Z����
	Vector3D vScreenUprPrj = convScreenPosToXZPlane(EntiPosForScreen+ vDirStickUpper);

	Vector3D vBaseZ = (vScreenUprPrj - vScreenCntPrj).normalize();
	Vector3D vBaseX = vBaseY % vBaseZ ;

	// ## �X�e�B�b�N�̏�����̃L�����N�^���ʏ�̃x�N�g���ƁA
	// ## �L�����N�^���ʂɑ΂��������x�N�g��������W�ϊ��s��i���̕ϊ��j���v�Z
	MATRIX TransMat = MGetAxis1( 
			vBaseX.toVECTOR(),
			vBaseY.toVECTOR(),
			vBaseZ.toVECTOR(),
			vPosOrign.toVECTOR() );

	// ## ���W�ϊ��s��ŃX�e�B�b�N�̌X��������ϊ����A�L�����N�^�i�s�������v�Z����
	Vector3D vSteeringForceDir;
	if( m_pVirCntrl->m_dStickL_len > 0 )
	{
		vSteeringForceDir = VTransform( 
			m_pVirCntrl->m_vStickL.normalize().toVector3D().toVECTOR(), 
			TransMat );
		vStickTiltFromCam = m_pVirCntrl->m_vStickL.toVector3D().len() * vSteeringForceDir;
	}
	else
	{
		// �X�e�B�b�N�̌X����0�̏ꍇ�̗�O����
		vStickTiltFromCam = vPosOrign;
	}

#endif

	// �J�����̃r���[�s������ɖ߂�
	SetCameraViewMatrix( MSaveViewMat );

	// �v�Z���̃Q�[���X�e�b�v����ۑ�
	LastGameStep = m_lGameStepCounter;

	// �f�o�b�N�p�ɋL��
	DBG_m_vStickPos = 10 * vStickTiltFromCam + Pos();

	return vStickTiltFromCam;

};

// 2017/04/30 �����������ύX
// y����"��"���W�n��y����"��"���W�n�iWindows���W�j�ւ̕ϊ��������폜�B
// i.e. �͂��߂���y����"��"���W�n�iWindows���W�j�ł���O��ł̌v�Z�B
// �͂��߂���3D�x�N�g����n���悤�ɂ���B
Vector3D PlayerCharacterEntity::convScreenPosToXZPlane(Vector3D vScreenPos3D)
{

	vScreenPos3D.z = 0.0;
	Vector3D BgnPos = ConvScreenPosToWorldPos( vScreenPos3D.toVECTOR() );
	vScreenPos3D.z = 1.0;
	Vector3D EndPos = ConvScreenPosToWorldPos( vScreenPos3D.toVECTOR() );
	Vector3D vCrossPos;
	int rtn = calcCrossPointWithXZPlane( BgnPos, EndPos, vCrossPos ); // ����́A�����ɂ̓L�����N�^�����ʂł͂Ȃ��̂ŁA�W�����v����ƕs���m�B�C�����K�v��
	vCrossPos.y = 0; // ���ՂȑΏ��@

	return vCrossPos;
}

void PlayerCharacterEntity::DBG_renderMovementDirFromStick()
{
	// �f�o�b�N�p
	// �iEntity���ʏ�ɓ��e�����j�X�e�B�b�N�̌X���̈ʒu��`��
	static PlaneRing RingIns( 
		0.6, 0.4, 16 );
	RingIns.setCenterPos( DBG_m_vStickPos );
	RingIns.Render(); 

};

// �A�i���O�X�e�B�b�N�̌X����Entity�̌�����`��
void PlayerCharacterEntity::DBG_renderStickTiltAndHeading( Vector2D RenderPos )
{
	static double RenderRadius = 25.0;
	static VirtualController::RenderStickTrajectory StickTrj(Vector2D(0.0,0.0), RenderRadius );

	// �̓x�N�g�������̂܂ܕ`�悷��ƃX�P�[�����傫������̒����p�����[�^
	static double ForceScale = 0.04;

	if( m_pCurrentState == OneEightyDegreeTurn::Instance() )
	{ // �ؕԂ�����̏ꍇ

		// �ؕԂ��̕�����`��
		Vector2D TurnDir = RenderRadius * DBG_m_vTurnDestination.normalize().toVector2D().reversY();
		DrawLine2D( RenderPos.toPoint(), (RenderPos+TurnDir).toPoint(), ColorPalette::Yellow, 3 );

		// Entity�̌�����`��
		Vector2D VelDir = RenderRadius * DBG_m_vVelocitySave.normalize().toVector2D().reversY();
		DrawLine2D( RenderPos.toPoint(), (RenderPos+VelDir).toPoint(), ColorPalette::Cyan );
	}
	else if(  m_pCurrentState == SurfaceMove::Instance() )
	{ // ��{�ړ�����̏ꍇ
	
		// �ړ�����ɂ����镨���p�����[�^���f�o�b�O�\��
		Vector2D vTmp;

		if( DBG_m_bTurnWithouReduceSpeed )
		{ // �@ ���x�𗎂Ƃ����ɐ���
		
			// �I���W�i���̓����̓x�N�g����΁E�א��ŕ`��
			vTmp = ForceScale * DBG_m_vSteeringForce.toVector2D().reversY();
			DrawArrow2D( RenderPos, (RenderPos+vTmp), ColorPalette::Green, FALSE );

			// ���S�͂�΁E�����ŕ`��B����l�ł���ΐԐF�ɂ���B
			vTmp = ForceScale * DBG_m_vCentripetalForce.toVector2D().reversY();
			if(DBG_m_bCentripetalForceIsBounded)
			{
				DrawArrow2D( RenderPos, (RenderPos+vTmp), ColorPalette::Red, TRUE, 3 );
			}
			else
			{
				DrawArrow2D( RenderPos, (RenderPos+vTmp), ColorPalette::Green, TRUE, 3 );
			}

			// ���x�����ւ̐��i�͂�΁E�����ŕ`��
			vTmp = ForceScale * DBG_m_vDriveForceForVel.toVector2D().reversY();
			DrawArrow2D( RenderPos, (RenderPos+vTmp), ColorPalette::Green, TRUE, 3 );

		}
		else
		{ // �A �X�e�B�b�N�X�����͂���������
		
			// �I���W�i���̓����̓x�N�g����΁E�����ŕ`��
			vTmp = ForceScale * DBG_m_vSteeringForce.toVector2D().reversY();
			DrawArrow2D( RenderPos, (RenderPos+vTmp), ColorPalette::Green, TRUE, 3 );
		}

		// Entity�̑��x�x�N�g����E�א��ŕ\��
		vTmp = DBG_m_vVelocitySave.toVector2D().reversY();
		DrawArrow2D( RenderPos, (RenderPos+vTmp), ColorPalette::Blue, FALSE );
	}

	// �L�����N�^������`��
	Vector2D vTmp = RenderRadius * Heading().toVector2D().reversY();
	DrawArrow2D(RenderPos, (RenderPos + vTmp), ColorPalette::Blue, FALSE);

	// �A�i���O�X�e�B�b�N�̏�Ԃ�`��
	Vector3D StickPos = MoveInput();
	StickTrj.Render( StickPos.toVector2D(), RenderPos );

	// ���͐VPC�Ɉڍs���悤���B
	// GitHub�ɕύX���グ��

};

// �ޔ������Ă������������̍X�V
void PlayerCharacterEntity::DBG_UpdateSavePhys()
{
	DBG_m_vVelocitySave = m_vVelocity;
};

// ����State�����擾
string PlayerCharacterEntity::DBG_getCurrentStateName()
{ 
	return m_pCurrentState->getStateName(); 
};

// �ؕԂ�����iOneEightyDegreeTurn�j�ɂ��Ċe�T�u��Ԃ̌p�����Ԃ��o�͂���
void PlayerCharacterEntity::DBG_exp_OneEightyDegreeTurn_SubStateDurations( int &c )
{
	OneEightyDegreeTurn::Instance()->DBG_expSubStateDurations( c );
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
	pAnimUnq->m_fExAnimStartTime  = 25.0f; // ���ł���i�������n�ʂ��痣��Ă���j��Ԃ���J�n�B�ؕԂ��̃��[�V��������̐ڑ��Ŏg�p�B

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
	pAnimUnq->m_CurAttachedMotion   = 8;
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 0.0f; 
	pAnimUnq->m_fAnimEndTime        = 12.0f; 

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
	
	// --------- TurnFirst --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[TurnFirst];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "TurnFirst"; 
	pAnimUnq->m_CurAttachedMotion  = 8;
	pAnimUnq->m_bRepeatAnimation   = false;
	pAnimUnq->m_bCutPartAnimation  = true;
	pAnimUnq->m_fAnimStartTime     = 12.0f; 
	pAnimUnq->m_fAnimEndTime       = 19.0f; 

	// --------- TurnLatter --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[TurnLatter];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "TurnLatter";
	pAnimUnq->m_CurAttachedMotion  = 8;
	pAnimUnq->m_bRepeatAnimation   = false;
	pAnimUnq->m_bCutPartAnimation  = true;
	pAnimUnq->m_fAnimStartTime     = 19.0f;
	pAnimUnq->m_fAnimEndTime       = 28.0f;

	// --------- TurnFull --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[TurnFull];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "TurnFull";
	pAnimUnq->m_CurAttachedMotion  = 8;
	pAnimUnq->m_bRepeatAnimation   = false;
	pAnimUnq->m_bCutPartAnimation  = true;
	pAnimUnq->m_fAnimStartTime     = 12.0f;
	pAnimUnq->m_fAnimEndTime       = 28.0f;

	// --------- TurnFixHead --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[TurnFixHead];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "TurnFixHead";
	pAnimUnq->m_CurAttachedMotion = 9;
	pAnimUnq->m_bRepeatAnimation = false;
	pAnimUnq->m_bCutPartAnimation = true;
	pAnimUnq->m_fAnimStartTime = 0.0f;
	pAnimUnq->m_fAnimEndTime = 16.0f;

	// --------- TurnFinalFly --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[TurnFinalFly];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "TurnFinalFly";
	pAnimUnq->m_vPosShift = Vector3D( 0.0, 0.0, 4.5 );
	pAnimUnq->m_CurAttachedMotion   = 0; // ����̃��[�V��������؂�o���Ă���
	pAnimUnq->m_bRepeatAnimation    = false;
	pAnimUnq->m_bCutPartAnimation   = true;
	pAnimUnq->m_fAnimStartTime      = 20.0f;
	pAnimUnq->m_fAnimEndTime        = 25.0f; 

	// --------- BreaktoTurn --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[BreaktoTurn];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "BreaktoTurn";
	pAnimUnq->m_CurAttachedMotion = 10;
	pAnimUnq->m_bRepeatAnimation = false;
	pAnimUnq->m_bCutPartAnimation = true;
	//pAnimUnq->m_fAnimStartTime = 1.0f; // 3.0�܂Ńu�����h��ԁi�͂��߂̂Q�t���[���j
	pAnimUnq->m_fAnimStartTime = 3.0f;
	pAnimUnq->m_fAnimEndTime = 7.0f;
	pAnimUnq->m_fAnimInterval = 4.0;

	// --------- Turn --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Turn];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Turn";
	pAnimUnq->m_CurAttachedMotion = 11;
	pAnimUnq->m_bRepeatAnimation = false;
	pAnimUnq->m_bCutPartAnimation = true;
	pAnimUnq->m_fAnimStartTime = 5.0f;
	pAnimUnq->m_fAnimEndTime = 10.0f;

	// --------- BreaktoStop --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[BreaktoStop];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "BreaktoStop";
	pAnimUnq->m_CurAttachedMotion = 12;
	pAnimUnq->m_bRepeatAnimation = false;
	pAnimUnq->m_bCutPartAnimation = true;
	//pAnimUnq->m_fAnimStartTime = 2.0f;  // 4.0�܂Ńu�����h��ԁi�͂��߂̂Q�t���[���j
	pAnimUnq->m_fAnimStartTime = 4.0f; 
	pAnimUnq->m_fAnimEndTime = 8.0f;
	pAnimUnq->m_fAnimInterval = 4.0;

	// --------- StandAfterBreak --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[StandAfterBreak];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "StandAfterBreak";
	pAnimUnq->m_CurAttachedMotion = 12;
	pAnimUnq->m_bRepeatAnimation = false;
	pAnimUnq->m_bCutPartAnimation = true;
	pAnimUnq->m_fAnimStartTime = 15.0f;
	pAnimUnq->m_fAnimEndTime = 23.0f;

	// --------- Standing2 --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Standing2];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Standing2";
	pAnimUnq->m_CurAttachedMotion = 12;
	pAnimUnq->m_bRepeatAnimation = false;
	pAnimUnq->m_bCutPartAnimation = true;
	pAnimUnq->m_fAnimStartTime = 23.0f;
	pAnimUnq->m_fAnimEndTime = 23.0f;

	// --------- Running2 --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[Running2];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "Running2";
	pAnimUnq->m_CurAttachedMotion = 13;
	pAnimUnq->m_bRepeatAnimation = true;
	pAnimUnq->m_bCutPartAnimation = false; 
	pAnimUnq->m_fAnimStartTime = 5.0f; // �Đ��J�n�̋N�_�A�܂��������[�V�����Ɠ������Ƃ邽�߂̓��ٓ_�I�ȏꏊ�B
	// pAnimUnq->m_fAnimEndTime = 14.0f; // m_bCutPartAnimation=false�A�j���[�V�����ɂƂ��āAm_fAnimEndTime�͎Q�Ƃ���Ȃ��B
	pAnimUnq->m_fUniquePlayPitch = (float)(31.11 / 79.80); // Running �̃A�j���[�V�����Ɠ��������邽�߁A�ŗL�̍Đ��s�b�`���`
	pAnimUnq->m_fAnimInterval = 14.0;
	pAnimUnq->m_fExAnimStartTime = 1.0f; // [Turn]���[�V��������̐ڑ��Ŏg�p�B

	// 2018/05/05

	// --------- BreaktoStop2 --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[BreaktoStop2];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "BreaktoStop2";
	pAnimUnq->m_CurAttachedMotion = 14;
	pAnimUnq->m_bRepeatAnimation = false;
	pAnimUnq->m_bCutPartAnimation = true;
	//pAnimUnq->m_fAnimStartTime = 2.0f;  // 4.0�܂Ńu�����h��ԁi�͂��߂̂Q�t���[���j
	pAnimUnq->m_fAnimStartTime = 15.0f;
	pAnimUnq->m_fAnimEndTime = 23.0f;
	pAnimUnq->m_fAnimInterval = 8.0;

	// --------- StandAfterBreak2 --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[StandAfterBreak2];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "StandAfterBreak2";
	pAnimUnq->m_CurAttachedMotion = 14;
	pAnimUnq->m_bRepeatAnimation = false;
	pAnimUnq->m_bCutPartAnimation = true;
	pAnimUnq->m_fAnimStartTime = 23.0f;
	pAnimUnq->m_fAnimEndTime = 29.0f;
	pAnimUnq->m_fUniquePlayPitch = 2.0f; // �ŗL�s�b�`�Q�{

	// --------- StartRunning --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[StartRunning];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "StartRunning";
	pAnimUnq->m_CurAttachedMotion = 14;
	pAnimUnq->m_bRepeatAnimation = false;
	pAnimUnq->m_bCutPartAnimation = true;
	pAnimUnq->m_fAnimStartTime = 0.0f;
	pAnimUnq->m_fAnimEndTime = 11.0f;
	pAnimUnq->m_fUniquePlayPitch = 1.5f; // �ŗL�s�b�`�Q�{

	// --------- StartWalking --------- 
	pAnimUnq = &m_pAnimUniqueInfoContainer[StartWalking];
	pAnimUnq->init();
	pAnimUnq->m_sAnimName = "StartWalking";
	pAnimUnq->m_CurAttachedMotion = 15;
	pAnimUnq->m_bRepeatAnimation = false;
	pAnimUnq->m_bCutPartAnimation = true;
	pAnimUnq->m_fAnimStartTime = 0.0f;
	pAnimUnq->m_fAnimEndTime = 6.0f;
};