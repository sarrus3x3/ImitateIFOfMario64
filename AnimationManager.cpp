#include <cassert>

#include "AnimationManager.h"

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

	// �u�Z���^�[�v�t���[���̃t���[���ԍ����擾
	m_iCenterFrameIndex = MV1SearchFrame( m_iModelHandle, "�Z���^�[" );
	assert( m_iCenterFrameIndex>=0 && "Fail to MV1SearchFrame." );

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
		MV1SetMaterialAmbColor( m_iModelHandle, i, GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ) ;
	
	}

	// ���f���̕�����x���������ɏC���B�i�f�t�H���g�ł�z�������Ɍ����Ă���ۂ��̂ŁB�j
	// �� ��]�l��ݒ肷�邽�тɏ���������邩��Ӗ��ˁ[�B

	// m_pCurAnimPlayInfo �� m_pPrvAnimPlayInfo �̏�����
	m_pCurAnimPlayInfo = new AnimPlayBackInfo;
	m_pPrvAnimPlayInfo = new AnimPlayBackInfo;
	m_pCurAnimPlayInfo->init();
	m_pPrvAnimPlayInfo->init();

	m_pCurAnimPlayInfo->m_bRemoved = true;
	m_pPrvAnimPlayInfo->m_bRemoved = true;

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
	if( CurAttachedMotion>=0 ){
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
	DrawAllow3D( Pos, Head );
	DrawLine3D( Pos.toVECTOR(), (Pos+10*Head).toVECTOR(), GetColor( 255, 0, 0 ) );

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
    MV1SetAttachAnimTime( m_iModelHandle, pPlayAnim->m_AttachIndex, pPlayAnim->m_CurPlayTime ) ;

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

