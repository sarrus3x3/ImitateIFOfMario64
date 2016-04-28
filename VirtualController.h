
#include <vector>

#include "DxLib.h"

#include "Vector3D.h"
#include "Vector2D.h"

#pragma once

// �R���g���[���[�i�W���C�p�b�h�j�Ǘ��N���X
class VirtualController
{
private:
	// �R���g���[���[�̕`��Ŏg�p������
	double controllerscale;
	double ButtonRad; // �{�^���̔��a
	Vector2D ControllerBodyTL;  // �R���g���[���[➑̂̍���p�̈ʒu
	Vector2D ControllerBodyBR;  // �R���g���[���[➑̂̉E���p�̈ʒu
	Vector2D ControllerBotA;    // �{�^��A�̈ʒu�i���j
	Vector2D ControllerBotB;    // �{�^��B�̈ʒu�i�E�j
	Vector2D ControllerBotY;    // �{�^��Y�̈ʒu�i���j
	Vector2D ControllerBotX;    // �{�^��X�̈ʒu�i��j
	std::vector<Vector2D> CrossKeyOutLine;  // �\���L�[�̗֊s

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

public:

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

	// �R���g���[���[�̓��͏�Ԃ�ێ�����ϐ�
	Button ButA;
	Button ButB;
	Button ButX;
	Button ButY;
	int Virti; // ��A���̕����L�[�̏��
	int Horiz; // �E�A���̕����L�[�̏��

	// �R���X�g���N�^
	VirtualController();

	// �R���g���[���[��ԁi���͏�ԁj���X�V����
	void Update();

	// �R���g���[���[��`�悷��
	void Render( Vector2D pos );


};

