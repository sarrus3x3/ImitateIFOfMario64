
#include <vector>

#include "DxLib.h"

#include "Vector3D.h"
#include "Vector2D.h"

#pragma once

// �R���g���[���[�i�W���C�p�b�h�j�Ǘ��N���X
class VirtualController
{
public:
	// ##### �R���t�B�O�l
	// �������́AVirtualController.cpp ��
	static const int    iANALOG_STICK_TILT_UP_LIMIT;           // GetJoypadXInputState �Ŏ擾����A�i���O�X�e�B�b�N�̌X���̍ő�l
	static const double dANALOG_STICK_TILT_IGNORE_THRESHOLD;   // �A�i���O�X�e�B�b�N�̌X�����\���ɏ������Ȃ����Ƃ���0�Ƃ݂Ȃ�臒l
	
	// ##### ���̓f�o�C�X���[�h

	// ���̓f�o�C�X���[�h�iCameraWorkManager���Q�l�Ɂj
	enum InputDeviceID
	{
		ID_KeyBord = 0, // �������l
		ID_GamePad = 1
	};

	// ���̓f�o�C�X���
	InputDeviceID m_eInpuDeviceMode;

	// ���̓f�o�C�X����@�\�i�Q�[���p�b�h���ڑ�����Ă���΁A���̓f�o�C�X���Q�[���p�b�h�ɃZ�b�g����j
	void CheckAndSetGamePadMode();

private:
	// ##### �R���g���[���[�̕`��Ŏg�p������
	double controllerscale;
	double ButtonRad; // �{�^���̔��a
	Vector2D ControllerBodyTL;  // �R���g���[���[➑̂̍���p�̈ʒu
	Vector2D ControllerBodyBR;  // �R���g���[���[➑̂̉E���p�̈ʒu
	Vector2D ControllerBotA;    // �{�^��A�̈ʒu�i���j
	Vector2D ControllerBotB;    // �{�^��B�̈ʒu�i�E�j
	Vector2D ControllerBotY;    // �{�^��Y�̈ʒu�i���j
	Vector2D ControllerBotX;    // �{�^��X�̈ʒu�i��j

	// �\���L�[�֘A
	Vector2D CrossKeyCenter;     // �\���L�[�̒��S
	double   CrossKeyAreaRadius; // �\���L�[�̈攼�a
	std::vector<Vector2D> CrossKeyOutLine;  // �\���L�[�̗֊s
	std::vector<Vector2D> CrossKeyTriangleUp;     // �\���L�[��L�[���
	std::vector<Vector2D> CrossKeyTriangleDown;   // �\���L�[���L�[���
	std::vector<Vector2D> CrossKeyTriangleLeft;   // �\���L�[���L�[���
	std::vector<Vector2D> CrossKeyTriangleRight;  // �\���L�[�E�L�[���

	int CntBodyColor;  // �{�̂̐F
	int CrossKeyColor; // �\���L�[�̐F
	int BotAColor;     // A�{�^���̐F
	int BotBColor;     // B�{�^���̐F
	int BotXColor;     // Y�{�^���̐F
	int BotYColor;     // X�{�^���̐F

	// �\���L�[�̉�����Ԃ̕⏕���
	int UpPushCnt;
	int DownPushCnt;
	int LeftPushCnt;
	int RightPushCnt;

	// ##### �������\�b�h
	void UpdateAsKeyBord(); // KeyBord�̏ꍇ��Update
	void UpdateAsGamePad(); // GamePad�̏ꍇ��Update

	// �X�e�B�b�N�̌X�����\���ɏ������ꍇ�� 0 �ɃJ�b�g�I�t����
	void RoundingTinyStickTilt();


public:

	// �A�i���O�X�e�B�b�N��ԕ`��̂��߃N���X
	class RenderStickTrajectory
	{
	public:
		// �O�Պi�[
		std::vector<Vector2D> TrajectoryList; 
		int CurIndex; // TrajectoryList�ɂ����錻�݂�index
		
		//Point2D CentRender; // �`�撆�S
		//int CanvasSize;

		Vector2D m_vCenterPos; // �`�撆�S
		double   m_dRadius;

		// �R���X�g���N�^
		RenderStickTrajectory() : TrajectoryList(5), CurIndex(0) {};
		RenderStickTrajectory( Vector2D CntPos, double Radius ) : 
			m_vCenterPos( CntPos ), 
			m_dRadius( Radius ), 
			TrajectoryList(5), 
			CurIndex(0) {};


		// �`��
		void Render( Vector2D CurStickPos, Vector2D RendPos );

	} m_RenderStickL, m_RenderStickR;
	
	class Button
	{
	public:
		// ���̃����o�ւ̃A�N�Z�X�X�R�[�v��e�N���X�iVirtualController�j�Ɍ��肷�邱�Ƃ��ł��Ȃ�
		int ButCounter;

		//�R���X�g���N�^
		Button() : ButCounter(0) {};

		// �{�^���͉�����Ă���
		bool isPushed()
		{
			if( ButCounter>0 ) return true;
			return false;
		};

		// �{�^���͂܂��ɂ��̃^�C�~���O�ŉ����ꂽ
		bool isNowPush()
		{
			if( ButCounter==1 ) return true;
			return false;
		};
	};

	// ##### �R���g���[���[�̓��͏�Ԃ�ێ�����ϐ�

	// ���W�N�[���R���g���[���̐����
	XINPUT_STATE m_XinputState;
	
	// �{�^����
	Button ButA;
	Button ButB;
	Button ButX;
	Button ButY;

	Button ButLB; // LB
	Button ButRB; // RB

	// �\���L�[
	int Virti; // ��A���̕����L�[�̏��
	int Horiz; // �E�A���̕����L�[�̏��

	// �A�i���O�X�e�B�b�N���
	Vector2D m_vStickL; // �A�i���O�X�e�B�b�N��
	Vector2D m_vStickR; // �A�i���O�X�e�B�b�N�E
	double m_dStickL_len; // �A�i���O�X�e�B�b�N���̌X���傫�� �iUpdate�ōX�V�A����臒l�K�p�ς݁j
	double m_dStickR_len; // �A�i���O�X�e�B�b�N�E�̌X���傫�� �i����j

	// ���X�e�B�b�N�X�����Ɋւ��闯�ӎ�����
	// �E�傫���ő�l=1.0�ɋK�i���A�������Am_vStickL.lem()<1 ���Ƃ�����B�i���͎��̂̑傫�������~�b�g32767�𒴂��邱�Ƃ����邽�߁j

	// �R���X�g���N�^
	VirtualController();

	// �R���g���[���[��ԁi���͏�ԁj���X�V����
	void Update( double time_elapsed );

	// �R���g���[���[��`�悷��
	void Render( Vector2D pos );

	// ##### �f�o�b�O�p

	// m_XinputState �̏�Ԃ��f�o�b�N�o��
	void DBG_ShowXinputState();


	// ##### ���쌟�؂̂��߂̎�������@�\
	// ����̌��ؗp�ɁA���O�ɐ݌v��������(����)�𗬂����ދ@�\

	//��������@�\��ON / OFF��ݒ肷�郁�\�b�h
	void AutoControlOnOff();
	//�EOn�̂Ƃ��͏�����
	//- TimeSinceAutoControlStart �� 0�N��

	//��������@�\�̗L����Ԃ��Ǘ����邽�߂̃t���O
	bool m_bAutoControlFlg;

	//����������^�C���X�X���C�X�ōX�V
	void UpdateAutoControl(double TimeElaps);

	//��������J�n���Ă���̌o�ߎ���
	double m_dTimeSinceAutoControlStart;

	// �e�ϐ��̏�������Y�ꂸ�ɁB


};

