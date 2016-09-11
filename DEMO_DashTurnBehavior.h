#include <vector>
#include <list>     

#include "DxLib.h"

// ��{���C�u����
#include "Vector3D.h"
#include "Vector2D.h"

//=======================================================================//
// 2016/08/28
// �X��Entity�̎����͂ł����̂ŁA
// Entity�Ǘ��� GameWorld Class �ɒ��肷��

//=======================================================================//


// ######################## FlipbookPlayer Class ########################
// �p���p������̍Đ�
// ����Entity����̎d�l��z�肵�Ă��邽�߁A�Đ��Ɋւ����ԁi���̍Đ��t���[���Ȃǁj�͕ێ����Ȃ��B

class FlipbookPlayer
{
private:
	int m_iMaxFrame;           // �A�j���[�V�����̍ő�t���[����
	vector<int> m_hGraphArray; // �e�t���[���̉摜�f�[�^�i�n���h���j���i�[����z��

public:
	// #### �R���X�g���N�^�E�f�X�g���N�^ ####
	FlipbookPlayer( string GraphFileName, int FrameNum , int XSize, int YSize );
	
	// #### ���\�b�h ####
	void Play( Vector3D Pos, float Width, int Frame );   // �A�j���[�V�����̍Đ����s

};

// ######################## SampleEffectAnimManager Class ########################
// SampleEffectEntity�� one - to one �ŃA�j���[�V�������Ǘ����s��

class SampleEffectAnimManager
{
private:
	// #### �A�j���[�V�����Đ��Ɋւ���
	int  m_iCurPlayFrame; // ���݂̍Đ��t���[����
	bool m_bEnd; // �A�j���[�V�����̍Đ����I������

	// #### �X�̃A�j���[�V�����̓����Ɋւ�����́i�A�j���[�V�����������ɂȂ�Ȃ�΁A�p�b�P�[�W������K�v������B�j
	// �A�j���[�V�������ɃN���X�����Ƃ����p�b�P�[�W���@���ʔ�����������Ȃ��B
	// �p���������B�i���݂̍Đ��t���[���Ȃǋ��ʂ����N���X�Œ�`���A�ŗL�̃A�j���[�V���������p����Œ�`����j

	// FlipbookPlayer�C���X�^���X�ւ̃|�C���^
	static FlipbookPlayer* m_pFlipbookPlayerIns; 

	static int   m_iMaxFrameNum;     // �A�j���[�V�����̍ő�t���[����
	static string m_sGraphFileName;  // �A�j���[�V�����̉摜�f�[�^�̃p�X
	static int   m_iXSize; // �A�j���[�V�����̂P�R�}��X�T�C�Y
	static int   m_iYSize; // �A�j���[�V�����̂P�R�}��Y�T�C�Y

	bool  m_bRepeatAnimation; // �A�j���[�V�����̌J�Ԃ��v��
	float m_fAnimWidth;       // �G�t�F�N�g�̕`��T�C�Y

public:
	// #### �R���X�g���N�^�E�f�X�g���N�^ ####
	SampleEffectAnimManager(); // ���Ɉ����œn���悤�Ȃ��̂͂Ȃ�...
	~SampleEffectAnimManager();

	// #### ���\�b�h ####
	static void Initialize(); // �g�p����O�Ɉ�x�����ĂԂ��ƁB�N���X��static�I�u�W�F�N�g�iFlipbookPlayer�j�̏��������s���B
	// * GameWorld �� initialize() �ňꊇ���ČĂԂ悤�ɂ���


	void Render( Vector3D Pos ); // �A�j���[�V�����̕`��

	bool isEnd();  // �A�j���[�V�������Đ��I���������H

};

// ######################## SampleItemAnimManager Class ########################
// SampleItemEntity�� one - to one �ŃA�j���[�V�������Ǘ����s��
// �������Ə����Ă��܂����I

class SampleItemAnimManager
{
private:
	// ##### MMD���f���̃n���h���̌��{�F
    // AnimationManager �̃C���X�^���X�𕡐������ɑΉ����邽�߁A
	// MV1LoadModel �Ő�������n���h����static��class�ň���������A
	// m_iModelHandle �́Am_iModelHandleMaster �� MV1DuplicateModel �ŕ������Ďg��
	static int m_iModelHandleMaster;  // MMD���f���̃n���h���̌��{
	static int m_iAllModelHandleNum;  // �S���f���n���h����

	// #### �i���݂́j�A�j���[�V�����Ɋւ�����A�����A�ݒ� �� ####
	// ���f�����
	int   m_iModelHandle; // MMD���f���̃n���h�� �� AnimationManager �̃C���X�^���X�𕡐������ɑΉ����邽�߁Astatic���i�b��Ώ��j

	float m_fModelScale;  // ���f���̔{��

	// ���x�ȋ@�\�͎������Ȃ��B

public:
	// #### �R���X�g���N�^�E�f�X�g���N�^ ####
	SampleItemAnimManager(); // ���Ɉ����œn���悤�Ȃ��̂͂Ȃ�...
	~SampleItemAnimManager();

	// #### ���\�b�h ####
	static void Initialize(); // �g�p����O�Ɉ�x�����ĂԂ��ƁB�N���X��static�I�u�W�F�N�g�iFlipbookPlayer�j�̏��������s���B
	// * GameWorld �� initialize() �ňꊇ���ČĂԂ悤�ɂ���

	void Render( Vector3D Pos, Vector3D Head ); // �A�j���[�V�����̕`��A�������ݒ�ł���悤��

};

// ######################## SampleGameWorld Class �̃v���g�^�C�v�錾 ########################
class SampleGameWorld;

// ######################## SampleBaseGameEntity Class ########################
// SampleEffectEntity �� SampleItemEntity �i���Ƃ��S�Ă�Entity�^�C�v�́j���N���X�i�̋C���j
// ���[��A�X�J�X�J���B�܂��A�P�Ȃ�e���v���[�g�����炱��ł����̂��B

class SampleBaseGameEntity
{
private:
	// ���[��A���ɂ͂Ȃ����ȁH

protected:
	// Entity�����ł��鎞�ɂ�����t���O�BEntityManager���A���X�g����Entity��Update����Ƃ��ɎQ�Ƃ���remove����
	bool m_bExtinct;

	// Entity�̈ʒu���
	Vector3D m_vPos; // �ʒu��񂾂������N���X�Ɏ�������Ӗ��́H

	// �R���X�g���N�^
	SampleBaseGameEntity( Vector3D Pos ) : m_vPos( Pos ),
		                                   m_bExtinct( false )
	{}

public:

	// �f�X�g���N�^
	virtual ~SampleBaseGameEntity(){}

	// ��Ԃ̍X�V
	virtual void Update(double time_elapsed){}; 

	// �`��
	virtual void Render(){};


	// �A�N�Z�T��
	Vector3D Pos()const{return m_vPos;}
	bool     isExtinct()const{return m_bExtinct;}


};

// ######################## SampleItemEntity Class ########################
// �t�B�[���h�ɂ����Ă���A�C�e���I�Ȃ��́i�R�C���A��΂Ȃǁj��Entity
// - ���邭���]���Ă��鉉�o
// - ����B
// - SampleBaseGameEntity ����p��

class SampleItemEntity : public SampleBaseGameEntity
{
private:
	// GameWorld�ւ̃|�C���^
	SampleGameWorld *m_pWorld;

	// �A�j���[�V�����}�l�[�W���̃C���X�^���X�ւ̃|�C���^
	SampleItemAnimManager* m_pAnimMgr; // �R���X�g���N�^���ŃC���X�^���X��

	// ���[�J�����W
	Vector3D m_vHeading;        // Entity�̐��ʌ���
	// Vector3D m_vSide;            // Entity�̉�����
	// Vector3D m_vUpper;			// Entity�̏����

	// ���ʌ����x�N�g���̃f�t�H���g
	static const Vector2D m_vDefaultHeading2D;

	// �A�C�e���̉�]���x
	static double m_dRotSpeed;

	// �A�C�e���̉�]�p�i���W�A���j
	double m_dRotAngle;

	// m_vSide    x m_vHeading = m_vUpper
	// m_vHeading x m_vUpper   = m_vSize
	// m_Upper    x m_vSide    = m_vHeading

	// �A�C�e�����擾���ꂽ���� TouchDetection �ŏグ�� �� Update �Ń`�F�b�N���Ď��̏������s��
	bool m_bGotten;

	static double m_dBoundingRadius; // �L�����N�^�[�̐ڐG�̈�𔼌am_dBoundingRadius�A����m_dCharacterHight�̉~���Ƃ��ăA�C�e���Ƃ̐ڐG���o����B
	static double m_dCharacterHight; 
	static double m_dSqBoundingRadius; // m_dBoundingRadius �̂Q��

public:
	// �R���X�g���N�^
	SampleItemEntity( SampleGameWorld *world, Vector3D Pos ); 

	// �f�X�g���N�^
	~SampleItemEntity(); // AnimMgr���폜���邱�Ɓ�

	// ��Ԃ̍X�V
	void Update(double time_elapsed); // �A�C�e�����A���邭���]����

	// �`��
	void Render();

	// �v���[���[�L�����N�^�Ƃ̐ڐG���o�� Gotten �t���O���グ��
	bool TouchDetection( Vector3D CharaPos );

	static bool   m_bExpBoundingRadius; // m_dBoundingRadius ��\������

	// �A�N�Z�T��
	Vector3D Heading()const{return m_vHeading; }
	// Vector3D Side()   const{return m_vSide;    }
	// Vector3D Uppder() const{return m_vUpper;   }

};

// ######################## SampleEffectEntity Class ########################
// �A�C�e�����������̗]�C�̃G�t�F�N�g
// ���̏��Effect�A�j���[�V�������쐬���āA���ł���B

class SampleEffectEntity : public SampleBaseGameEntity
{
private:
	// GameWorld�ւ̃|�C���^
	SampleGameWorld *m_pWorld;

	// �A�j���[�V�����}�l�[�W���̃C���X�^���X�ւ̃|�C���^
	SampleEffectAnimManager* m_pAnimMgr; // �R���X�g���N�^���ŃC���X�^���X��

public:
	// �R���X�g���N�^
	SampleEffectEntity( SampleGameWorld *world, Vector3D Pos );

	// �f�X�g���N�^
	~SampleEffectEntity();

	// ��Ԃ̍X�V
	void Update(double time_elapsed); // Effect�A�j���[�V�����̍Đ������������玩�ȏI������B
	// �`��
	void Render();

	// �A�N�Z�T��

};

// ����SampleGameWorld Class�̎���
// �悸�́A�A�C�e���ƃG�t�F�N�g�̓������݂邽�߂Ɏb��I�Ȏ����ɂ���B


// ######################## SampleGameWorld Class �̒�` ########################

// �Q�[����̑SEntity�̊Ǘ����s���N���X

class SampleGameWorld
{
private:
	
	// World �ɑ��݂��邷�ׂĂ�Entity�̃��X�g�B
	list<SampleBaseGameEntity*> m_pAllEntityList;

	// �A�C�e��Entity�݂̂̃��X�g
	list<SampleItemEntity*> m_pIttemEntityList;

public:
	// �R���X�g���N�^
	SampleGameWorld();

	// ������
	static void Initialize(); // DXlib��������������łȂ��Ƃł��Ȃ����������������{����B�摜�f�[�^�̃������ւ̃��[�h�Ƃ��A���f���̃��[�h�Ƃ��B

	// �X�V
	void Update( double time_elapsed, Vector3D CharaPos );

	// �`��
	void Render();

	// SampleItemEntity ��o�^
	void RegisterSampleItemEntity( SampleItemEntity* );

	// SampleEffectEntity ��o�^
	void RegisterSampleEffectEntity( SampleEffectEntity* );

	// �A�C�e���z�u��`�r�b�g�}�b�v��ǂݍ��݃t�B�[���h�ɃA�C�e���z�u���s��
	void SetItemsToWorld( double Separation , double ItemHight, char *BitMapFileName );

};







