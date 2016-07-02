
#include "Vector3D.h"
#include "Vector2D.h"

// �C���N���[�h����ƃR���p�C���G���[
//#include "State.h"
//#include "AnimationManager.h"
//#include "VirtualController.h"

#pragma once

class State;
class AnimationManager;
struct AnimUniqueInfo;
class VirtualController;

// #### �����ʂ̕ω����`����\���́iState::Calculate�̌��ʓn���j ####
struct PhysicalQuantityVariation
{
	Vector3D Force;   // ��
	Vector3D VelVar;  // ���x�̕ψʁi�P�ʎ��Ԃ�����j
	Vector3D PosVar;  // �ʒu�̕ψʁi�P�ʎ��Ԃ�����j
	Vector3D DstVar;  // �����̕ψʁi�P�ʎ��Ԃ�����j
	bool UseVelVar;   // VelVar �g�p����flg
	bool UsePosVar;   // PosVar �g�p����flg

	// ���������\�b�h
	void init()
	{
		Force  = Vector3D(0,0,0);
		VelVar = Vector3D(0,0,0);
		PosVar = Vector3D(0,0,0);
		DstVar = Vector3D(0,0,0);
		UseVelVar = false;
		UsePosVar = false;
	};
};

// ################## PlayerCharacterEntity #######################
class PlayerCharacterEntity
{
private:
	// #### GameWorld �֘A ####
	double m_dTimeElapsed;           // �^�C���X���C�X�iUpdate�ōX�V�j

	// #### �X�e�[�g�Ǘ��֘A ####
	State* m_pCurrentState;    // ���݂� State
	State* m_pPreviousState;   // �P�O�� State

public:
	// #### �X�e�[�g�ɕR�t����Entity�̏�Ԃ�ێ����郁���o�ϐ� ####
	// �W�����v�`���[�W
	bool   m_bJmpChrgFlg;	   // �W�����v�`���[�W�����ۂ���flg

	// �ړ����x��
	enum MoveLevelID
	{
		MvLvWalking = 0,
		MvLvRunning = 1
	} m_eMoveLevel;

private:
	// #### ������� ####
	Vector3D m_vPos;            // Entity�̈ʒu
	Vector3D m_vVelocity;       // Entity�̑��x
	Vector3D m_vHeading;        // Entity�̌���
	Vector3D m_vSide;           // Entity�̃��[�J�����W�ɂ����鉡��������
	Vector3D m_vUpper;			// Entity�̃��[�J�����W�ɂ���������
	double   m_dMass;           // Entity�̎���
	double   m_dBoundingRadius; // Entity�̃o�E���f�B���O���a

	// #### Collision�֘A ####

public:
	// #### �R���X�g���N�^�E�f�X�g���N�^ ####
	PlayerCharacterEntity(
		Vector3D vPos,
		Vector3D vVelocity,
		Vector3D vHeading,
		Vector3D vSide,
		double   dMass,
		double   dBoundingRadius,
		VirtualController* pVirCntrl
		); 
	// ~PlayerCharacterEntity(); // ���������

	// #### GameWorld �֘A ####
	VirtualController* m_pVirCntrl;  // ���z�R���g���[���[�ւ̃|�C���^

	// #### Anmation�Ǘ� ####
	AnimationManager* m_pAnimMgr; // AnimationManager�̃C���X�^���X�ւ̃|�C���^

	// #### ���\�b�h ####
	virtual void Update(double time_elapsed); // Entity�̏�Ԃ��X�V
	virtual void Render(); // Entity��`��
	void ChangeState( State* ); // �ێ����Ă���State���X�V����

	// #### Anmation����enum���` ####
	enum AnimationID
	{
		NoAnim   = 0,  // �A�j���[�V�����ݒ�Ȃ������̃A�j���[�V�����Đ���ɂ͐ݒ�ł��Ȃ��H�i�S�ẴA�j���[�V�������f�^�b�`���Ă��A�A�j���[�V����������������Ȃ�DX���C�u�����̎d�l�H�j
		Standing = 1,  // ����
		Walking  = 2,  // ����
		Running  = 3,  // ����
		Jumping  = 4,  // �W�����v
		Jump_PreMotion = 5, // �W�����v�O��"����"�̃��[�V����
		Jump_Ascent    = 6, // �W�����v�㏸��
		Jump_Descent   = 7, // �W�����v���~��
		Jump_Landing   = 8,  // �W�����v��̒��n
		Jump_Landing_Short = 9,  // �W�����v��̒��n�i�Z���^�[�ʒu�������Ƃ�������Ƃ���܂ŁB�W�����v���n������o���̃A�j���[�V�����Ŏg�p���邽�߁j
		DBG_HairUp = 10
	};

	static const int m_iAnimIDMax=11;

	// #### �A�j���[�V�����ŗL���Ǘ��N���X
	// �S�ẴA�j���[�V�����ŗL��񂪊i�[���ꂽ�R���e�i���Ǘ����邽�߂̃V���O���g��
	class AnimUniqueInfoManager
	{
	private:
		AnimUniqueInfoManager();

		// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
		AnimUniqueInfoManager(const AnimUniqueInfoManager&);
		AnimUniqueInfoManager& operator=(const AnimUniqueInfoManager&);

	public:
		// �V���O���g��
		static AnimUniqueInfoManager* Instance();

		// �S�ẴA�j���[�V�����ŗL��񂪊i�[���ꂽ�R���e�i
		AnimUniqueInfo* m_pAnimUniqueInfoContainer;
	
	};

	// #### �A�N�Z�T�� ####
	// �Q�ƌn
	double   TimeElaps()const{return m_dTimeElapsed;}
	Vector3D Pos()const{return m_vPos;}
	Vector3D Velocity()const{return m_vVelocity;}
	double   Mass()const{return m_dMass;}
	Vector3D Side()const{return m_vSide;}
	Vector3D Uppder()const{return m_vUpper;}
	double   Speed()const{return m_vVelocity.len();}
	double   SpeedSq()const{return m_vVelocity.sqlen();}
	Vector3D Heading()const{return m_vHeading;}
	double   BRadius()const{return m_dBoundingRadius;}

	// �^����ꂽ�X�e�[�g��PrvState����v���邩���`�F�b�N�i��v���遨true�j
	bool isMatchPrvState( State *pCheckState ){ return pCheckState==m_pPreviousState; }

	// �ݒ�n
	void    setTimeElaps( double telaps ){ m_dTimeElapsed=telaps; }
	void    setVelocity( Vector3D newVel ){ m_vVelocity=newVel; };
	void    setPos( Vector3D newPos ){ m_vPos=newPos; };

	// #### �^�C�}�[�� ####
private:
	double m_dStopWatchCounter;
	
public:
	void   StopWatchOn(){ m_dStopWatchCounter=0; }; // �X�g�b�v�E�H�b�`�I��
	double getStopWatchTime(){ return m_dStopWatchCounter; }; // �X�g�b�v�E�H�b�`�̑��莞�Ԃ��擾

	// #### �b�� ####
	bool m_bTouchGroundFlg;  // �ڒnflg
	bool m_bJmpChrgUsageFlg; // �W�����v�`���[�W�ŃW�����v�͂𒲐�����IF���g�p����Flg

	double DBG_m_dDBG;

};