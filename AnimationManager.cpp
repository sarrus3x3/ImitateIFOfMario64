#include <cassert>

#include "AnimationManager.h"
#include "SolidObjects.h"

#define DEBUG_SWITCH_PHYSICS_TYPE

int AnimationManager::m_iModelHandleMaster = -1; // m_iModelHandle �̏������B�i�ÓI�����o�ϐ��j

// #### �R���X�g���N�^ ####
AnimationManager::AnimationManager() :
	m_PlayPitch(20.0),
	m_dBankAngle( 0.0 ),
	DBG_m_bPauseOn(false),
	DBG_m_bBlendPauseOn( false )

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

	m_AnimPlayInfoArray.insert(m_AnimPlayInfoArray.begin(), AnimPlayBackInfo(this, (PlayerCharacterEntity::AnimationID)-1, 0.0f));
	m_AnimPlayInfoArray[0].AttachAni();

	// �A�j���[�V�����̕������Z�֘A�̏�����
	initAnimPhysics();

};

// �R���X�g���N�^
AnimPlayBackInfo::AnimPlayBackInfo( AnimationManager* pAnimationManager, PlayerCharacterEntity::AnimationID AnimID, float AnimSwitchTime) :
	m_pAnimationManager(pAnimationManager),
	m_eAnimID(AnimID),
	m_AttachIndex(-1),
	m_MotionTotalTime(0),
	m_CurPlayTime(0),
	m_fBlendRate(1.0f),
	m_iPlayCount(0),
	m_bPause(false),
	m_bFinished(false),
	m_fAnimSwitchTime(AnimSwitchTime),
	m_fBlendRemain(AnimSwitchTime)
{


};

void AnimPlayBackInfo::AttachAni()
{
	// �A�j���[�V�����ŗL���ւ̃|�C���^���擾
	AnimUniqueInfo* pAnimUnq = getAnimUnqPointer();

	// �iMotionID�ɏ]���jm_AttachIndex �̐ݒ�
	int CurAttachedMotion = pAnimUnq->m_CurAttachedMotion;
	if (CurAttachedMotion >= 0)
	{
		m_AttachIndex = MV1AttachAnim(m_pAnimationManager->m_iModelHandle, CurAttachedMotion, -1, FALSE);
	}
	// m_MotionTotalTime ���擾
	if (m_AttachIndex == -1)
	{ // �A�j���[�V�������ݒ�
		m_MotionTotalTime = 0.0f;
	}
	else if (!(pAnimUnq->m_bCutPartAnimation))
	{ // ���[�V�����؏o�� OFF
		m_MotionTotalTime = MV1GetAttachAnimTotalTime(m_pAnimationManager->m_iModelHandle, m_AttachIndex);
		m_CurPlayTime = pAnimUnq->m_fAnimStartTime;
		m_fAnimLength = m_MotionTotalTime - pAnimUnq->m_fAnimStartTime;
	}
	else
	{ // ���[�V�����؏o�� ON
	  // PlaySub �̎d�g�݂���A����Ő؏o�������ł���͂�
		m_CurPlayTime = pAnimUnq->m_fAnimStartTime;
		m_MotionTotalTime = pAnimUnq->m_fAnimEndTime;
		m_fAnimLength = pAnimUnq->m_fAnimEndTime - pAnimUnq->m_fAnimStartTime;
	}
}

void AnimPlayBackInfo::DetachAni()
{
	MV1DetachAnim(m_pAnimationManager->m_iModelHandle, m_AttachIndex); // �Â��A�j���[�V�����̃f�^�b�`�i�f�^�b�`���Ȃ���Animation���������ĕςȂ��ƂɂȂ�j
}


void AnimationManager::setAnim( PlayerCharacterEntity::AnimationID AnimID, double AnimSwitchTime, bool StopPrvAnim, bool SyncToPrv, float StartFrame )
{
	// ���荞�݂ŐV�����A�j���[�V������setAnim���ꂽ�ꍇ�́A�\�񒆂̐ݒ�͔j�������B
	DiscardReservedAnim();

	// setAnim�̏����{�̂��Ă�
	setAnimMain( AnimID, AnimSwitchTime, StopPrvAnim, SyncToPrv, StartFrame);
};

// �u�����h�w�肷��ꍇ�́A�����ɁA�u�����h���ԁi�f�t�H���g0�j�A���݂̃A�j���[�V�����̒�~�v�ہi�f�t�H���g�v�j��ݒ�
// AnimSwitchTime �ȍ~���w�肵�Ȃ��ƃu�����h�Ȃ��Őؑւ���B
void AnimationManager::setAnimMain( 
	PlayerCharacterEntity::AnimationID AnimID,
	double AnimSwitchTime, 
	bool StopPrvAnim, 
	bool SyncToPrv,
	float StartFrame )
{
	m_AnimPlayInfoArray[0].m_bPause = StopPrvAnim; // �O�̃��[�V�����̍Đ����~�B

	// �u�����h�w��L���ɂ�����炸�A�擪�ɐV�����A�j���[�V�����\���̂�����ŁA�����̃p�����[�^���Ԃ����ށB
	m_AnimPlayInfoArray.insert(m_AnimPlayInfoArray.begin(), AnimPlayBackInfo( this, AnimID, (float)AnimSwitchTime ));

	//m_AnimPlayInfoArray.emplace(m_AnimPlayInfoArray.begin(), this, AnimID, (float)AnimSwitchTime);

	m_AnimPlayInfoArray[0].AttachAni();

	// ################## ���[�V�����̐ݒ� #######################

	// ##### �A�j���[�V�������u�����v������ #####
	if( SyncToPrv )
	{
		// �A�j���[�V������ؑւ��鎞�A�u�ʑ��v�������p�������Ƃ����ꍇ������B
		// �Ⴆ�΁A����������ɐ؂�ւ������A������ԑO�ɓ˂��o���Ă��鎞�ɐؑւ�����A
		// �����̃A�j���[�V�����ł������悤�ɑ�����ԑO�ɓ˂��o������Ԃɂ������B

		float CurInterval = m_AnimPlayInfoArray[0].getAnimUnqPointer()->m_fAnimInterval;
		float PrvInterval = m_AnimPlayInfoArray[1].getAnimUnqPointer()->m_fAnimInterval;
		float CurAminStart = m_AnimPlayInfoArray[0].getAnimUnqPointer()->m_fAnimStartTime;
		float PrvAminStart = m_AnimPlayInfoArray[1].getAnimUnqPointer()->m_fAnimStartTime;
		
		assert( (CurInterval>0) && (PrvInterval>0) && "�����I�V�t�g����ǂ��炩�̃��[�V������m_fAnimInterval�����ݒ�." );
		float scale = CurInterval/PrvInterval;
		m_AnimPlayInfoArray[0].m_CurPlayTime = scale * (m_AnimPlayInfoArray[1].m_CurPlayTime - PrvAminStart) + CurAminStart;

	}

	// ##### ���[�V�����̊J�n�ʒu���w�肷�� #####
	if (StartFrame >= 0.0)
	{
		// �J�n������ StartFrame �ɕύX
		m_AnimPlayInfoArray[0].m_CurPlayTime = StartFrame;
	}


};

void AnimationManager::Play( PlayerCharacterEntity* pEntity )
{
	// ######## �A�j���[�V�����̍Đ� ########

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

	// ######## �A�j���[�V�����̃u�����h���� ########

	// 2018/05/08 ���d�u�����h�Ή�
	vector<AnimPlayBackInfo>::iterator it = m_AnimPlayInfoArray.begin();
	double OverBlendRate = 1.0; // �㑱���[�V�����Ɉ����p�����u�����h��

	while (it != m_AnimPlayInfoArray.end())
	{
		it->m_fBlendRemain -= m_PlayPitch * (float)TimeElaps; // �u�����h�c�莞�Ԃ��X�V

		if (it->m_fBlendRemain > 0)
		{
			// �u�����h�����v�Z���čX�V
			float BRate = it->m_fBlendRemain / it->m_fAnimSwitchTime;
			it->m_fBlendRate = (1.0f-BRate) * OverBlendRate; // ���[�V�����̃u�����h���́AOverBlendRate�ȏ�ɂȂ�Ȃ��B
			OverBlendRate *= BRate; // �u�����h���̌㑱�̃��[�V�����ւ̈����p���B
		}
		else
		{
			it->m_fBlendRate = OverBlendRate;

			it++;
			// ���[�V�����̃f�^�b�`�����i���ǁA���������������ɂȂ���...�j
			for(vector<AnimPlayBackInfo>::iterator itmp = it; itmp != m_AnimPlayInfoArray.end(); itmp++ )
			{ 
				itmp->DetachAni();
			}
			m_AnimPlayInfoArray.erase(it, m_AnimPlayInfoArray.end()); // �ȍ~�̃��[�V�����폜
			break; // ���[�v���甲����
		}
		it++;
	}

	// DBG 
	float time_elaps;
	if( DBG_m_bPauseOn )
	{
		time_elaps = 0.0;
	}
	else
	{
		time_elaps = TimeElaps;
	}

	// ######## �A�j���[�V�����̍Đ����� ########

	// �z��̒��g�ɂ���A�j���[�V������S�čĐ�
	for (int i = 0; i < m_AnimPlayInfoArray.size(); i++)
	{
		PlayOneAnim(time_elaps, Pos, Head, &m_AnimPlayInfoArray[i]); 
	}

	// ���f���Z���^�ւ̃��[�V�����ʒu�̕␳�x�N�g�����A
	// PlayOneAnim�Ōv�Z���ꂽAnimPlayBackInfo.m_vCorrectionVec����v�Z
	
	// #### �ʒu�␳���Z���^�[�t���[���̍��W�ϊ��s��ɔ��f

	// �ʒu�␳�p�̍��W�ϊ��s��𐶐�
	Vector3D CorrectionVec(0, 0, 0);
	MATRIX TransMac = MGetTranslate(CorrectionVec.toVECTOR());

	// �p���X���i�o���N�p�j�̐ݒ�
	TransMac = MMult( TransMac, MGetRotZ( (float)m_dBankAngle ) ); // �� ���f���̓f�t�H���g�ł�z���������������Ă���

	// ���f���̓f�t�H���g��z�������Ɍ����Ă��邽�߁Ax�������Ɍ����悤�ɉ�]
	TransMac = MMult( TransMac, MGetRotY( DX_PI * -0.5 ) );

	// Entity�̌����ݒ�p�̍��W�ϊ��s��𐶐�
	
	// �b�菈���B�{���͈����ɃL�����N�^�̃��[�J�����W����n���Ȃ���΂Ȃ�Ȃ��B
	Vector3D Upper( 0, 1, 0 );
	Vector3D Orign( 0, 0, 0 );
	Vector3D Side = Head % Upper;

	// �s�� [ Head, Upper, Side ]
	MATRIX MLoc = MGetAxis1(
		Head.toVECTOR(),
		Upper.toVECTOR(),
		Side.toVECTOR(),
		Orign.toVECTOR()
		);
	
	TransMac = MMult( TransMac, MLoc );

	// Entity�̈ʒu�ݒ�p�̍��W�ϊ��s��𐶐�
	TransMac = MMult( TransMac, MGetTranslate(Pos.toVECTOR()) );

	// ���W�ϊ��s������f���ɓK�p
	MV1SetMatrix( m_iModelHandle, TransMac );

	// m_vHeading �̃x�N�g����`��i�⏕�j
	//DrawAllow3D( Pos, Head );
	
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
	if( !pPlayAnim->m_bPause && !pPlayAnim->m_bFinished ){
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
		if( pPlayAnim->m_CurPlayTime > (pPlayAnim->m_MotionTotalTime) )
		{ 
			pPlayAnim->m_CurPlayTime = (pPlayAnim->m_MotionTotalTime);
			pPlayAnim->m_bFinished = true;
		}
	}

	// �u�����h����ݒ�
	MV1SetAttachAnimBlendRate( m_iModelHandle, pPlayAnim->m_AttachIndex, pPlayAnim->m_fBlendRate ) ; // �u�����h���s�����ɂ�����炸�A�����΂ꂿ�Ⴄ���ǁA�������Ȃ��̂��H�i�������\�I�ɖ��Ȃ����H�j

    // �Đ����Ԃ��Z�b�g����
	if( pPlayAnim->m_AttachIndex != -1 )
	{
		MV1SetAttachAnimTime( m_iModelHandle, pPlayAnim->m_AttachIndex, pPlayAnim->m_CurPlayTime ) ;
	}

};

void AnimationManager::ReserveAnim( PlayerCharacterEntity::AnimationID AnimID, double AnimSwitchTime, bool StopPrvAnim, float StartFrame )
{
	// �L���[�ɐςނ���
	m_qAnimReservationQueue.push( ArgumentOfSetAnim( AnimID, AnimSwitchTime, StopPrvAnim, StartFrame ) );
};

void AnimationManager::PlayReservedAnim()
{
	// ���s�A�j���[�V�����̍Đ��������������H
	if( (!m_qAnimReservationQueue.empty()) && m_AnimPlayInfoArray[0].m_bFinished )
	{
		// ��ԍŏ��ɗ\�񂳂ꂽ�A�j���[�V�������Đ��ݒ�
		ArgumentOfSetAnim Arg = m_qAnimReservationQueue.front();
		setAnimMain( Arg.m_eAnimID, Arg.m_dAnimSwitchTime, Arg.m_bStopPrvAnim, false, Arg.m_fStartFrame );
		m_qAnimReservationQueue.pop();
	}
};

AnimPlayBackInfo * AnimationManager::getAnimPlayBackInfoFromAnimID(PlayerCharacterEntity::AnimationID AnimID)
{
	for (int i = 0; i < m_AnimPlayInfoArray.size(); i++)
	{
		if (m_AnimPlayInfoArray[i].m_eAnimID == AnimID)
		{
			return &m_AnimPlayInfoArray[i];
		}
	}

	// ������Ȃ������� null ��Ԃ��B
	return nullptr;
}

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
		5.5, 1.25, 32  );
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

	//AnimPlayBackInfo* AnimInfoList[2]={ m_pCurAnimPlayInfo, m_pPrvAnimPlayInfo };

	// �Â����f���ɃA�^�b�`���ꂽ�A�j���[�V��������x�f�^�b�`
	for (int i = 0; i < m_AnimPlayInfoArray.size(); i++)
	{
		MV1DetachAnim(m_iModelHandle, m_AnimPlayInfoArray[i].m_AttachIndex); // �Â��A�j���[�V�����̃f�^�b�`�i�f�^�b�`���Ȃ���Animation���������ĕςȂ��ƂɂȂ�j
	}

	// FrameIndex�n�͑��v�Ȃ͂��i�{�[���͐G���ĂȂ��̂Łj

	// ���f���n���h����n���ꂽ���̂ɍX�V
	m_iModelHandle = ReneModelHandle;

	// �A�j���[�V������V�������f���ɍēx�A�^�b�`
	// AnimPlayInfo->m_AttachIndex ���X�V
	for (int i = 0; i < m_AnimPlayInfoArray.size(); i++)
	{
		int CurAttachedMotion = m_AnimPlayInfoArray[i].getAnimUnqPointer()->m_CurAttachedMotion;
		if (CurAttachedMotion >= 0)
		{
			m_AnimPlayInfoArray[i].m_AttachIndex = MV1AttachAnim(m_iModelHandle, CurAttachedMotion, -1, FALSE);
		}
	}

	// �������Z�p�̃T�u�t�@���N�V�����̎����Ă郂�f���n���h�����X�V����K�v������
	m_pRightHairPhysics->DBG_RenewModelHandles( m_iModelHandle );
	m_pRightHairRender->DBG_RenewModelHandles( m_iModelHandle );
	m_pLeftHairPhysics->DBG_RenewModelHandles( m_iModelHandle );
	m_pLeftHairRender->DBG_RenewModelHandles( m_iModelHandle );


};
