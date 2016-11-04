
#include "PlayerCharacterEntity.h"

// #### State �̊��N���X ####
class State
{
public:

	virtual ~State(){}

	// �X�e�[�g�ɓ������ۂɎ��s�����
	virtual void Enter(PlayerCharacterEntity*)=0;

	// �X�e�[�g����o��ۂɎ��s�����
	virtual void Exit(PlayerCharacterEntity*)=0;

	// State�J�ڂɊւ��鏈��������i���͏�񓙂�]����State�X�V�̔���E���s������j
	virtual void StateTranceDetect(PlayerCharacterEntity*)=0;

	// Entity�̉^������������i����State�ɂ�����Entity�ɓ����͂��v�Z����j
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& )=0;

	// Entity�̕`�����������
	virtual void Render(PlayerCharacterEntity*)=0;

	// ������State�̖��O���擾����
	virtual string getStateName()=0;


protected:
	
};

// #### �_�~�[�X�e�[�g�i�f�o�b�O�p�Ɏg�p�j ####
class Dammy : public State
{
private:
	Dammy(){}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	Dammy(const Dammy&);
	Dammy& operator=(const Dammy&);

	// �����o

public:

	// �V���O���g��
	static Dammy* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );
	virtual string getStateName(){ return "Dammy"; };
};

// #### Standing �X�e�[�g ####
class Standing : public State
{
private:
	Standing(){}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	Standing(const Standing&);
	Standing& operator=(const Standing&);

public:

	// �V���O���g��
	static Standing* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );
	virtual string getStateName(){ return "Standing"; };


};

// #### Jump �X�e�[�g ####
class Jump : public State
{
private:
	Jump(){}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	Jump(const Jump&);
	Jump& operator=(const Jump&);

	// #### �����o ####

	// �W�����v�T�C�Y�̎���ID
	enum JumpSizeID
	{
		Small = 0,
		Middl = 1,
		Large = 2
	};
	
	JumpSizeID m_eJumpSize;

	// State�ɓ��������߂�Calculate�ŃW�����v���邽�߂̌��͂������邽�߂̃t���O
	bool m_bInitImplusivForce; 

	bool m_bDescenting;      // �����t���O

public:

	// �V���O���g��
	static Jump* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );
	virtual string getStateName(){ return "Jump"; };


};


// #### SurfaceMove �X�e�[�g ####
class SurfaceMove : public State
{
private:
	SurfaceMove(){}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	SurfaceMove(const SurfaceMove&);
	SurfaceMove& operator=(const SurfaceMove&);

	// ##### �O���[�o���萔
	static const double ThresholdSticktiltRunToWark;   // Running<->Warking �̃X�e�B�b�N�X����臒l
	static const double MaxCentripetalForce;   // ���񎞂̍ő���S��
	static const double ViscousRsisInert;  // �������i���̔S����R�W��

public:
	static const double ThresholdSpeedRunToWark;  // Running<->Warking �̑��x��臒l�i�����l�j
	static const double MaxVelocity; // �L�����N�^�[�̍ő呬�x�i�X�e�B�b�N��max�܂œ|�������̍ő呬�x�j
	static const double ViscousRsisAccel;  // �������̔S����R�W��


	// ##### �����o�ϐ�
private:
	double m_dCentripetalForce; // ���S�͂̑傫�� �i���S�͂ɂ��o���N���o�̂��߂Ɏg�p�j

public:

	// �V���O���g��
	static SurfaceMove* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );
	virtual string getStateName(){ return "SurfaceMove"; };



	// �⏕
	static Vector3D calculateForce( 
		Vector3D vVel, 
		Vector3D vUpper,
		Vector3D vArrangeSteeringForce,
		double DriveForce,
		double eta,
		double &CentripetalForce // �v�Z�������S�͂�Ԃ�
		);

	// #### �f�o�b�N�p ####

	Vector3D DBG_m_vSteeringForce;

};

// #### OneEightyDegreeTurn �X�e�[�g ####
// �_�b�V������̐؂�Ԃ��A�}�����]���B= 180���x�����]��
class OneEightyDegreeTurn : public State
{
private:
	OneEightyDegreeTurn(){}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	OneEightyDegreeTurn(const OneEightyDegreeTurn&);
	OneEightyDegreeTurn& operator=(const OneEightyDegreeTurn&);

	// ##### �萔
	//static const double SlowDownEnough;    // ���̑��x�ȉ��ɂȂ�����u���[�L��ԁ��ؕԂ���ԂɈڍs����
	static const double TurningDulation;   // �ؕԂ���Ԃ̌p������
	static const double BrakingDulation;   // �u���[�L��Ԃ̌p������

	static const double TurningForceSize;  // �؏o�����̉����͑傫��
	static const double BrakingForceSize;  // �u���[�L���̐����͂̑傫��

	//static const double MaxVelocity;       // �L�����N�^�[�̍ő呬�x�i�X�e�B�b�N��max�܂œ|�������̍ő呬�x�j
	//static const double ViscousRsisTurn;   // �i�ؕԂ����́j�S����R�W��
	//static const double ViscousRsisBreak;  // �i�u���[�L���́j�S����R�W��

	static const double InnerProductForStartTurn; // ���x�x�N�g���i�K�i���ς݁j�ƈړ������x�N�g���̓��ϒl�����̒l�ȉ��ł���΁A�ؕԂ��Ɣ��肷��B


	// �����o
	Vector3D m_vVelDirBeginning; // OneEightyDegreeTurn��Enter�������̑��x����
	//Vector3D m_vStickTiltBeginning; // OneEightyDegreeTurn��Enter�������̃X�e�B�b�N�̌X�����L���B�i�ؕԂ���"����"�������v�Z���鎞�Ɏg�p�j�����P�����~�߂��B�i�ؕԂ��������̕����v�Z���P������s���R����Ȃ��Ȃ����̂Łj
	Vector3D m_vTurnDestination; // �ؕԂ������"����"��������B�؏o������J�n�������̃X�e�B�b�N�̌����i�K�i���j���ݒ�
	Vector3D m_vVelEnterTurning; // �ؕԂ��J�n���̑��x���L���B�i�ؕԂ����̉����x�̌v�Z�Ɏg�p�j


	// �T�u���
	enum SubStateID
	{
		SUB_BREAKING = 0,
		SUB_TURNING  = 1
	};
	SubStateID m_eSubState;

	// ##### �f�o�b�O�p

	double DBG_m_dAngle;
	double DBG_m_dRemainingTime;

public:

	// �V���O���g��
	static OneEightyDegreeTurn* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );
	virtual string getStateName(){ return "OneEightyDegreeTurn"; };

};

/*

class DraggingViewWindow : public State
{
private:

	DraggingViewWindow(){}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	DraggingViewWindow(const DraggingViewWindow&);
	DraggingViewWindow& operator=(const DraggingViewWindow&);

	// �����o
	RealVector m_clickpt;	// �h���b�O�J�n���ɃN���b�N�����ꏊ
	bool m_draging;			// �h���b�O�����H
	RealVector m_viewloc;	// �\����ʂ̈ʒu
	RealVector m_viewvel;	// �\����ʂ̑��x

public:

	// �V���O���g��
	static DraggingViewWindow* Instance();
  
	virtual void Enter(DrawGraphMgr* );
	virtual void Execute(DrawGraphMgr* );
	virtual void Exit(DrawGraphMgr* );

	virtual void click( Point& );
	virtual void unclk( Point& );

};

class DraggingNodeMove: public State
{
private:

	DraggingNodeMove(){}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	DraggingNodeMove(const DraggingNodeMove&);
	DraggingNodeMove& operator=(const DraggingNodeMove&);

public:

	// �V���O���g��
	static DraggingNodeMove* Instance();
  
	virtual void Enter(DrawGraphMgr* );
	virtual void Execute(DrawGraphMgr* );
	virtual void Exit(DrawGraphMgr* );

	virtual void click( Point& );
	virtual void unclk( Point& );

};

// �}�E�X����̃N���b�N���󂯎~�߁A�eState�֐U�蕪�����s���B
class WatingForMouseInpt: public State
{
private:

	WatingForMouseInpt(){}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	WatingForMouseInpt(const WatingForMouseInpt&);
	WatingForMouseInpt& operator=(const WatingForMouseInpt&);

public:

	// �V���O���g��
	static WatingForMouseInpt* Instance();
  
	virtual void Enter(DrawGraphMgr* );
	virtual void Execute(DrawGraphMgr* );
	virtual void Exit(DrawGraphMgr* );

	virtual void click( Point& );
	virtual void unclk( Point& );

};

*/

