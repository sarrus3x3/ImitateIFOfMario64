#include <queue>

#include <cassert>

#include "DxLib.h"
#include "PlayerCharacterEntity.h"

#include "AnimationPhysics.h"

//class PlayerCharacterEntity;


// #### �A�j���[�V�����ŗL���
// �A�j���[�V�������̂̑����B
// ex. �A���Đ����邩�H�P�񂵂��Đ������A��~���邩�H�A�Z���^�[�ʒu�Œ肩�H�Ƃ��Ƃ�
// ���ǂ��Ŏ���������
struct AnimUniqueInfo
{
	string  m_sAnimName; // �A�j���[�V�����̖��O�B�f�o�b�N���ŏo�͎��Ɏg�p�B

	int   m_CurAttachedMotion; // ���݃A�^�b�`���Ă���Motion�ԍ��i �����~�Nxxx.vmd �� xxx �̔ԍ� �j

	// �A�j���[�V�����ʒu����
	Vector3D m_vPosShift;           // Motion�̈ʒu�̒����Ɏg�p�iMotion�̈ʒu��Entity�̈ʒu�����킹�邽�߂̕␳�Ɏg�p�j

	// �Z���^�[�Œ�ʒu - �Z���^�[�ʒu���w��Bm_vPosShift ���L���Ȃ��Ƃɒ���
	bool     m_bCorrectionToCenter;     // ON ���ƃZ���^�[�ʒu���Œ肷��
	bool     m_bCorrectionToCenterButY; // ON ����"Y������������"�Z���^�[�ʒu���Œ肷��Bm_bCorrectionToCenter �� ON �ɂ��邱�ƁB�� �g�p�񐄏��I
	Vector3D m_vFixCenterPosLocal;  // m_bCorrectionToCenter:ON �̏ꍇ�ɌŒ肷��Z���^�[�ʒu�i���f���ʒu�ɑ΂��郍�[�J�����W�j�����Ƃ��͍l�����Ăˁ[����B

	// �A�j���[�V�����̌J�Ԃ��Đ��Ɋւ���
	bool     m_bRepeatAnimation;    // �A�j���[�V�������J��Ԃ����H�i �f�t�H���g ON �j
	
	// �A�j���[�V�����̍Đ��J�n�ʒu���w�肷��ꍇ�́A����m_fAnimStartTime���w��

	// �I���W�i���̃��[�V��������A�j���[�V������؂�o��
	bool     m_bCutPartAnimation;   // �I���W�i���̃��[�V��������A�j���[�V������؂�o�����i �f�t�H���g OFF �j�BON �̏ꍇ�́A���� m_fAnimStartTime �� m_fAnimEndTime ��ݒ�BON �̏ꍇ�̓��s�[�g�͋��e���Ȃ��im_bRepeatAnimation �� OFF �ɂ��邱�Ɓj�B
	float    m_fAnimStartTime;      // �i���̃��[�V�����ɑ΂��āj�A�j���[�V�����̊J�n���ԁBRepeat=ON �̏ꍇ�͂��̈ʒu����Đ��J�n
	float    m_fAnimEndTime;        // �i���̃��[�V�����ɑ΂��āj�A�j���[�V�����̏I�����ԁBRepeat=ON �̏ꍇ�͐ݒ�͖���

	// �A�j���[�V�����̓����Ɋւ���
	float    m_fUniquePlayPitch;    // �A�j���[�V�����ŗL�̍Đ��s�b�`�i�ŏI�I�ȍĐ��s�b�`�� m_PlayPitch(��) * UniquePlayPitch �Ōv�Z�����B��AnimationManager�N���X�̃����o�j	
	float    m_fAnimInterval;       // �A�j���[�V�����ؑ֎��ɓ��������Ƃ��Ɏg����Bm_MotionTotalTime�Ƃ͈Ⴄ���̂ł��邱�Ƃɒ��ӁB


	// �W���ȊO�̃A�j���[�V�����̊J�n���ԁiAnimationManager.setAnimExStartTime()���\�b�h�Ŏg�p�j
	float    m_fExAnimStartTime;

	// ���������\�b�h
	void init()
	{
		m_sAnimName           = "UNDEFINE";
		m_CurAttachedMotion   = -1;
		m_bCorrectionToCenter = false;
		m_bCorrectionToCenterButY = false;
		m_vPosShift           = Vector3D( 0.0, 0.0, 0.0 );
		m_vFixCenterPosLocal  = Vector3D( 0.0, 8.0, 0.0 );
		m_bRepeatAnimation    = true;
		m_bCutPartAnimation   = false;
		m_fAnimStartTime      = 0.0;
		m_fAnimEndTime        = 0.0;
		m_fUniquePlayPitch    = 1.0;
		m_fAnimInterval       = 0.0;
		m_fExAnimStartTime    = 0.0;
	};

};

// #### AnimationManager::setAnim �̈������\���̂ɂ�������
// �A�j���[�V�����̗\��Ǘ��Ɏg�p����
struct ArgumentOfSetAnim
{
	PlayerCharacterEntity::AnimationID m_eAnimID; // �Z�b�g����A�j���[�V����ID
	double m_dAnimSwitchTime; // �u�����h����
	bool   m_bStopPrvAnim;    // ���݂̃A�j���[�V�����̒�~�v��
	float  m_fStartFrame;     // �C�ӂ̃��[�V�����̍Đ��J�n�_

	// �R���X�g���N�^
	ArgumentOfSetAnim( PlayerCharacterEntity::AnimationID AnimID, double AnimSwitchTime, bool StopPrvAnim, float StartFrame )
	{
		m_eAnimID         = AnimID;
		m_dAnimSwitchTime = AnimSwitchTime;
		m_bStopPrvAnim    = StopPrvAnim;
		m_fStartFrame     = StartFrame;
	};

};

// #### �A�j���[�V�����Đ��Ǘ��\����
// �A�j���[�V�����Đ���Ԃɏ]���ω�������́B
// ex. ���݂̍Đ��t���[���A�Đ����ԁA�Đ��񐔓��B
struct AnimPlayBackInfo
{
	// �e�ɂȂ�AnimationManager�̃|�C���^
	// - m_iModelHandle �����\�b�h���Ŏg�p���邽�߁B�R���X�g���N�^�ň����Ƃ��ēn���B
	AnimationManager* m_pAnimationManager;

	// �Đ��A�j���[�V�����̃A�j���[�V����ID
	PlayerCharacterEntity::AnimationID m_eAnimID;

	// ���[�V�����̃A�^�b�`�C���f�b�N�X�i MV1AttachAnim �̃��^�[���l �j
	int   m_AttachIndex;

	// �A�j���[�V�����Đ����
	float m_MotionTotalTime;   // ���[�V�����̑��Đ����� - �A�j���[�V�����؏o��ON�̏ꍇ�́AAnimUniqueInfo.m_fAnimEndTime�̒l���ݒ肳���B�؏o���A�j���[�V�����̍Đ����Ԃ�m_fAnimLength�Ɋi�[����
	float m_CurPlayTime;       // ���݂̍Đ�����
	float m_fBlendRate;        // �A�j���[�V�����̃u�����h���i 0.0 �` 1.0 �̊� �j
	int   m_iPlayCount;        // �iset����Ă���́j�A�j���[�V�����̍Đ���
	bool  m_bPause;            // �A�j���[�V������~�i�Î~�j�t���O
	bool  m_bFinished;         // �A�j���[�V�����I���t���O �A�j���[�V���������Đ����Ԃ܂ōĐ����ꂽ�ꍇ�ȂǁB���[�v����A�j���[�V�����̏ꍇ�� ON �ɂȂ�Ȃ��B
	bool  m_bRemoved;          // �A�j���[�V�����j���t���O ON�Ȃ�A�j���[�V�����Đ����Ȃ��B�A�j���[�V�������f�^�b�`����^�C�~���O�� ON �ɂ��Ă���
	Vector3D m_vCorrectionVec; // m_CurAttachedMotion ON �̏ꍇ�̃��[�V�����ʒu�̕␳�x�N�g���iPlayOneAnim�Ōv�Z����APlaySub�Ńu�����h�l�����Ď��ۂ̈ʒu�␳�����s����B�j
	float m_fAnimLength;       // ���̃A�j���[�V�����̍Đ��ɂ����鎞��

							   // �u�����h����Ɏg�p
	float m_fBlendRemain;      // �u�����h�̎c�莞�ԁB0�ɂȂ������Ɋ��S�ɂ��̃A�j���[�V�����ɑJ�ڊ�������B�u�����h�Ȃ��̏ꍇ=0
	float m_fAnimSwitchTime;   // �A�j���[�V�����ؑւ̐ݒ莞��

	inline AnimUniqueInfo* getAnimUnqPointer()
	{
		return &(PlayerCharacterEntity::AnimUniqueInfoManager::Instance()->m_pAnimUniqueInfoContainer[m_eAnimID]);
	};

	// �R���X�g���N�^
	AnimPlayBackInfo(AnimationManager* pAnimationManager, PlayerCharacterEntity::AnimationID AnimID, float AnimSwitchTime);

	// �f�X�g���N�^
	//~AnimPlayBackInfo();

	// �A�j���[�V�����̃A�^�b�`����
	void AttachAni();

	// �A�j���[�V�����̃f�^�b�`����
	void DetachAni();

};


// ########################################################################
// ######################## AnimationManager Class ########################
// ########################################################################

class AnimationManager
{
private:
	// ##### MMD���f���̃n���h���̌��{�F
    // AnimationManager �̃C���X�^���X�𕡐������ɑΉ����邽�߁A
	// MV1LoadModel �Ő�������n���h����static��class�ň���������A
	// m_iModelHandle �́Am_iModelHandleMaster �� MV1DuplicateModel �ŕ������Ďg��
	static int m_iModelHandleMaster;  

	// #### �i���݂́j�A�j���[�V�����Ɋւ�����A�����A�ݒ� �� ####
	// ���f�����
public:
	int   m_iModelHandle;              // MMD���f���̃n���h�� �� AnimationManager �̃C���X�^���X�𕡐������ɑΉ����邽�߁Astatic���i�b��Ώ��j

private:
	int   m_iCenterFrameIndex;         // �u�Z���^�[�v�t���[���̃t���[���ԍ��i MV1SearchFrame �̃��^�[���l �j

	// �Đ��s�b�`�i�Đ����x�j
	float m_PlayPitch;         // �Đ��s�b�`�i�Đ����x�j

	//  �A�j���[�V�����Đ��Ǘ��\���̔z��
	// - �u�����h�����A�j���[�V�����̔z��B���̗v�f�قǐV�����A�j���[�V�����ŁA�Ō�����ŐV�̃A�j���[�V�����ɂȂ�B
	// ����ς����ς�A�擪���ŐV�ɂȂ�悤�ɁB�B�B
	vector<AnimPlayBackInfo> m_AnimPlayInfoArray;

	// �A�j���[�V�����\��@�\�Ɋւ���
	queue<ArgumentOfSetAnim> m_qAnimReservationQueue;

	// �p���̌X���i���񓮍쎞�ɉ��S�͂ő̂��X�����o�Ɏg�p�j
	// �����ŌX�����E�����A�������������B
	double m_dBankAngle;

	// #### �⏕���\�b�h ####
	void PlayOneAnim( double TimeElaps, Vector3D Pos, Vector3D Head, AnimPlayBackInfo* pPlayAnim ); // Entity�����Q�Ƃ����Ȃ��Œ��ڏ������w�肵��Animation��`�悷��B
	void CleanUpAnim( AnimPlayBackInfo* pAnimInfo ); // �A�j���[�V�����̌㏈�����s��
	//void InitAnimPlayInfoAsAnim( AnimPlayBackInfo* pAnimInfo, PlayerCharacterEntity::AnimationID AnimID ); // ������pAnimInfo���w�肳�ꂽ�A�j���[�V�����ŏ���������
	void PlayReservedAnim(); // CurAnim���Đ��I�����������`�F�b�N���A�Đ��I�����Ă���Η\�񂳂ꂽ�A�j���[�V�������Đ��ݒ肷��B
	void setAnimMain( 
		PlayerCharacterEntity::AnimationID, 
		double AnimSwitchTime=0.0, 
		bool StopPrvAnim =true, 
		bool SyncToPrv   =false,
		float StartFrame = -1.0f
		); 

public:
	// #### �R���X�g���N�^�E�f�X�g���N�^ ####
	AnimationManager();
	
	// #### ���\�b�h ####
	void Play( PlayerCharacterEntity* );   // �A�j���[�V�����̍Đ����s
	
	void setPitch( float playpitch ){ m_PlayPitch = playpitch; }; // �A�j���[�V�����̍Đ��s�b�`�̃Z�b�g

	void setBankAngle( double bankangle ){ m_dBankAngle = bankangle; }; // �o���N�p�̐ݒ�


	// ----- �A�j���[�V�����̃Z�b�g�i�eEntity Class�Œ�`�����A�j���[�V�����ŗLenum���w��j
	// �u�����h�w�肷��ꍇ�́A�����ɁA�u�����h���ԁi�f�t�H���g0�j�A���݂̃A�j���[�V�����̒�~�v�ہi�f�t�H���g�v�j��ݒ�
	// AnimSwitchTime �ȍ~���w�肵�Ȃ��ƃu�����h�Ȃ��Őؑւ���B
	// SyncToPrv ON �ɂ���ƁA�ʑ���ۂ��ăA�j���[�V�����ؑ֍s���B�������ARunning<->Walking�ɂ����Ή����ĂȂ�
	//�ySyncToPrv�i���[�V�����̓����I�V�t�g�j���g�p����ɓ������Ă̒��Ӂz
	//	* �V�t�g����Q�̃��[�V������ AnimUniqueInfo �ŁA�������um_fUniquePlayPitch�v�Ɓum_fAnimInterval�v���ݒ肳��Ă���K�v������B
	//	* �V�t�g��̃��[�V�����ɂ�����Đ��J�n�_�́A�Q�̃��[�V�����̊J�n��`�ʒu���l�����āA�Q�̃��[�V�����̍Đ����Ԃ̔䂩��V�t�g�����[�V�����̌��ݍĐ��ʒu����V�t�g�惂�[�V�����Ƀ}�b�s���O�����B�ڍׂ͎��������āB
	// StartFrame : �f�t�H���g�ȊO�̃��[�V�����̊J�n�ʒu�i�t���[���j���w��B���l�̏ꍇ�́A�f�t�H���g�̍Đ��ʒu�ōĐ�����B
	void setAnim( PlayerCharacterEntity::AnimationID, double AnimSwitchTime=0.0, bool StopPrvAnim=true, bool SyncToPrv=false, float StartFrame=-1.0f ); 

	// ----- �A�j���[�V�����̍Đ��\��B
	// �A�j���[�V�����ݒ���� AnimReservationQueue �ɃX�^�b�N����A�Đ����̃A�j���[�V�������Đ��I��������ݒ肳���B
	// �� ���荞�݂ŐV�����A�j���[�V������setAnim���ꂽ�ꍇ�́A�\�񒆂̐ݒ�͔j�������B
	void ReserveAnim( PlayerCharacterEntity::AnimationID, double AnimSwitchTime=0.0, bool StopPrvAnim=true, float StartFrame = -1.0f );

	// ----- �A�j���[�V�����̍Đ��\����̔j��
	// �Đ��\�񂳂ꂽ�A�j���[�V������j������ = �L���[����ɂ���
	// http://qiita.com/D-3/items/9930591bb78df544c066
	void DiscardReservedAnim(){ queue<ArgumentOfSetAnim>().swap(m_qAnimReservationQueue); };

	// ----- m_pCurAnimPlayInfo��AnimationID���擾
	PlayerCharacterEntity::AnimationID getCurAnimID() { return m_AnimPlayInfoArray[0].m_eAnimID; };

	// ----- m_pCurAnimPlayInfo�̃A�j���[�V���������擾
	string getCurAnimName(){ return m_AnimPlayInfoArray[0].getAnimUnqPointer()->m_sAnimName; };

	// ----- m_pPrvAnimPlayInfo�̃A�j���[�V���������擾
	// ������āAm_pPrvAnimPlayInfo ���Z�b�g����Ă��Ȃ������ꍇ�̓�����Ăǂ��Ȃ�񂾁H
	// �܁A���v����B
	string getPrvAnimName() { return "Now this method is not supported..."; };

	// ----- m_pCurAnimPlayInfo�̍Đ��ɂ����鎞�Ԃ��擾
	float getCurAnimLength(){ return m_AnimPlayInfoArray[0].m_fAnimLength; };

	// ----- AnimUniqueInfo �i�e���[�V�����̌ŗL���j�̏����擾����
	inline AnimUniqueInfo* getAnimUnqPointer(PlayerCharacterEntity::AnimationID AnimID )
	{
		return &(PlayerCharacterEntity::AnimUniqueInfoManager::Instance()->m_pAnimUniqueInfoContainer[AnimID]);
	}


	// #### �⏕���\�b�h ####
	void  PlayMain( double TimeElaps, Vector3D Pos, Vector3D Head );
	float CurPlayTime(){ return m_AnimPlayInfoArray[0].m_CurPlayTime; }
	void  DrawAllow3D( Vector3D cnt, Vector3D heading ); // ����`��
	float getMotionTotalTime(){ return m_AnimPlayInfoArray[0].m_MotionTotalTime; }
	
// ############ �������Z�i���̖т�h�炷�Ƃ��j�֘A ############
public:
	// �������Z�̎�ʁi�񋓌^�j
	enum PhysicsTypeID
	{
		PHYSICS_NONE     = 0, // �������Z�Ȃ�
		PHYSICS_SELFMADE = 1, // ����̕������Z
		PHYSICS_DXLIB    = 2  // DX���C�u�����@�\�g�p
	};

//private:
	// ���݂̕������Z�̎��
	PhysicsTypeID m_eCurPhysicsType;

	// �{�[���\�� / ���f���\���i ExpBoneOfPhysicsPart �Ŏg�p�j
	bool m_bCurBoneExpress; // ON �Ȃ� �{�[���\��

	//### ����̕������Z�iPHYSICS_SELFMADE�j�Ŏg���C���X�^���X��
	
	// �E���p
	StraightMultiConnectedSpringModel *m_pRightHairPhysics;
	MultiJointBoneMotionControl       *m_pRightHairRender;
	int m_iRightHair1FrameIndex;  // "�E���P" �t���[�� �i�t���[���̕\���^��\���Ŏg�p�j
	
	// �����p
	StraightMultiConnectedSpringModel *m_pLeftHairPhysics;
	MultiJointBoneMotionControl       *m_pLeftHairRender;
	int m_iLeftHair1FrameIndex;   // "�����P" �t���[�� �i�t���[���̕\���^��\���Ŏg�p�j

public:
	//##### ���\�b�h #####
	//- �������Z�̎�ʂ�ݒ�i DX���C�u�����̕������Z�A�I���W�i���̕������Z�A�������Z�Ȃ� �j
	void setAnimPhysicsType( PhysicsTypeID id );
	
	//- �������Z���i���̖сj�� �{�[���\�� / ���f���\�� �̐ؑցi�I���W�i���̕������Z�̏ꍇ�j
	void ExpBoneOfPhysicsPart( bool BoneExpress ); // true-�{�[���\���Afalse-���f���\��

	bool getCurBoneExpress(){ return m_bCurBoneExpress; };
	PhysicsTypeID getPhysicsType(){ return m_eCurPhysicsType; };

private:
	//###### �⏕���\�b�h ######
	//������
	// - �R���X�g���N�^�����s����
	void initAnimPhysics();

	//�������Z�̎��s�A�t���[���ɍ��W�ϊ��s���ݒ�
	// - AnimationManager::PlayMain���AMV1DrawModel �̒��O�ŌĂ�
	void UpdateAnimPhysics( double TimeElaps );

// ############ �f�o�b�N�p�@�\ ############
private:

public:
	bool DBG_m_bPauseOn; // �I���Ȃ�Animation���~������

	bool DBG_getPauseState(){ return DBG_m_bPauseOn; }
    Vector3D DBG_RenderCenterFramePos(); // �u�Z���^�[�v�t���[���̍��W�ʒu��`�悷��A���łɍ��W�ʒu��ԋp����B
	void DBG_setCurPlayTimeOfCurAnim( float time ){ m_AnimPlayInfoArray[0].m_CurPlayTime = time; };
	int  DBG_getModelHandle(){ return m_iModelHandle; };

	int  DGB_m_iHairFrameIndex;    // ���t���[��
	//MATRIX DGB_m_mHairFrameMatrix;   // ���t���[����Matirix

	// �R���X�g���N�^�ŏ�������ɁA���f����ύX�������ꍇ�Ɏg�p�B�f�o�b�N�p�̎���
	// AnimManager�������C���X�^���X�������ꍇ�̓���͕ۏ؂��Ȃ����B
	void DBG_RenewModel( int ReneModelHandle ); // �X�V���������f���̃n���h����n������
	
	int DBG_m_iModelHandle_Original; // �I���W�i���̃��f���̃n���h���̑ޔ�p
	int DBG_m_iModelHandle_Physics;  // �������Z����œǂݍ��񂾃��f���̃n���h��
	int DBG_m_iModelHandle_HideHair; // ���̖э폜���폜�������f���̃n���h��

	float DBG_getCurAnimBRate(){ return m_AnimPlayInfoArray[0].m_fBlendRate; };

	int DBG_getAnimPlayInfoArray_Size() { return m_AnimPlayInfoArray[0].m_AttachIndex; };

	float DBG_getCurAnimPlayTime(){ return m_AnimPlayInfoArray[0].m_CurPlayTime; };

	bool DBG_m_bBlendPauseOn; // �A�j���[�V�����u�����f�B���O�̈ꎞ��~

	//void DBG_setAnimPlayTime( float playtime ){ m_pCurAnimPlayInfo->m_CurPlayTime = playtime; }; // �A�j���[�V������C�ӂ̍Đ����Ԃɐݒ�



};

 