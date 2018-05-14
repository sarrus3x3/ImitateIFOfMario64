#include <fstream> 
#include <sstream>  //�J���}�ŋ�؂邽�߂ɕK�v
#include <string>

#include "VirtualController.h"

#include "MyUtilities.h" // �R���g���[���[�`��̂��߂̃c�[���g�p

const int    VirtualController::iANALOG_STICK_TILT_UP_LIMIT = 32767;        // GetJoypadXInputState �Ŏ擾����A�i���O�X�e�B�b�N�̌X���̍ő�l
const double VirtualController::dANALOG_STICK_TILT_IGNORE_THRESHOLD = 0.1; // �A�i���O�X�e�B�b�N�̌X�����\���ɏ������Ȃ����Ƃ���0�Ƃ݂Ȃ�臒l 

// ################## VirtualController �N���X�̃����o�E���\�b�h�̒�` #######################

VirtualController::VirtualController() : 
	Virti(0), 
	Horiz(0), 
	UpPushCnt(0), 
	DownPushCnt(0), 
	LeftPushCnt(0), 
	RightPushCnt(0),
	m_eInpuDeviceMode( ID_KeyBord ),
	m_bAutoControlFlg(false),
	m_dTimeSinceAutoControlStart(0),
	m_eRecodeReplayState(ID_DoNothing),
	m_dRecodeReplayElapsed(0)

{
	// ���e��ݒ�
	controllerscale = 8.0;

	// �\���L�[�̒��S
	CrossKeyCenter = controllerscale * Vector2D( 2.0, 2.0 );

	// �\���L�[�̈攼�a
	CrossKeyAreaRadius = controllerscale * 1.5;

	// �\���L�[�̗֊s
	CrossKeyOutLine.resize(12);
	CrossKeyOutLine[0]  = controllerscale * Vector2D( 1.5, 0.5 );
	CrossKeyOutLine[1]  = controllerscale * Vector2D( 1.5, 1.5 );
	CrossKeyOutLine[2]  = controllerscale * Vector2D( 0.5, 1.5 );
	CrossKeyOutLine[3]  = controllerscale * Vector2D( 0.5, 2.5 );
	CrossKeyOutLine[4]  = controllerscale * Vector2D( 1.5, 2.5 );
	CrossKeyOutLine[5]  = controllerscale * Vector2D( 1.5, 3.5 );
	CrossKeyOutLine[6]  = controllerscale * Vector2D( 2.5, 3.5 );
	CrossKeyOutLine[7]  = controllerscale * Vector2D( 2.5, 2.5 );
	CrossKeyOutLine[8]  = controllerscale * Vector2D( 3.5, 2.5 );
	CrossKeyOutLine[9]  = controllerscale * Vector2D( 3.5, 1.5 );
	CrossKeyOutLine[10] = controllerscale * Vector2D( 2.5, 1.5 );
	CrossKeyOutLine[11] = controllerscale * Vector2D( 2.5, 0.5 );
	
	// �\���L�[��L�[���
	CrossKeyTriangleUp.resize(3);
	CrossKeyTriangleUp[0] = CrossKeyOutLine[1];
	CrossKeyTriangleUp[1] = CrossKeyOutLine[10];
	CrossKeyTriangleUp[2] = 0.5*(CrossKeyOutLine[0]+CrossKeyOutLine[11]);

	// �\���L�[���L�[���
	CrossKeyTriangleDown.resize(3);
	CrossKeyTriangleDown[0] = CrossKeyOutLine[4];
	CrossKeyTriangleDown[1] = CrossKeyOutLine[7];
	CrossKeyTriangleDown[2] = 0.5*(CrossKeyOutLine[5]+CrossKeyOutLine[6]);

	// �\���L�[���L�[���
	CrossKeyTriangleLeft.resize(3);
	CrossKeyTriangleLeft[0] = CrossKeyOutLine[1];
	CrossKeyTriangleLeft[1] = CrossKeyOutLine[4];
	CrossKeyTriangleLeft[2] = 0.5*(CrossKeyOutLine[2]+CrossKeyOutLine[3]);

	// �\���L�[�E�L�[���
	CrossKeyTriangleRight.resize(3);
	CrossKeyTriangleRight[0] = CrossKeyOutLine[10];
	CrossKeyTriangleRight[1] = CrossKeyOutLine[7];
	CrossKeyTriangleRight[2] = 0.5*(CrossKeyOutLine[9]+CrossKeyOutLine[8]);

	// �R���g���[���[➑�
	ControllerBodyTL = controllerscale * Vector2D( 0.0, 0.0 );
	ControllerBodyBR = controllerscale * Vector2D( 12.0, 6.5 );

	// �{�^��
	double CrossToBottLen = 8.0;
	Vector2D BotShift( CrossToBottLen-4.0, 0.0 ); 
	ControllerBotA	= controllerscale * ( BotShift + Vector2D( 6.0, 3.0 ));
	ControllerBotB	= controllerscale * ( BotShift + Vector2D( 7.0, 2.0 ));
	ControllerBotY	= controllerscale * ( BotShift + Vector2D( 5.0, 2.0 ));
	ControllerBotX	= controllerscale * ( BotShift + Vector2D( 6.0, 1.0 ));
	
	ButtonRad = controllerscale * 0.5; // �{�^���̔��a

	// �A�i���O�X�e�B�b�N
	m_RenderStickL.m_dRadius = controllerscale * 1.5;
	m_RenderStickR.m_dRadius = controllerscale * 1.5; 

	double len = 3.5*0.5*sqrt(2.0);
	Vector2D Shift;
	Shift =  Vector2D( 2.0, 2.0 );
	Shift += Vector2D( len, len );
	m_RenderStickL.m_vCenterPos = controllerscale * Shift;
	
	Shift =  Vector2D( 2.0, 2.0 );
	Shift += Vector2D( CrossToBottLen, 0.0 );
	Shift += Vector2D( -len, len );
	m_RenderStickR.m_vCenterPos = controllerscale * Shift;

	// �F�̐ݒ�
	CntBodyColor = GetColor( 220, 220, 220 );
	CrossKeyColor = GetColor( 105, 105, 105 );
	BotAColor = GetColor( 255,   0,   0 );
	BotBColor = GetColor( 255, 255,   0 );
	BotXColor = GetColor(   0,   0, 255 );
	BotYColor = GetColor(   0, 255,   0 );

};

void VirtualController::Update( double time_elapsed )
{
	switch( m_eInpuDeviceMode )
	{
	case ID_KeyBord:
		UpdateAsKeyBord();
		break;
	case ID_GamePad:
		UpdateAsGamePad();
		if (m_bAutoControlFlg) UpdateAutoControl(time_elapsed);
		break;
	}

	// �R���g���[������̋L�^���Đ��@�\ �̏���
	switch (m_eRecodeReplayState)
	{
	case ID_Recoding:
		Update_RecodeControl(time_elapsed);
		break;
	case ID_Replaying:
		Update_ReplayRecodedControl(time_elapsed);
		break;
	}

}

void VirtualController::UpdateAsKeyBord()
{
	// �S�ẴL�[��Ԃ��擾
	char KeyStateBuf[ 256 ] ;
	GetHitKeyStateAll( KeyStateBuf ) ;

	// �\���L�[�̏���
	if( KeyStateBuf[KEY_INPUT_UP]     ){ UpPushCnt++;    }
	else{ UpPushCnt=0;    } 
	if( KeyStateBuf[KEY_INPUT_DOWN]   ){ DownPushCnt++;  }
	else{ DownPushCnt=0;  }
	if( KeyStateBuf[KEY_INPUT_LEFT]   ){ LeftPushCnt++;  }
	else{ LeftPushCnt=0;  }
	if( KeyStateBuf[KEY_INPUT_RIGHT]  ){ RightPushCnt++; }
	else{ RightPushCnt=0; }

	// ��������
	if( KeyStateBuf[KEY_INPUT_UP] && KeyStateBuf[KEY_INPUT_DOWN] )
	{
		if( UpPushCnt  ==1 ){ Virti= 1; }
		if( DownPushCnt==1 ){ Virti=-1; }
	}
	else if( KeyStateBuf[KEY_INPUT_UP]   ){ Virti= 1; }
	else if( KeyStateBuf[KEY_INPUT_DOWN] ){ Virti=-1; }
	else{ Virti = 0; }

	// ��������
	if( KeyStateBuf[KEY_INPUT_LEFT] && KeyStateBuf[KEY_INPUT_RIGHT] )
	{
		if( RightPushCnt==1 ){ Horiz= 1; }
		if( LeftPushCnt ==1 ){ Horiz=-1; }
	}
	else if( KeyStateBuf[KEY_INPUT_RIGHT]){ Horiz= 1; }
	else if( KeyStateBuf[KEY_INPUT_LEFT] ){ Horiz=-1; }
	else{ Horiz = 0; }

	// �{�^��
	// ButA
	if( KeyStateBuf[KEY_INPUT_SPACE] ){ ButA.ButCounter++; }
	else{ ButA.ButCounter=0; }

	// ButB
	if( KeyStateBuf[KEY_INPUT_LCONTROL] ){ ButB.ButCounter++; }
	else{ ButB.ButCounter=0; }

	// ButX
	if( KeyStateBuf[KEY_INPUT_X] ){ ButX.ButCounter++; }
	else{ ButX.ButCounter=0; }

	// ButY
	if( KeyStateBuf[KEY_INPUT_Y] ){ ButY.ButCounter++; }
	else{ ButY.ButCounter=0; }

};

void VirtualController::UpdateAsGamePad()
{
	// ----------------------------------------------------
	// ���W�N�[���Q�[���p�b�h�Ƃ̘_���{�^���ԍ��̑Ή���
	// �� �X�[�t�@�~�z�� | ���W�N�[���z��
	//     A�i�ԁj| B(��) :12
	//     B�i���j| A(��) :13
	//     X�i�j| Y(��) :15
	//     Y�i�΁j| X(��) :14
	//     LB:8
	//     RB:9
	//     �\����:0
	//     �\����:1
	//     �\����:2
	//     �\���E:3
	// ----------------------------------------------------


	// ##### �R���g���[����Ԃ��X�V
	GetJoypadXInputState( DX_INPUT_PAD1, &m_XinputState ) ;

	// ##### �{�^���ނ̍X�V
	// A�i�ԁj| B(��) :12
	// ButA
	if( m_XinputState.Buttons[12] ){ ButA.ButCounter++; }
	else{ ButA.ButCounter=0; }

	// B�i���j| A(��) :13
	// ButB
	if( m_XinputState.Buttons[13] ){ ButB.ButCounter++; }
	else{ ButB.ButCounter=0; }

	// X�i�j| Y(��) :15
	// ButX
	if( m_XinputState.Buttons[15] ){ ButX.ButCounter++; }
	else{ ButX.ButCounter=0; }

	// Y�i�΁j| X(��) :14
	// ButY
	if( m_XinputState.Buttons[14] ){ ButY.ButCounter++; }
	else{ ButY.ButCounter=0; }

	//     LB:8
	if( m_XinputState.Buttons[8] ){ ButLB.ButCounter++; }
	else{ ButLB.ButCounter=0; }

	//     RB:9
	if( m_XinputState.Buttons[9] ){ ButRB.ButCounter++; }
	else{ ButRB.ButCounter=0; }

	// ##### �\���L�[
	// ��������
	if( m_XinputState.Buttons[0] && m_XinputState.Buttons[1] )
	{
		if( UpPushCnt  ==1 ){ Virti= 1; }
		if( DownPushCnt==1 ){ Virti=-1; }
	}
	else if( m_XinputState.Buttons[0] ){ Virti= 1; }
	else if( m_XinputState.Buttons[1] ){ Virti=-1; }
	else{ Virti = 0; }

	// ��������
	if( m_XinputState.Buttons[2] && m_XinputState.Buttons[3] )
	{
		if( RightPushCnt==1 ){ Horiz= 1; }
		if( LeftPushCnt ==1 ){ Horiz=-1; }
	}
	else if( m_XinputState.Buttons[3] ){ Horiz= 1; }
	else if( m_XinputState.Buttons[2] ){ Horiz=-1; }
	else{ Horiz = 0; }

	// ##### �A�i���O�X�e�B�b�N
	static double  MaxOut = iANALOG_STICK_TILT_UP_LIMIT; // input.ThumbLX���̍ő�l

	// ��
	m_vStickL.x = (double)(m_XinputState.ThumbLX)/MaxOut;
	m_vStickL.y = (double)(m_XinputState.ThumbLY)/MaxOut;

	// �E
	m_vStickR.x = (double)(m_XinputState.ThumbRX)/MaxOut;
	m_vStickR.y = (double)(m_XinputState.ThumbRY)/MaxOut;

	// �X�e�B�b�N�̌X�����\���ɏ������ꍇ�� 0 �ɃJ�b�g�I�t����
	RoundingTinyStickTilt();

};

// �X�e�B�b�N�̌X�����\���ɏ������ꍇ�� 0 �ɃJ�b�g�I�t����
void VirtualController::RoundingTinyStickTilt()
{
	double lentmp;
	// ��
	lentmp = m_vStickL.len();
	if (lentmp < dANALOG_STICK_TILT_IGNORE_THRESHOLD)
	{
		m_dStickL_len = 0.0;
		m_vStickL.x = 0.0;
		m_vStickL.y = 0.0;
	}
	else m_dStickL_len = lentmp;

	// �E
	lentmp = m_vStickR.len();
	if (lentmp < dANALOG_STICK_TILT_IGNORE_THRESHOLD)
	{
		m_dStickR_len = 0.0;
		m_vStickR.x = 0.0;
		m_vStickR.y = 0.0;
	}
	else m_dStickR_len = lentmp;

};


void VirtualController::Render( Vector2D ControllerShift )
{
	// ################## �R���g���[���[��`�� #######################
	/*
	// �R���g���[���[➑�
	DrawBox2D( 
		(ControllerShift + ControllerBodyTL).toPoint(),
		(ControllerShift + ControllerBodyBR).toPoint(),
		CntBodyColor, TRUE );
		*/

	/*
	// �\���L�[��h��Ԃ�
	DrawBox2D( 
		(ControllerShift + CrossKeyOutLine[0]).toPoint(), 
		(ControllerShift + CrossKeyOutLine[6]).toPoint(),
		CrossKeyColor, TRUE );
	DrawBox2D( 
		(ControllerShift + CrossKeyOutLine[2]).toPoint(), 
		(ControllerShift + CrossKeyOutLine[8]).toPoint(),
		CrossKeyColor, TRUE );
	*/

	DrawCircle2D( 
		(ControllerShift + CrossKeyCenter).toPoint(),
		(int)CrossKeyAreaRadius,
		CrossKeyColor, TRUE );

	/*
	// �\���L�[�֊s
	for( int b=0; b<CrossKeyOutLine.size(); b++ ){
		int e = (b+1)%CrossKeyOutLine.size();
		DrawLine2D( 
			(ControllerShift + CrossKeyOutLine[b]).toPoint(), 
			(ControllerShift + CrossKeyOutLine[e]).toPoint(),
			GetColor( 0,  0,  0 ) );
			//GetColor( 255,255,255 ) );
	}
	*/

	// �\���L�[�O�p�`�֊s
	/*
	DrawTriangle2D( 
		(ControllerShift + CrossKeyTriangleUp[0]).toPoint(),
		(ControllerShift + CrossKeyTriangleUp[1]).toPoint(),
		(ControllerShift + CrossKeyTriangleUp[2]).toPoint(),
		GetColor( 255,255,255 ), FALSE );

	DrawTriangle2D( 
		(ControllerShift + CrossKeyTriangleDown[0]).toPoint(),
		(ControllerShift + CrossKeyTriangleDown[1]).toPoint(),
		(ControllerShift + CrossKeyTriangleDown[2]).toPoint(),
		GetColor( 255,255,255 ), FALSE );

	DrawTriangle2D( 
		(ControllerShift + CrossKeyTriangleRight[0]).toPoint(),
		(ControllerShift + CrossKeyTriangleRight[1]).toPoint(),
		(ControllerShift + CrossKeyTriangleRight[2]).toPoint(),
		GetColor( 255,255,255 ), FALSE );

	DrawTriangle2D( 
		(ControllerShift + CrossKeyTriangleLeft[0]).toPoint(),
		(ControllerShift + CrossKeyTriangleLeft[1]).toPoint(),
		(ControllerShift + CrossKeyTriangleLeft[2]).toPoint(),
		GetColor( 255,255,255 ), FALSE );
		*/
	
		// �e��{�^�� 
	DrawCircle2D( (ControllerShift + ControllerBotA).toPoint(), (int)ButtonRad, BotAColor, TRUE );
	DrawCircle2D( (ControllerShift + ControllerBotB).toPoint(), (int)ButtonRad, BotBColor, TRUE );
	DrawCircle2D( (ControllerShift + ControllerBotY).toPoint(), (int)ButtonRad, BotYColor, TRUE );
	DrawCircle2D( (ControllerShift + ControllerBotX).toPoint(), (int)ButtonRad, BotXColor, TRUE );


	// ###### ��������Ă����Ԃ�`��

	// �\���L�[
	if( Virti > 0 ) // ��
	{
		/*
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[0] ).toPoint(),
			(ControllerShift + CrossKeyOutLine[10]).toPoint(),
			GetColor( 255,255,255 ), TRUE );
			*/
		DrawTriangle2D( 
			(ControllerShift + CrossKeyTriangleUp[0]).toPoint(),
			(ControllerShift + CrossKeyTriangleUp[1]).toPoint(),
			(ControllerShift + CrossKeyTriangleUp[2]).toPoint(),
			GetColor( 255,255,255 ), TRUE );
	}
	if( Virti < 0 ) // ��
	{
		/*
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[4] ).toPoint(),
			(ControllerShift + CrossKeyOutLine[6] ).toPoint(),
			GetColor( 255,255,255 ), TRUE );
			*/
		DrawTriangle2D( 
			(ControllerShift + CrossKeyTriangleDown[0]).toPoint(),
			(ControllerShift + CrossKeyTriangleDown[1]).toPoint(),
			(ControllerShift + CrossKeyTriangleDown[2]).toPoint(),
			GetColor( 255,255,255 ), TRUE );

	}
	if( Horiz > 0 ) // �E
	{
		/*
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[10]).toPoint(),
			(ControllerShift + CrossKeyOutLine[8] ).toPoint(),
			GetColor( 255,255,255 ), TRUE );
			*/
		DrawTriangle2D( 
			(ControllerShift + CrossKeyTriangleRight[0]).toPoint(),
			(ControllerShift + CrossKeyTriangleRight[1]).toPoint(),
			(ControllerShift + CrossKeyTriangleRight[2]).toPoint(),
			GetColor( 255,255,255 ), TRUE );
	}
	if( Horiz < 0 )  // ��
	{
		/*
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[2] ).toPoint(),
			(ControllerShift + CrossKeyOutLine[4] ).toPoint(),
			GetColor( 255,255,255 ), TRUE );
			*/
		DrawTriangle2D( 
			(ControllerShift + CrossKeyTriangleLeft[0]).toPoint(),
			(ControllerShift + CrossKeyTriangleLeft[1]).toPoint(),
			(ControllerShift + CrossKeyTriangleLeft[2]).toPoint(),
			GetColor( 255,255,255 ), TRUE );
	}

	// A,B,X,Y�{�^��
	if( ButA.isPushed() )
	{
		DrawCircle2D( (ControllerShift + ControllerBotA).toPoint(), (int)ButtonRad, GetColor( 255,255,255 ), TRUE );
	}
	if( ButB.isPushed() )
	{
		DrawCircle2D( (ControllerShift + ControllerBotB).toPoint(), (int)ButtonRad, GetColor( 255,255,255 ), TRUE );
	}
	if( ButX.isPushed() )
	{
		DrawCircle2D( (ControllerShift + ControllerBotX).toPoint(), (int)ButtonRad, GetColor( 255,255,255 ), TRUE );
	}
	if( ButY.isPushed() )
	{
		DrawCircle2D( (ControllerShift + ControllerBotY).toPoint(), (int)ButtonRad, GetColor( 255,255,255 ), TRUE );
	}

	// ##### �A�i���O�X�e�B�b�N�̏�Ԃ�`��
	m_RenderStickL.Render(m_vStickL, ControllerShift);
	m_RenderStickR.Render(m_vStickR, ControllerShift);

};

// �`��⏕�֐�
void VirtualController::RenderStickTrajectory::Render( Vector2D CurStickPos, Vector2D RendPos ) 
{
	// �A�i���O�X�e�B�b�N�̌��݈ʒu���v�Z
	Vector2D Pos;
	Pos.x = ( m_dRadius*CurStickPos.x ) + m_vCenterPos.x ;
	Pos.y = (-m_dRadius*CurStickPos.y ) + m_vCenterPos.y ;
		
	CurIndex=++CurIndex%TrajectoryList.size();
	TrajectoryList[CurIndex] = Pos;

	// �`��̈�̘g��`��
	DrawCircle2D( 
		(RendPos+m_vCenterPos).toPoint(),
		(int)m_dRadius ,
		GetColor( 255, 255, 255 ), FALSE );

	// �O�Ղ�`��
	// ���̃��W�b�N�o�O���Ă�ȁB�B�B
	// �E�n�܂肪 CurIndex ����ɂȂ��ĂȂ�
	// �E�H�������Ă���
	for( int i=0; i<TrajectoryList.size()-1; i++ )
	{
		int s = (i)%TrajectoryList.size();
		int e = (i+1)%TrajectoryList.size();
		DrawLine2D(
			(RendPos+TrajectoryList[s]).toPoint(),
			(RendPos+TrajectoryList[e]).toPoint(),
			GetColor( 255, 0, 0 ) );
	}

	// ���݈ʒu��`��
	DrawCircle2D(
		(RendPos+TrajectoryList[CurIndex]).toPoint(),
		3 , GetColor( 255, 255, 255 ), FALSE );
};


// ���̓f�o�C�X����@�\�i�Q�[���p�b�h���ڑ�����Ă���΁A���̓f�o�C�X���Q�[���p�b�h�ɃZ�b�g����j
void VirtualController::CheckAndSetGamePadMode()
{
	// �P���ɁA�֐� GetJoypadNum �Ńp�b�h�ڑ��L�����m�F���A
	// �ڑ�����Ă���΁A�ȍ~�́ADX_INPUT_PAD1 �����Ԏ擾���s���B
	// i.e.�����p�b�h�ڑ���z�肵�Ȃ��B
	if( GetJoypadNum()>0 )
	{
		m_eInpuDeviceMode = ID_GamePad;
	}

	return ;
};

void VirtualController::DBG_ShowXinputState()
{
	// ��ʂ� XINPUT_STATE �̒��g��`��
	int Color = GetColor( 255,255,255 ) ;
	DrawFormatString( 0, 0, Color, "LeftTrigger:%d RightTrigger:%d",
					m_XinputState.LeftTrigger, m_XinputState.RightTrigger ) ;
	DrawFormatString( 0, 16, Color, "ThumbLX:%d ThumbLY:%d",
					m_XinputState.ThumbLX, m_XinputState.ThumbLY ) ;
	DrawFormatString( 0, 32, Color, "ThumbRX:%d ThumbRY:%d",
					m_XinputState.ThumbRX, m_XinputState.ThumbRY ) ;
	DrawString( 0, 64, "Button", Color ) ;
	for( int i = 0 ; i < 16 ; i ++ )
	{
		DrawFormatString( 64 + i % 8 * 64, 64 + i / 8 * 16, Color,
							"%2d:%d", i, m_XinputState.Buttons[ i ] ) ;
	}
};

// ##### ���쌟�؂̂��߂̎�������@�\

//��������@�\��ON / OFF��ݒ肷�郁�\�b�h
void VirtualController::AutoControlOnOff()
{
	// ON/OFF���]
	m_bAutoControlFlg = !m_bAutoControlFlg;

	// OFF��ON �̂Ƃ��͏���������
	if (m_bAutoControlFlg == true) m_dTimeSinceAutoControlStart = 0;

};

//����������^�C���X�X���C�X�ōX�V
void VirtualController::UpdateAutoControl(double TimeElaps)
{
	// * TimeSinceAutoControlStart �̍X�V
	m_dTimeSinceAutoControlStart += TimeElaps;

	// * ��������V�i���I�ɂ����ăX�e�B�b�N��ԁi���̂Ƃ��낱�ꂾ���j���X�V����B
	
	// ������
	static int TotalSections = 9;     // ����Ԑ�
	
	/*
	// ## �u�ؕԂ��ڑ��p�u���[�L�v���[�V�����̎B�e�p
	// �ړ�����X�e�B�b�N���W�z��
	static Vector2D StickPoss[9 + 1] = {
		Vector2D(0,0),
		Vector2D(1,0),
		Vector2D(1,0),
		Vector2D(-cos(0.01),sin(0.01)), // Vector2D(1,0)����180���]������ƁARotateTowards3D �̃o�O�H�����邽�߁A�����������炷�B
		Vector2D(-cos(0.01),sin(0.01)),
		Vector2D(1,0),
		Vector2D(1,0),
		Vector2D(-cos(0.01),sin(0.01)), // Vector2D(1,0)����180���]������ƁARotateTowards3D �̃o�O�H�����邽�߁A�����������炷�B
		Vector2D(-cos(0.01),sin(0.01)),
		Vector2D(0,0),
	};

	// ���Ԕz���̔z��i�ݐϒl�j
	static double   TimeAlloc[9 + 1] = {
		0.0,
		0.1,
		0.1 + 3.0,
		0.1 + 3.0 + 0.1,
		0.1 + 3.0 + 0.1 + 0.8,
		0.1 + 3.0 + 0.1 + 0.8 + 0.1,
		0.1 + 3.0 + 0.1 + 0.8 + 0.1 + 0.8,
		0.1 + 3.0 + 0.1 + 0.8 + 0.1 + 0.8 + 0.1,
		0.1 + 3.0 + 0.1 + 0.8 + 0.1 + 0.8 + 0.1 + 0.8,
		0.1 + 3.0 + 0.1 + 0.8 + 0.1 + 0.8 + 0.1 + 0.8 + 0.1
	};
	*/

	// ## �u��~�ڑ��p�u���[�L�v���[�V�����̎B�e�p
	// �ړ�����X�e�B�b�N���W�z��
	static Vector2D StickPoss[11 + 1] = {
		Vector2D(0,0),
		Vector2D(1,0),
		Vector2D(1,0),
		Vector2D(0,0),
		Vector2D(0,0),
		Vector2D(-1,0),
		Vector2D(-1,0),
		Vector2D(0,0),
		Vector2D(0,0),
		Vector2D(1,0),
		Vector2D(1,0),
		Vector2D(0,0),
	};

	// ���Ԕz���̔z��i�ݐϒl�j
	static double   TimeAlloc[11 + 1] = {
		0.0,
		0.05,
		0.05 + 2.0,
		0.05 + 2.0 + 0.05,
		0.05 + 2.0 + 0.05 + 1.0,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05 + 1.0,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05 + 1.0 + 0.05,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05
	};

	Vector2D StickPos;
	for (int i=0; i<TotalSections; i++)
	{
		if (TimeAlloc[i] <= m_dTimeSinceAutoControlStart && m_dTimeSinceAutoControlStart < TimeAlloc[i + 1])
		{ // i �Ԗڂ̋�Ԃɑ����Ă���΁A
			double raito = (m_dTimeSinceAutoControlStart - TimeAlloc[i]) / (TimeAlloc[i + 1] - TimeAlloc[i]);
			StickPos = LerpV2D(StickPoss[i], StickPoss[i + 1], raito );

			// �X�e�B�b�N��Ԃ��X�V
			m_vStickL = StickPos;

			// �X�e�B�b�N�̌X�����\���ɏ������ꍇ�� 0 �ɃJ�b�g�I�t����
			RoundingTinyStickTilt();

			return;
		}
	}

	// ��������Đ��������������߁A��������t���O��OFF�ɂ���
	m_bAutoControlFlg = false;

	return;

}

// ����L�^�̊J�n�^�I��
// - �R���g���[���X�e�[�g�̍X�V
// - �I�����́A����L�^�z����t�@�C���Ƀ_���v
void VirtualController::RecodeControlONOFF()
{
	// �Đ����́A��x�A�Đ����[�h���I�������Ă���łȂ��ƁA�L�^���[�h�Ɉڂ邱�Ƃ͂ł��Ȃ��B
	if (m_eRecodeReplayState == ID_Replaying) return;

	if (m_eRecodeReplayState == ID_Recoding) // �L�^���[�h��
	{
		// ### �L�^���[�h�̏I������ 

		// �L�^���[�h���I��
		m_eRecodeReplayState = ID_DoNothing;

		// �L�^�L���[�̓��e���t�@�C���Ƀ_���v����
		SaveRecodeControlArray();

		// �L�^�L���[���������i�O�ׁ̈j
		m_RecodeControlArray.clear();

	}
	else // ����ȊO
	{
		// ### �L�^���[�h�̊J�n���� 

		// �L�^���[�h���J�n
		m_eRecodeReplayState = ID_Recoding;

		// �L�^�L���[���������i�O�ׁ̈j
		m_RecodeControlArray.clear();

		// �L�^�J�n����̎��Ԃ̏����������Ȃ��Ă͂Ȃ�Ȃ��I
		m_dRecodeReplayElapsed = 0;

	}

}

// �L�^��������̍Đ��̊J�n�^�I��
// - �R���g���[���X�e�[�g�̍X�V
// - �J�n���ɁA����L�^�z��Ƀt�@�C�����烍�[�h�B
void VirtualController::ReplayRecodedControlONOFF()
{
	// �^�惂�[�h���́A�Đ����[�h�ɓ���Ȃ�
	if (m_eRecodeReplayState == ID_Recoding) return;

	if (m_eRecodeReplayState == ID_Replaying) // �Đ����[�h��
	{
		// ### �Đ����[�h�̏I������ 

		// �Đ����[�h���I��
		m_eRecodeReplayState = ID_DoNothing;

		// �L�^�L���[���������i�O�ׁ̈j
		m_RecodeControlArray.clear();

	}
	else // ����ȊO
	{
		// ### �Đ����[�h�̊J�n���� 

		// �Đ����[�h���J�n
		m_eRecodeReplayState = ID_Replaying;

		// �L�^�L���[���������i�O�ׁ̈j
		m_RecodeControlArray.clear();

		// ����L�^�z����t�@�C�����烍�[�h�B
		LoadRecodeControlArray();

		// �L�^�J�n����̎��Ԃ̏����������Ȃ��Ă͂Ȃ�Ȃ��I
		m_dRecodeReplayElapsed = 0;

	}

}

// �L�^�L���[(m_RecodeControlArray)�̓��e���t�@�C���Ƀ_���v����
void VirtualController::SaveRecodeControlArray()
{
	std::ofstream fs("VirtualController-RecodeControlArray.txt", ios::out | ios::trunc); // �����̓��e���������A�������݁B

	// ���ׂĂ̗v�f��csv�`���Ńt�@�C���o�͂���B
	for (int i = 0; i < m_RecodeControlArray.size(); i++)
	{
		fs << m_RecodeControlArray[i].m_dTime << "," 
			<< m_RecodeControlArray[i].m_vStickLoc.x << "," 
			<< m_RecodeControlArray[i].m_vStickLoc.y << std::endl;
	}

}


// �L�^�L���[(m_RecodeControlArray)���t�@�C�����烍�[�h
//   C++���� STL���ł��邾���g����CSV��ǂݍ���ł݂�|http://www.tetsuyanbo.net/tetsuyanblog/23821

// http://cplplus.web.fc2.com/Last3.html
// http://www.cellstat.net/readcsv/ .. getline �̕������ʂ�vector�z��Ɋi�[
// http://marycore.jp/prog/cpp/convert-string-to-number/ .. ������(string)�𐔒l(int��float)�ɕϊ�
void VirtualController::LoadRecodeControlArray()
{
	std::ifstream fs("VirtualController-RecodeControlArray.txt");

	// �t�@�C���̒��g����s���ǂݎ��
	string str = "";
	while (getline(fs, str))
	{
		string tmp = "";
		istringstream stream(str);
		vector<string> result;
		// ��؂蕶�����Ȃ��Ȃ�܂ŕ�������؂��Ă���
		while (getline(stream, tmp, ','))
		{
			result.push_back(tmp);
		}
		
		m_RecodeControlArray.push_back(
			RecodeControl(
				stod(result[0]) , Vector2D(stod(result[1]), stod(result[2]))
			)
		);

	}

	fs.close();

}

// ����L�^���̃R���g���[���̍X�V
void VirtualController::Update_RecodeControl(double TimeElaps)
{
	// �o�ߎ��Ԃ̍X�V
	m_dRecodeReplayElapsed += TimeElaps;

	// ���݂̃R���g���[���̏�Ԃ��L�^�L���[�ɐςݏグ�邾���B
	m_RecodeControlArray.push_back( RecodeControl(m_dRecodeReplayElapsed, m_vStickL) );
}

// �L�^��������̍Đ��̃R���g���[���̍X�V
void VirtualController::Update_ReplayRecodedControl(double TimeElaps)
{
	// �o�ߎ��Ԃ̍X�V
	m_dRecodeReplayElapsed += TimeElaps;

	// �Đ��J�n���Ă���̌o�ߎ��Ԃ��A�L�^�L���[�̋L�^���Ԃ���I�[�o�[���Ă�ΏI������
	if (m_RecodeControlArray.back().m_dTime < m_dRecodeReplayElapsed)
	{
		ReplayRecodedControlONOFF();
		return;
	}

	// m_RecodeControlArray ����Q���@�ŁA�⊮���邽�߂̗��[�̃C���f�b�N�X�𒲂ׂ�B
	int bgn = 0;
	int end = m_RecodeControlArray.size() - 1;
	while ((end - bgn) > 1) // �s�����A�Ԉ���Ă�
	{
		int mid = (bgn + end) / 2; // �������m�̊��Z�͏����_�ȉ��͐؎̂Ă���B
		
		(m_RecodeControlArray[mid].m_dTime > m_dRecodeReplayElapsed ? end = mid : bgn = mid ); // �^�[�Q�b�g���Ԃ��L�^���Ԃ���v�����ꍇ��bgn���Ɋ񂹂���悤�ɐ݌v
		
	}

	assert( end >= bgn );

	// �o�ߎ��Ԃɂ�����X�e�B�b�N�ʒu��⊮�������Čv�Z����
	Vector2D NewStickLoc;
	if (bgn == end) 
	{
		// bgn == end �̏ꍇ
		NewStickLoc = m_RecodeControlArray[bgn].m_vStickLoc;
	}
	else
	{
		// ���`��Ԃ����{
		double ratio = 
			(m_RecodeControlArray[end].m_dTime - m_dRecodeReplayElapsed) / 
			(m_RecodeControlArray[end].m_dTime - m_RecodeControlArray[bgn].m_dTime);
		NewStickLoc = m_RecodeControlArray[bgn].m_vStickLoc + 
			ratio * (m_RecodeControlArray[end].m_vStickLoc - m_RecodeControlArray[bgn].m_vStickLoc);
	}


	// �R���g���[����Ԃ��X�V
	m_vStickL = NewStickLoc;

	RoundingTinyStickTilt(); // �Ȃ�Ƃ��ꂪ�Ȃ��Ɠ����Ȃ��B������ m_dStickL_len �̍X�V�����Ă��āA���̐������Ƃ�Ă��Ȃ��ƃo�O��悤��...



}
string VirtualController::getRecodeReplayState()
{
	string str;

	switch (m_eRecodeReplayState)
	{
	case ID_DoNothing:
		str = "ID_DoNothing";
		break;
	case ID_Recoding:
		str = "ID_Recoding";
		break;
	case ID_Replaying:
		str = "ID_Replaying";
		break;
	}

	return str;
}
;