#include <cassert>

#include "DEMO_StickTiltProjectorDemo.h"

int GlobalLineColor = GetColor( 0, 0, 0 );


// �^����ꂽ�ʒu�ɁA���W���i���[���h���W�j��\������֐�
void DrawCoordi( Vector3D TargetPosVec, double scale )
{
	static const Vector3D vUnitX( 1.0, 0.0, 0.0 );
	static const Vector3D vUnitY( 0.0, 1.0, 0.0 );
	static const Vector3D vUnitZ( 0.0, 0.0, 1.0 );

	// �^�[�Q�b�gpos��`�悷��
	DrawLine3D( TargetPosVec.toVECTOR(), (TargetPosVec+scale*vUnitX).toVECTOR(), GetColor( 255, 0, 0 )); // x����Ԑ��ŕ`��
	DrawLine3D( TargetPosVec.toVECTOR(), (TargetPosVec+scale*vUnitY).toVECTOR(), GetColor( 0, 255, 0 )); // y����ΐ��ŕ`��
	DrawLine3D( TargetPosVec.toVECTOR(), (TargetPosVec+scale*vUnitZ).toVECTOR(), GetColor( 0, 0, 255 )); // z������ŕ`��

};

// �^����ꂽ�ʒu�ɁA�i���[���h���W����݁j�����[�J�����W�̊�ꎲ��\������֐�
void DrawCoordi(  MATRIX M, double scale )
{
	Vector3D vUnitX(M.m[0][0], M.m[0][1], M.m[0][2]);
	Vector3D vUnitY(M.m[1][0], M.m[1][1], M.m[1][2]);
	Vector3D vUnitZ(M.m[2][0], M.m[2][1], M.m[2][2]);
	Vector3D vTgtPs(M.m[3][0], M.m[3][1], M.m[3][2]);

	// �^�[�Q�b�gpos��`�悷��
	DrawLine3D( vTgtPs.toVECTOR(), (vTgtPs+scale*vUnitX).toVECTOR(), GetColor( 255, 0, 0 )); // x����Ԑ��ŕ`��
	DrawLine3D( vTgtPs.toVECTOR(), (vTgtPs+scale*vUnitY).toVECTOR(), GetColor( 0, 255, 0 )); // y����ΐ��ŕ`��
	DrawLine3D( vTgtPs.toVECTOR(), (vTgtPs+scale*vUnitZ).toVECTOR(), GetColor( 0, 0, 255 )); // z������ŕ`��
};

// ########################## SectorFigure2D ##########################

// �`�悷��Ƃ��̔{��
double SectorFigure2D::m_dRenderScale = 25.0;

// �R���X�g���N�^
// �@ �J�n�p�ƏI���p�i���W�A���j��^����ꂽ��A���_���S�ɐ�`�𐶐�����
SectorFigure2D::SectorFigure2D( double Radius, double StartAng, double EndAng, int DivNum ) : 
	m_iRenderColor( 0 ),
	m_dRotation( 0 )
{
	m_iVertexNum = DivNum+2;
	m_pVertexes = new Vector2D[m_iVertexNum];

	assert( EndAng > StartAng );
	assert( DivNum>1 );

	m_pVertexes[0] = Vector2D( 0.0, 0.0 );

	// �����̊Ԋu
	double IntervalAng = (EndAng - StartAng)/((double)DivNum);

	for( int i=0; i<=DivNum; i++)
	{
		double ang  = StartAng + i*IntervalAng;
		Vector2D vAng( cos(ang), sin(ang) );
		vAng *= Radius;
		m_pVertexes[i+1] = vAng;
	}

};

// �R���X�g���N�^
// �A �J�n�p�ƏI���p���x�N�g���ŗ^���邱�Ƃ��ł���
SectorFigure2D::SectorFigure2D( double Radius, Vector2D StartVec, Vector2D EndVec, int DivNum ) : 
	m_iRenderColor( 0 ),
	m_dRotation( 0 )
{
	double StartAng = atan2( StartVec.y, StartVec.x );
	double EndAng   = atan2( EndVec.y,   EndVec.x );

	m_iVertexNum = DivNum+2;
	m_pVertexes = new Vector2D[m_iVertexNum];

	//assert( EndAng > StartAng );
	// �t�O�p�֐��́h�؂�ځh�ɓ������Ă��܂����ꍇ�̑΍������B
	// �� �p�̊J�����A�΂��傫���Ȃ��Ă���ꍇ�͂Q�΃V�t�g����B
	if( !(EndAng > StartAng) )
	{
		EndAng += 2*DX_PI_F;
	}

	assert( DivNum>1 );

	m_pVertexes[0] = Vector2D( 0.0, 0.0 );

	// �����̊Ԋu
	double IntervalAng = (EndAng - StartAng)/((double)DivNum);

	for( int i=0; i<=DivNum; i++)
	{
		double ang  = StartAng + i*IntervalAng;
		Vector2D vAng( cos(ang), sin(ang) );
		vAng *= Radius;
		m_pVertexes[i+1] = vAng;
	}
}


SectorFigure2D::~SectorFigure2D()
{
	delete m_pVertexes;
	delete m_pPolyVertex;
};


// �`��p�|���S���̐���
void SectorFigure2D::GeneratePolygons()
{
	// Vertex ���v�Z����
	m_iPolygonNum = m_iVertexNum - 2;

	// Vectex�̃��������m�� Vertex���FDivNum * 4(��ʁE��ӁE���ʁi�Q�{�j) * 3�i�P�|���S���̒��_���j
	int iPolyVectexNum = m_iPolygonNum*3;
	m_pPolyVertex = new Vector2D[iPolyVectexNum];
	
	// �֊s���_��񂩂�A�|���S�����𐶐�����B
	// ���߂̗v�f pVertex2D[0] �𒆐S�ɕ��ˏ�ɎO�p�`��������

	int c=0; //�J�E���^
	for( int i=2; i<m_iVertexNum; i++ )
	{
		// �O�p�`
		m_pPolyVertex[ 3*c+0 ] = m_pVertexes[  0];
		m_pPolyVertex[ 3*c+1 ] = m_pVertexes[i-1];
		m_pPolyVertex[ 3*c+2 ] = m_pVertexes[  i];
		c++;
	}

};

// ���ʃL�����o�X��ɕ`�悷��@�\����������
void SectorFigure2D::RenderOnCanvas( Vector2D RenderPos )
{
	// �\�����̉�]�ʂ����]�s����v�Z����
	C2DMATRIX RotMat = C2DMGetRot( m_dRotation );

	// 2�����s��N���X���������Ȃ��Ƃ����Ȃ���.. �� �Q�[��AI�̒�`���Q�l�ɂ��邩

	for( int i=0; i<m_iPolygonNum; i++)
	{
		// Window�ɕ`�悷��Ƃ��͉�ʉ�������y���������ɂȂ邽�߁A��`�̌`��ۂ��߂�y�����𔽓]������
		Vector2D TmpVec1 = C2DVTransform((m_dRenderScale * m_pPolyVertex[ 3*i+0 ]), RotMat ).reversY();
		Vector2D TmpVec2 = C2DVTransform((m_dRenderScale * m_pPolyVertex[ 3*i+1 ]), RotMat ).reversY();
		Vector2D TmpVec3 = C2DVTransform((m_dRenderScale * m_pPolyVertex[ 3*i+2 ]), RotMat ).reversY();

		Point2D TmpPos1 = (RenderPos + TmpVec1).toPoint();
		Point2D TmpPos2 = (RenderPos + TmpVec2).toPoint();
		Point2D TmpPos3 = (RenderPos + TmpVec3).toPoint();

		DrawTriangle( 
			TmpPos1.x, TmpPos1.y,
			TmpPos2.x, TmpPos2.y,
			TmpPos3.x, TmpPos3.y,
			m_iRenderColor ,
			TRUE );
	}

}

// ########################## StickTiltProjectorDemo ##########################

// �X�e�B�b�N�̋O�Ղ̕�����
int StickTiltProjectorDemo::m_iStickTrackDivNum = 256;

// ��`�̕�����
int StickTiltProjectorDemo::m_iSectorDivNum = 32;

double StickTiltProjectorDemo::SectorRadius = 3.0;
double StickTiltProjectorDemo::AngeSize = DX_PI_F/12;

Vector2D XPlusDir( 1.0, 0.0 );
Vector2D YPlusDir( 0.0, 1.0 );

// �R���X�g���N�^
StickTiltProjectorDemo::StickTiltProjectorDemo(
		Vector3D vModelCamPos,          // �����p�J�����̈ʒu
		Vector3D vModelCamGazePoint,    // �����p�J�����̏œ_
		double   dModelCamNearClipDist, // �����p�J�����̃N���b�v����
		Vector3D vPlayerPos             // �i���삷��j�L�����N�^�̈ʒu
		) : m_vModelCamPos( vModelCamPos ),
			m_vModelCamGazePoint( vModelCamGazePoint ),
			m_dModelCamNearClipDist( dModelCamNearClipDist ),
			m_vPlayerPos( vPlayerPos ),
			m_vStickTiltOnScreen( Vector3D( 1.0, 0.0, 0.0 ).normalize() ),
			//m_vStickTiltOnScreen( Vector3D( 1.0, 0.0, 0.0 ).normalize() ),
			m_ScPlane( ScreenPlane( 5.0, 5.0, 3.0 ) ),
			m_dGridRangeOnScreen( 3.0 ),
			m_StickTrackOnScreen( LineFreeCycle( m_iStickTrackDivNum, GetColor(0,0,255)) ),
			m_StickTrackProjection( LineFreeCycle( m_iStickTrackDivNum, GetColor(255,0,0)) ),
			m_StickTrackProjectionNormalize( LineFreeCycle( m_iStickTrackDivNum, GlobalLineColor) ),
			m_vVertiStickTiltDirBgn( YPlusDir.rot( -AngeSize ) ),
			m_vVertiStickTiltDirEnd( YPlusDir.rot(  AngeSize ) ),
			m_vHorizStickTiltDirBgn( XPlusDir.rot( -AngeSize ) ),
			m_vHorizStickTiltDirEnd( XPlusDir.rot(  AngeSize ) )
{
	// �֐��|�C���^�֑��
	fpCalcStickTiltPosProjection = &StickTiltProjectorDemo::calcStickTiltPos_RigidTrans;
	//fpCalcStickTiltPosProjection = &StickTiltProjectorDemo::calcStickTiltPos_HomogTrans;

	// �J�������f���̓ǂݍ���
	m_iModelCamHandle = MV1LoadModel( "..\\mmd_model\\Camera\\�J����.x" );

	// MatConf�̐ݒ� :: ���f���T�C�Y�E����������
	MatConf = MGetScale( VGet( 10.0, 10.0, 10.0) );
	MatConf = MMult( MatConf, MGetRotX( 0.5*DX_PI_F ) ) ;// ��������
	MatConf = MMult( MatConf, MGetRotY( 0.5*DX_PI_F ) ) ;// ��������

	// #### ��`�̃f�[�^�̏����� ####

	// ## �I���W�i���̃X�e�B�b�N�̌X�� �̏�����
	UpdateSectorOrgStickTiltDir();

	// �e�f�B�X�v���C�p��`���f���̏�����
	m_pVertiSectorOnScreen = new PlaneConvexFill( 
		m_pVertiSectorOrgStickTiltDir->m_pVertexes, 
		m_pVertiSectorOrgStickTiltDir->m_iVertexNum, 
		GetColorF( 0.0f, 1.0f, 0.0f, 0.0f ) );

	m_pVertiSectorProjection = new PlaneConvexFill( 
		m_pVertiSectorOrgStickTiltDir->m_pVertexes, 
		m_pVertiSectorOrgStickTiltDir->m_iVertexNum, 
		GetColorF( 0.0f, 1.0f, 0.0f, 0.0f ) );

	m_pHorizSectorOnScreen = new PlaneConvexFill( 
		m_pHorizSectorOrgStickTiltDir->m_pVertexes, 
		m_pHorizSectorOrgStickTiltDir->m_iVertexNum, 
		GetColorF( 0.0f, 0.0f, 1.0f, 0.0f ) );

	m_pHorizSectorProjection = new PlaneConvexFill( 
		m_pHorizSectorOrgStickTiltDir->m_pVertexes, 
		m_pHorizSectorOrgStickTiltDir->m_iVertexNum, 
		GetColorF( 0.0f, 0.0f, 1.0f, 0.0f ) );

	// ���f���X�e�[�^�X������W�ϊ��s����X�V
	UpdateTransMats();

	// ���f���X�e�[�^�X����􉽓I���W�ʒu���X�V
	UpdateGeoPoss();

	// ## �i�X�e�B�b�N�̌X������j�v�Z���ꂽ�L�����N�^�̐i�s�����i��}�������`�j���v�Z����
	UpdateSectorCharactrMoveDir();

	// ## ���o�p�O���b�h�̏�����
	UpdateGrids();

}

void StickTiltProjectorDemo::UpdateTransMats()
{
	// �X�N���[�����[�J�����W�́i���[���h���W�ɂ�����j��ꎲ���v�Z����B

	// y'�� = �J�����̕���
	Vector3D Ysc = (m_vModelCamGazePoint-m_vModelCamPos).normalize();

	// x'�� = �J�����̏������y��������
	Vector3D Xsc( 0, 1.0, 0 );

	// z'�� = x' �~ y' �� ���̋K�����Ԉ���Ă���H�H DEMO_DashTurnBehavior.h ���݂�
	Vector3D Zsc = VCross( Ysc.toVECTOR(), Xsc.toVECTOR() ); // �W�����W�n�𔽓]���Ă��邱�Ƃɒ���
	Zsc = Zsc.normalize();

	// x'�� �̕␳
	Xsc = VCross( Zsc.toVECTOR(), Ysc.toVECTOR() ); // �W�����W�n�𔽓]���Ă��邱�Ƃɒ���

	// mModelCamLocalToWorld �̌v�Z
	mModelCamLocalToWorld = 
		MGetAxis1( 
			Xsc.toVECTOR(),
			Ysc.toVECTOR(),
			(-1*Zsc).toVECTOR(), // ���W�n�����]���Ă��邽��
			m_vModelCamPos.toVECTOR() );

	// mModelCamWorldToLocal �̌v�Z
	mModelCamWorldToLocal = MInverse( mModelCamLocalToWorld );

	// mModelCamViewMat �̌v�Z�i�����p�J�����̃r���[�s��j
	MATRIX mModelCamViewMatBase =
		MGetAxis1( 
			(-1*Zsc).toVECTOR(), // ���W�n��ۂ��߂ɂ͌����𔽓]������K�v����
			Xsc.toVECTOR(), // y��������
			Ysc.toVECTOR(), // z���������i�J�����̌����j
			m_vModelCamPos.toVECTOR() );

	mModelCamViewMat = MInverse( mModelCamViewMatBase );
	//mModelCamViewMat = mModelCamViewMatBase;

	// ���[���h���W�ɂ�����X�N���[�����W���S�ʒu�̌v�Z
	Vector3D vScreenCntPos = m_dModelCamNearClipDist*Ysc + m_vModelCamPos; 

	// mScreenLocalToWorld �̌v�Z
	mScreenLocalToWorld = 
		MGetAxis1( 
			Xsc.toVECTOR(),
			Ysc.toVECTOR(),
			Zsc.toVECTOR(),
			vScreenCntPos.toVECTOR() );

	// mScreenWorldToLocal �̌v�Z
	mScreenWorldToLocal = MInverse( mScreenLocalToWorld );

	// mScreenPosRigidTrans �̌v�Z�i�J��������݂��������ۂ��ăX�N���[�����W�����[�J�����W�֍��̕Ԋҁj
	Vector3D vScUpper( 0.0, 0.0, 1.0 );
	Vector3D vBaseY( 0.0, 1.0, 0.0 );
	Vector3D vBaseZ, vOrign;
	Vector3D vScUpperWorld, vScUpperProj;
	
	vScUpperWorld = VTransform( vScUpper.toVECTOR(), mScreenLocalToWorld );
	calcCrossPointWithXZPlane( m_vModelCamPos, vScUpperWorld, vScUpperProj );
	calcCrossPointWithXZPlane( m_vModelCamPos, vScreenCntPos, vOrign );

	vBaseZ = (vScUpperProj-vOrign).normalize();
	//Vector3D vBaseX = vBaseY * vBaseZ ; 
	// �� �Ȃ����R���p�C���G���[�ɂȂ�B�����͂킩��Ȃ�... �z���[���B
	Vector3D vBaseX = VCross( vBaseY.toVECTOR(), vBaseZ.toVECTOR() ) ;  // �d�����Ȃ��̂ŁAdx���C�u�����̑g���݊֐����g��...

	mScreenPosRigidTrans = MGetAxis1( 
		vBaseX.toVECTOR(),
		vBaseY.toVECTOR(),
		vBaseZ.toVECTOR(),
		vOrign.toVECTOR() );

	// ���o�̂��߁A�����ϊ��{�����グ��H�i���e�����Ɣ�r���鎞�ɃO���b�h���������Ȃ�j
	mScreenPosRigidTrans = MMult( MGetScale(Vector3D(5,5,5).toVECTOR()), mScreenPosRigidTrans ); // 5�{�Ɋg��

	m_vScreenCntPosOnScreen      = vScreenCntPos;
	m_vScreenCntPosOnEntityPlane = vOrign;

	// ��x�A�����ƕϊ�����Ă��邩�A���Ă݂邩�B
	DrawCoordi( mModelCamLocalToWorld, 2.0 );
	DrawCoordi( mScreenLocalToWorld, 2.0 );

};

// ���f���X�e�[�^�X����􉽓I���W�ʒu���X�V
void StickTiltProjectorDemo::UpdateGeoPoss()
{
	// ## �X�N���[����̃L�����N�^�[�ʒu�����Ƃ߂�
		
	// �X�N���[�����[�J�����W�ɂ�����L�����N�^�[�ʒu�����߂�
	Vector3D vEntiPosForScLocal = VTransform( m_vPlayerPos.toVECTOR(), mScreenWorldToLocal );

	// �X�N���[�����[�J�����W�ɂ�����J�����ʒu�����߂�
	Vector3D vCamPosForScLocal = VTransform( m_vModelCamPos.toVECTOR(), mScreenWorldToLocal );

	// �X�N���[�����[�J�����W�ɂ�����J�����ʒu-�L�����N�^�[�ʒu��xz���ʌ�_�����߂�
	calcCrossPointWithXZPlane( vCamPosForScLocal, vEntiPosForScLocal, m_vEntiPosAsScreenForScLocal );

	// ��L��_�����[���h���W�ɕϊ� �� ���ꂪ���[���h���W�ɂ�����A�X�N���[����i�ɉf��j�̃L�����N�^�ʒu
	m_vEntiPosAsScreenForWorld = VTransform( m_vEntiPosAsScreenForScLocal.toVECTOR(), mScreenLocalToWorld );

	// ######
	// �߂������ƂɁA�P��݂̂����ĂԂ��Ƃ��l���Ă��Ȃ��B


	// ## �X�N���[����̃X�e�B�b�N�̋O�ՂƁA���̓��e�~���v�Z

	LineRing StickTrackForScLocal( 3.0, m_iStickTrackDivNum, GlobalLineColor );
	
	for( int i=0; i<m_iStickTrackDivNum; i++)
	{
		(this->*fpCalcStickTiltPosProjection)( 
			StickTrackForScLocal.editVertexes()[i],
			m_StickTrackOnScreen.editVertexes()[i],
			m_StickTrackProjection.editVertexes()[i]
		);
	}

	// �`��p���_�f�[�^�̏�����
	m_StickTrackOnScreen.resetVertex();
	m_StickTrackProjection.resetVertex();


	// ## �O�Ղ𓊉e�����Ƃ��̕����ɂ�鍷�����v���[�����邽�߂̐�`


	for( int i=0; i<m_pVertiSectorOnScreen->getAllVertexNum(); i++)
	{
		(this->*fpCalcStickTiltPosProjection)( 
			m_pVertiSectorOnScreen->editVertexes()[i],
			m_pVertiSectorOnScreen->editVertexes()[i],
			m_pVertiSectorProjection->editVertexes()[i]
		);

		(this->*fpCalcStickTiltPosProjection)( 
			m_pHorizSectorOnScreen->editVertexes()[i],
			m_pHorizSectorOnScreen->editVertexes()[i],
			m_pHorizSectorProjection->editVertexes()[i]
		);

	}

	// �`��p���_�f�[�^�̏�����
	m_pVertiSectorOnScreen->resetVertex();
	m_pVertiSectorProjection->resetVertex();
	m_pHorizSectorOnScreen->resetVertex();
	m_pHorizSectorProjection->resetVertex();


	// ## ���e�𐳋K�������ꍇ�̉~��`��
	
	// ���̔��a�����߂�̂���� �� �͂����Ōv�Z���悤

	Vector3D vPlayerPosProj; // vPlayerPos ��xz���ʂɓ��e�����ʒu
	Vector3D vTmp;
	(this->*fpCalcStickTiltPosProjection)( m_vEntiPosAsScreenForScLocal, vTmp, vPlayerPosProj );

	double minimize = 10000; // vPlayerPosProj �� �֊s�� m_StickTrackProjection �̍ŏ����� = m_StickTrackProjectionNormalize �̔��a�Ƃ���B

	for( int i=0; i<m_iStickTrackDivNum; i++)
	{
		minimize = min( minimize, (vPlayerPosProj-m_StickTrackProjection.editVertexes()[i]).sqlen() );
	}
	minimize = sqrt(minimize);

	LineRing TmpRing( minimize, m_iStickTrackDivNum, GlobalLineColor );

	// ���S�ʒu���V�t�g���� m_StickTrackProjectionNormalize �ɐݒ�
	for( int i=0; i<m_iStickTrackDivNum; i++)
	{
		m_StickTrackProjectionNormalize.editVertexes()[i] = TmpRing.editVertexes()[i] + vPlayerPosProj;
	}

	// �`��p���_�f�[�^�̏�����
	m_StickTrackProjectionNormalize.resetVertex();

}

// �X�e�B�b�N�̌X���̕����ix-z���ʏ�j�� ���[��x-z���ʏ�֓��e�������W�֕ϊ�����֐���p�ӂ���
// * �ˉe�ϊ� HomogTrans �i�z���O���t�B�ϊ��j
//   ���ŁB��ʃX�N���[���ɓ��e�����X�e�B�b�N�̌X��������Entity�̕��ʂɓ��e����
void StickTiltProjectorDemo::calcStickTiltPos_HomogTrans(
	Vector3D vStickTiltPosForScLocal, // [IN] �X�N���[�����[�J�����W�ɂ�����X�e�B�b�N�̌X�������ʒu
	Vector3D &vStickTiltPosForWorld,  // [OUT] ���[���h���W�ɂ�����X�e�B�b�N�̌X�������ʒu
	Vector3D &vStickTiltPosProjection // [OUT] xz���ʏ�ɓ��e�����X�e�B�b�N�̌X�������ʒu�i���[���h���W�ɂ�����j
	)
{
	// ## �L�����N�^�[�ʒu���S�ɃX�e�B�b�N�̌X��������`��
		
	// �X�N���[�����[�J�����W�ɂ�����X�e�B�b�N�̌X�������ʒu�����߂�
	//Vector3D vStickTiltPosForScLocal = vStickTilt + vEntiPosAsScreenForScLocal;

	// ��L�ʒu�����[���h���W�ɕϊ� �� �X�N���[����X�e�B�b�N�X�������ʒu
	vStickTiltPosForWorld = VTransform( vStickTiltPosForScLocal.toVECTOR(), mScreenLocalToWorld );

	// ## �X�N���[�����猩���X�e�B�b�N�̌X���������L�����N�^�[�̒n���ʂɓ��e

	// �J�����ʒu-�X�N���[����X�e�B�b�N�X�������ʒu��xz���ʌ�_�����߂�
	calcCrossPointWithXZPlane( m_vModelCamPos, vStickTiltPosForWorld, vStickTiltPosProjection );


};

// * ���̕ϊ� RigidTrans
//   ���ǔŁB������̌��������������ڂƍ����A�p�x��ۑ����ĕϊ�����B
void StickTiltProjectorDemo::calcStickTiltPos_RigidTrans(
	Vector3D vStickTiltPosForScLocal, // [IN] �X�N���[�����[�J�����W�ɂ�����X�e�B�b�N�̌X�������ʒu
	Vector3D &vStickTiltPosForWorld,  // [OUT] ���[���h���W�ɂ�����X�e�B�b�N�̌X�������ʒu
	Vector3D &vStickTiltPosProjection // [OUT] xz���ʏ�ɓ��e�����X�e�B�b�N�̌X�������ʒu�i���[���h���W�ɂ�����j
	)
{
	// ��L�ʒu�����[���h���W�ɕϊ� �� �X�N���[����X�e�B�b�N�X�������ʒu
	vStickTiltPosForWorld = VTransform( vStickTiltPosForScLocal.toVECTOR(), mScreenLocalToWorld );

	// Entity���ʏ�̍��W��ϊ��s�񂩂�v�Z
	vStickTiltPosProjection = VTransform( vStickTiltPosForScLocal.toVECTOR(), mScreenPosRigidTrans );


};


// �J�������f����`��
void StickTiltProjectorDemo::RenderModelCamera()
{
	// �J�������f���̎p������
	MV1SetMatrix( m_iModelCamHandle, MMult( MatConf, mModelCamLocalToWorld ) );
		
	// �J�����̕`��
	MV1DrawModel( m_iModelCamHandle );

};

// �X�N���[����`��
void StickTiltProjectorDemo::RenderScreen()
{
	// �O����
	m_ScPlane.setCntStickRing( m_vEntiPosAsScreenForScLocal );
	m_ScPlane.m_mLocalToWorldMatrix = mScreenLocalToWorld;
	m_ScPlane.setVertex();
	// �� �{����Update�̒��ł��ׂ�����

	// �`��
	m_ScPlane.Render();
	
};

// �e��⏕����`��
void StickTiltProjectorDemo::RenderAuxiliaryLines()
{
	// �X�N���[����̃L�����N�^�ʒu
	//DrawSphere3D( m_vEntiPosAsScreenForWorld.toVECTOR() , 0.3f, 16, GetColor( 255,0,0 )  , GetColor( 0,0,0 ), TRUE ) ;
		
	// �X�N���[����̃X�e�B�b�N�X���ʒu�̕`��
	//DrawSphere3D( vStickTiltPos.toVECTOR()    , 0.3f, 16, GetColor( 0,0,255 )  , GetColor( 0,0,0 ), TRUE ) ;

	// �L�����N�^�[�n���ʂ֓��e�������Ǘ͕�����`��
	//DrawSphere3D( vSteeringForceDir.toVECTOR(), 0.3f, 16, GetColor( 255,255,0 ), GetColor( 0,0,0 ), TRUE ) ;

	// �J�����ʒu - �L�����N�^�ʒu �̐���
	DrawLine3D( m_vModelCamPos.toVECTOR(), m_vPlayerPos.toVECTOR(), GetColor( 255,0,0 ) );

	// �J�����ʒu - �X�N���[����X�e�B�b�N�X�������ʒu �̐���
	//DrawLine3D( m_vModelCamPos.toVECTOR(), vSteeringForceDir.toVECTOR(), GetColor( 0,0,255 ) );


	// �J��������
	DrawLine3D( m_vModelCamPos.toVECTOR(), m_vModelCamGazePoint.toVECTOR(), GetColor( 255,0,255 ) );

	// ���f���J�������_ - �X�N���[����̃X�e�B�b�N�X���ʒu ��ʂ�����A�L�����N�^���ʂ܂ň����B
	// �i���P�����̐����p�B�j
	Vector3D vOnEntiPlane;
	Vector3D vOnScreen; // ����̓_�~�[
	calcStickTiltPos_HomogTrans( 
		m_dGridRangeOnScreen*m_vStickTiltOnScreen, 
		vOnScreen,
		vOnEntiPlane
		);

	DrawLine3D( m_vModelCamPos.toVECTOR(), vOnEntiPlane.toVECTOR(), GetColor( 255,0,255 ) );

};

// �X�N���[����̉~��`��
void StickTiltProjectorDemo::RenderStickTrackOnScreen()
{
	m_StickTrackOnScreen.Render();
};

// �X�N���[����̉~��xz���ʏ�ɓ��e�����}�`��`��
void StickTiltProjectorDemo::RenderStickTrackProjection()
{
	m_StickTrackProjection.Render();
};

// �B �A�𓊉e���ʏ�Ő��K���������~�̕`��
void StickTiltProjectorDemo::RenderStickTrackProjectionNormalize()
{
	m_StickTrackProjectionNormalize.Render();
};


// ��`�̕`��
void StickTiltProjectorDemo::RenderSectors()
{
	m_pVertiSectorOnScreen->Render();
	m_pVertiSectorProjection->Render();
	m_pHorizSectorOnScreen->Render();
	m_pHorizSectorProjection->Render();
		
};

// �I���W�i���̃X�e�B�b�N�̌X���̕ω��ʂ�\����^��`��
void StickTiltProjectorDemo::RenderOrgStickTiltDirVariation( Vector2D RenderPos )
{
	// ��^�̎��ۂɕ`�悳��锼�a
	double RenderRadius = SectorFigure2D::m_dRenderScale * SectorRadius ;

	// ���������̐�`��`��
	m_pVertiSectorOrgStickTiltDir->RenderOnCanvas( RenderPos );

	// ���������̐�`��`��
	m_pHorizSectorOrgStickTiltDir->RenderOnCanvas( RenderPos );

	// �g�ƂȂ�~��`��
	Point2D CircleCnt = RenderPos.toPoint();
	DrawCircle( CircleCnt.x, CircleCnt.y, ((int)RenderRadius)-1, GlobalLineColor, FALSE );

}

// �i�X�e�B�b�N�̌X���j�v�Z���ꂽ�L�����N�^�̐i�s�����̕ω��ʂ�\����^��`��
void StickTiltProjectorDemo::RenderCharactrMoveDirVariation( Vector2D RenderPos )
{
	// ��^�̎��ۂɕ`�悳��锼�a
	double RenderRadius = SectorFigure2D::m_dRenderScale * SectorRadius ;

	// ���������̐�`��`��
	m_pVertiSectorCharactrMoveDir->RenderOnCanvas( RenderPos );

	// ���������̐�`��`��
	m_pHorizSectorCharactrMoveDir->RenderOnCanvas( RenderPos );

	// �g�ƂȂ�~��`��
	Point2D CircleCnt = RenderPos.toPoint();
	DrawCircle( CircleCnt.x, CircleCnt.y, ((int)RenderRadius)-1, GlobalLineColor, FALSE );

	// ����������������Ɏ����Ă��邽�߂ɂ́A����ɋ@�\�ǉ����K�v......
	// �����܂ł͂�邩..

}

// �I���W�i���̃X�e�B�b�N�̌X���̐�` �̏�����
void StickTiltProjectorDemo::UpdateSectorOrgStickTiltDir()
{
	m_pVertiSectorOrgStickTiltDir = new SectorFigure2D( 
		SectorRadius, m_vVertiStickTiltDirBgn, m_vVertiStickTiltDirEnd, m_iSectorDivNum );
	m_pHorizSectorOrgStickTiltDir = new SectorFigure2D( 
		SectorRadius, m_vHorizStickTiltDirBgn, m_vHorizStickTiltDirEnd, m_iSectorDivNum );

	// �F�̐ݒ�
	m_pVertiSectorOrgStickTiltDir->m_iRenderColor = GetColor( 0, 255, 0 );
	m_pHorizSectorOrgStickTiltDir->m_iRenderColor = GetColor( 0, 0, 255 );

	// ��`�̉�]�p�̐ݒ�
	m_pVertiSectorOrgStickTiltDir->m_dRotation = DX_PI_F * (0.5);
	m_pHorizSectorOrgStickTiltDir->m_dRotation = DX_PI_F * (0.5);

	// �|���S���̐���
	m_pVertiSectorOrgStickTiltDir->GeneratePolygons();
	m_pHorizSectorOrgStickTiltDir->GeneratePolygons();

};

// �i�X�e�B�b�N�̌X������j�v�Z���ꂽ�L�����N�^�̐i�s�����i��}�������`�j�̏�����
void StickTiltProjectorDemo::UpdateSectorCharactrMoveDir()
{
	Vector3D vTmp;

	Vector3D vVertiCharactrMoveDirBgn;
	Vector3D vVertiCharactrMoveDirEnd;
	(this->*fpCalcStickTiltPosProjection)( m_vVertiStickTiltDirBgn.toVector3D(), vTmp, vVertiCharactrMoveDirBgn );
	(this->*fpCalcStickTiltPosProjection)( m_vVertiStickTiltDirEnd.toVector3D(), vTmp, vVertiCharactrMoveDirEnd );
	m_pVertiSectorCharactrMoveDir = new SectorFigure2D( 
		SectorRadius, 
		vVertiCharactrMoveDirBgn.toVector2D(), 
		vVertiCharactrMoveDirEnd.toVector2D(), 
		m_iSectorDivNum );

	Vector3D vHorizCharactrMoveDirBgn;
	Vector3D vHorizCharactrMoveDirEnd;
	(this->*fpCalcStickTiltPosProjection)( m_vHorizStickTiltDirBgn.toVector3D(), vTmp, vHorizCharactrMoveDirBgn );
	(this->*fpCalcStickTiltPosProjection)( m_vHorizStickTiltDirEnd.toVector3D(), vTmp, vHorizCharactrMoveDirEnd );
	m_pHorizSectorCharactrMoveDir = new SectorFigure2D( 
		SectorRadius, 
		vHorizCharactrMoveDirBgn.toVector2D(), 
		vHorizCharactrMoveDirEnd.toVector2D(), 
		m_iSectorDivNum );

	// �F�̐ݒ�
	m_pVertiSectorCharactrMoveDir->m_iRenderColor = GetColor( 0, 255, 0 );
	m_pHorizSectorCharactrMoveDir->m_iRenderColor = GetColor( 0, 0, 255 );

	// ��`�̉�]�p�̐ݒ�
	m_pVertiSectorCharactrMoveDir->m_dRotation = DX_PI_F * (1.0+0.5);
	m_pHorizSectorCharactrMoveDir->m_dRotation = DX_PI_F * (1.0+0.5);


	// �`��p�|���S�����̏�����
	m_pVertiSectorCharactrMoveDir->GeneratePolygons();
	m_pHorizSectorCharactrMoveDir->GeneratePolygons();

};

// ���o�p�O���b�h�̏�����
void StickTiltProjectorDemo::UpdateGrids()
{
	// �O���b�h�̃C���X�^���X��
	m_pGridOriginal = new GroundGrid( m_dGridRangeOnScreen, 8, 0 );
	m_pGridOnScreen = new GroundGrid( 5.0, 8, GetColor(0,0,255) );
	m_pGridGrandPrj = new GroundGrid( 5.0, 8, GetColor(255,0,0) );

	// ���̃C���X�^���X��
	m_pArrowToStickTiltOnEntityPlane = new Arrow3D( 2,   5, 1.6, GetColorF(1.0, 0.0, 0.0, 0.0) );
	//m_pArrowToStickTiltOnScreen      = new Arrow3D( 0.5, 0.4, 0.1, GetColorF(0.0, 0.0, 1.0, 0.0) ); // �J���������p
	m_pArrowToStickTiltOnScreen      = new Arrow3D( 1,   2.5, 0.8, GetColorF(0.0, 0.0, 1.0, 0.0) );  // ���Ր}�p

	// �X�N���[����y�тɃL�����N�^���ʂ֓��e�����O���b�h���v�Z
	for( int i=0; i<m_pGridOriginal->m_iMaxVectorNum; i++ )
	{
		(this->*fpCalcStickTiltPosProjection)( 
			m_pGridOriginal->editVertexes(i),
			m_pGridOnScreen->editVertexes(i),
			m_pGridGrandPrj->editVertexes(i) );
	}

	// �X�N���[����y�тɃL�����N�^���ʂ֓��e���������v�Z
	Vector3D vOrign( 0, 0, 0 );

	// �n�_�̌v�Z
	(this->*fpCalcStickTiltPosProjection)( 
		vOrign,
		m_vArrowBgnScreen,
		m_vArrowBgnEntPln );

	// �I�_�̌v�Z
	(this->*fpCalcStickTiltPosProjection)( 
		m_dGridRangeOnScreen*m_vStickTiltOnScreen,
		m_vArrowEndScreen,
		m_vArrowEndEntPln );

};

// �X�N���[����̃O���b�h��`��
void StickTiltProjectorDemo::RenderGridOnScreen()
{
	// �O���b�h�̕`��
	m_pGridOnScreen->Render();

	// ���̕`��
	Vector3D vUpper( 0.0, 1.0, 0.0 );
	vUpper = VTransform( vUpper.toVECTOR(), mScreenLocalToWorld );
	m_pArrowToStickTiltOnScreen->Render(
		m_vArrowBgnScreen,
		m_vArrowEndScreen,
		vUpper);

};

// Entity���ʂɓ��e�����O���b�h��`��
void StickTiltProjectorDemo::RenderGridGrandPrj()
{
	// �O���b�h�̕`��
	m_pGridGrandPrj->Render();

	// ���̕`��
	Vector3D vUpper( 0.0, 1.0, 0.0 );
	m_pArrowToStickTiltOnEntityPlane->Render(
		m_vArrowBgnEntPln,
		m_vArrowEndEntPln,
		vUpper);

	// �Ȃ񂩂�������
	//DrawCoordi( m_vArrowEndEntPln, 2.0 );
	//DrawSphere3D( m_vArrowEndEntPln.toVECTOR() , 0.3f, 16, GetColor( 255,0,0 )  , GetColor( 0,0,0 ), TRUE ) ;
	//DrawSphere3D( m_vArrowBgnEntPln.toVECTOR() , 0.3f, 16, GetColor( 0,255,0 )  , GetColor( 0,0,0 ), TRUE ) ;

};



// ########################## ScreenPlane ##########################

// �R���X�g���N�^
ScreenPlane::ScreenPlane( double Width, double Hight, double Radius ):
	m_oFrameT( Vector3D( -Width, 0,  Hight), Vector3D(  Width, 0,  Hight ), GlobalLineColor ), 
	m_oFrameB( Vector3D( -Width, 0, -Hight), Vector3D(  Width, 0, -Hight ), GlobalLineColor ), 
	m_oFrameL( Vector3D( -Width, 0, -Hight), Vector3D( -Width, 0,  Hight ), GlobalLineColor ), 
	m_oFrameR( Vector3D(  Width, 0, -Hight), Vector3D(  Width, 0,  Hight ), GlobalLineColor ), 
	m_oAxisX( Vector3D( -Width, 0, 0 ), Vector3D(  Width,  0, 0 ), GlobalLineColor ), 
	m_oAxisY( Vector3D( 0, 0, -Hight ), Vector3D(  0, 0,  Hight ), GlobalLineColor ), 
	m_oStickRing( Radius, 32, GetColor( 255, 0, 0 ) ), 
	m_vCntStickRing( 0.0, 0.0, 0.0 )
{
	m_mLocalToWorldMatrix = MGetIdent();
};

void ScreenPlane::setVertex()
{
	static MATRIX WorkMat;

	// �S�`��I�u�W�F�N�g��Vertex�������Z�b�g����
	m_oFrameT.resetVertex();
	m_oFrameB.resetVertex();
	m_oFrameL.resetVertex();
	m_oFrameR.resetVertex();
	m_oAxisX.resetVertex();
	m_oAxisY.resetVertex();
	m_oStickRing.resetVertex();

	// �X�e�B�b�N�͈͉̔~���V�t�g����
	WorkMat = MGetIdent();
	WorkMat.m[3][0] = m_vCntStickRing.x;
	WorkMat.m[3][1] = m_vCntStickRing.y;
	WorkMat.m[3][2] = m_vCntStickRing.z;
	m_oStickRing.MatTransVertex(WorkMat);

	// m_mLocalToWorldMatrix �ō��W�ϊ�
	m_oFrameT.MatTransVertex(m_mLocalToWorldMatrix);
	m_oFrameB.MatTransVertex(m_mLocalToWorldMatrix);
	m_oFrameL.MatTransVertex(m_mLocalToWorldMatrix);
	m_oFrameR.MatTransVertex(m_mLocalToWorldMatrix);
	m_oAxisX.MatTransVertex(m_mLocalToWorldMatrix);
	m_oAxisY.MatTransVertex(m_mLocalToWorldMatrix);
	m_oStickRing.MatTransVertex(m_mLocalToWorldMatrix);

};

void ScreenPlane::Render()
{
	// �e�I�u�W�F�N�g�̕`�惁�\�b�h���ĂԂ���
	m_oFrameT.Render();
	m_oFrameB.Render();
	m_oFrameL.Render();
	m_oFrameR.Render();
	m_oAxisX.Render();
	m_oAxisY.Render();

	// �O�Ղ͕ʓr�`�悷��悤�ɂ���
	//m_oStickRing.Render();
};

