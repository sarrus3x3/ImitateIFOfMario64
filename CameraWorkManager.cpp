#include <fstream> /// saveViewMatrix, loadViewMatrix �Ŏg�p�B

#include "DxLib.h"

#include "Vector3D.h"
#include "Vector2D.h"
#include "CameraWorkManager.h"

static const float ROTATE_SPEED = DX_PI_F/90;  //��]�X�s�[�h

// �R���X�g���N�^
CameraWorkManager::CameraWorkManager() :
	m_CurCamMode(RotateCamOnGazePoint),
	m_dTilt(0),
	m_dHead(0),
	m_dCamDistFrmFP(100.0),
	m_dCamHight(30.0),
	m_dDstCamToTrgtDef(30.0),
	m_TrgtHight(20.0),
	m_MViewLocal( MGetIdent() ),
	m_MViewWorld( MGetIdent() )
{

	m_dSqDstCamToTrgtDef = m_dDstCamToTrgtDef * m_dDstCamToTrgtDef;
	m_vFinalCamPos = Vector3D(0,m_dCamHight,0);
	m_TargetPos    = Vector3D(0,0,0);
	
	// �J�����̃r���[�s����O���t�@�C������ϐ��ɓǂݍ���
	loadViewMatrix();

}

CameraWorkManager* CameraWorkManager::Instance()
{
	static CameraWorkManager instance;
	return &instance;
};

void CameraWorkManager::getKeyInput()
{
	// �L�[���͂ɂ�鋅�̂̈ړ�
	static const double SPHERE_MOVE=3.0;
	if( CheckHitKey(KEY_INPUT_NUMPAD8)      > 0 )
	{ //���L�[��������Ă�����
		m_dTilt += ROTATE_SPEED;
	}
	else if( CheckHitKey(KEY_INPUT_NUMPAD2) > 0 )
	{ //�E�L�[��������Ă�����
		m_dTilt -= ROTATE_SPEED;
	}
	else if( CheckHitKey(KEY_INPUT_NUMPAD4) > 0 )
	{ //��L�[��������Ă�����
		m_dHead -= ROTATE_SPEED;
	}
	else if( CheckHitKey(KEY_INPUT_NUMPAD6) > 0 )
	{ //���L�[��������Ă�����
		m_dHead += ROTATE_SPEED;
	}
};

void CameraWorkManager::getMouseWheelInput( double timeslice )
{
	// C:\Users\kotaro MIHARA\Dropbox\Projects\GraphDraw_v0.0\GraphDraw_v0.0
	// DrawGraphMgr::update_scale() ��藬�p
 
	static double m_scroll; // �X�N���[����
	static const double maxsc =  2.302585093;   // log(10)  - �{���̏���l
	static const double minsc = -2.302585093;   // log(0.1) - �{���̉����l

	// �X�N���[�������炩�ɂ��邽�߁A�������f���𓱓�
	static const double MASS           = 0.5;	// ����
	static const double SPRING	       = 20;	// �o�l�萔
	static const double DAMPER         = 5;		// �����W��

	static double scrlloc = 0; // �ʒu
	static double scrlvel = 0; // ���x

	// m_scroll ���X�V
	int val = GetMouseWheelRotVol();
	m_scroll -= (16.0/100) * (double)(val);
	if(m_scroll>maxsc) m_scroll=maxsc; // ���
	if(m_scroll<minsc) m_scroll=minsc; // ����

	// ���f���ɏ]��Force(��)���v�Z
	double Force = SPRING * ( m_scroll - scrlloc );
	Force += -1 * ( DAMPER * scrlvel );

	// �����ʂ��X�V
	scrlvel += ( timeslice / MASS ) * Force;
	scrlloc += timeslice * scrlvel;

	// m_viewscale �̍X�V
	m_dCamDistFrmFP = 100*exp( scrlloc );

};

void CameraWorkManager::Update( double timeslice )
{
	switch (m_CurCamMode)
	{
	case RotateCamOnGazePoint:
		Update_RotateCamOnGazePoint( timeslice );
		break;

	case TrackingMovingTarget:
		Update_TrackingMovingTarget( timeslice );
		break;

	case SetSavedViewMatrix:
		Update_SetSavedViewMatrix();
		break;

	default:
		break;
	}
};

void CameraWorkManager::Update_RotateCamOnGazePoint( double timeslice )
{
	static const double DrugMoveScale=0.005; // �h���b�O�ʂɑ΂��鎋�_�̈ړ��ʂ̃p�����[�^

	static Vector2D ScreenDrugStartPoint;  // �}�E�X�Ńh���b�O���J�n�����X�N���[����̍��W���i�[
	static Vector2D SavedCamDist;          // �}�E�X�Ńh���b�O���J�n�����Ƃ��́Am_dTilt�Am_dHead ��ێ�
	static bool     nowDruging=false;      // ���h���b�O�����H

	// �L�[���͂���Am_dTilt �� m_dHead ���X�V
	//getKeyInput();

	// �X�N���[�����삩��A�J�����ƒ����_�̋������X�V
	getMouseWheelInput(timeslice);

	// �}�E�X�ɂ��h���b�O����ɂ��Am_dTilt �� m_dHead ���X�V
	if( nowDruging )
	{ // ���h���b�O��
		// �h���b�O�I���������𔻒�
		if( ( GetMouseInput() & MOUSE_INPUT_LEFT ) == 0 )
		{ // �h���b�O�I��
			nowDruging = false; 
		}
		else
		{
			// ���̃}�E�X�ʒu���擾
			Point2D pos;
			GetMousePoint( &(pos.x) , &(pos.y) );
			Vector2D CurMousePos( (double)pos.x, (double)pos.y);
			
			// m_dTilt �� m_dHead ���X�V
			Vector2D NewCamDist = -DrugMoveScale*(CurMousePos-ScreenDrugStartPoint)+SavedCamDist;
			m_dTilt = NewCamDist.y;
			m_dHead = NewCamDist.x;

		}
	}
	else
	{ // �h���b�O���łȂ�
		// �h���b�O�J�n�������𔻒�
		int button;
		Point2D pos;
		if( GetMouseInputLog( &button, &pos.x, &pos.y, TRUE )==0
			&& ( button & MOUSE_INPUT_LEFT ) != 0 )
		{ // �h���b�O���J�n���ꂽ
			// �h���b�O�J�n���� �}�E�X���W�� ScreenDrugStartPoint �ɕێ�
			ScreenDrugStartPoint = pos.toRealVector();

			// �h���b�O�J�n���� m_dTilt�Am_dHead �̒l�� SavedCamDist �ɕێ�
			SavedCamDist.y = m_dTilt;
			SavedCamDist.x = m_dHead;

			// �h���b�O�t���O�𗧂Ă�
			nowDruging = true;
		}
	}
	

	// m_dHead �� m_dTilt ����A�iGazingPoint�����_�Ƃ����j�J�����̈ʒu���v�Z����
	Vector3D Hedding3D( 1.0, 0, 0 );
	Hedding3D = VTransform( Hedding3D.toVECTOR(), MGetRotZ( (float)m_dTilt ) ); // ������������A�}�C�i�X�ɂ��Ȃ��Ƒʖڂ���
	Hedding3D = VTransform( Hedding3D.toVECTOR(), MGetRotY( -1*(float)m_dHead ) );
	Vector3D toCamPos3D = m_dCamDistFrmFP * Hedding3D ;

	// ### �r���[�s��𐶐����m�F

	// �J�����̍��W�ϊ��s���z�������i=�J�����̌����j
	Vector3D vViewBaseZ = -1 * Hedding3D; 

	// �J�����̍��W�ϊ��s���y�����������u��
	Vector3D vViewBaseY( 0.0, 1.0, 0.0 );

	// �J�����̍��W�ϊ��s���x�������i= y �~ z �j
	Vector3D vViewBaseX = VCross( vViewBaseY.toVECTOR(), vViewBaseZ.toVECTOR() );
	vViewBaseX = vViewBaseX.normalize();

	// �J�����̍��W�ϊ��s���y���������Ē���
	vViewBaseY = VCross( vViewBaseZ.toVECTOR(), vViewBaseX.toVECTOR() );

	// ����g�ݍ��킹�ăJ�������W�ϊ��s��iEntity���[�J�����W�j�𐶐�
	MATRIX MCamTransMatLocal = 
		MGetAxis1(
		//MakeMatrixFromBaseVec( 
			vViewBaseX.toVECTOR(),
			vViewBaseY.toVECTOR(),
			vViewBaseZ.toVECTOR(),
			toCamPos3D.toVECTOR() );
	MCamTransMatLocal.m[3][1] += m_TrgtHight;

	// Entity�ʒu�����V�t�g�������̂����[���h���W�ł̃J�������W�ϊ��s��
	MATRIX MCamTransMatWorld = MCamTransMatLocal;
	MCamTransMatWorld.m[3][0] += m_TargetPos.x;
	//MCamTransMatWorld.m[3][1] += m_TrgtHight; <- MCamTransMatLocal �̒i�K�ŃV�t�g
	MCamTransMatWorld.m[3][2] += m_TargetPos.z;

	// �t�s�񂪃r���[�s��
	m_MViewLocal = MInverse( MCamTransMatLocal );
	m_MViewWorld = MInverse( MCamTransMatWorld );
	
	// �w�i�p�m���}���̕`��Ɏg�p���Ă��邽�� m_vFinalCamPos ���Q�Ƃ��Ă���
	m_vFinalCamPos = VTransform( Vector3D( 0,0,0 ).toVECTOR(), MCamTransMatWorld );
};

void CameraWorkManager::Update_TrackingMovingTarget( double timeslice )
{
	// �i�n�ʂɎˉe�������́j�J�����Ɣ�ʑ̂̋�����
	//  DstCamToTrgt �ȉ��ɂȂ�悤�ɃJ�������ʑ̂Ɉ����񂹂�
	if( (m_TargetPos-m_vFinalCamPos).toVector2D().sqlen() > m_dSqDstCamToTrgtDef )
	{
		Vector2D CamPos2D = (m_vFinalCamPos-m_TargetPos).toVector2D().normalize() * m_dDstCamToTrgtDef;
		CamPos2D += m_TargetPos.toVector2D();
		m_vFinalCamPos = CamPos2D.toVector3D();
	}

	// m_vFinalCamPos �̍����𒲐�
	m_vFinalCamPos.y = m_TargetPos.y+m_dCamHight;

	// �J�����ʒu�����f���̍����ɍ����Ē���
	m_vFinalTargetPos = m_TargetPos;
	m_vFinalTargetPos.y += m_TrgtHight;

};

void CameraWorkManager::Update_SetSavedViewMatrix()
{
	m_MViewWorld = m_MSaveViewWorld;
};

void CameraWorkManager::setCamera()
{
	/*
	// DXlib�ŃJ�����̈ʒu��ݒ�
	SetCameraPositionAndTarget_UpVecY( m_vFinalCamPos.toVECTOR(), m_vFinalTargetPos.toVECTOR() );
	*/

	// �r���[�s��ݒ�
	SetCameraViewMatrix( m_MViewWorld );
};

std::string CameraWorkManager::getCurCamModeName()
{
	std::string str;
	switch (m_CurCamMode)
	{
	case RotateCamOnGazePoint:
		str = "RotateCamOnGazePoint";
		break;
	case TrackingMovingTarget:
		str = "TrackingMovingTarget";
		break;
	case SetSavedViewMatrix:
		str = "SetSavedViewMatrix";
		break;

	default:
		break;
	}
	return str; // ����œ����̂�...�H
};


// �J�����̃r���[�s���ۑ��i�O���t�@�C���ւ̏��o�����ϐ��ɕێ��j
void CameraWorkManager::saveViewMatrix( MATRIX mViewMat )
{
	m_MSaveViewWorld = mViewMat;

	std::ofstream ofs( "CameraWorkManager-SaveViewMatrix.txt" );

	for( int i=0; i<4; i++)
	{
		for( int j=0; j<4; j++ )
		{
			ofs << m_MSaveViewWorld.m[i][j] << std::endl;
		}
	}

};

// �J�����̃r���[�s����O���t�@�C������ϐ��ɓǂݍ���
void CameraWorkManager::loadViewMatrix()
{
	//�t�@�C���̓ǂݍ���
	ifstream ifs("CameraWorkManager-SaveViewMatrix.txt");
	if(!ifs)
	{
		// �t�@�C���Ǎ��Ɏ��s�i���邢�̓t�@�C�����̂��̂��Ȃ���΁j�X���[
		return ;
	}

	for( int i=0; i<4; i++)
	{
		for( int j=0; j<4; j++ )
		{
			//1�s���ǂݍ���
			string str;
			getline(ifs,str);

			// ���l(float)�ɕϊ�
			float tmp=stof(str); 
			m_MSaveViewWorld.m[i][j] = tmp;

		}
	}

};

