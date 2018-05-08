
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

// �u���[�L�X�e�[�g�i�ؕԂ�����j
class Break : public State
{
private:
	Break() {}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	Break(const Break&);
	Break& operator=(const Break&);

	// #### �����o ####
	float m_fAnim_BreaktoTurn_PlayPitch;
	float m_fAnim_BreaktoStop_PlayPitch;

public:

	// �V���O���g��
	static Break* Instance();

	virtual void Enter(PlayerCharacterEntity*);
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation&);
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity*);
	virtual string getStateName() { return "Break"; };
};

// �ؕԂ��X�e�[�g�i�ؕԂ�����j
class Turn : public State
{
private:
	Turn() {}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	Turn(const Turn&);
	Turn& operator=(const Turn&);


public:
	// #### ����p�����[�^
	static const double TurnDulation;  // �ؕԂ��X�e�[�g�̌p������
	static const double TurnSpeed;     // �ؕԂ����̐��񑬓x

	// �V���O���g��
	static Turn* Instance();

	virtual void Enter(PlayerCharacterEntity*);
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation&);
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity*);
	virtual string getStateName() { return "Turn"; };
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
	
	// �ȉ��̓f�o�b�N�p�ɕێ�
	//Vector3D DBG_m_vDriveForceForVel;      // ���x�����̐��i�� �������Q�N�b�^�@���g�p���Ă��邽�ߐ��m�ł͂Ȃ�
	bool DBG_m_bCentripetalForceIsBounded; // ���S�͂�����ɒB��������flg �������Q�N�b�^�@���g�p���Ă��邽�ߐ��m�ł͂Ȃ�
	bool DBG_m_bTurnWithouReduceSpeed;     // ���x�𗎂Ƃ������񂩂�flg

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
	//static Vector3D calculateForce( 
	Vector3D calculateForce( 
		Vector3D vVel, 
		Vector3D vUpper,
		Vector3D vArrangeSteeringForce,
		double DriveForce,
		double eta,
		double LimitCentripetalForce,
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
	OneEightyDegreeTurn() : DBG_m_SubStateDurations( vector<float>(6,0) )
	{}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	OneEightyDegreeTurn(const OneEightyDegreeTurn&);
	OneEightyDegreeTurn& operator=(const OneEightyDegreeTurn&);

	// ##### �萔
	
	// �T�u��Ԃ̌p������
	static const double BreakPreDulation;  // �u���[�L�����iSUB_BREAK_PRE�j�̌p������
	static const double TurnRotDulation;   // �ؕԂ���]�iSUB_TURN_ROT�j�̌p������
	static const double TurnFlyDulation;   // �ؕԂ����ˁiSUB_TURN_FLY�j�̌p������

	// ����̕������f���p�����[�^
	static const double SqSlowDownEnough;  // �u���[�L���(SUB_BREAK_STAND/SUB_BREAK_TURN)�̏I�������Ŏg�p�B

	static const double TurningForceSize;  // �؏o�����̉����͑傫��
	static const double BrakingForceSize;  // �u���[�L���̐����͂̑傫��

	static const double InnerProductForStartTurn; // ���x�x�N�g���i�K�i���ς݁j�ƈړ������x�N�g���̓��ϒl�����̒l�ȉ��ł���΁A�ؕԂ��Ɣ��肷��B

	// �����o
	Vector3D m_vVelDirBeginning; // OneEightyDegreeTurn��Enter�������̑��x����
	//Vector3D m_vStickTiltBeginning; // OneEightyDegreeTurn��Enter�������̃X�e�B�b�N�̌X�����L���B�i�ؕԂ���"����"�������v�Z���鎞�Ɏg�p�j�����P�����~�߂��B�i�ؕԂ��������̕����v�Z���P������s���R����Ȃ��Ȃ����̂Łj
	Vector3D m_vTurnDestination; // �ؕԂ������"����"��������B�؏o������J�n�������̃X�e�B�b�N�̌����i�K�i���j���ݒ�
	Vector3D m_vVelEnterTurning; // �ؕԂ��J�n���̑��x���L���B�i�ؕԂ����̉����x�̌v�Z�Ɏg�p�j


	// �T�u���
	enum SubStateID
	{
		//SUB_BREAKING = 0,
		//SUB_TURNING  = 1,
		SUB_BREAK_PRE   = 0, // �u���[�L����
		SUB_BREAK_STAND = 1, // �ؕԂ��Ȃ��u���[�L
		SUB_BREAK_TURN  = 2, // �ؕԂ�����u���[�L
		SUB_TURN_ROT    = 3, // �ؕԂ���]
		SUB_TURN_FLY    = 4, // �ؕԂ�����
		SUB_TURN_FIN    = 5  // �ؕԂ����X�g�i���x�����̒����Ɏg�p�j
	};
	SubStateID m_eSubState;


	double DBG_m_dAngle;
	double DBG_m_dRemainingTime;

	// �T�u��Ԃ̌p�����Ԃ̑���
	vector<float> DBG_m_SubStateDurations;   // �T�u��Ԃ̌p�����Ԃ��L�^����z��

public:

	// �V���O���g��
	static OneEightyDegreeTurn* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );
	virtual string getStateName(){ return "OneEightyDegreeTurn"; };

	// ##### �f�o�b�O�p
	void DBG_expSubStateDurations( int &c ); // �e�T�u��Ԃ̌p�����Ԃ��o�͂���

};


// ����X�e�[�g�i��������A���蓮��j
class Run : public State
{
private:
	Run() {}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	Run(const Run&);
	Run& operator=(const Run&);

public:
	// #### ����p�����[�^
	static const double runSpeed;  // ����̍ō����x�i�X�e�B�b�N���ő���ɓ|�����Ƃ��ɷ�׸����o����ō����x�j
	//static const double turnSpeed; // ��׸��̐���\�ȍő呬�x �� TurnState �� TurnSpeed �Ƌ��p����B

	// �V���O���g��
	static Run* Instance();

	virtual void Enter(PlayerCharacterEntity*);
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation&);
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity*);
	virtual string getStateName() { return "Run"; };
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

