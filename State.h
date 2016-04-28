
#include "PlayerCharacterEntity.h"

// #### State �̊��N���X ####
class State
{
public:

	virtual ~State(){}

	// �X�e�[�g�ɓ������ۂɎ��s�����
	virtual void Enter(PlayerCharacterEntity*)=0;

	// ���͏�񓙂�]����State�X�V�̔���E���s������
	virtual void StateTranceDetect(PlayerCharacterEntity*)=0;

	// Entity��Update�Ŏ��s�����
	// ����State�ɂ�����Entity�ɓ����͂��v�Z����B
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& )=0;

	// Entity��Render�Ŏ��s�����
	virtual void Render(PlayerCharacterEntity*)=0;

	// �X�e�[�g����o��ۂɎ��s�����
	virtual void Exit(PlayerCharacterEntity*)=0;

protected:

	// #### Calculate �̃T�u���\�b�h ####
	
	// ����
	void Rotate( 
		double RotateVelSpeed, 
		PlayerCharacterEntity* pEntity, 
		PhysicalQuantityVariation& PhyVar );

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

};

// #### Standing �X�e�[�g ####
class Standing : public State
{
private:
	Standing(){}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	Standing(const Standing&);
	Standing& operator=(const Standing&);

	// �����o

public:

	// �V���O���g��
	static Standing* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );

};

// #### Running �X�e�[�g ####
class Running : public State
{
private:
	Running(){}

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	Running(const Running&);
	Running& operator=(const Running&);

	// �����o

public:

	// �V���O���g��
	static Running* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );

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