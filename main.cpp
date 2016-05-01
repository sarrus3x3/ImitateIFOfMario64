#include <cassert>

#include "DxLib.h"

// ��{���C�u����
#include "utils.h"
#include "Vector3D.h"
#include "Vector2D.h"

// �Q�[����Ռn
#include "PrecisionTimer.h"
#include "VirtualController.h"
#include "PlayerCharacterEntity.h"

// �����n�H
#include "CameraWorkManager.h"
#include "SolidObjects.h"

// 2016/03/16
// �����̃��[�V���� �� �n�ʁF�A�j����8:20 ���炢�����傤�ǂ����B

// 2016/04/03
// �� �W�����v��ɁA�n�ʂɂ߂荞��ōs���o�O���L��B
//    ���炭�A���n����Heading���A�������Ɍ����Ă���̂ŁA��̈ړ�������Heading�̒n�������ɓ����Ă���̂ł́H
//    ���Ƃ͏������낢��o�͂����Ă݂�

//#define LIGHT_ON

static const float ROTATE_SPEED = DX_PI_F/90;//��]�X�s�[�h


// ################## FPS����p #######################
class MeasureFPS
{
public:
	static const int SampleRate = 20; // FPS�\���ŕ��ς��Ƃ鐔

	std::vector<double> m_FrameTimeHistory;
	int m_iNextUpdateSlot;

	// �R���X�g���N�^
	MeasureFPS():
		m_FrameTimeHistory(SampleRate,0.0),
		m_iNextUpdateSlot(0)
	{}

	// �X�V�ƕ��ϒl�̌v�Z
	double Update( double NewFrameTime )
	{
		m_FrameTimeHistory[m_iNextUpdateSlot]=NewFrameTime;
		
		m_iNextUpdateSlot = (m_iNextUpdateSlot+1)%SampleRate;

		double sum=0;
		std::vector<double>::iterator it = m_FrameTimeHistory.begin();
		for( it; it!=m_FrameTimeHistory.end(); it++ )
		{
			sum += *it;
		}

		return sum / (double)SampleRate ;
	}

};



// #############################################################################
//                                   main
// #############################################################################

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	VirtualController VController; // ���z�R���g���[���[�̃C���X�^���X��

	// �w�i�F�̐ݒ�
	//SetBackgroundColor( 135, 206, 235 ); // skyblue 87ceeb
	
	if( ChangeWindowMode(TRUE) != DX_CHANGESCREEN_OK || DxLib_Init() == -1 ) return -1; //Window���[�h
	//if( ChangeWindowMode(FALSE) != DX_CHANGESCREEN_OK || DxLib_Init() == -1 ) return -1; //�S��ʃ��[�h

	SetDrawScreen( DX_SCREEN_BACK );        //����ʂɐݒ�

	SetUseZBuffer3D( TRUE ); // Z�o�b�t�@ON�iColumn�`��ɕK�v�j
	SetWriteZBuffer3D( TRUE );

	SetCameraNearFar( 1.0f, 1500.0f ) ; // �J������ ��O�N���b�v������ ���N���b�v������ݒ�

	// ################## �n�ʂ̖͗l�`��Ɏg�p����ϐ��̏��� #######################
	
	// �~���̖��т𐶐�����
	int    MaxColumnsNum     = 250;
	double ColumnForestRange = 4*250.0;
	double AverageHight      = 20.0;
	Column** ppColmunList;

	ppColmunList = new Column*[MaxColumnsNum]; // ���������m��

	vector<Vector2D> ColmunPoss(MaxColumnsNum);
	for( int i=0; i<MaxColumnsNum; i++ )
	{
		// �ʒu�́AColumnForestRange * ColumnForestRange �̗̈�̃����_��
		Vector2D Pos2D;
		Pos2D.x = ColumnForestRange * RandFloat();
		Pos2D.y = ColumnForestRange * RandFloat();
		ColmunPoss[i] = Pos2D;
	}

	for( int i=0; i<MaxColumnsNum; i++ )
	{
		Vector3D Pos3D = ColmunPoss[i].toVector3D();

		double columnhight = AverageHight*(1+0.25*RandGaussian());
		double columnradius = 2.5*(1+0.25*RandGaussian());

		// ���l�ɂȂ邱�Ƃ�h��
		columnhight  = max( columnhight,  0.1 );
		columnradius = max( columnradius, 0.1 );

		//int GlayLv = rand();
		int GlayLv = 254;
		COLOR_U8 col = GetColorU8( GlayLv%255, GlayLv%255, GlayLv%255, 0 );

		ppColmunList[i] = new Column( 
			Pos3D,
			columnradius,
			columnhight,
			16*(int)(columnradius+1), // ���ꂾ�Əd���Ȃ�B���a�̑傫�ȉ~�������ׂɂȂ�悤��
			//16, 
			col, 
			col ) ;

	}

	// �n�ʗp�̉~��
	/*
	Column GroundDisk( 
		Vector3D(0,-1.0,0),
		ColumnForestRange*3,
		1.0,
		64,
		GetColorU8( 255, 255, 255, 0 ),
		GetColorU8( 255, 255, 255, 0 )
		);
		*/

	FineMeshedFlatPlate GrateEarthGround(
		Vector2D( -ColumnForestRange, -ColumnForestRange),
		Vector2D(  ColumnForestRange,  ColumnForestRange),
		//250,
		1,
		GetColorU8( 255, 255, 255, 0 ),
		GetColorU8( 255, 255, 255, 0 )
		);
	/*
	ParallelBox3D ColorfullBox(
		Vector3D( 0,0,0),
		Vector3D( 20,20,20),
		LoadGraph( "kirby_connect.bmp" ),
		GetColorU8( 255, 255, 255, 0 ),
		GetColorU8( 255, 255, 255, 0 )
		);
		*/

	TextureSphere3D ExperimentSphere(
		Vector3D( 0,0,0),
		500.0,
		false,
		32,
		32,
		LoadGraph( "Resize_PanoramaPict.jpg" ),
		GetColorU8( 255, 255, 255, 0 ),
		GetColorU8( 255, 255, 255, 0 )
		);

	// ################## �ϐ��̒�`�E������ #######################

	// �^�C�}�𐶐�
	PrecisionTimer timer;
	timer.Start();

	// �J�������[�h��ݒ�
	CameraWorkManager::Instance()->setCameraMode( CameraWorkManager::TrackingMovingTarget );
	//CameraWorkManager::Instance()->setCameraMode( CameraWorkManager::RotateCamOnGazePoint );
	int CurCameraMode = 0;

	// Entity�̃C���X�^���X��
	PlayerCharacterEntity PCEnti(
		Vector3D(0,0,0),
		Vector3D(0,0,0),
		Vector3D(1.0,0,0),
		Vector3D(0.0,0,1.0),
		10.0,
		5.0,
		&VController );	

	// FPS����
	MeasureFPS InsMeasureFPS;


	// ���C�g�֌W�p�����[�^
	Vector3D LightPos;
	Vector3D LightHead;
    float OutAngle = 90.0f ;
    float InAngle = 45.0f ;
    float Range = 2000.0f ;
    float Atten0 = 0.0f ;
    float Atten1 = 0.0f ;
    float Atten2 = 0.0001f ;


	// ################## ���C�����[�v�̊J�n #######################
	while(!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen()){
		//�R�R�ɏ����������Ă���

		// �^�C���X���C�X���擾
		double timeelaps = timer.TimeElapsed();

		// ################## �R���g���[���[���X�V #######################
		VController.Update();

		// �����AB�{�^���iCtrl�L�[�j�������ꂽ��A�J�������[�h��ύX
		if( VController.ButB.isNowPush() )
		{
			CurCameraMode = (CurCameraMode+1)%CameraWorkManager::m_iCameraModeMax;
			CameraWorkManager::Instance()->setCameraMode( (CameraWorkManager::CameraModeID)CurCameraMode );
		}

		//MV1SetRotationXYZ( ModelHandle, VGet( angleX, angleY, 0.0f ) ) ;


		// ################## Entity��Update #######################
		PCEnti.Update( timeelaps );

		// ################## ���C�g�i�Ɩ��j�̐ݒ� #######################
#ifdef LIGHT_ON
		if( CheckHitKey( KEY_INPUT_L ) == 0 )
		{
			LightPos     = PCEnti.Pos();
			LightHead    = PCEnti.Heading();
			LightPos.y  += 10.0;
			LightHead.y -=  0.1;
			LightPos    += 3.0 * LightHead;
		}

		ChangeLightTypeSpot(
            LightPos.toVECTOR(),
            LightHead.toVECTOR(),
            OutAngle * DX_PI_F / 180.0f,
            InAngle * DX_PI_F / 180.0f,
            Range,
            Atten0,
            Atten1,
            Atten2 ) ;
#endif
		// ################## �J�����ݒ� #######################
		CameraWorkManager::Instance()->setTarget(PCEnti.Pos());
		CameraWorkManager::Instance()->Update( timeelaps );
		CameraWorkManager::Instance()->setCamera();

		// ################## �n�ʂ̖͗l�̕`�� #######################

		// �w�i�p�m���}�̕`��
		ExperimentSphere.setCenterPos( CameraWorkManager::Instance()->getCamPos() ); // �p�m���}���̒��S�ʒu���J�������S��
		ExperimentSphere.Render();

		// �n�ʗp�̉~�Ղ�`��
		//GroundDisk.Render();
		GrateEarthGround.Render();

		// �~���̖��т�`��
		for( int i=0; i<MaxColumnsNum; i++ ){ ppColmunList[i]->Render(); }

		// ################## Entity�̕`�� #######################
		PCEnti.Render();

		// ################## �R���g���[���[��`�� #######################
		VController.Render( Vector2D(25,25) );

		// ################## �f�o�b�N�����o�� #######################
		//�s��
		int colmun= 0;
		int width = 15;

		// m_FrameTimeHistory���X�V
		int AvFPS = (int)(1.0 / InsMeasureFPS.Update(timeelaps));

		// FPS���o��
		DrawFormatString( 0, width*colmun, 0xffffff, "FPS:%d", AvFPS ); 
		colmun++;

		// �J�������[�h���o��
		std::string sCamMode = CameraWorkManager::Instance()->getCurCamModeName();
		DrawFormatString( 0, width*colmun, 0xffffff, "CamMode:%s", sCamMode.c_str() ); 
		colmun++;

		// Entity�̑��x��\��
		DrawFormatString( 0, width*colmun, 0xffffff, "Entity Speed:%8f",PCEnti.Speed() ); 
		colmun++;

		// ����sleep
		//Sleep( 100 );

	}

	DxLib_End() ;			// �c�w���C�u�����g�p�̏I������
	return 0 ;				// �\�t�g�̏I�� 
}

