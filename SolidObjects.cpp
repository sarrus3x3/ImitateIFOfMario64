#include <cassert>

#include "SolidObjects.h"

// ###############################################
// ########## class Column
// ###############################################

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

// ###############################################
// ########## class FineMeshedFlatPlate
// ###############################################

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

// ###############################################
// ########## class ParallelBox3D
// ###############################################

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

