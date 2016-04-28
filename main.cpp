#include <cassert>

#include "DxLib.h"
#include "utils.h"

#include "Vector3D.h"
#include "Vector2D.h"
#include "PrecisionTimer.h"
#include "VirtualController.h"
#include "PlayerCharacterEntity.h"

#include "CameraWorkManager.h"

// 2016/03/16
// �����̃��[�V���� �� �n�ʁF�A�j����8:20 ���炢�����傤�ǂ����B

// 2016/04/03
// �� �W�����v��ɁA�n�ʂɂ߂荞��ōs���o�O���L��B
//    ���炭�A���n����Heading���A�������Ɍ����Ă���̂ŁA��̈ړ�������Heading�̒n�������ɓ����Ă���̂ł́H
//    ���Ƃ͏������낢��o�͂����Ă݂�

//#define LIGHT_ON

static const float ROTATE_SPEED = DX_PI_F/90;//��]�X�s�[�h

// �~����`�悷��class
class Column
{
private:
	VERTEX3D* m_pVertex; // �|���S���W����ێ�
	int m_PolygonNum;    // �|���S����

public:
	// �R���X�g���N�^
	Column( 
		Vector3D  CenterPos,       // ��ʂ̉~�`�̒��S�ʒu
		double    Radius,          // ���a
		double    Hight,           // �~���̍���
		int       DivNum,          // �������i����ʂ̉~�`�̕������j
		COLOR_U8  DifColor, // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor  // ���̒��_�X�y�L�����J���[
		);

	// �`��
	void Render();

};

Column::Column( 
		Vector3D  CenterPos,       // ��ʂ̉~�`�̒��S�ʒu
		double    Radius,          // ���a
		double    Hight,           // �~���̍���
		int       DivNum,          // �������i����ʂ̉~�`�̕������j
		COLOR_U8  DifColor, // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor  // ���̒��_�X�y�L�����J���[
		)
{
	// Vertex ���v�Z����

	m_PolygonNum = DivNum*4;

	// #### �悸�́A�P�ʉ~������Vertex�𐶐����A������V�t�g�A�X�P�[�����O����

	// Vectex�̃��������m�� Vertex���FDivNum * 4(��ʁE��ӁE���ʁi�Q�{�j) * 3�i�P�|���S���̒��_���j
	int VectexNum = DivNum*4*3;
	m_pVertex = new VERTEX3D[VectexNum];

	// ���������琳���p�`�̒��S�p���v�Z
	double CenterAng = 2*DX_PI_F/((double)DivNum);
	
	Vector3D V3D0B( 0,  0,0 );
	Vector3D V3D0T( 0,1.0,0 );

	for( int i=0; i<DivNum; i++ )
	{
		int j=(i+1)%DivNum; // ���̓Y���i�~���Ή��j
		Vector2D V2D1( cos(CenterAng*i), sin(CenterAng*i) );
		Vector2D V2D2( cos(CenterAng*j), sin(CenterAng*j) );
		Vector2D V2Dn( cos(CenterAng*((double)i+0.5)), sin(CenterAng*((double)i+0.5)) ); // �@��
		
		Vector3D V3D1B = V2D1.toVector3D();
		Vector3D V3D1T = V2D1.toVector3D(1.0);
		Vector3D V3D2B = V2D2.toVector3D();
		Vector3D V3D2T = V2D2.toVector3D(1.0);
		Vector3D V3Dn  = V2Dn.toVector3D();

		// ���
		m_pVertex[ 12*i+3*0+0 ].pos  = V3D0B.toVECTOR();
		m_pVertex[ 12*i+3*0+1 ].pos  = V3D1B.toVECTOR();
		m_pVertex[ 12*i+3*0+2 ].pos  = V3D2B.toVECTOR();
		for( int k=0; k<3; k++) m_pVertex[ 12*i+3*0+k ].norm = Vector3D( 0, -1.0, 0).toVECTOR();

		// ���ʂP
		m_pVertex[ 12*i+3*1+0 ].pos  = V3D1T.toVECTOR();
		m_pVertex[ 12*i+3*1+1 ].pos  = V3D1B.toVECTOR();
		m_pVertex[ 12*i+3*1+2 ].pos  = V3D2B.toVECTOR();
		for( int k=0; k<3; k++) m_pVertex[ 12*i+3*1+k ].norm = V3Dn.toVECTOR();

		// ���ʂQ
		m_pVertex[ 12*i+3*2+0 ].pos  = V3D2B.toVECTOR();
		m_pVertex[ 12*i+3*2+1 ].pos  = V3D1T.toVECTOR();
		m_pVertex[ 12*i+3*2+2 ].pos  = V3D2T.toVECTOR();
		for( int k=0; k<3; k++) m_pVertex[ 12*i+3*2+k ].norm = V3Dn.toVECTOR();

		// ���
		m_pVertex[ 12*i+3*3+0 ].pos  = V3D0T.toVECTOR();
		m_pVertex[ 12*i+3*3+1 ].pos  = V3D1T.toVECTOR();
		m_pVertex[ 12*i+3*3+2 ].pos  = V3D2T.toVECTOR();
		for( int k=0; k<3; k++) m_pVertex[ 12*i+3*3+k ].norm = Vector3D( 0, 1.0, 0).toVECTOR();

	}

	// color �Ǝg��Ȃ��v�f��������
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].dif = DifColor;
		m_pVertex[i].spc = SpcColor;
		m_pVertex[i].u   = 0.0f;
		m_pVertex[i].v   = 0.0f;
		m_pVertex[i].su  = 0.0f;
		m_pVertex[i].sv  = 0.0f;
	}

	// �V�t�g�A�X�P�[�����O����
	for( int i=0; i<VectexNum; i++ )
	{
		Vector3D tmp = m_pVertex[i].pos;
		tmp.x *= Radius;
		tmp.z *= Radius;
		tmp.y *= Hight;
		tmp += CenterPos;
		m_pVertex[i].pos = tmp.toVECTOR();
	}

};

void Column::Render()
{
	DrawPolygon3D( m_pVertex, m_PolygonNum, DX_NONE_GRAPH, FALSE ) ;
};

// �ׂ������b�V���̕���
class FineMeshedFlatPlate
{
private:
	VERTEX3D* m_pVertex; // �|���S���W����ێ�
	int m_PolygonNum;    // �|���S����

public:
	// �R���X�g���N�^
	FineMeshedFlatPlate( 
		Vector2D  BLCorner,        // ���́i���E���ʏ�́j�������_���W
		Vector2D  TRCorner,        // ���́i���E���ʏ�́j�E�㒸�_���W
		int       DivNum,          // �������i�c�E�����̐��ŕ�������邽�߁A�|���S���O�p�`���� DivNum * DivNum * 2 �ƂȂ�j
		COLOR_U8  DifColor,        // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor         // ���̒��_�X�y�L�����J���[
		);

	// �`��
	void Render();

};

FineMeshedFlatPlate::FineMeshedFlatPlate( 
		Vector2D  BLCorner,        // ���́i���E���ʏ�́j�������_���W
		Vector2D  TRCorner,        // ���́i���E���ʏ�́j�E�㒸�_���W
		int       DivNum,          // �������i�c�E�����̐��ŕ�������邽�߁A�|���S���O�p�`���� DivNum * DivNum * 2 �ƂȂ�j
		COLOR_U8  DifColor,        // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor         // ���̒��_�X�y�L�����J���[
		)
{
	// Vertex ���v�Z����

	// �|���S���O�p�`���������H
	m_PolygonNum = DivNum*DivNum*2;

	// Vectex�̃��������m�� Vertex���F
	// DivNum*DivNum�i�c���������j* 2�i�P�l�p���Q�O�p�`�j * 3�i�P�|���S���̒��_���j
	int VectexNum = DivNum*DivNum*2*3;
	m_pVertex = new VERTEX3D[VectexNum];

	double VertiDivWidth = (TRCorner.y - BLCorner.y)/(double)DivNum; // ������������
	double HorizDivWidth = (TRCorner.x - BLCorner.x)/(double)DivNum; // �c����������

	// �����������l�łȂ����Ƃ��m�F
	assert ( VertiDivWidth>0 && "VertiDivWidth is negative.");
	assert ( HorizDivWidth>0 && "HorizDivWidth is negative.");

	// �c���S�Ẵ��b�V���ɑ΂�
	for( int i=0; i<DivNum; i++ ){
		for( int j=0; j<DivNum; j++ ){
			double x = (double)i;
			double y = (double)j;
			Vector2D PolyBL(BLCorner.x + HorizDivWidth*   x ,BLCorner.y + VertiDivWidth*   y );
			Vector2D PolyBR(BLCorner.x + HorizDivWidth*(x+1),BLCorner.y + VertiDivWidth*   y );
			Vector2D PolyTL(BLCorner.x + HorizDivWidth*   x ,BLCorner.y + VertiDivWidth*(y+1));
			Vector2D PolyTR(BLCorner.x + HorizDivWidth*(x+1),BLCorner.y + VertiDivWidth*(y+1));

			int suffix = DivNum*i + j; // �c���ʔ�

			// �O�p�`�P
			m_pVertex[ 6*suffix+0 ].pos = PolyBL.toVector3D().toVECTOR();
			m_pVertex[ 6*suffix+1 ].pos = PolyBR.toVector3D().toVECTOR();
			m_pVertex[ 6*suffix+2 ].pos = PolyTL.toVector3D().toVECTOR();

			// �O�p�`�Q
			m_pVertex[ 6*suffix+3 ].pos = PolyBR.toVector3D().toVECTOR();
			m_pVertex[ 6*suffix+4 ].pos = PolyTL.toVector3D().toVECTOR();
			m_pVertex[ 6*suffix+5 ].pos = PolyTR.toVector3D().toVECTOR();

		}
	}

	// �@���x�N�g���ƁAcolor �Ǝg��Ȃ��v�f��������
	Vector3D vNorm( 0, 1.0, 0 ); // �@���x�N�g���Ay��������B
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].norm = vNorm.toVECTOR();
		m_pVertex[i].dif = DifColor;
		m_pVertex[i].spc = SpcColor;
		m_pVertex[i].u   = 0.0f;
		m_pVertex[i].v   = 0.0f;
		m_pVertex[i].su  = 0.0f;
		m_pVertex[i].sv  = 0.0f;
	}

};

void FineMeshedFlatPlate::Render()
{
	DrawPolygon3D( m_pVertex, m_PolygonNum, DX_NONE_GRAPH, FALSE ) ;
};

// �����̂�`�悷��class�A�摜�\��t���̎���
class ParallelBox3D
{
private:
	VERTEX3D* m_pVertex; // �|���S���W����ێ�
	int m_PolygonNum;    // �|���S����

	// �e�N�X�`���摜�̃n���h��
	int m_iTexturesHandle;

public:
	// �R���X�g���N�^
	ParallelBox3D( 
		Vector3D  SmallVertex,      // �����̂̒��_�ŁA���W�I�Ɉ�ԏ�����
		Vector3D  LargeVertex,      // �����̂̒��_�ŁA���W�I�Ɉ�ԑ傫��
		COLOR_U8  DifColor, // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor  // ���̒��_�X�y�L�����J���[
		);

	// �`��
	void Render();

};
ParallelBox3D::ParallelBox3D( 
		Vector3D  SmallVertex,      // �����̂̒��_�ŁA���W�I�Ɉ�ԏ�����
		Vector3D  LargeVertex,      // �����̂̒��_�ŁA���W�I�Ɉ�ԑ傫��
		COLOR_U8  DifColor, // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor  // ���̒��_�X�y�L�����J���[
		)
{
	// �e�N�X�`���p�̉摜�̓ǂݍ���
    m_iTexturesHandle = LoadGraph( "kirby_connect.bmp" ) ;

	// Vertex ���v�Z����
	m_PolygonNum = 6*2;

	// Vectex�̃��������m�� Vertex���FDivNum * 4(��ʁE��ӁE���ʁi�Q�{�j) * 3�i�P�|���S���̒��_���j
	int VectexNum = m_PolygonNum*3;
	m_pVertex = new VERTEX3D[VectexNum];

	// #### �悸�́A�����̂���Vertex�𐶐����A������V�t�g�A�X�P�[�����O����

	// color �Ǝg��Ȃ��v�f��������
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].dif = DifColor;
		m_pVertex[i].spc = SpcColor;
		m_pVertex[i].u   = 0.0f;
		m_pVertex[i].v   = 0.0f;
		m_pVertex[i].su  = 0.0f;
		m_pVertex[i].sv  = 0.0f;
	}

	// �e���_���`
	Vector3D V000( 0, 0, 0 );
	Vector3D V001( 0, 0, 1 );
	Vector3D V010( 0, 1, 0 );
	Vector3D V100( 1, 0, 0 );
	Vector3D V011( 0, 1, 1 );
	Vector3D V101( 1, 0, 1 );
	Vector3D V110( 1, 1, 0 );
	Vector3D V111( 1, 1, 1 );

	// 6�ʕ���`
	int n=0;
	int save=0;

	save = n;
	m_pVertex[ n++ ].pos = V000.toVECTOR();
	m_pVertex[ n++ ].pos = V001.toVECTOR();
	m_pVertex[ n++ ].pos = V010.toVECTOR();
	m_pVertex[ n++ ].pos = V001.toVECTOR();
	m_pVertex[ n++ ].pos = V010.toVECTOR();
	m_pVertex[ n++ ].pos = V011.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D(-1,0,0).toVECTOR();

	save = n;
	m_pVertex[ n++ ].pos = V100.toVECTOR();
	m_pVertex[ n++ ].pos = V101.toVECTOR();
	m_pVertex[ n++ ].pos = V110.toVECTOR();
	m_pVertex[ n++ ].pos = V101.toVECTOR();
	m_pVertex[ n++ ].pos = V110.toVECTOR();
	m_pVertex[ n++ ].pos = V111.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D( 1,0,0).toVECTOR();

	save = n;
	m_pVertex[ n++ ].pos = V000.toVECTOR();
	m_pVertex[ n++ ].pos = V001.toVECTOR();
	m_pVertex[ n++ ].pos = V100.toVECTOR();
	m_pVertex[ n++ ].pos = V001.toVECTOR();
	m_pVertex[ n++ ].pos = V100.toVECTOR();
	m_pVertex[ n++ ].pos = V101.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D(0,-1,0).toVECTOR();
	
	save = n;
	m_pVertex[ n++ ].pos = V010.toVECTOR();
	m_pVertex[ n++ ].pos = V011.toVECTOR();
	m_pVertex[ n++ ].pos = V110.toVECTOR();
	m_pVertex[ n++ ].pos = V011.toVECTOR();
	m_pVertex[ n++ ].pos = V110.toVECTOR();
	m_pVertex[ n++ ].pos = V111.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D(0, 1,0).toVECTOR();

	save = n;
	m_pVertex[ n++ ].pos = V000.toVECTOR();
	m_pVertex[ n++ ].pos = V010.toVECTOR();
	m_pVertex[ n++ ].pos = V100.toVECTOR();
	m_pVertex[ n++ ].pos = V010.toVECTOR();
	m_pVertex[ n++ ].pos = V100.toVECTOR();
	m_pVertex[ n++ ].pos = V110.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D(0,0,-1).toVECTOR();
	
	save = n;
	m_pVertex[ n++ ].pos = V001.toVECTOR();
	m_pVertex[ n++ ].pos = V011.toVECTOR();
	m_pVertex[ n++ ].pos = V101.toVECTOR();
	m_pVertex[ n++ ].pos = V011.toVECTOR();
	m_pVertex[ n++ ].pos = V101.toVECTOR();
	m_pVertex[ n++ ].pos = V111.toVECTOR();
	for( int k=0; k<6; k++) m_pVertex[ k+save ].norm=Vector3D(0,0, 1).toVECTOR();

	// �摜�\��t�����W��ݒ�
	for( int i=0; i<6; i++ )
	{
		m_pVertex[ 6*i+0 ].u = 0.0f;
		m_pVertex[ 6*i+0 ].v = 0.0f;
		m_pVertex[ 6*i+1 ].u = 0.0f;
		m_pVertex[ 6*i+1 ].v = 1.0f;
		m_pVertex[ 6*i+2 ].u = 1.0f;
		m_pVertex[ 6*i+2 ].v = 0.0f;
		m_pVertex[ 6*i+3 ].u = 0.0f;
		m_pVertex[ 6*i+3 ].v = 1.0f;
		m_pVertex[ 6*i+4 ].u = 1.0f;
		m_pVertex[ 6*i+4 ].v = 0.0f;
		m_pVertex[ 6*i+5 ].u = 1.0f;
		m_pVertex[ 6*i+5 ].v = 1.0f;
	}

	// �V�t�g�A�X�P�[�����O����
	Vector3D ScaleVec = LargeVertex - SmallVertex;
	for( int i=0; i<VectexNum; i++ )
	{
		Vector3D tmp = m_pVertex[i].pos;
		tmp.x *= ScaleVec.x;
		tmp.y *= ScaleVec.y;
		tmp.z *= ScaleVec.z; // ���ςƂ��Ⴄ�Ȃɂ�
		tmp += SmallVertex;
		m_pVertex[i].pos = tmp.toVECTOR();
	}

};

void ParallelBox3D::Render()
{
	DrawPolygon3D( m_pVertex, m_PolygonNum, m_iTexturesHandle, FALSE ) ;
};



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

	ParallelBox3D ColorfullBox(
		Vector3D( 0,0,0),
		Vector3D( 20,20,20),
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

		// �n�ʗp�̉~�Ղ�`��
		//GroundDisk.Render();
		GrateEarthGround.Render();

		// �~���̖��т�`��
		for( int i=0; i<MaxColumnsNum; i++ ){ ppColmunList[i]->Render(); }

		// �e�N�X�`�������p�̔���`��
		ColorfullBox.Render();

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

