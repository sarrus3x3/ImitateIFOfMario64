#include <queue>

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
	float    m_fUniquePlayPitch;    // ���̃A�j���[�V�����ƕʂɍĐ��s�b�`���w�肵�����ꍇ�Ɏg�p�iglobal��PlayPich�̉e���͎󂯂�j	
	float    m_fAnimInterval;       // �A�j���[�V�����ؑ֎��ɓ��������Ƃ��Ɏg����Bm_MotionTotalTime�Ƃ͈Ⴄ���̂ł��邱�Ƃɒ���

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

// #### �A�j���[�V�����Đ����
// �A�j���[�V�����Đ���Ԃɏ]���ω�������́B
// ex. ���݂̍Đ��t���[���A�Đ����ԁA�Đ��񐔓��B
struct AnimPlayBackInfo
{
	// �Đ��A�j���[�V�����̃A�j���[�V����ID
	PlayerCharacterEntity::AnimationID m_eAnimID; 

	// ���[�V�����̃A�^�b�`�C���f�b�N�X�i MV1AttachAnim �̃��^�[���l �j
	int   m_AttachIndex;       

	// �A�j���[�V�����Đ����
	float m_MotionTotalTime;   // ���[�V�����̑��Đ�����
		// * �A�j���[�V�����؏o��ON�̏ꍇ�́AAnimUniqueInfo.m_fAnimEndTime�̒l���ݒ肳���B
		//   �؏o���A�j���[�V�����̍Đ����Ԃ�m_fAnimLength�Ɋi�[����
	
	float m_CurPlayTime;       // ���݂̍Đ�����
	float m_fBlendRate;        // �A�j���[�V�����̃u�����h���i 0.0 �` 1.0 �̊� �j
	int   m_iPlayCount;        // �iset����Ă���́j�A�j���[�V�����̍Đ���
	bool  m_bPause;            // �A�j���[�V������~�i�Î~�j�t���O
	bool  m_bFinished;         // �A�j���[�V�����I���t���O �A�j���[�V���������Đ����Ԃ܂ōĐ����ꂽ�ꍇ�ȂǁB���[�v����A�j���[�V�����̏ꍇ�� ON �ɂȂ�Ȃ��B
	bool  m_bRemoved;          // �A�j���[�V�����j���t���O ON�Ȃ�A�j���[�V�����Đ����Ȃ��B�A�j���[�V�������f�^�b�`����^�C�~���O�� ON �ɂ��Ă���
	Vector3D m_vCorrectionVec; // m_CurAttachedMotion ON �̏ꍇ�̃��[�V�����ʒu�̕␳�x�N�g���iPlayOneAnim�Ōv�Z����APlaySub�Ńu�����h�l�����Ď��ۂ̈ʒu�␳�����s����B�j
	float m_fAnimLength;       // ���̃A�j���[�V�����̍Đ��ɂ����鎞��

	// ���������\�b�h
	void init()
	{
		m_eAnimID = (PlayerCharacterEntity::AnimationID)-1;
		m_AttachIndex     = -1;
		m_MotionTotalTime = 0;
		m_CurPlayTime     = 0;
		m_fBlendRate      = 1.0;
		m_iPlayCount      = 0;
		m_bPause          = false;
		m_bFinished       = false;
		m_bRemoved        = false;
	}

	inline AnimUniqueInfo* getAnimUnqPointer()
	{
		return &(PlayerCharacterEntity::AnimUniqueInfoManager::Instance()->m_pAnimUniqueInfoContainer[m_eAnimID]);
	}

};

// #### AnimationManager::setAnim �̈������\���̂ɂ�������
// �A�j���[�V�����̗\��Ǘ��Ɏg�p����
struct ArgumentOfSetAnim
{
	PlayerCharacterEntity::AnimationID m_eAnimID; // �Z�b�g����A�j���[�V����ID
	double m_dAnimSwitchTime; // �u�����h����
	bool   m_bStopPrvAnim;    // ���݂̃A�j���[�V�����̒�~�v��

	// �R���X�g���N�^
	ArgumentOfSetAnim( PlayerCharacterEntity::AnimationID AnimID, double AnimSwitchTime, bool StopPrvAnim )
	{
		m_eAnimID         = AnimID;
		m_dAnimSwitchTime = AnimSwitchTime;
		m_bStopPrvAnim    = StopPrvAnim;
	}

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
	int   m_iModelHandle;              // MMD���f���̃n���h�� �� AnimationManager �̃C���X�^���X�𕡐������ɑΉ����邽�߁Astatic���i�b��Ώ��j
	int   m_iCenterFrameIndex;         // �u�Z���^�[�v�t���[���̃t���[���ԍ��i MV1SearchFrame �̃��^�[���l �j

	// �Đ��s�b�`�i�Đ����x�j
	float m_PlayPitch;         // �Đ��s�b�`�i�Đ����x�j

	// �A�j���[�V�����Đ������i�[
	AnimPlayBackInfo* m_pCurAnimPlayInfo; // ���ݍĐ����̃A�j���[�V����
	AnimPlayBackInfo* m_pPrvAnimPlayInfo; // �ЂƂO�ɍĐ������A�j���[�V���� ���A�j���[�V�����ؑ֒��͗������u�����h����čĐ������

	// �A�j���[�V�����u�����h�i�A�j���[�V�����ؑւ����炩�ɂ���j�֘A
	bool   m_bIsNowBlending;    // �����u�����h�����̃t���O
	double m_dBlendElapsed;     // �u�����h�J�n����̌o�ߎ���
	double m_dAnimSwitchTime;   // �A�j���[�V�����ؑւ̐ݒ莞��

	// �A�j���[�V�����\��@�\�Ɋւ���
	queue<ArgumentOfSetAnim> m_qAnimReservationQueue;

	// �p���̌X���i���񓮍쎞�ɉ��S�͂ő̂��X�����o�Ɏg�p�j
	// �����ŌX�����E�����A�������������B
	double m_dBankAngle;

	// #### �⏕���\�b�h ####
	void PlayOneAnim( double TimeElaps, Vector3D Pos, Vector3D Head, AnimPlayBackInfo* pPlayAnim ); // Entity�����Q�Ƃ����Ȃ��Œ��ڏ������w�肵��Animation��`�悷��B
	void CleanUpAnim( AnimPlayBackInfo* pAnimInfo ); // �A�j���[�V�����̌㏈�����s��
	void InitAnimPlayInfoAsAnim( AnimPlayBackInfo* pAnimInfo, PlayerCharacterEntity::AnimationID AnimID ); // ������pAnimInfo���w�肳�ꂽ�A�j���[�V�����ŏ���������
	void PlayReservedAnim(); // CurAnim���Đ��I�����������`�F�b�N���A�Đ��I�����Ă���Η\�񂳂ꂽ�A�j���[�V�������Đ��ݒ肷��B
	void setAnimMain( 
		PlayerCharacterEntity::AnimationID, 
		double AnimSwitchTime=0.0, 
		bool StopPrvAnim=true, 
		bool SyncToPrv=false 
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
	// StopPrvAnim ON �ɂ���ƁA�ʑ���ۂ��ăA�j���[�V�����ؑ֍s���B�������ARunning<->Walking�ɂ����Ή����ĂȂ�
	void setAnim( PlayerCharacterEntity::AnimationID, double AnimSwitchTime=0.0, bool StopPrvAnim=true, bool SyncToPrv=false ); 

	// ----- �W���ȊO�̊J�n���ԂŃA�j���[�V�����̃Z�b�g
	// �A�j���[�V�����̊J�n���Ԃ�W���ȊO�ōĐ����������ꍇ�B
	// �ǉ��̊J�n���Ԃ́A�A�j���[�V�������� AnimUniqueInfo.m_fAddAnimStartTime �Őݒ�B
	// ����ȊO�� setAnim �Ɠ����B�������AStopPrvAnim = true �͂��肦�Ȃ��̂ŁA��������폜�����B
	void setAnimExStartTime( PlayerCharacterEntity::AnimationID, double AnimSwitchTime=0.0, bool StopPrvAnim=true ); 

	// ----- �A�j���[�V�����̍Đ��\��B
	// �A�j���[�V�����ݒ���� AnimReservationQueue �ɃX�^�b�N����A�Đ����̃A�j���[�V�������Đ��I��������ݒ肳���B
	// �� ���荞�݂ŐV�����A�j���[�V������setAnim���ꂽ�ꍇ�́A�\�񒆂̐ݒ�͔j�������B
	void ReserveAnim( PlayerCharacterEntity::AnimationID, double AnimSwitchTime=0.0, bool StopPrvAnim=true ); 

	// ----- �A�j���[�V�����̍Đ��\����̔j��
	// �Đ��\�񂳂ꂽ�A�j���[�V������j������ = �L���[����ɂ���
	// http://qiita.com/D-3/items/9930591bb78df544c066
	void DiscardReservedAnim(){ queue<ArgumentOfSetAnim>().swap(m_qAnimReservationQueue); };

	// ----- m_pCurAnimPlayInfo�̃A�j���[�V���������擾
	string getCurAnimName(){ return m_pCurAnimPlayInfo->getAnimUnqPointer()->m_sAnimName; };

	// ----- m_pCurAnimPlayInfo�̍Đ��ɂ����鎞�Ԃ��擾
	float getCurAnimLength(){ return m_pCurAnimPlayInfo->m_fAnimLength; };

	// #### �⏕���\�b�h ####
	void  PlayMain( double TimeElaps, Vector3D Pos, Vector3D Head );
	float CurPlayTime(){ return m_pCurAnimPlayInfo->m_CurPlayTime; }
	void  DrawAllow3D( Vector3D cnt, Vector3D heading ); // ����`��
	float getMotionTotalTime(){ return m_pCurAnimPlayInfo->m_MotionTotalTime; }
	
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
	void DBG_setCurPlayTimeOfCurAnim( float time ){ m_pCurAnimPlayInfo->m_CurPlayTime = time; };
	int  DBG_getModelHandle(){ return m_iModelHandle; };

	int  DGB_m_iHairFrameIndex;    // ���t���[��
	//MATRIX DGB_m_mHairFrameMatrix;   // ���t���[����Matirix

	// �R���X�g���N�^�ŏ�������ɁA���f����ύX�������ꍇ�Ɏg�p�B�f�o�b�N�p�̎���
	// AnimManager�������C���X�^���X�������ꍇ�̓���͕ۏ؂��Ȃ����B
	void DBG_RenewModel( int ReneModelHandle ); // �X�V���������f���̃n���h����n������
	
	int DBG_m_iModelHandle_Original; // �I���W�i���̃��f���̃n���h���̑ޔ�p
	int DBG_m_iModelHandle_Physics;  // �������Z����œǂݍ��񂾃��f���̃n���h��
	int DBG_m_iModelHandle_HideHair; // ���̖э폜���폜�������f���̃n���h��

	float DBG_getCurAnimBRate(){ return m_pCurAnimPlayInfo->m_fBlendRate; };
	float DBG_getPrvAnimBRate(){ return m_pPrvAnimPlayInfo->m_fBlendRate; };

	float DBG_getCurAnimPlayTime(){ return m_pCurAnimPlayInfo->m_CurPlayTime; }; 
	float DBG_getPrvAnimPlayTime(){ return m_pPrvAnimPlayInfo->m_CurPlayTime; }; 

	bool DBG_m_bBlendPauseOn; // �A�j���[�V�����u�����f�B���O�̈ꎞ��~

	//void DBG_setAnimPlayTime( float playtime ){ m_pCurAnimPlayInfo->m_CurPlayTime = playtime; }; // �A�j���[�V������C�ӂ̍Đ����Ԃɐݒ�



};

 