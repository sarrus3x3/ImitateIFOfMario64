#include <cassert>

#include "AnimationManager.h"
#include "SolidObjects.h"

#define DEBUG_SWITCH_PHYSICS_TYPE

int AnimationManager::m_iModelHandleMaster = -1; // m_iModelHandle �̏������B�i�ÓI�����o�ϐ��j

// #### �R���X�g���N�^ ####
AnimationManager::AnimationManager() :
	m_PlayPitch(20.0),
	m_bIsNowBlending( false ),
	m_dBlendElapsed( 0 ),
	m_dAnimSwitchTime( 0 ),
	DBG_m_bPauseOn(false)
{
	// ################## ���f���̓ǂݍ��� #######################

	if( m_iModelHandleMaster == -1 )
	{
		// ���ɓǂݍ��ރ��f���̕������Z���[�h�����A���^�C���������Z�ɂ���
		
		//MV1SetLoadModelUsePhysicsMode( DX_LOADMODEL_PHYSICS_DISABLE ) ;
		MV1SetLoadModelUsePhysicsMode( DX_LOADMODEL_PHYSICS_REALTIME ) ;
		
		// ���f���f�[�^�̓ǂݍ���
		m_iModelHandleMaster = MV1LoadModel( "..\\mmd_model\\�����~�N.pmd" ) ;

		// ����̃C���X�^���X���� m_iModelHandle = m_iModelHandleMaster
		m_iModelHandle = m_iModelHandleMaster;

	}
	else
	{
		// �Q��߈ȍ~�� m_iModelHandle �́Am_iModelHandleMaster �� MV1DuplicateModel �ŕ������Ďg��
		m_iModelHandle = MV1DuplicateModel( m_iModelHandleMaster );
	}

#ifdef DEBUG_SWITCH_PHYSICS_TYPE
	// ######## [DBG] �Q�[���̓r���Ń��f����ύX���� ########

	// �I���W�i���̃��f���̃n���h����ޔ�
	DBG_m_iModelHandle_Original = m_iModelHandle;

	// ���̖э폜���폜�������f���̃n���h��
	DBG_m_iModelHandle_HideHair = MV1LoadModel( "..\\mmd_model\\�����~�N�i���̖э폜�j.pmd" ) ;

	// �������Z����œǂݍ��񂾃��f���̃n���h��
	MV1SetLoadModelUsePhysicsMode( DX_LOADMODEL_PHYSICS_REALTIME );
	DBG_m_iModelHandle_Physics = MV1LoadModel( "..\\mmd_model\\�����~�N.pmd" ) ;

	// ######## 
#endif

	// �u�Z���^�[�v�t���[���̃t���[���ԍ����擾
	m_iCenterFrameIndex = MV1SearchFrame( m_iModelHandle, "�Z���^�[" );
	assert( m_iCenterFrameIndex>=0 && "Fail to MV1SearchFrame." );

	// �u�E���P�v�t���[���̃t���[���ԍ����擾
	DGB_m_iHairFrameIndex = MV1SearchFrame( m_iModelHandle, "�E���P" ); // ����͂�IK�t���[���͑���ł��Ȃ��̂�...
	//DGB_m_iHairFrameIndex = MV1SearchFrame( m_iModelHandle, "�E�Ђ�" );
	assert( DGB_m_iHairFrameIndex>=0 && "Fail to MV1SearchFrame." );

#ifndef DEBUG_SWITCH_PHYSICS_TYPE
	// �֊s�ُ�̕␳
	int MaterialNum, i ;
	MaterialNum = MV1GetMaterialNum( m_iModelHandle ) ;		// �}�e���A���̐����擾
	for( i = 0 ; i < MaterialNum ; i ++ )
	{
		// �}�e���A���̌��X�̗֊s���̑������擾
		//DotWidth = MV1GetMaterialOutLineDotWidth( m_iModelHandle, i ) ;

		// �}�e���A���̗֊s���̑�����10���̂P�ɂ���
		//MV1SetMaterialOutLineDotWidth( m_iModelHandle, i, DotWidth / 5.0f ) ;

		// ### �֊s��������
		MV1SetMaterialOutLineDotWidth( m_iModelHandle, i, 0.0f ) ;
		MV1SetMaterialOutLineWidth( m_iModelHandle, i, 0.0f ) ;

		// ### �}�e���A���̃A���r�G���g�J���[�𖳂��Ɂi�^���Ɂj
		//MV1SetMaterialAmbColor( m_iModelHandle, i, GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ) ;
	
	}
#endif

#ifdef DEBUG_SWITCH_PHYSICS_TYPE
	int ModelHandleList[3] = { 
		DBG_m_iModelHandle_Original, 
		DBG_m_iModelHandle_HideHair, 
		DBG_m_iModelHandle_Physics };

	for( int j=0; j<3; j++ )
	{
		int MaterialNum, i ;
		MaterialNum = MV1GetMaterialNum( ModelHandleList[j] ) ;		// �}�e���A���̐����擾
		for( i = 0 ; i < MaterialNum ; i ++ )
		{
			// ### �֊s��������
			MV1SetMaterialOutLineDotWidth( ModelHandleList[j], i, 0.0f ) ;
			MV1SetMaterialOutLineWidth( ModelHandleList[j], i, 0.0f ) ;
		}
	}
#endif

	// m_pCurAnimPlayInfo �� m_pPrvAnimPlayInfo �̏�����
	m_pCurAnimPlayInfo = new AnimPlayBackInfo;
	m_pPrvAnimPlayInfo = new AnimPlayBackInfo;
	m_pCurAnimPlayInfo->init();
	m_pPrvAnimPlayInfo->init();

	m_pCurAnimPlayInfo->m_bRemoved = true;
	m_pPrvAnimPlayInfo->m_bRemoved = true;

	// �A�j���[�V�����̕������Z�֘A�̏�����
	initAnimPhysics();

};
void AnimationManager::CleanUpAnim( AnimPlayBackInfo* pAnimInfo )
{
	pAnimInfo->m_bRemoved = true;
	MV1DetachAnim( m_iModelHandle, pAnimInfo->m_AttachIndex ); // �Â��A�j���[�V�����̃f�^�b�`�i�f�^�b�`���Ȃ���Animation���������ĕςȂ��ƂɂȂ�j
};

// ������pAnimInfo���w�肳�ꂽ�A�j���[�V�����ŏ���������
void AnimationManager::InitAnimPlayInfoAsAnim( AnimPlayBackInfo* pAnimInfo, PlayerCharacterEntity::AnimationID AnimID )
{
	// �\���̂��ė��p���āA�V���ɐݒ肳�ꂽ�A�j���[�V�����̃R���e�i���쐬����B
	pAnimInfo->init(); // ������
	pAnimInfo->m_eAnimID = AnimID; // AnimID���L��
	 
	// �A�j���[�V�����ŗL���ւ̃|�C���^���擾
	AnimUniqueInfo* pAnimUnq = pAnimInfo->getAnimUnqPointer();

	// �iMotionID�ɏ]���jm_AttachIndex �̐ݒ�
	int CurAttachedMotion = pAnimUnq->m_CurAttachedMotion;
	if( CurAttachedMotion>=0 )
	{
		pAnimInfo->m_AttachIndex = MV1AttachAnim( m_iModelHandle, CurAttachedMotion, -1, FALSE ) ;
	}

	// m_MotionTotalTime ���擾
	if( pAnimInfo->m_AttachIndex==-1 )
	{ // �A�j���[�V�������ݒ�
		pAnimInfo->m_MotionTotalTime = 0.0f;
	}
	else if( !(pAnimUnq->m_bCutPartAnimation) )
	{ // ���[�V�����؏o�� OFF
		pAnimInfo->m_MotionTotalTime = MV1GetAttachAnimTotalTime( m_iModelHandle, pAnimInfo->m_AttachIndex ) ;	
		pAnimInfo->m_CurPlayTime     = pAnimUnq->m_fAnimStartTime;
	}
	else
	{ // ���[�V�����؏o�� ON
		// PlaySub �̎d�g�݂���A����Ő؏o�������ł���͂�
		pAnimInfo->m_CurPlayTime     = pAnimUnq->m_fAnimStartTime;
		pAnimInfo->m_MotionTotalTime = pAnimUnq->m_fAnimEndTime;
	}
};

void AnimationManager::setAnim( PlayerCharacterEntity::AnimationID AnimID, double AnimSwitchTime, bool StopPrvAnim, bool SyncToPrv )
{
	// ���荞�݂ŐV�����A�j���[�V������setAnim���ꂽ�ꍇ�́A�\�񒆂̐ݒ�͔j�������B
	DiscardReservedAnim();

	// setAnim�̏����{�̂��Ă�
	setAnimMain( AnimID, AnimSwitchTime, StopPrvAnim, SyncToPrv );
};

// �u�����h�w�肷��ꍇ�́A�����ɁA�u�����h���ԁi�f�t�H���g0�j�A���݂̃A�j���[�V�����̒�~�v�ہi�f�t�H���g�v�j��ݒ�
// AnimSwitchTime �ȍ~���w�肵�Ȃ��ƃu�����h�Ȃ��Őؑւ���B
void AnimationManager::setAnimMain( 
	PlayerCharacterEntity::AnimationID AnimID,
	double AnimSwitchTime, 
	bool StopPrvAnim, 
	bool SyncToPrv )
{

	// �u�����h���̏ꍇ�̓u�����h��������������
	if( m_bIsNowBlending )
	{ 
		m_pCurAnimPlayInfo->m_fBlendRate = 1.0; // �����I�ɃA�j���[�V������ؑ�
		CleanUpAnim( m_pPrvAnimPlayInfo ); // Prv�̃A�j���[�V�����̌㏈�����s��
		m_bIsNowBlending = false;          // �u�����h�͉������ꂽ
	}
	
	// �u�����h�w��̏ꍇ�iAnimSwitchTime>0�j
	if( AnimSwitchTime > 0 )
	{
		m_bIsNowBlending  = true;
		m_dBlendElapsed   = 0;
		m_dAnimSwitchTime = AnimSwitchTime;

		// �t���O�������Ă���ΑO�̃A�j���[�V�����Đ����~�i�Î~�j����
		m_pCurAnimPlayInfo->m_bPause = StopPrvAnim;

		// ���ݐݒ肳��Ă���A�j���[�V������m_pPrvAnimPlayInfo�Ɉڂ�
		swap( m_pCurAnimPlayInfo, m_pPrvAnimPlayInfo );

	}
	else
	{
		// ���ݐݒ肳��Ă���A�j���[�V������m_pPrvAnimPlayInfo�Ɉڂ��A�㏈�����s��
		swap( m_pCurAnimPlayInfo, m_pPrvAnimPlayInfo );
		CleanUpAnim( m_pPrvAnimPlayInfo );
	}

	// ################## ���[�V�����̐ݒ� #######################
	
	// m_pCurAnimPlayInfo ���Z�b�g���ꂽ�A�j���[�V�����ŏ�����
	InitAnimPlayInfoAsAnim( m_pCurAnimPlayInfo, AnimID );

	if( SyncToPrv )
	{
		// ##### �A�j���[�V�������u�����v������ #####
		// �A�j���[�V������ؑւ��鎞�A�u�ʑ��v�������p�������Ƃ����ꍇ������B
		// �Ⴆ�΁A����������ɐ؂�ւ������A������ԑO�ɓ˂��o���Ă��鎞�ɐؑւ�����A
		// �����̃A�j���[�V�����ł������悤�ɑ�����ԑO�ɓ˂��o������Ԃɂ������B

		float CurInterval = m_pCurAnimPlayInfo->getAnimUnqPointer()->m_fAnimInterval;
		float PrvInterval = m_pPrvAnimPlayInfo->getAnimUnqPointer()->m_fAnimInterval;
		assert( (CurInterval>0) && (PrvInterval>0) && "Unsupport." );
		float scale = CurInterval/PrvInterval;
		m_pCurAnimPlayInfo->m_CurPlayTime = scale * m_pPrvAnimPlayInfo->m_CurPlayTime;

	}

};

void AnimationManager::Play( PlayerCharacterEntity* pEntity )
{
	// ################## �A�j���[�V�����̍Đ� #######################

	// ����̍Đ��^�C�~���O���v�Z����
	double   telaps    = pEntity->TimeElaps();
	Vector3D Modelpos  = pEntity->Pos();
	Vector3D Modelhead = pEntity->Heading();

	PlayMain( telaps, Modelpos, Modelhead );

}
void AnimationManager::PlayMain( double TimeElaps, Vector3D Pos, Vector3D Head )
{
	// �\��A�j���[�V�����̍Đ��ۂ��m�F
	PlayReservedAnim();

	// ################## �A�j���[�V�����̃u�����h���� #######################

	// �A�j���[�V�����u�����h�����H
	if( m_bIsNowBlending )
	{
		// m_dBlendElapsed ���X�V�im_PlayPitch�l �͍l���j
		m_dBlendElapsed += m_PlayPitch * (float)TimeElaps;

		// �����Am_dAnimSwitchTime < m_dBlendElapsed �̏ꍇ�́ACur�̃u�����h�l��1�ɂ��A�u�����h�������I������
		if( m_dAnimSwitchTime < m_dBlendElapsed )
		{
			m_pCurAnimPlayInfo->m_fBlendRate = 1.0;
			CleanUpAnim( m_pPrvAnimPlayInfo ); //Prv�̃A�j���[�V�����̌㏈�����s��
			m_bIsNowBlending = false;
		}
		else
		{
			// m_dAnimSwitchTime �� m_dBlendElapsed ����A�u�����h�����v�Z
			float BRate = (float)(m_dBlendElapsed/m_dAnimSwitchTime);
			m_pCurAnimPlayInfo->m_fBlendRate = BRate;
			m_pPrvAnimPlayInfo->m_fBlendRate = (float)(1.0 - BRate);
		}
	}

	// �Đ��v�ۂ� m_bRemoved �̒������Ĕ��f
	if( !m_pCurAnimPlayInfo->m_bRemoved ) PlayOneAnim( TimeElaps, Pos, Head, m_pCurAnimPlayInfo );
	if( !m_pPrvAnimPlayInfo->m_bRemoved ) PlayOneAnim( TimeElaps, Pos, Head, m_pPrvAnimPlayInfo );
	


	// �����̐ݒ�
	// m_vHeading ��x-z���ʂɓ��e����������y���𒆐S�Ƀ��f������]������
	Vector2D head2D = Head.toVector2D();               // heading��2D�ϊ�
	double headangle = atan2( head2D.x, head2D.y );    // heading�̉�]�p���擾
	Vector3D RotVec( 0, headangle+DX_PI, 0 );
	MV1SetRotationXYZ( m_iModelHandle, RotVec.toVECTOR() ); // ��]�p + shift �� ���f������]������
	

	// ���f���Z���^�ւ̃��[�V�����ʒu�̕␳�x�N�g�����A
	// PlayOneAnim�Ōv�Z���ꂽAnimPlayBackInfo.m_vCorrectionVec����v�Z
	
	// �Z���^�[�ʒu���Œ�ɂȂ�悤�ɁA���f���i�`��j�ʒu��␳����x�N�g�� CorrectionVec ���v�Z����
	
	// Cur���̕␳���擾
	Vector3D CurCorrectVec = m_pCurAnimPlayInfo->m_vCorrectionVec;
	CurCorrectVec += m_pCurAnimPlayInfo->getAnimUnqPointer()->m_vPosShift;
	float    CurBlendRate  = m_pCurAnimPlayInfo->m_fBlendRate;
	
	// Prv���̕␳���擾
	Vector3D PrvCorrectVec = m_pPrvAnimPlayInfo->m_vCorrectionVec;
	PrvCorrectVec += m_pPrvAnimPlayInfo->getAnimUnqPointer()->m_vPosShift;
	float    PrvBlendRate  = m_pPrvAnimPlayInfo->m_fBlendRate;
	if( m_pPrvAnimPlayInfo->m_bRemoved ) PrvBlendRate = 0;
	
	// �u�����h�l�������␳�x�N�g�����v�Z
	Vector3D CorrectionVec = ( CurBlendRate*CurCorrectVec + PrvBlendRate*PrvCorrectVec ) / (CurBlendRate+PrvBlendRate);

	// Entity�̈ʒu�Ƀ��f����z�u
	Vector2D PosShiftRot2D = CorrectionVec.toVector2D().rot( -headangle ); // ���f���̉�]�̓V�t�g�O�̈ʒu�����ɍs���邽�߁A�V�t�g�x�N�g���̉�]���l�����Ȃ��Ƃ����Ȃ�
	Vector3D PosShiftFinally( PosShiftRot2D.x, CorrectionVec.y, PosShiftRot2D.y );
	Vector3D ModPos = Pos + PosShiftFinally; // Motion�̕\���ʒu��Entity�̈ʒu����V�t�g
	MV1SetPosition( m_iModelHandle, ModPos.toVECTOR() );
	
	//MV1SetPosition( m_iModelHandle, Pos.toVECTOR() );
	// �������l�����āA���f���i�`��j�ʒu��ݒ�

	// m_vHeading �̃x�N�g����`��i�⏕�j
	//DrawAllow3D( Pos, Head );
	
	//DrawLine3D( Pos.toVECTOR(), (Pos+10*Head).toVECTOR(), GetColor( 255, 0, 0 ) );

	// �A�j���[�V�����̕������Z���s
	UpdateAnimPhysics( TimeElaps );

	// ���f���̕`��
    MV1DrawModel( m_iModelHandle ) ;

}

void AnimationManager::PlayOneAnim( double TimeElaps, Vector3D Pos, Vector3D Head, AnimPlayBackInfo* pPlayAnim )
{
	// �A�j���[�V�����ŗL���ւ̃|�C���^���擾
	AnimUniqueInfo* pAnimUnq = pPlayAnim->getAnimUnqPointer();

	// ����̍Đ��^�C�~���O���v�Z����
	float telaps = (float)TimeElaps;

	// curplaytime : ���̍Đ�����
	float curplaytime = pPlayAnim->m_CurPlayTime;
	if( !pPlayAnim->m_bPause && !pPlayAnim->m_bFinished && !DBG_m_bPauseOn){
		float UniquePlayPitch = pPlayAnim->getAnimUnqPointer()->m_fUniquePlayPitch;
		curplaytime += m_PlayPitch * UniquePlayPitch * telaps;
	}

	if(pAnimUnq->m_bRepeatAnimation)
	{ // �J�Ԃ��Đ� ON
		// �Đ��C���^�[�o�����ɍă}�b�s���O����
		if( pPlayAnim->m_MotionTotalTime > 0 )
		{
			while( curplaytime >= pPlayAnim->m_MotionTotalTime ){ curplaytime -= pPlayAnim->m_MotionTotalTime; }
			pPlayAnim->m_CurPlayTime = curplaytime;
			pPlayAnim->m_iPlayCount++;
		}
		else
		{ // �Đ��C���^�[�o�� = 0 �̃��[�V�����i�Î~��j 
			pPlayAnim->m_CurPlayTime = 0.0;
		}
	}
	else
	{ // �J�Ԃ��Đ� OFF
		// m_MotionTotalTime �𒴂��Ă����� m_MotionTotalTime ��ݒ肷��
		pPlayAnim->m_CurPlayTime = curplaytime;
		if( pPlayAnim->m_CurPlayTime > pPlayAnim->m_MotionTotalTime )
		{ 
			pPlayAnim->m_CurPlayTime = pPlayAnim->m_MotionTotalTime;
			pPlayAnim->m_bFinished = true;
		}
	}

	// �u�����h����ݒ�
	if( pAnimUnq->m_bCorrectionToCenter )
	{
		MV1SetAttachAnimBlendRate( m_iModelHandle, pPlayAnim->m_AttachIndex, pPlayAnim->m_fBlendRate ) ; // �u�����h���s�����ɂ�����炸�A�����΂ꂿ�Ⴄ���ǁA�������Ȃ��̂��H�i�������\�I�ɖ��Ȃ����H�j
	}
	else
	{
		MV1SetAttachAnimBlendRate( m_iModelHandle, pPlayAnim->m_AttachIndex, pPlayAnim->m_fBlendRate ) ; // �u�����h���s�����ɂ�����炸�A�����΂ꂿ�Ⴄ���ǁA�������Ȃ��̂��H�i�������\�I�ɖ��Ȃ����H�j
	}

    // �Đ����Ԃ��Z�b�g����
	if( pPlayAnim->m_AttachIndex != -1 )
	{
		MV1SetAttachAnimTime( m_iModelHandle, pPlayAnim->m_AttachIndex, pPlayAnim->m_CurPlayTime ) ;
	}

	// �Z���^�[�ʒu���Œ�ɂȂ�悤�ɁA���f���i�`��j�ʒu��␳����x�N�g�� CorrectionVec ���v�Z����
	Vector3D CorrectionVec( 0, 0, 0 );
	if( pAnimUnq->m_bCorrectionToCenter )
	{
		Vector3D DesiredCntPos = pAnimUnq->m_vFixCenterPosLocal;
		Vector3D CurFrmPos( MV1GetAttachAnimFrameLocalPosition( m_iModelHandle, pPlayAnim->m_AttachIndex, m_iCenterFrameIndex ) );
		CorrectionVec = DesiredCntPos - CurFrmPos; // �Z���^�[�t���[���̈ʒu�Ɩ]�ރZ���^�[�ʒu�Ƃ̍������v�Z
	}

	// �v�Z���� CorrectionVec ��ԋp
	pPlayAnim->m_vCorrectionVec = CorrectionVec; 

};

void AnimationManager::ReserveAnim( PlayerCharacterEntity::AnimationID AnimID, double AnimSwitchTime, bool StopPrvAnim )
{
	// �L���[�ɐςނ���
	m_qAnimReservationQueue.push( ArgumentOfSetAnim( AnimID, AnimSwitchTime, StopPrvAnim ) );
};

void AnimationManager::PlayReservedAnim()
{
	// ���s�A�j���[�V�����̍Đ��������������H
	if( (!m_qAnimReservationQueue.empty()) && m_pCurAnimPlayInfo->m_bFinished )
	{
		// ��ԍŏ��ɗ\�񂳂ꂽ�A�j���[�V�������Đ��ݒ�
		ArgumentOfSetAnim Arg = m_qAnimReservationQueue.front();
		setAnimMain( Arg.m_eAnimID, Arg.m_dAnimSwitchTime, Arg.m_bStopPrvAnim );
		m_qAnimReservationQueue.pop();
	}
};

void AnimationManager::DrawAllow3D( Vector3D cnt, Vector3D heading )
{
	// cnt �𒆐S�� heading ��x-z���ʂɓ��e���������֖���`��B
	// �i�@i.e. ���́Ax-z���ʂƐ����ł��� �j


	static const double AllowLength    = 10.0f;   // ���̑S��
	static const double AllowHeadHight = 4.0f;    // ���̓��̕����̒���
	static const double AllowHeadWidth = 6.0f;    // ���̓��̕����̕�

	static Vector2D PosH( AllowLength, 0 );
	static Vector2D PosL( AllowLength-AllowHeadHight,  0.5*AllowHeadWidth );
	static Vector2D PosR( AllowLength-AllowHeadHight, -0.5*AllowHeadWidth );

	// heading ��x-z���ʂɓ��e�����x�N�g�����擾
	Vector2D head2D = heading.toVector2D().normalize();
	Vector2D side2D = head2D.side();

	// ���̎O�p�`�̈ʒu�����[�J�����W�����[���h���W�ɕϊ� 
	// Vector2D �� Vecto3D �֍ĕϊ�
	Vector3D posh = Vector2DToWorldSpace( PosH, head2D, side2D ).toVector3D();
	Vector3D posl = Vector2DToWorldSpace( PosL, head2D, side2D ).toVector3D();
	Vector3D posr = Vector2DToWorldSpace( PosR, head2D, side2D ).toVector3D();

	SetUseZBuffer3D( FALSE );
	SetWriteZBuffer3D( FALSE );

	// �ւ�����`��
	static PlaneRing RingIns( 
		5.5, 1.25, 32, 
		GetColorU8(255,   0,   0, 0 ),
		GetColorU8(255, 255, 255, 0 ) );
	RingIns.setCenterPos( cnt );
	RingIns.Render(); 

	// cnt�V�t�g
	// �|���S����`��
	DrawTriangle3D( 
		(cnt+posh).toVECTOR(), 
		(cnt+posl).toVECTOR(), 
		(cnt+posr).toVECTOR(), 
		GetColor( 255, 0, 0 ), TRUE ); // �ԐF

	SetUseZBuffer3D( TRUE );
	SetWriteZBuffer3D( TRUE );
};

void AnimationManager::initAnimPhysics()
{
	// �����o�̏������E�ݒ�
	m_eCurPhysicsType = PHYSICS_NONE; // �������Z�Ȃ�
	//m_eCurPhysicsType = PHYSICS_DXLIB; // Dx���C�u����
	
	m_bCurBoneExpress = false;        // ���f���\��

	std::vector<int> FrameIndexList(8);

	// �e�함���p�����[�^
	double M = 0.2;    // ���_�̏d�ʁi�Œ�j
	double V = 2.0;    // �S����R�i�Œ�j
	double G = 400;     // �d�͒萔
	double S = 200.0;    // �o�l�萔�i�x�[�X�j
	double N = 0.9;      // ���R���Z�o�����ł̕␳�W��

	// �E���p�C���X�^���X��
	FrameIndexList[0] = MV1SearchFrame( m_iModelHandle, "��" );
	FrameIndexList[1] = MV1SearchFrame( m_iModelHandle, "�E���P" );
	FrameIndexList[2] = MV1SearchFrame( m_iModelHandle, "�E���Q" );
	FrameIndexList[3] = MV1SearchFrame( m_iModelHandle, "�E���R" );
	FrameIndexList[4] = MV1SearchFrame( m_iModelHandle, "�E���S" );
	FrameIndexList[5] = MV1SearchFrame( m_iModelHandle, "�E���T" );
	FrameIndexList[6] = MV1SearchFrame( m_iModelHandle, "�E���U" );
	FrameIndexList[7] = MV1SearchFrame( m_iModelHandle, "�E���V" );

	m_pRightHairPhysics = new StraightMultiConnectedSpringModel( m_iModelHandle, FrameIndexList, 7, M, V, G, S, N );
	m_pRightHairRender  = new MultiJointBoneMotionControl( m_iModelHandle, FrameIndexList, 6 );
	m_iRightHair1FrameIndex = FrameIndexList[1];

	// �����p�C���X�^���X��
	FrameIndexList[0] = MV1SearchFrame( m_iModelHandle, "��" );
	FrameIndexList[1] = MV1SearchFrame( m_iModelHandle, "�����P" );
	FrameIndexList[2] = MV1SearchFrame( m_iModelHandle, "�����Q" );
	FrameIndexList[3] = MV1SearchFrame( m_iModelHandle, "�����R" );
	FrameIndexList[4] = MV1SearchFrame( m_iModelHandle, "�����S" );
	FrameIndexList[5] = MV1SearchFrame( m_iModelHandle, "�����T" );
	FrameIndexList[6] = MV1SearchFrame( m_iModelHandle, "�����U" );
	FrameIndexList[7] = MV1SearchFrame( m_iModelHandle, "�����V" );

	m_pLeftHairPhysics = new StraightMultiConnectedSpringModel( m_iModelHandle, FrameIndexList, 7, M, V, G, S, N );
	m_pLeftHairRender  = new MultiJointBoneMotionControl( m_iModelHandle, FrameIndexList, 6 );
	m_iLeftHair1FrameIndex = FrameIndexList[1];

};

void AnimationManager::UpdateAnimPhysics( double TimeElaps )
{
	if( m_eCurPhysicsType == PHYSICS_SELFMADE )
	{
		// �E��
		if(m_pRightHairPhysics->Update( TimeElaps ))
		{
			m_pRightHairRender->setBoneAsJointList( m_pRightHairPhysics->m_pPosList );
		}
		if( m_bCurBoneExpress ) m_pRightHairPhysics->DebugRender();

		// ����
		if(m_pLeftHairPhysics->Update( TimeElaps ))
		{
			m_pLeftHairRender->setBoneAsJointList( m_pLeftHairPhysics->m_pPosList );
		}
		if( m_bCurBoneExpress ) m_pLeftHairPhysics->DebugRender();
	}
	else if( m_eCurPhysicsType == PHYSICS_DXLIB )
	{
		static const float rate = 10.0;
		MV1PhysicsCalculation( m_iModelHandle, rate * (float)(TimeElaps*1000) );
	}


};

void AnimationManager::ExpBoneOfPhysicsPart( bool BoneExpress )
{
	if( !m_bCurBoneExpress && BoneExpress )
	{ // ���f���\�� �� �{�[���\��
		// ���̃t���[�����\���ɂ���B
		DBG_RenewModel( DBG_m_iModelHandle_HideHair );
	}
	else if( m_bCurBoneExpress && !BoneExpress )
	{ // �{�[���\�� �� ���f���\��
		// ���̃t���[����\���ɖ߂�
		DBG_RenewModel( DBG_m_iModelHandle_Original );
	}
	
	m_bCurBoneExpress = BoneExpress;

};

void AnimationManager::setAnimPhysicsType( PhysicsTypeID id )
{
	// �O�̏�Ԃ̂��|���i��x�A�������Z�Ȃ��̏�Ԃɖ߂��j
	if( m_eCurPhysicsType == PHYSICS_SELFMADE )
	{
		// ���W�ϊ��s����f�t�H���g�ɖ߂�
		m_pRightHairRender->Reset();
		m_pLeftHairRender->Reset();

		// �����\���ɂ��Ă�ꍇ�����ǂ�����Ƃ�

	}
	else if( m_eCurPhysicsType == PHYSICS_DXLIB )
	{
		DBG_RenewModel( DBG_m_iModelHandle_Original );
	}

	// ����̏�Ԃ̐ݒ�
	if( id == PHYSICS_SELFMADE )
	{
		// �W���C���g�ʒu���t���[���ʒu�ɐݒ肷��
		m_pRightHairPhysics->setJointPosAsFrame();
		m_pLeftHairPhysics->setJointPosAsFrame();
	}
	else if( id == PHYSICS_DXLIB )
	{
		DBG_RenewModel( DBG_m_iModelHandle_Physics );
	}

	m_eCurPhysicsType = id;
};

Vector3D AnimationManager::DBG_RenderCenterFramePos()
{
	// �t���[���̌��݂̃��[���h���W���擾
	VECTOR FramePosVEC = MV1GetFramePosition( m_iModelHandle, m_iCenterFrameIndex );

	// �t���[���̏ꏊ�ɋ���`��
	DrawSphere3D( 
		FramePosVEC,
		1.0f, 
		32, 
		GetColor( 255,0,0 ), 
		GetColor( 255, 255, 255 ), 
		TRUE ) ;

	return Vector3D(FramePosVEC);
};

void AnimationManager::DBG_RenewModel( int ReneModelHandle )
{
	// �ێ����Ă���ϐ��ŁA�����ւ���K�v������̂́A
	// m_iModelHandle
	// m_pCurAnimPlayInfo->m_AttachIndex
	// m_pPrvAnimPlayInfo->m_AttachIndex

	AnimPlayBackInfo* AnimInfoList[2]={ m_pCurAnimPlayInfo, m_pPrvAnimPlayInfo };

	// �Â����f���ɃA�^�b�`���ꂽ�A�j���[�V��������x�f�^�b�`
	for( int i=0; i<2; i++ )
	{
		if(!(AnimInfoList[i]->m_bRemoved))
		{
			MV1DetachAnim( m_iModelHandle, AnimInfoList[i]->m_AttachIndex ); // �Â��A�j���[�V�����̃f�^�b�`�i�f�^�b�`���Ȃ���Animation���������ĕςȂ��ƂɂȂ�j
		}
	}

	// FrameIndex�n�͑��v�Ȃ͂��i�{�[���͐G���ĂȂ��̂Łj

	// ���f���n���h����n���ꂽ���̂ɍX�V
	m_iModelHandle = ReneModelHandle;

	// �A�j���[�V������V�������f���ɍēx�A�^�b�`
	// AnimPlayInfo->m_AttachIndex ���X�V
	for( int i=0; i<2; i++ )
	{
		if(!(AnimInfoList[i]->m_bRemoved))
		{
			int CurAttachedMotion = AnimInfoList[i]->getAnimUnqPointer()->m_CurAttachedMotion;
			if( CurAttachedMotion>=0 )
			{
				AnimInfoList[i]->m_AttachIndex = MV1AttachAnim( m_iModelHandle, CurAttachedMotion, -1, FALSE ) ;
			}
		}
	}

	// �������Z�p�̃T�u�t�@���N�V�����̎����Ă郂�f���n���h�����X�V����K�v������
	m_pRightHairPhysics->DBG_RenewModelHandles( m_iModelHandle );
	m_pRightHairRender->DBG_RenewModelHandles( m_iModelHandle );
	m_pLeftHairPhysics->DBG_RenewModelHandles( m_iModelHandle );
	m_pLeftHairRender->DBG_RenewModelHandles( m_iModelHandle );


};
