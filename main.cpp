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
#include "SolidObjects.h"

// �f�o�b�N�p
#include "AnimationManager.h"


// ########### ����p defin ###########

//#define MONUMENT_ON // ���j�������g�i�~���̖��сj����

#define GROUND_MESH_ON  // �n�ʂ̕���͗l����

#define FARFAR_AWAY // ���i�i�F�����x���j�ŉ����I�u�W�F�N�g��`�悷��ꍇ


// �ؐ��q���n�̃W�I���}
class JupiterSystemDiorama
{
private:
	// #### TextureSphere3D�^�̘f���I�u�W�F�N�g

	// �ؐ��I�u�W�F�N�g
	TextureSphere3D *m_pPrimalyObj;

	// �q���I�u�W�F�N�g�i�z��j
	std::vector<TextureSphere3D> m_cSatelliteObjList;

	// �q���O���̐��փI�u�W�F�N�g�i�z��j
	std::vector<LineRing> m_cSatelliteOrbitalObjList;

	// #### �����p�����[�^ ####

	// �启���a
	double m_dPrimalyStarRadius;

	// �启���]����(rotation period)
	double m_dPrimalyRotationPeriod;

public:
	// �q����
	static const int m_iSatelliteNum = 4;

	// �q�����i�񋓌^�j
	enum SatelliteID
	{
		SATELLITE_IO       = 0,
		SATELLITE_EUROPA   = 1,
		SATELLITE_GANYMEDE = 2,
		SATELLITE_CALLISTO = 3
	};

private:
	// �q�����a�i�z��j
	double *m_pSatelliteStarRadius;

	// �q���O�����a�i�z��j
	double *m_pSatelliteOrbitalRadius;

	// �q�����]�����i�P�ʁF���j�i�z��j
	double *m_pSatelliteRotationPeriod;

	// �q�����]�����i�P�ʁF���j�i�z��j
	double *m_pSatelliteOrbitalPeriod;

	// �启���]���x
	double m_dPrimalyRotationSpeed;

	// �q�����]���x�i���W�A���^���j�i�z��j
	double *m_pSatelliteRotationSpeed;

	// �q�����]���x�i���W�A���^���j�i�z��j
	double *m_pSatelliteOrbitalSpeed;


	// #### ��ԃp�����[�^ ####

	// �启���]�p
	double m_dPrimalyRotateAngle;

	// �q�����]�p�i�z��j
	std::vector<double> m_dSatelliteRotateAngleList;

	// �q�����]�p�i�z��j
	std::vector<double> m_dSatelliteOrbitalAngleList;

public:
	// ���[���h���W�ւ̕ϊ��s��
	MATRIX m_mLocalToWorldMatrix;

	// #### ���\�b�h ####

	// �R�X�g���N�^
	JupiterSystemDiorama();

	// �q���ʒu�X�V
	void Update( double TimeElapse );

	// �I�u�W�F�N�g�̒��_���ʒu�v�Z
	//   Render()�ŕ`�悷��O�ɁA
	//   �O���ʒu�Ȃǂ̌n��Ԃƕϊ��s�񂩂�e�I�u�W�F�N�g�̒��_�ʒu���v�Z����B
	//   Render()�ƕ������Ă���̂́A�n��ԁE�ϊ��s��̍X�V�Ȃ����ɏ����y�����������߁B
	void setVertex();

	// �`��
	void Render();

	// ���e�X�g���Ă݂��ق��������ł��傤�B

};

// �R���X�g���N�^�i�p�`�p�`�ƃf�[�^����͂��Ă����j
JupiterSystemDiorama::JupiterSystemDiorama() :
	m_mLocalToWorldMatrix( MGetIdent() ), // �}�g���N�X�̏�����
	m_dSatelliteRotateAngleList( m_iSatelliteNum, 0 ),
	m_dSatelliteOrbitalAngleList( m_iSatelliteNum, 0 ),
	m_dPrimalyRotateAngle( 0 )
{
	// ######### �e��ϐ��̏��������� #########

	// #### �����p�����[�^ ####
	// Desktop\�f��\�f���e�N�X�`��\�ؐ��Ɖq��

	// �启���a
	m_dPrimalyStarRadius = 8.93650000;

	// �启���]����(rotation period)
	m_dPrimalyRotationPeriod = 0.4135;

	// �q�����a�i�z��j
	m_pSatelliteStarRadius = new double[m_iSatelliteNum];
	m_pSatelliteStarRadius[SATELLITE_IO      ] = 0.22766667;
	m_pSatelliteStarRadius[SATELLITE_EUROPA  ] = 0.19510000;
	m_pSatelliteStarRadius[SATELLITE_GANYMEDE] = 0.32890000;
	m_pSatelliteStarRadius[SATELLITE_CALLISTO] = 0.30128750;
	/*
	m_pSatelliteStarRadius[SATELLITE_IO      ] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_EUROPA  ] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_GANYMEDE] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_CALLISTO] = 3.0;
	*/

	// �q���O�����a�i�z��j
	m_pSatelliteOrbitalRadius = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalRadius[SATELLITE_IO      ] =  52.71250000;
	m_pSatelliteOrbitalRadius[SATELLITE_EUROPA  ] =  83.87925000;
	m_pSatelliteOrbitalRadius[SATELLITE_GANYMEDE] = 133.80150000;
	m_pSatelliteOrbitalRadius[SATELLITE_CALLISTO] = 235.33862500;

	// �q�����]�����i�z��j
	m_pSatelliteRotationPeriod = new double[m_iSatelliteNum];
	m_pSatelliteRotationPeriod[SATELLITE_IO      ] = 1.769137786;
	m_pSatelliteRotationPeriod[SATELLITE_EUROPA  ] = 3.551181041;
	m_pSatelliteRotationPeriod[SATELLITE_GANYMEDE] = 7.15455296;
	m_pSatelliteRotationPeriod[SATELLITE_CALLISTO] = 16.6890184;

	// �q�����]�����i�z��j�iorbital period�j
	m_pSatelliteOrbitalPeriod = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalPeriod[SATELLITE_IO      ] = 1.769137786;
	m_pSatelliteOrbitalPeriod[SATELLITE_EUROPA  ] = 3.551181041;
	m_pSatelliteOrbitalPeriod[SATELLITE_GANYMEDE] = 7.15455296;
	m_pSatelliteOrbitalPeriod[SATELLITE_CALLISTO] = 16.6890184;

	// #### TextureSphere3D�^�̘f���I�u�W�F�N�g

	// �ؐ��I�u�W�F�N�g
	m_pPrimalyObj = new TextureSphere3D(
				Vector3D( 0, 0, 0),
				m_dPrimalyStarRadius,
				true,
				32,
				32,
				LoadGraph( "JupiterSystemDiorama\\jupiter.jpg" ),
				TextureSphere3D::OBJECT_NOSPECULAR
			);

	// �q���I�u�W�F�N�g�i�z��j
	//   �z��ō\�z�i���m�ہj���āAvector�^�̃����o�ɑ��������@���Ƃ�
	//   ���������N���X�̔z��̏��������@
	//     http://brain.cc.kogakuin.ac.jp/~kanamaru/lecture/C++2/09/09-01.html
	TextureSphere3D Tmp[] = {
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_IO      ],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\0_io_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_EUROPA  ],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\1_europa_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_GANYMEDE],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\2_ganymede_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_CALLISTO],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\3_callisto_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			)
	};

	m_cSatelliteObjList.assign( &Tmp[0], &Tmp[m_iSatelliteNum] ); // vector�^�̃����o�ɑ��

	// �q���O���̐��փI�u�W�F�N�g�i�z��j
	LineRing Tmp2[] = {
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_IO      ],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_EUROPA  ],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_GANYMEDE],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_CALLISTO],
			32,
			GetColor( 255, 255, 255 )
			)
	};

	m_cSatelliteOrbitalObjList.assign( &Tmp2[0], &Tmp2[m_iSatelliteNum] ); // vector�^�̃����o�ɑ��

	// ### ���]���x�E���]���x���v�Z���Ċi�[
	
	// �启
	m_dPrimalyRotationSpeed = 2*DX_PI_F/(double)m_dPrimalyRotationPeriod;

	// �q��
	m_pSatelliteRotationSpeed = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalSpeed  = new double[m_iSatelliteNum];
	for( int i=0; i<m_iSatelliteNum; i++ )
	{
		m_pSatelliteRotationSpeed[i] = 2*DX_PI_F/(double)m_pSatelliteRotationPeriod[i];
		m_pSatelliteOrbitalSpeed[i]  = 2*DX_PI_F/(double)m_pSatelliteOrbitalPeriod[i];
	}

};

void JupiterSystemDiorama::setVertex()
{
	static MATRIX WorkMat;

	// #### ���]�p�A���]�p����A���[�J�����W�ɂ�����ʒu���v�Z

	m_pPrimalyObj->resetVertex();

	// �启 - ���]
	WorkMat = MGetRotY( (float)m_dPrimalyRotateAngle );

	// �启 - ���[���h���W�ɕϊ�
	WorkMat = MMult( WorkMat, m_mLocalToWorldMatrix );

	// Vertex�ɔ��f
	m_pPrimalyObj->MatTransVertex( WorkMat );


	// �q��
	for( int i=0; i<m_iSatelliteNum; i++ )
	{ 
		m_cSatelliteObjList[i].resetVertex();

		// ���]
		WorkMat = MGetRotY( (float)m_dSatelliteRotateAngleList[i] );

		// ���]
		Vector2D OrbitalPos2D( 1, 0 );
		OrbitalPos2D = m_pSatelliteOrbitalRadius[i] * OrbitalPos2D.rot(m_dSatelliteOrbitalAngleList[i]);
		//Vector3D OrbitalPos3D = OrbitalPos2D.toVector3D();
		
		WorkMat.m[3][0] = OrbitalPos2D.x;
		WorkMat.m[3][2] = OrbitalPos2D.y;

		// ���[���h���W�ɕϊ�
		WorkMat = MMult( WorkMat, m_mLocalToWorldMatrix ); // ���W�ϊ��s�����������Ƃ��́A��ɍ�p���������s��� MMult �̍����ɒu�����ƁB

		// Vertex�ɔ��f
		m_cSatelliteObjList[i].MatTransVertex( WorkMat );

		// �O����
		m_cSatelliteOrbitalObjList[i].resetVertex();
		m_cSatelliteOrbitalObjList[i].MatTransVertex( m_mLocalToWorldMatrix );

	}

};

void JupiterSystemDiorama::Update( double TimeElapse )
{
	// ���]�p�A���]�p���X�V
	m_dPrimalyRotateAngle += TimeElapse*m_dPrimalyRotationSpeed;

	for( int i=0; i<m_iSatelliteNum; i++ )
	{
		m_dSatelliteRotateAngleList[i]  += TimeElapse*m_pSatelliteRotationSpeed[i];
		m_dSatelliteOrbitalAngleList[i] -= TimeElapse*m_pSatelliteOrbitalSpeed[i];
	}
};

void JupiterSystemDiorama::Render()
{
	// �e�I�u�W�F�N�g�̕`�惁�\�b�h���ĂԂ���

	// �启
	m_pPrimalyObj->Render();

	// �q��
	for( int i=0; i<m_iSatelliteNum; i++ )
	{ 
		m_cSatelliteObjList[i].Render();

		// �O����\��
		m_cSatelliteOrbitalObjList[i].Render();

	}
	
};

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
	VirtualController VController; // ���z�R���g���[���[�̃C���X�^���X��

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
	GroundGrid ObjGroundGrid( 1000.0, 100 );

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
	SetLightDirection( VGet( 1.0, 0, 0 ) );

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

		// ################## �R���g���[���[���X�V #######################
		VController.Update();

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
			PCEnti.m_pAnimMgr->ExpBoneOfPhysicsPart( !PCEnti.m_pAnimMgr->getCurBoneExpress() );
			MV1PhysicsResetState( PCEnti.m_pAnimMgr->DBG_getModelHandle() ); // �������Z��Ԃ����Z�b�g�i����Ŕ��U����̂�����ł���H�j�� ��肭�����Ȃ�
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

	}

	DxLib_End() ;			// �c�w���C�u�����g�p�̏I������
	return 0 ;				// �\�t�g�̏I�� 
}

