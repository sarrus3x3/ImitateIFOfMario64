#include <cassert>

#include "DxLib.h"

// ��{���C�u����
#include "utils.h"
#include "MyUtilities.h"
#include "Vector3D.h"
#include "Vector2D.h"

// �Q�[����Ռn
#include "PrecisionTimer.h"
#include "VirtualController.h"
#include "PlayerCharacterEntity.h"

// �����n�H
#include "CameraWorkManager.h"
//#include "SolidObjects.h" // Scenery.h����include�ς�
#include "Scenery.h" // �w�i�Z�b�g

// �f�o�b�N�p
#include "AnimationManager.h"

#include "DEMO_DashTurnBehavior.h"


// 2017/04/23
// �VPC����R�~�b�g�e�X�e�X

// ########### ����p defin ###########

//#define MONUMENT_ON // ���j�������g�i�~���̖��сj����
#define GROUND_MESH_ON  // �n�ʂ̕���͗l����
//#define FARFAR_AWAY // ���i�i�F�����x���j�ŉ����I�u�W�F�N�g��`�悷��ꍇ
//#define FLOATING_DUNGEON  // ���V�_���W�����W�I���}
//#define ITEM_GETTING // �A�C�e�����W���o


// ####################################


// 2016/03/16
// �����̃��[�V���� �� �n�ʁF�A�j����8:20 ���炢�����傤�ǂ����B

// 2016/04/03
// �� �W�����v��ɁA�n�ʂɂ߂荞��ōs���o�O���L��B
//    ���炭�A���n����Heading���A�������Ɍ����Ă���̂ŁA��̈ړ�������Heading�̒n�������ɓ����Ă���̂ł́H
//    ���Ƃ͏������낢��o�͂����Ă݂�

//#define LIGHT_ON

static const float ROTATE_SPEED = DX_PI_F/90;//��]�X�s�[�h

// #############################################################################
//                                   main
// #############################################################################

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	// �w�i�F�̐ݒ�
	//SetBackgroundColor( 135, 206, 235 ); // skyblue 87ceeb
	//SetBackgroundColor( 255, 255, 255 ); // skyblue 87ceeb
	
	if( ChangeWindowMode(TRUE) != DX_CHANGESCREEN_OK || DxLib_Init() == -1 ) return -1; //Window���[�h
	//if( ChangeWindowMode(FALSE) != DX_CHANGESCREEN_OK || DxLib_Init() == -1 ) return -1; //�S��ʃ��[�h

	SetDrawScreen( DX_SCREEN_BACK );        //����ʂɐݒ�

	SetUseZBuffer3D( TRUE ); // Z�o�b�t�@ON�iColumn�`��ɕK�v�j
	SetWriteZBuffer3D( TRUE );

	SetCameraNearFar( 1.0f, 1500.0f ) ; // �J������ ��O�N���b�v������ ���N���b�v������ݒ�

	// ################## �n�ʂ̖͗l�`��Ɏg�p����ϐ��̏��� #######################

#ifdef MONUMENT_ON	
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
#endif

#ifdef GROUND_MESH_ON
	// ���Ⴡ�b�V���̃C���X�^���X��
	GroundGrid ObjGroundGrid( 1000.0, 100, GetColor( 255,255,255) );

#endif

#ifdef FARFAR_AWAY
	// �w�i�p�m���}���̃C���X�^���X��
	TextureSphere3D ExperimentSphere(
		Vector3D( 0, 0, 0 ),
		500.0,
		false,
		32,
		32,
		LoadGraph( "Resize_PanoramaPict.jpg" ),
		//GetColorU8( 255, 255, 255, 0 ),
		//GetColorU8( 255, 255, 255, 0 ),
		TextureSphere3D::OBJECT_SKYDOME
		);
	ExperimentSphere.rotEuler( 10*ROTATE_SPEED, 0, 20*ROTATE_SPEED );

	// �ؐ�����
	TextureSphere3D BackgroundObjectJupiter(
		Vector3D( 1000000000, 0, 0),
		10000000.0,
		true,
		32,
		32,
		LoadGraph("JupiterSystemDiorama\\jupiter.jpg" ),
		//GetColorU8( 255, 255, 255, 0 ),
		//GetColorU8( 255, 255, 255, 0 ),
		TextureSphere3D::OBJECT_NOSPECULAR
		);
	
	// ��]
	BackgroundObjectJupiter.rotEuler( 10*ROTATE_SPEED, 0, 20*ROTATE_SPEED );
	BackgroundObjectJupiter.setCenterPos( Vector3D( 100000000, 0, 0) );
#endif

	// �ؐ��W�I���}
	JupiterSystemDiorama JupiterDioramaIns;

	// �����p����ς���
	MATRIX TmpMat = MGetIdent();
	//MATRIX TmpMat = MGetRotZ( DX_PI_F/3 );

	// �X�P�[���s����擾�i ���k�� : 100,000,000�{ �j
	TmpMat = MScale( TmpMat, 100000000.0 );

	// �ؐ��n�ɑ΂��郏�[���h�ʒu��ݒ�i���k�ڂŁj
	TmpMat.m[3][0] = 5500000000.0;
	
	TmpMat = MMult( TmpMat, MGetRotX( DX_PI_F/8 ) );

	JupiterDioramaIns.m_mLocalToWorldMatrix = TmpMat;

	JupiterDioramaIns.setVertex();

	// ���C�g�ݒ�e�X�g
	//SetLightDifColor( GetColorF( 1.0, 1.0, 1.0, 1.0 ) );
	SetLightDirection( VGet( 1.0, -0.5, 0 ) ); // �ؐ��i�n�j�̌������ƁA���̓�������i=���z�̕����j�����`���N�`�������ǁA�܂��A�������B

#ifdef FLOATING_DUNGEON
	// ���V�_���W����
	FloatingDungeon Dungeon( 20.0, 200.0, "mapdef.bmp" );
#endif

	// ################## �ϐ��̒�`�E������ #######################

	// �Q�[�����Ԃ̃J�E���^
	int GameWorldCounter=0;

	// �^�C�}�𐶐�
	PrecisionTimer timer;
	timer.Start();

	// ���z�R���g���[���[�̃C���X�^���X��
	VirtualController VController; 
	VController.CheckAndSetGamePadMode();

	// �J�������[�h��ݒ�
	//CameraWorkManager::Instance()->setCameraMode( CameraWorkManager::TrackingMovingTarget );
	CameraWorkManager::Instance()->setCameraMode( CameraWorkManager::RotateCamOnGazePoint );
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

#ifdef ITEM_GETTING
	// �݂������A�C�e������@�\����
	SampleGameWorld::Initialize();
	SampleGameWorld GameWorldIns;

	GameWorldIns.SetItemsToWorld( 20.0, 5.0, "ItemPlaceDef.bmp" );
#endif

	// �L�����N�^���[�J�����W���f���`��e�X�g
	CoordinateAxisModel CharCordiModelTest( 0.5, 20, 20, 20 );

	// ���C�g�֌W�p�����[�^
	Vector3D LightPos;
	Vector3D LightHead;
    float OutAngle = 90.0f ;
    float InAngle = 45.0f ;
    float Range = 2000.0f ;
    float Atten0 = 0.0f ;
    float Atten1 = 0.0f ;
    float Atten2 = 0.0001f ;



#ifdef FARFAR_AWAY
	int Sx , Sy , Cb ;
    GetScreenState( &Sx , &Sy , &Cb ) ;
	int SubScreenHandle = MakeScreen( Sx, Sy, FALSE );
#endif

	// ################## ���C�����[�v�̊J�n #######################
	while(!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen()){
		//�R�R�ɏ����������Ă���

		// �^�C���X���C�X���擾
		double timeelaps = timer.TimeElapsed();

		// �Q�[�����Ԃ̃J�E���^���X�V
		GameWorldCounter++;

		// ################## �R���g���[���[���X�V #######################
		VController.Update( timeelaps );

		// �����AB�{�^���iCtrl�L�[�j�������ꂽ��A�J�������[�h��ύX
		if( VController.ButB.isNowPush() )
		{
			CurCameraMode = (CurCameraMode+1)%CameraWorkManager::m_iCameraModeMax;
			CameraWorkManager::Instance()->setCameraMode( (CameraWorkManager::CameraModeID)CurCameraMode );
		}

		//MV1SetRotationXYZ( ModelHandle, VGet( angleX, angleY, 0.0f ) ) ;

		// X�{�^���iX�L�[�j�������ꂽ��A�������Z���[�h�ύX
		if( VController.ButX.isNowPush() )
		{
			static int phystype = 0;
			phystype = (++phystype)%3 ;
			PCEnti.m_pAnimMgr->setAnimPhysicsType( (AnimationManager::PhysicsTypeID)phystype );

			// PHYSICS_NONE
			// PHYSICS_SELFMADE
			// PHYSICS_DXLIB

		}

		// Y�{�^���iy�L�[�j�������ꂽ��A�{�[���\���^���f���\���ؑ�
		if( VController.ButY.isNowPush() )
		{
			// �������샂�[�h�I���^�I�t
			VController.AutoControlOnOff();

			/*
			PCEnti.m_pAnimMgr->ExpBoneOfPhysicsPart( !PCEnti.m_pAnimMgr->getCurBoneExpress() );
			MV1PhysicsResetState( PCEnti.m_pAnimMgr->DBG_getModelHandle() ); // �������Z��Ԃ����Z�b�g�i����Ŕ��U����̂�����ł���H�j�� ��肭�����Ȃ�
			*/
		}

		// �A�j���[�V�����������Z�̃^�C���X�e�b�v�𐧌䂷��
		static double AnimPhysTimeElapsed = 0.001;
		if( CheckHitKey(KEY_INPUT_NUMPAD8)      > 0 ){ //���L�[��������Ă�����
			AnimPhysTimeElapsed += 0.001;
			PCEnti.m_pAnimMgr->m_pLeftHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
			PCEnti.m_pAnimMgr->m_pRightHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
		}
		else if( CheckHitKey(KEY_INPUT_NUMPAD2) > 0 ){ //�E�L�[��������Ă�����
			AnimPhysTimeElapsed -= 0.001;
			if( AnimPhysTimeElapsed < 0.0001 ) AnimPhysTimeElapsed = 0.0001;
			PCEnti.m_pAnimMgr->m_pLeftHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
			PCEnti.m_pAnimMgr->m_pRightHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
		}
		else if( CheckHitKey(KEY_INPUT_NUMPAD4) > 0 ){ //��L�[��������Ă�����
			AnimPhysTimeElapsed += 0.00001;
			PCEnti.m_pAnimMgr->m_pLeftHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
			PCEnti.m_pAnimMgr->m_pRightHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
		}
		else if( CheckHitKey(KEY_INPUT_NUMPAD6) > 0 ){ //���L�[��������Ă�����
			AnimPhysTimeElapsed -= 0.00001;
			if( AnimPhysTimeElapsed < 0.0001 ) AnimPhysTimeElapsed = 0.0001;
			PCEnti.m_pAnimMgr->m_pLeftHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
			PCEnti.m_pAnimMgr->m_pRightHairPhysics->setTimeElapsedPhys( AnimPhysTimeElapsed );
		}

		// ################## Entity��Update #######################
		PCEnti.Update( timeelaps );

#ifdef ITEM_GETTING
		// ######### �݂����������A�C�e������@�\�̎��� #########
		GameWorldIns.Update( timeelaps , PCEnti.Pos() );
#endif

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

#ifdef FARFAR_AWAY
		// �T�u��ʂ�`��Ώۂɐݒ�
		SetDrawScreen( SubScreenHandle );
		ClearDrawScreen(); // �T�u��ʂ��N���A

		CameraWorkManager::Instance()->setCamera();

		// �w�i�p�m���}�̕`��
		ExperimentSphere.setCenterPos( CameraWorkManager::Instance()->getCamPos() ); // �p�m���}���̒��S�ʒu���J�������S��
		ExperimentSphere.Render();

		// �ؐ��`��̂��߃N���b�v������ύX
		SetCameraNearFar( 10000000.0f, 50000000000.0f ) ; // �J������ ��O�N���b�v������ ���N���b�v������ݒ�

		// 100000000.0f
		// 1,500,000,000.0f

		// �ؐ�����
		//BackgroundObjectJupiter.Render();

		// �ؐ��W�I���}�̕`��
		JupiterDioramaIns.Update( 0.0001 );
		JupiterDioramaIns.setVertex();
		JupiterDioramaIns.Render();

		// ����ʂ�`��Ώۂɂ���
		SetDrawScreen(DX_SCREEN_BACK);

		// �T�u��ʂ̃O���t�B�b�N�𗠉�ʂɓ��ߏ������ŕ`��
		DrawGraph(0,0,SubScreenHandle,FALSE);

		// �N���b�v������߂�
		SetCameraNearFar( 1.0f, 1500.0f ) ; // �J������ ��O�N���b�v������ ���N���b�v������ݒ�
#endif
		// �J�����ʒu�Đݒ�
		CameraWorkManager::Instance()->setCamera();

		// ################## �n�ʂ̖͗l�̕`�� #######################
		

#ifdef MONUMENT_ON
		// �n�ʗp�̉~�Ղ�`��
		//GroundDisk.Render();
		GrateEarthGround.Render();

		// �~���̖��т�`��
		for( int i=0; i<MaxColumnsNum; i++ ){ ppColmunList[i]->Render(); }
#endif

#ifdef GROUND_MESH_ON
		// �n�ʂ̕���͗l��`��
		ObjGroundGrid.Render(); // �f���炵��..
#endif

#ifdef FLOATING_DUNGEON
		// ���V�_���W�����W�I���}�̕`��
		Dungeon.Render();
#endif

		// ################## Entity�̕`�� #######################
		PCEnti.Render();

#ifdef ITEM_GETTING
		// ######### �݂����������A�C�e������@�\�̎��� #########
		GameWorldIns.Render();
#endif
		// �L�����N�^���[�J�����W���f���`��e�X�g

		// �L�����N�^���[�J�����W�ɍ��킹�ĕ`�悷��悤�ɂ���
		MATRIX Mtns = MGetAxis1(	// �L�����N�^�̎p������ [ Head, Upper, Side ]
			PCEnti.Heading().toVECTOR(),
			PCEnti.Uppder().toVECTOR(),
			PCEnti.Side().toVECTOR(),
			PCEnti.Pos().toVECTOR()
			); 

		// ���͂��I�Ȃ񂩂��������ˁI�������Q�Ȃ����I

		CharCordiModelTest.setMatrix(Mtns);
		CharCordiModelTest.Render();

		// ################## �R���g���[���[��`�� #######################
		//VController.Render( Vector2D(25,25) );

		// ################## �f�o�b�N�����o�� #######################
		//�s��
		int colmun= 10;
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

		// ���݂�State�̕`��
		DrawFormatString( 0, width*colmun, 0xffffff, "StateName:%s", PCEnti.DBG_getCurrentStateName().c_str() ); 
		colmun++;


		// �A�i���O�X�e�B�b�N��Ԃ�Entity�̌�����`��
		PCEnti.DBG_renderStickTiltAndHeading( Vector2D( 400, 100 ) );

		// ���݂̃A�j���[�V�����̍Đ����Ԃ��o��
		DrawFormatString( 0, width*colmun, 0xffffff, "Current Animation Plya Time :%8f",PCEnti.m_pAnimMgr->CurPlayTime() ); 
		colmun++;

		// �Đ����̃A�j���[�V���������o��
		DrawFormatString(0, width*colmun, 0xffffff, "Current  Anim Name:%s", PCEnti.m_pAnimMgr->getCurAnimName().c_str());
		colmun++;

		DrawFormatString(0, width*colmun, 0xffffff, "Previous Anim Name:%s", PCEnti.m_pAnimMgr->getPrvAnimName().c_str());
		colmun++;

		/*
		// �L�����N�^�̃��[�J�����W�����o��
		DrawFormatString( 0, width*colmun, 0xffffff, "Head :%4f, %4f, %4f",PCEnti.Heading().x, PCEnti.Heading().y, PCEnti.Heading().z ); 
		colmun++;

		DrawFormatString( 0, width*colmun, 0xffffff, "Side :%4f, %4f, %4f",PCEnti.Side().x, PCEnti.Side().y, PCEnti.Side().z ); 
		colmun++;

		DrawFormatString( 0, width*colmun, 0xffffff, "Upper:%4f, %4f, %4f",PCEnti.Uppder().x, PCEnti.Uppder().y, PCEnti.Uppder().z ); 
		colmun++;
		*/

		// �T�u��Ԃ̌p�����Ԃ��o��
		//PCEnti.DBG_exp_OneEightyDegreeTurn_SubStateDurations( colmun );

		/*
		// Entity�̈ړ����x����\��
		DrawFormatString( 0, width*colmun, 0xffffff, "Entity Mode Level:%d",PCEnti.m_eMoveLevel ); 
		colmun++;

		// �A�i���O�X�e�B�b�N�̌X����
		DrawFormatString( 0, width*colmun, 0xffffff, "Analog Stick Tilt:%f",PCEnti.m_pVirCntrl->m_dStickL_len ); 
		colmun++;

		DrawFormatString( 0, width*colmun, 0xffffff, "pEntity->SpeedSq():%f",PCEnti.DBG_m_dDBG ); 
		colmun++;
		*/


		/*
		// ���݂̃A�j���[�V�����������Z���
		DrawFormatString( 0, width*colmun, 0xffffff, "AnimationPhysicsType:%d", PCEnti.m_pAnimMgr->getPhysicsType() ); 
		colmun++;

		// ���݂̃{�[���\���^���f���\��
		DrawFormatString( 0, width*colmun, 0xffffff, "AnimationPhysics-CurBoneExpress:%d", PCEnti.m_pAnimMgr->getCurBoneExpress() ); 
		colmun++;


		DrawFormatString( 0, width*colmun, 0xffffff, 
			"AnimationPhysics-CurBoneExpress(DXlib):%d", 
			MV1GetFrameVisible( 
				PCEnti.m_pAnimMgr->DBG_getModelHandle(), 
				PCEnti.m_pAnimMgr->m_iLeftHair1FrameIndex 
				)
			); 
		colmun++;

		// �A�j���[�V�����������Z�̃^�C���X�e�b�v��\��
		DrawFormatString( 0, width*colmun, 0xffffff, "AnimPhysTimeElapsed:%f", AnimPhysTimeElapsed ); 
		colmun++;

		// �A�j���[�V�����������Z�̏������Ԃ�\��
		DrawFormatString( 0, width*colmun, 0xffffff, 
			"DBG_m_dAverageTimeForUpdate:%f", 
			PCEnti.m_pAnimMgr->m_pLeftHairPhysics->DBG_m_dAverageTimeForUpdate ); 
		colmun++;
		*/

		// ################## �ޔ������Ă������������̍X�V #######################
		PCEnti.DBG_UpdateSavePhys();

	}

	DxLib_End() ;			// �c�w���C�u�����g�p�̏I������
	return 0 ;				// �\�t�g�̏I�� 
}

