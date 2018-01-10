#include <cassert>
#include <vector>

#include "Vector3D.h"
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
		int       TexturesHandle,   // �e�N�X�`���摜�̃n���h��
		COLOR_U8  DifColor, // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor  // ���̒��_�X�y�L�����J���[
		)
{
	// �e�N�X�`���p�̉摜�̓ǂݍ���
    m_iTexturesHandle = TexturesHandle ;

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
	if( m_iTexturesHandle<0 )
	{
		DrawPolygon3D( m_pVertex, m_PolygonNum, DX_NONE_GRAPH , FALSE ) ;
	}
	else
	{
		DrawPolygon3D( m_pVertex, m_PolygonNum, m_iTexturesHandle, FALSE ) ;
	}
};

// ###############################################
// ########## class TextureSphere
// ###############################################

TextureSphere3D::TextureSphere3D( 
		Vector3D  CntPos,           // �����S�̈ʒu
		double    Radius,           // ���̔��a 
		bool      Outward,          // true:�O�����Afalse:������
		int       DivNumLongi,      // �o�x�����̕�����
		int       DivNumLati,       // �ܓx�����̕�����
		int       TexturesHandle,   // �e�N�X�`���摜�̃n���h��
		ObjectTypeID ObjectType     // �I�u�W�F�N�g�^�C�v
		) :
	m_bOutward( Outward ),
	m_eObjectType( ObjectType )
{
	// �e�N�X�`���p�̉摜�̓ǂݍ���
    m_iTexturesHandle = TexturesHandle ;

	// ���S�ʒu�ۑ�
	m_vCntPos = CntPos;

	// �|���S���O�p�`���������H
	m_iPolygonNum = DivNumLongi*DivNumLati*2;

	// Vectex�̃��������m�� Vertex���F
	int VectexNum = m_iPolygonNum * 3;
	m_pVertex       = new VERTEX3D[VectexNum];
	m_pRawVertexPos = new Vector3D[VectexNum];
	m_pRawVertexNrm = new Vector3D[VectexNum];

	// ���_�ʒu�ƁA�g�|���W�\���A�@���x�N�g���A�e�L�X�`���}�b�s���O���v�Z����
	double LongiSlot = DX_TWO_PI / (double)DivNumLongi; // �ܓx�̕�����
	double LatiSlot  = DX_PI / (double)DivNumLati;      // �ܓx�̕�����
	double XTxSlot   = 1.0 / (double)DivNumLongi;       // �e�N�X�`��X�������̕�����
	double YTxSlot   = 1.0 / (double)DivNumLati;        // �e�N�X�`��Y�������̕�����

	// �c���S�Ẵ��b�V���ɑ΂�
	for( int i=0; i<DivNumLongi; i++ ){
		for( int j=0; j<DivNumLati; j++ ){

			// ���_�ʒu�̌v�Z
			double LongiL = LongiSlot *  i;                 // �ܓxL
			double LongiR = LongiSlot * (i+1);              // �ܓxR
			double LatiB  = LatiSlot *  j    - (0.5*DX_PI); // �ܓxB
			double LatiT  = LatiSlot * (j+1) - (0.5*DX_PI); // �ܓxT
			Vector3D PolyBL( cos(LatiB)*cos(LongiL), sin(LatiB), cos(LatiB)*sin(LongiL) );
			Vector3D PolyBR( cos(LatiB)*cos(LongiR), sin(LatiB), cos(LatiB)*sin(LongiR) );
			Vector3D PolyTL( cos(LatiT)*cos(LongiL), sin(LatiT), cos(LatiT)*sin(LongiL) );
			Vector3D PolyTR( cos(LatiT)*cos(LongiR), sin(LatiT), cos(LatiT)*sin(LongiR) );
			PolyBL *= -Radius; // ���a�𔽉f + �����𔽓]����邽��-1��������
			PolyBR *= -Radius;
			PolyTL *= -Radius;
			PolyTR *= -Radius;

			// �e�L�X�`���}�b�s���O�ʒu�̌v�Z
			double TxL = XTxSlot *  i;         
			double TxR = XTxSlot * (i+1);      
			double TxB = YTxSlot *  j;
			double TxT = YTxSlot * (j+1);
			Vector2D TxBL( TxL, TxB );
			Vector2D TxBR( TxR, TxB );
			Vector2D TxTL( TxL, TxT );
			Vector2D TxTR( TxR, TxT );

			// �@���x�N�g���̌v�Z
			double dist;
			if( Outward ) dist =  1.0;
			else          dist = -1.0;
			Vector3D NorBL = (dist * PolyBL).normalize();
			Vector3D NorBR = (dist * PolyBR).normalize();
			Vector3D NorTL = (dist * PolyTL).normalize();
			Vector3D NorTR = (dist * PolyTR).normalize();

			int suffix = DivNumLati*i + j; // �c���ʔ�

			// ##### m_pVertex
			// �O�p�`�P
			//m_pVertex[ 6*suffix+0 ].pos = PolyBL.toVECTOR();
			m_pVertex[ 6*suffix+0 ].u   = (float)TxBL.x;
			m_pVertex[ 6*suffix+0 ].v   = (float)TxBL.y;
			m_pVertex[ 6*suffix+0 ].norm= NorBL.toVECTOR();
			//m_pVertex[ 6*suffix+1 ].pos = PolyBR.toVECTOR();
			m_pVertex[ 6*suffix+1 ].u   = (float)TxBR.x;
			m_pVertex[ 6*suffix+1 ].v   = (float)TxBR.y;
			m_pVertex[ 6*suffix+1 ].norm= NorBR.toVECTOR();
			//m_pVertex[ 6*suffix+2 ].pos = PolyTL.toVECTOR();
			m_pVertex[ 6*suffix+2 ].u   = (float)TxTL.x;
			m_pVertex[ 6*suffix+2 ].v   = (float)TxTL.y;
			m_pVertex[ 6*suffix+2 ].norm= NorTL.toVECTOR();

			// �O�p�`�Q
			//m_pVertex[ 6*suffix+3 ].pos = PolyBR.toVECTOR();
			m_pVertex[ 6*suffix+3 ].u   = (float)TxBR.x;
			m_pVertex[ 6*suffix+3 ].v   = (float)TxBR.y;
			m_pVertex[ 6*suffix+3 ].norm= NorBR.toVECTOR();
			//m_pVertex[ 6*suffix+4 ].pos = PolyTL.toVECTOR();
			m_pVertex[ 6*suffix+4 ].u   = (float)TxTL.x;
			m_pVertex[ 6*suffix+4 ].v   = (float)TxTL.y;
			m_pVertex[ 6*suffix+4 ].norm= NorTL.toVECTOR();
			//m_pVertex[ 6*suffix+5 ].pos = PolyTR.toVECTOR();
			m_pVertex[ 6*suffix+5 ].u   = (float)TxTR.x;
			m_pVertex[ 6*suffix+5 ].v   = (float)TxTR.y;
			m_pVertex[ 6*suffix+5 ].norm= NorTR.toVECTOR();

			// ##### m_pRawVertexPos
			// �O�p�`�P
			m_pRawVertexPos[ 6*suffix+0 ] = PolyBL;
			m_pRawVertexPos[ 6*suffix+1 ] = PolyBR;
			m_pRawVertexPos[ 6*suffix+2 ] = PolyTL;

			// �O�p�`�Q
			m_pRawVertexPos[ 6*suffix+3 ] = PolyBR;
			m_pRawVertexPos[ 6*suffix+4 ] = PolyTL;
			m_pRawVertexPos[ 6*suffix+5 ] = PolyTR;

			// ##### m_pRawVertexNrm
			// �O�p�`�P
			m_pRawVertexNrm[ 6*suffix+0 ] = NorBL;
			m_pRawVertexNrm[ 6*suffix+1 ] = NorBR;
			m_pRawVertexNrm[ 6*suffix+2 ] = NorTL;
										  
			// �O�p�`�Q					  
			m_pRawVertexNrm[ 6*suffix+3 ] = NorBR;
			m_pRawVertexNrm[ 6*suffix+4 ] = NorTL;
			m_pRawVertexNrm[ 6*suffix+5 ] = NorTR;

		}
	}

	// color �Ǝg��Ȃ��v�f��������
	COLOR_U8 DifColor = GetColorU8( 255, 255, 255, 0 );
	COLOR_U8 SpcColor = GetColorU8( 255, 255, 255, 0 );
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].dif = DifColor;
		m_pVertex[i].spc = SpcColor;
		m_pVertex[i].su  = 0.0f;
		m_pVertex[i].sv  = 0.0f;
	}

	// ���S�ʒu���V�t�g
	setCenterPos( m_vCntPos );

	// �}�e���A���p�����[�^�̏�����
	m_Material.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Emissive = GetColorF( 1.0f, 1.0f, 1.0f, 1.0f ) ;
	m_Material.Power    = 0.0f ;

	m_MaterialDefault.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Emissive = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Power    = 20.0f ;
};

void TextureSphere3D::Render()
{
	// �\�ʂ��O�����̏ꍇ�́Azbuf�𖳌��ɂ��Ȃ�
	if( !m_bOutward )
	{
		// Z�o�b�t�@ OFF
		SetUseZBuffer3D( FALSE );
		SetWriteZBuffer3D( FALSE );
	}

	// �I�u�W�F�N�g�^�C�v�ŕ`����@���g������
	switch( m_eObjectType )
	{
	case OBJECT_SKYDOME: // �X�J�C�h�[���i�p�m���}���j: �G�~�b�V�u���̂�
		// ���C�g�̉e�����󂯂Ȃ��悤�ɐݒ�
		SetMaterialUseVertDifColor( FALSE ) ; // ���_�f�[�^�̃f�B�t���[�Y�J���[���g�p���Ȃ��悤�ɂ���
		SetMaterialUseVertSpcColor( FALSE ) ; // ���_�f�[�^�̃X�y�L�����J���[���g�p���Ȃ��悤�ɂ���
		SetMaterialParam( m_Material ) ; // �}�e���A���̐ݒ��L��
		break;
	case OBJECT_NOSPECULAR: //  ����Ȃ� : �X�y�L����OFF
		SetMaterialUseVertSpcColor( FALSE ) ; // ���_�f�[�^�̃X�y�L�����J���[���g�p���Ȃ��悤�ɂ���
		break;
	}

	// �`����{
	/*
	if(m_bOutward)
	{
		DrawPolygon3D( m_pVertex, m_iPolygonNum, DX_NONE_GRAPH , FALSE ) ;
	}
	else
	{
		DrawPolygon3D( m_pVertex, m_iPolygonNum, m_iTexturesHandle, FALSE ) ;
	}
	*/
	DrawPolygon3D( m_pVertex, m_iPolygonNum, m_iTexturesHandle, FALSE ) ;


	// �ύX�����`����@�����ɖ߂�
	switch( m_eObjectType )
	{
	case OBJECT_SKYDOME: // �X�J�C�h�[���i�p�m���}���j: �G�~�b�V�u���̂�
		// �}�e���A���ݒ�����ɖ߂�
		SetMaterialUseVertDifColor( TRUE ) ;
		SetMaterialUseVertSpcColor( TRUE ) ;
		SetMaterialParam( m_MaterialDefault ) ;
		break;
	case OBJECT_NOSPECULAR: //  ����Ȃ� : �X�y�L����OFF
		SetMaterialUseVertSpcColor( TRUE ) ;
		break;
	}

	if( !m_bOutward )
	{
		// Z�o�b�t�@�L����
		SetUseZBuffer3D( TRUE );
		SetWriteZBuffer3D( TRUE );
	}

	/*
	// �f�o�b�N
	int VectexNum = m_iPolygonNum * 3;
	for( int i=0; i<VectexNum; i++ )
	{
		DrawPixel3D( m_pVertex[i].pos, GetColor(255, 0, 0 ) );
	};
	*/

};

void TextureSphere3D::resetVertex()
{
	int VectexNum = m_iPolygonNum * 3;

	// m_pVertex.pos �� m_pRawVertexPos �ŏ㏑��
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos  = m_pRawVertexPos[i].toVECTOR();
		m_pVertex[i].norm = m_pRawVertexNrm[i].toVECTOR();
	}
};

void TextureSphere3D::setCenterPos( Vector3D CntPos )
{
	int VectexNum = m_iPolygonNum * 3;

	// ���S�ʒu���V�t�g
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos = (m_pRawVertexPos[i]+CntPos).toVECTOR();
	}
};

void TextureSphere3D::rotEuler( double angX, double angZ, double angY )
{
	// 2018/01/09 �R���p�C���G���[�i���������ϐ��̎g�p�j�ɂȂ邽�߁A���W�b�N�C���B
	// �I�C���[�p�ŉ�]�s����v�Z
	bool inited = false;
	MATRIX RotMut = MGetIdent();
	if( angY!=0 ) RotMut = MMult( MGetRotY( (float)angY ), RotMut );
	if( angZ!=0 ) RotMut = MMult( MGetRotZ( (float)angZ ), RotMut );
	if( angX!=0 ) RotMut = MMult( MGetRotX( (float)angX ), RotMut );

	// ��]�s�����p
	int VectexNum = m_iPolygonNum * 3;
	for( int i=0; i<VectexNum; i++ )
	{
		m_pRawVertexPos[i] = VTransformSR( m_pRawVertexPos[i].toVECTOR(), RotMut );
		m_pRawVertexNrm[i] = VTransformSR( m_pRawVertexNrm[i].toVECTOR(), RotMut ); // �@���x�N�g�����ϊ����K�v�ɂȂ�
	}

};

void TextureSphere3D::MatTransVertex( const MATRIX &Mat )
{
	int VectexNum = m_iPolygonNum * 3;
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos  = VTransform( m_pVertex[i].pos, Mat );
		m_pVertex[i].norm = VTransformSR( m_pVertex[i].norm, Mat ); // �@���x�N�g�����ϊ����K�v�ɂȂ�
	}
};

// ###############################################
// ########## class GroundGrid
// ###############################################

GroundGrid::GroundGrid( 
		double GridRange,	// �O���b�h�͈�
		int    GridNum,		// �O���b�h��
		int    Color		// �O���b�h�̐F
		) : m_iColor( Color )
{
	m_dGridRange = GridRange;
	m_iGridNum = GridNum;

	m_iGridLinesNum = GridNum+1;
	m_pVertiGrid = new LINE[m_iGridLinesNum];
	m_pHorizGrid = new LINE[m_iGridLinesNum];

	double GridSize = (2*GridRange) / (double)GridNum;

	// m_pVertiGrid �� m_pHorizGrid �𐶐�
	for( int i=0; i<=m_iGridNum; i++ )
	{
		m_pHorizGrid[i].from = Vector2D( -m_dGridRange, GridSize*i-m_dGridRange ).toVector3D();
		m_pHorizGrid[i].to   = Vector2D(  m_dGridRange, GridSize*i-m_dGridRange ).toVector3D();
		m_pVertiGrid[i].from = Vector2D( GridSize*i-m_dGridRange, -m_dGridRange ).toVector3D();
		m_pVertiGrid[i].to   = Vector2D( GridSize*i-m_dGridRange,  m_dGridRange ).toVector3D();	
	}

	m_iMaxVectorNum = 2 * 2 * m_iGridLinesNum;

};

void GroundGrid::Render()
{
	for( int i=0; i<=m_iGridNum; i++ )
	{
		DrawLine3D( m_pHorizGrid[i].from.toVECTOR(), m_pHorizGrid[i].to.toVECTOR(), m_iColor );
		DrawLine3D( m_pVertiGrid[i].from.toVECTOR(), m_pVertiGrid[i].to.toVECTOR(), m_iColor );
	}
};

// ���_�𒼐ڕҏW
Vector3D& GroundGrid::editVertexes( int VectorIndex )
{
	assert( VectorIndex < m_iMaxVectorNum );

	int DirFlg = VectorIndex / (2*m_iGridLinesNum);
	int Rest = VectorIndex % (2*m_iGridLinesNum);
	int LineIndex = Rest / 2;
	int Edge = Rest % 2;

	LINE* pLineArray;
	if( DirFlg==0 )
	{
		pLineArray = m_pVertiGrid;
	}
	else
	{
		pLineArray = m_pHorizGrid;
	}

	if( Edge==0 )
	{
		return (pLineArray[LineIndex].to);
	}
	else
	{
		return (pLineArray[LineIndex].from);
	}

}

// ###############################################
// ########## class BasePlaneFigure
// ###############################################

// �R���X�g���N�^
BasePlaneFigure::BasePlaneFigure( 
		int       PolygonNum,       // �������i���_���j
		COLOR_F   EmissivColor,     // �I�u�W�F�N�g�̐F�i���Ȕ����F�j
		bool      UseZBuffer        // Z�o�b�t�@���g�p���邩�H
		) : m_bUseZBuffer(UseZBuffer), m_iPolygonNum(PolygonNum)
{
	
	// Vectex�̃��������m��
	int VectexNum = m_iPolygonNum*3;
	m_pVertex       = new VERTEX3D[VectexNum];
	m_pRawVertexPos = new Vector3D[VectexNum];
	m_pRawVertexNrm = new Vector3D[VectexNum];
	
	// color �Ǝg��Ȃ��v�f��������
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos  = m_pRawVertexPos[i].toVECTOR();
		m_pVertex[i].norm = m_pRawVertexNrm[i].toVECTOR();
		m_pVertex[i].dif  = GetColorU8( 0,0,0,0); //DifColor;
		m_pVertex[i].spc  = GetColorU8( 0,0,0,0); //SpcColor;
		m_pVertex[i].u    = 0.0f;
		m_pVertex[i].v    = 0.0f;
		m_pVertex[i].su   = 0.0f;
		m_pVertex[i].sv   = 0.0f;
	}

	// �}�e���A���p�����[�^�̏�����
	m_Material.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Emissive = EmissivColor ;
	m_Material.Power    = 0.0f ;

	m_MaterialDefault.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Emissive = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Power    = 20.0f ;

};


void BasePlaneFigure::resetVertex()
{
	int VectexNum = m_iPolygonNum * 3;

	// m_pVertex.pos �� m_pRawVertexPos �ŏ㏑��
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos  = m_pRawVertexPos[i].toVECTOR();
		m_pVertex[i].norm = m_pRawVertexNrm[i].toVECTOR();
	}
};

void BasePlaneFigure::Render()
{
	if( !m_bUseZBuffer )
	{
		// Z�o�b�t�@ OFF
		SetUseZBuffer3D( FALSE );
		SetWriteZBuffer3D( FALSE );
	}

	// ���C�g�̉e�����󂯂Ȃ��悤�ɐݒ�
	SetMaterialUseVertDifColor( FALSE ) ; // ���_�f�[�^�̃f�B�t���[�Y�J���[���g�p���Ȃ��悤�ɂ���
	SetMaterialUseVertSpcColor( FALSE ) ; // ���_�f�[�^�̃X�y�L�����J���[���g�p���Ȃ��悤�ɂ���
	SetMaterialParam( m_Material ) ; // �}�e���A���̐ݒ��L��

	DrawPolygon3D( m_pVertex, m_iPolygonNum, DX_NONE_GRAPH, FALSE ) ;

	// �}�e���A���ݒ�����ɖ߂�
	SetMaterialUseVertDifColor( TRUE ) ;
	SetMaterialUseVertSpcColor( TRUE ) ;
	SetMaterialParam( m_MaterialDefault ) ;

	if( !m_bUseZBuffer )
	{
		// Z�o�b�t�@�L����
		SetUseZBuffer3D( TRUE );
		SetWriteZBuffer3D( TRUE );
	}


};

void BasePlaneFigure::MatTransVertex( const MATRIX &Mat )
{
	int VectexNum = m_iPolygonNum * 3;
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos  = VTransform( m_pVertex[i].pos, Mat );
		m_pVertex[i].norm = VTransformSR( m_pVertex[i].norm, Mat ); // �@���x�N�g�����ϊ����K�v�ɂȂ�
	}
};



// ###############################################
// ########## class PlaneRing
// ###############################################
PlaneRing::PlaneRing( 
		double    Radius,           // �ւ̔��a�i���a�j
		double    Width,			// �ւ̕� 
		int       DivNum            // ������
		) : 
	BasePlaneFigure( DivNum*2, GetColorF( 1.0f, 0.0f, 0.0f, 0.0f ), false ),
	m_vCntPos( Vector3D(0,0,0) )
{
	// #### �悸�́A�P�ʉ~������Vertex�𐶐����A������V�t�g�A�X�P�[�����O����
	
	// ���������琳���p�`�̒��S�p���v�Z
	double CenterAng = 2*DX_PI_F/((double)DivNum);
	
	Vector3D V3D0B( 0,  0,0 );
	Vector3D V3D0T( 0,1.0,0 );
	Vector3D Nrm( 0.0, 1.0, 0.0 );

	for( int i=0; i<DivNum; i++ )
	{
		int j=(i+1)%DivNum; // ���̓Y���i�~���Ή��j
		Vector2D V2D1( cos(CenterAng*i), sin(CenterAng*i) );
		Vector2D V2D2( cos(CenterAng*j), sin(CenterAng*j) );
		
		Vector3D V3D1i = ( Radius       *V2D1).toVector3D();
		Vector3D V3D1o = ((Radius+Width)*V2D1).toVector3D();
		Vector3D V3D2i = ( Radius       *V2D2).toVector3D();
		Vector3D V3D2o = ((Radius+Width)*V2D2).toVector3D();


		// �O�p�`�P
		m_pRawVertexPos[ 6*i+3*0+0 ] = V3D1o;
		m_pRawVertexPos[ 6*i+3*0+1 ] = V3D2o;
		m_pRawVertexPos[ 6*i+3*0+2 ] = V3D1i;
		
		// �O�p�`�Q
		m_pRawVertexPos[ 6*i+3*1+0 ] = V3D1i;
		m_pRawVertexPos[ 6*i+3*1+1 ] = V3D2i;
		m_pRawVertexPos[ 6*i+3*1+2 ] = V3D2o;

		for( j=0; j<6; j++ )
		{
			m_pRawVertexNrm[ 6*i+j ] = Nrm;
		}

	}

	resetVertex();

};

void PlaneRing::setCenterPos( Vector3D CntPos )
{
	int VectexNum = m_iPolygonNum * 3;

	// ���S�ʒu���V�t�g
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos = (m_pRawVertexPos[i]+CntPos).toVECTOR();
	}
};


// ###############################################
// ########## class PlaneConvexFill
// ###############################################

// �R���X�g���N�^
PlaneConvexFill::PlaneConvexFill( 
		Vector2D  *pVertex2D,       // �ʌ`�}�`�̗֊s���_�̔z��
		int       DivNum,           // �������i���_���j
		COLOR_F   EmissivColor      // �I�u�W�F�N�g�̐F�i���Ȕ����F�j
		) : BasePlaneFigure( DivNum - 2, EmissivColor, true )
{
	// �֊s���_��񂩂�A�|���S�����𐶐�����B
	// ���߂̗v�f pVertex2D[0] �𒆐S�ɕ��ˏ�ɎO�p�`��������

	// �@�������x�N�g��
	Vector3D vNrm = Vector3D( 0.0, 1.0, 0.0 );

	int c=0; //�J�E���^
	for( int i=2; i<DivNum; i++ )
	{
		// �O�p�`
		m_pRawVertexPos[ 3*c+0 ] = pVertex2D[0].toVector3D();
		m_pRawVertexNrm[ 3*c+0 ] = vNrm;

		m_pRawVertexPos[ 3*c+1 ] = pVertex2D[i-1].toVector3D();
		m_pRawVertexNrm[ 3*c+1 ] = vNrm;

		m_pRawVertexPos[ 3*c+2 ] = pVertex2D[i].toVector3D();
		m_pRawVertexNrm[ 3*c+2 ] = vNrm;
		c++;

	}
	assert( c==m_iPolygonNum );

	resetVertex();

};


// ###############################################
// ########## class LineRing
// ###############################################


LineRing::LineRing(
		double    Radius,           // �ւ̔��a
		int       DivNum,           // ������
		unsigned int Color          // ���̐F
		) :
	m_iColor( Color )
{
	// m_pVECTORs, m_pRawVertexPos �̃��������m��
	m_iVertexNum = DivNum;
	m_pVECTORs      = new VECTOR[m_iVertexNum];
	m_pRawVertexPos = new Vector3D[m_iVertexNum];

	// ���������琳���p�`�̒��S�p���v�Z
	double CenterAng = 2*DX_PI_F/((double)m_iVertexNum);

	// m_pRawVertexPos �𐶐�
	for( int i=0; i<m_iVertexNum; i++ )
	{
		Vector2D V2D( cos(CenterAng*i), sin(CenterAng*i) );
		m_pRawVertexPos[i] = (Radius*V2D).toVector3D();
	}
};

void LineRing::resetVertex()
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = m_pRawVertexPos[i].toVECTOR();
	}
}

void LineRing::Render()
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		int j=(i+1)%m_iVertexNum; // ���̓Y���i�~���Ή��j
		DrawLine3D( m_pVECTORs[i], m_pVECTORs[j], m_iColor );
	}
};

void LineRing::MatTransVertex( const MATRIX &Mat )
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = VTransform( m_pVECTORs[i], Mat );
	}
};


// ###############################################
// ########## class LineSegment
// ###############################################


LineSegment::LineSegment(
		Vector3D bgn,  // �n�_
		Vector3D end,  // �I�_
		unsigned int Color          // ���̐F
		) :
	m_iColor( Color )
{
	// m_pVECTORs, m_pRawVertexPos �̃��������m��
	m_iVertexNum = 2;
	m_pVECTORs      = new VECTOR[m_iVertexNum];
	m_pRawVertexPos = new Vector3D[m_iVertexNum];

	m_pRawVertexPos[0] = bgn;
	m_pRawVertexPos[1] = end;
};

void LineSegment::resetVertex()
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = m_pRawVertexPos[i].toVECTOR();
	}
}

void LineSegment::Render()
{
	DrawLine3D( m_pVECTORs[0], m_pVECTORs[1], m_iColor );
};

void LineSegment::MatTransVertex( const MATRIX &Mat )
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = VTransform( m_pVECTORs[i], Mat );
	}
};

// ###############################################
// ########## class LineFreeCycle
// ###############################################

LineFreeCycle::LineFreeCycle(
		int       DivNum,           // ������
		unsigned int Color          // ���̐F
		) :
	m_iColor( Color )
{
	// m_pVECTORs, m_pRawVertexPos �̃��������m��
	m_iVertexNum = DivNum;
	m_pVECTORs      = new VECTOR[m_iVertexNum];
	m_pRawVertexPos = new Vector3D[m_iVertexNum];

	// ���������琳���p�`�̒��S�p���v�Z
	double CenterAng = 2*DX_PI_F/((double)m_iVertexNum);

	// m_pRawVertexPos �𐶐�
	for( int i=0; i<m_iVertexNum; i++ )
	{
		// �����l�͒P�ʉ~�ɂȂ�悤�ɂ���B
		Vector2D V2D( cos(CenterAng*i), sin(CenterAng*i) );
		m_pRawVertexPos[i] = (1.0*V2D).toVector3D();
	}
};

void LineFreeCycle::resetVertex()
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = m_pRawVertexPos[i].toVECTOR();
	}
}

void LineFreeCycle::Render()
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		int j=(i+1)%m_iVertexNum; // ���̓Y���i�~���Ή��j
		DrawLine3D( m_pVECTORs[i], m_pVECTORs[j], m_iColor );
	}
};

void LineFreeCycle::MatTransVertex( const MATRIX &Mat )
{
	for( int i=0; i<m_iVertexNum; i++ )
	{
		m_pVECTORs[i] = VTransform( m_pVECTORs[i], Mat );
	}
};

// ###############################################
// ########## class Arrow3D
// ###############################################
// �R���X�g���N�^
Arrow3D::Arrow3D( 
	double    ArrowHight, 
	double    ArrowWidth, 
	double    BodyWidth, 
	COLOR_F   EmissivColor
	) : m_dArwHight( ArrowHight ),
		m_dArwWidth( ArrowWidth ),
		m_dBdyWidth( BodyWidth )
{
	// �|���S���z����m�ہE������
	m_iPolygonNum = 3; // 1(���̖�̕���) + 2(���̖_�̕���)

	// Vectex�̃��������m��
	int VectexNum = m_iPolygonNum*3;
	m_pVertex       = new VERTEX3D[VectexNum];
	
	// color �Ǝg��Ȃ��v�f��������
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].dif  = GetColorU8( 0,0,0,0); //DifColor;
		m_pVertex[i].spc  = GetColorU8( 0,0,0,0); //SpcColor;
		m_pVertex[i].u    = 0.0f;
		m_pVertex[i].v    = 0.0f;
		m_pVertex[i].su   = 0.0f;
		m_pVertex[i].sv   = 0.0f;
	}

	// �}�e���A���p�����[�^�̏�����
	m_Material.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_Material.Emissive = EmissivColor ;
	m_Material.Power    = 0.0f ;

	m_MaterialDefault.Diffuse  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Specular = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Ambient  = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Emissive = GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ;
	m_MaterialDefault.Power    = 20.0f ;

};

// �`��
void Arrow3D::Render( Vector3D vBgn, Vector3D vEnd, Vector3D vUpper )
{
	// �@ ������{���ʂɕϊ����邽�߂̍��W�ϊ��s����v�Z����

	// �e���x�N�g�����v�Z����
	Vector3D vHeading = (vEnd-vBgn).normalize();
	//Vector3D vSide = (vHeading * vUpper).normalize(); // ��͂�G���[�ɂȂ�
	Vector3D vSide = VCross( vHeading.toVECTOR() , vUpper.toVECTOR() );
	vSide = vSide.normalize();

	Vector3D vNewUpper = VCross( vSide.toVECTOR() , vHeading.toVECTOR() );

	// ���W�ϊ��s����v�Z����
	MATRIX TransMat = MGetAxis1(
		vHeading.toVECTOR(),
		vNewUpper.toVECTOR(),
		vSide.toVECTOR(),
		vBgn.toVECTOR() );

	// �A ��{���ʏ�Ŗ��̊e���_�̍��W���v�Z����
	double dArwLength = (vEnd-vBgn).len();

	Vector3D vArwT (  dArwLength            , 0,                0 );
	Vector3D vArwL (  dArwLength-m_dArwHight, 0, -0.5*m_dArwWidth );
	Vector3D vArwR (  dArwLength-m_dArwHight, 0,  0.5*m_dArwWidth );
	Vector3D vBdyTL(  dArwLength-m_dArwHight, 0, -0.5*m_dBdyWidth );
	Vector3D vBdyTR(  dArwLength-m_dArwHight, 0,  0.5*m_dBdyWidth );
	Vector3D vBdyBL(                       0, 0, -0.5*m_dBdyWidth );
	Vector3D vBdyBR(                       0, 0,  0.5*m_dBdyWidth );

	// �B �A�Ōv�Z������{���ʏ�̒��_���W���@�Ōv�Z�������W�ϊ��s��Ń��[���h���W�ɕϊ�����i�|���S���̒��_���W���X�V����B�j

	vArwT  = VTransform( vArwT .toVECTOR(), TransMat);
	vArwL  = VTransform( vArwL .toVECTOR(), TransMat);
	vArwR  = VTransform( vArwR .toVECTOR(), TransMat);
	vBdyTL = VTransform( vBdyTL.toVECTOR(), TransMat);
	vBdyTR = VTransform( vBdyTR.toVECTOR(), TransMat);
	vBdyBL = VTransform( vBdyBL.toVECTOR(), TransMat);
	vBdyBR = VTransform( vBdyBR.toVECTOR(), TransMat);

	m_pVertex[0].pos  = vArwT.toVECTOR();
	m_pVertex[1].pos  = vArwL.toVECTOR();
	m_pVertex[2].pos  = vArwR.toVECTOR();

	m_pVertex[3].pos  = vBdyTL.toVECTOR();
	m_pVertex[4].pos  = vBdyTR.toVECTOR();
	m_pVertex[5].pos  = vBdyBL.toVECTOR();

	m_pVertex[6].pos  = vBdyBL.toVECTOR();
	m_pVertex[7].pos  = vBdyBR.toVECTOR();
	m_pVertex[8].pos  = vBdyTR.toVECTOR();

	// �C �|���S����`�悷��B

	// Z�o�b�t�@ OFF
	//SetUseZBuffer3D( FALSE );
	//SetWriteZBuffer3D( FALSE );

	// ���C�g�̉e�����󂯂Ȃ��悤�ɐݒ�
	SetMaterialUseVertDifColor( FALSE ) ; // ���_�f�[�^�̃f�B�t���[�Y�J���[���g�p���Ȃ��悤�ɂ���
	SetMaterialUseVertSpcColor( FALSE ) ; // ���_�f�[�^�̃X�y�L�����J���[���g�p���Ȃ��悤�ɂ���
	SetMaterialParam( m_Material ) ; // �}�e���A���̐ݒ��L��

	DrawPolygon3D( m_pVertex, m_iPolygonNum, DX_NONE_GRAPH, FALSE ) ;

	// �}�e���A���ݒ�����ɖ߂�
	SetMaterialUseVertDifColor( TRUE ) ;
	SetMaterialUseVertSpcColor( TRUE ) ;
	SetMaterialParam( m_MaterialDefault ) ;

	// Z�o�b�t�@�L����
	//SetUseZBuffer3D( TRUE );
	//SetWriteZBuffer3D( TRUE );

};

// ###############################################
// ########## class BasicFig_Base
// ###############################################

// �R���X�g���N�^
// - �}�`�̓����ɍ��킹�āA�l�X�ȏ��������@�ɂȂ�B
//--------------------------
BasicFig_Base::BasicFig_Base( 
		int       PolygonNum,       // �|���S���i�O�p�`�j��
		COLOR_U8  DifColor,         // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor          // ���̒��_�X�y�L�����J���[
		) : 
		m_iPolygonNum(PolygonNum)
{
	m_iVectexNum = 3* m_iPolygonNum ;
	
	//   - �|�C���^�̃������m�ہB
	// Vectex�̃��������m��
	m_pVertex   = new VERTEX3D[m_iVectexNum];
	m_pOrgVertx = new Vector3D[m_iVectexNum];
	m_pOrgNorms = new Vector3D[m_iVectexNum];

	//   - m_pVertex �֐F�i�{���j�̐ݒ�B
	// color �Ǝg��Ȃ��v�f��������
	for( int i=0; i<m_iVectexNum; i++ )
	{
		m_pVertex[i].pos  = m_pOrgVertx[i].toVECTOR();
		m_pVertex[i].norm = m_pOrgNorms[i].toVECTOR();
		m_pVertex[i].dif  = DifColor; //DifColor;
		m_pVertex[i].spc  = SpcColor; //SpcColor;
		m_pVertex[i].u    = 0.0f;
		m_pVertex[i].v    = 0.0f;
		m_pVertex[i].su   = 0.0f;
		m_pVertex[i].sv   = 0.0f;
	}

};

// ��{�ό`�̎��{
// - �I���W�i����vertex�ɓK�p�����i=�}�`�̃f�t�H���g�̎p���j
//--------------------------
void BasicFig_Base::setDefault( const MATRIX &Mat )
{
	for( int i=0; i<m_iVectexNum; i++ )
	{
		m_pOrgVertx[i]  = VTransform(   m_pOrgVertx[i].toVECTOR() , Mat );
		m_pOrgNorms[i]  = VTransformSR( m_pOrgNorms[i].toVECTOR() , Mat ); // �@���x�N�g�����ϊ����K�v�ɂȂ�
	}

	// * m_pVertex ���X�V�isetMatrix �����s�j�B
	setMatrix(m_MTransMat);

};

// ���W�ϊ��s��̐ݒ�
// - �`�掞�ɓK�p�����ό`
//--------------------------
void BasicFig_Base::setMatrix( const MATRIX &Mat )
{
	// �s��� m_MTransMat �ɕۑ�
	m_MTransMat = Mat;

	// * m_pOrgVertx/m_pOrgNorms �� m_pVertex �֑���B
	// * m_pVertex �� TransMat �̍��W�ϊ������{
	for( int i=0; i<m_iVectexNum; i++ )
	{
		m_pVertex[i].pos   = VTransform(   m_pOrgVertx[i].toVECTOR() , m_MTransMat );
		m_pVertex[i].norm  = VTransformSR( m_pOrgNorms[i].toVECTOR() , m_MTransMat ); // �@���x�N�g�����ϊ����K�v�ɂȂ�
	}

};

// �`��
//--------------------------
void BasicFig_Base::Render() 
{
	// * �`��
	DrawPolygon3D( m_pVertex, m_iPolygonNum, DX_NONE_GRAPH, FALSE ) ;

};


// ###############################################
// ########## class BasicFig_Column : BasicFig_Base
// ###############################################

// �R���X�g���N�^
BasicFig_Column::BasicFig_Column( 
		Vector3D  CenterPos,       // ��ʂ̉~�`�̒��S�ʒu
		double    Radius,          // ���a
		double    Hight,           // �~���̍���
		int       DivNum,          // �������i����ʂ̉~�`�̕������j
		COLOR_U8  DifColor,        // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor         // ���̒��_�X�y�L�����J���[
		) :
		BasicFig_Base( DivNum*4, DifColor, SpcColor )
{
	// �|���S�����F
	// DivNum * 4(��ʁE��ӁE���ʁi�Q�{�j) 

	// ��͑O�ɍ쐬�����~���N���X�̃��W�b�N���R�s�[
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
		m_pOrgVertx[ 12*i+3*0+0 ]  = V3D0B;
		m_pOrgVertx[ 12*i+3*0+1 ]  = V3D1B;
		m_pOrgVertx[ 12*i+3*0+2 ]  = V3D2B;
		for( int k=0; k<3; k++) m_pOrgNorms[ 12*i+3*0+k ] = Vector3D( 0, -1.0, 0);

		// ���ʂP
		m_pOrgVertx[ 12*i+3*1+0 ]  = V3D1T;
		m_pOrgVertx[ 12*i+3*1+1 ]  = V3D1B;
		m_pOrgVertx[ 12*i+3*1+2 ]  = V3D2B;
		for( int k=0; k<3; k++) m_pOrgNorms[ 12*i+3*1+k ] = V3Dn;

		// ���ʂQ
		m_pOrgVertx[ 12*i+3*2+0 ]  = V3D2B;
		m_pOrgVertx[ 12*i+3*2+1 ]  = V3D1T;
		m_pOrgVertx[ 12*i+3*2+2 ]  = V3D2T;
		for( int k=0; k<3; k++) m_pOrgNorms[ 12*i+3*2+k ] = V3Dn;

		// ���
		m_pOrgVertx[ 12*i+3*3+0 ]  = V3D0T;
		m_pOrgVertx[ 12*i+3*3+1 ]  = V3D1T;
		m_pOrgVertx[ 12*i+3*3+2 ]  = V3D2T;
		for( int k=0; k<3; k++) m_pOrgNorms[ 12*i+3*3+k ] = Vector3D( 0, 1.0, 0);

	}
	
	// �V�t�g�A�X�P�[�����O����
	for( int i=0; i<m_iVectexNum; i++ )
	{
		Vector3D tmp = m_pOrgVertx[i];
		tmp.x *= Radius;
		tmp.z *= Radius;
		tmp.y *= Hight;
		tmp += CenterPos;
		m_pOrgVertx[i] = tmp;
	}

	// �s��̏����l�i�P�ʍs��j�̐ݒ��m_pVertex �̏�����
	// * �S�Ă̊�{�}�`�N���X�Ŏ��{���K�v�B
	setMatrix(MGetIdent());

};

// ###############################################
// ########## class BasicFig_Column : BasicFig_Base
// ###############################################

// �R���X�g���N�^
BasicFig_Cone::BasicFig_Cone( 
		Vector3D  CenterPos,       // ��ʂ̉~�`�̒��S�ʒu
		double    Radius,          // ���a
		double    Hight,           // �~���̍���
		int       DivNum,          // �������i����ʂ̉~�`�̕������j
		COLOR_U8  DifColor,        // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor         // ���̒��_�X�y�L�����J���[
		) :
		BasicFig_Base( DivNum*2, DifColor, SpcColor )
{
	// �|���S�����F
	// DivNum * 2(��ӁE����) 

	// ��͑O�ɍ쐬�����~���N���X�̃��W�b�N���R�s�[
	// ���������琳���p�`�̒��S�p���v�Z
	double CenterAng = 2*DX_PI_F/((double)DivNum);
	
	Vector3D V3D0B( 0,  0,0 );
	Vector3D V3D0T( 0,1.0,0 );

	for( int i=0; i<DivNum; i++ )
	{
		int j=(i+1)%DivNum; // ���̓Y���i�~���Ή��j
		Vector2D V2D1( cos(CenterAng*i), sin(CenterAng*i) );
		Vector2D V2D2( cos(CenterAng*j), sin(CenterAng*j) );
		
		Vector3D V3D1B = V2D1.toVector3D();
		Vector3D V3D2B = V2D2.toVector3D();

		// ���
		m_pOrgVertx[ 6*i+3*0+0 ]  = V3D0B;
		m_pOrgVertx[ 6*i+3*0+1 ]  = V3D1B;
		m_pOrgVertx[ 6*i+3*0+2 ]  = V3D2B;
		for( int k=0; k<3; k++) m_pOrgNorms[ 6*i+3*0+k ] = Vector3D( 0, -1.0, 0);

		// ���ʂP
		m_pOrgVertx[ 6*i+3*1+0 ]  = V3D0T;
		m_pOrgVertx[ 6*i+3*1+1 ]  = V3D1B;
		m_pOrgVertx[ 6*i+3*1+2 ]  = V3D2B;

		// �@�������͊O�ς���v�Z����
		for( int k=0; k<3; k++) m_pOrgNorms[ 6*i+3*1+k ] = ((V3D2B-V3D0T)%(V3D1B-V3D0T)).normalize();

	}
	
	// �V�t�g�A�X�P�[�����O����
	for( int i=0; i<m_iVectexNum; i++ )
	{
		Vector3D tmp = m_pOrgVertx[i];
		tmp.x *= Radius;
		tmp.z *= Radius;
		tmp.y *= Hight;
		tmp += CenterPos;
		m_pOrgVertx[i] = tmp;
	}

	// �s��̏����l�i�P�ʍs��j�̐ݒ��m_pVertex �̏�����
	// * �S�Ă̊�{�}�`�N���X�Ŏ��{���K�v�B
	setMatrix(MGetIdent());

};

// ###############################################
// ########## class CoordinateAxisModel
// ###############################################

// �R���X�g���N�^
CoordinateAxisModel::CoordinateAxisModel(
		double thickness,		// ���W���̑����i�����傫���j
		double axis_x_length,	// �w���̒���
		double axis_y_length,	// �x���̒���
		double axis_z_length	// �y���̒���
		)
{
	// �e��{�}�`�̐��@�����߂邽�߂̌v�Z�B
	
	// * ���̖_�����̔��a     = thickness
	// * ���̐�[�̉~���̔��a = thickness * 1.5
	// * ���̐�[�̉~���̍��� = thickness * 2.0

	double scale_botom = 2.0; 
	double scale_hight = 4.0;

	// ��{�}�`�̃C���X�^���X��

	m_pAxisX_Bar = new BasicFig_Column( 
		Vector3D( 0, 0, 0 ),
		thickness,
		axis_x_length,
		32,
		GetColorU8( 255, 0, 0, 0 ),
		GetColorU8( 255, 0, 0, 0 )
		);

	m_pAxisY_Bar = new BasicFig_Column( 
		Vector3D( 0, 0, 0 ),
		thickness,
		axis_y_length,
		32,
		GetColorU8( 0, 255, 0, 0 ),
		GetColorU8( 0, 255, 0, 0 )
		);

	m_pAxisZ_Bar = new BasicFig_Column( 
		Vector3D( 0, 0, 0 ),
		thickness,
		axis_z_length,
		32,
		GetColorU8( 0, 0, 255, 0 ),
		GetColorU8( 0, 0, 255, 0 )
		);

	m_pAxisX_Tip = new BasicFig_Cone( 
		Vector3D( 0, 0, 0 ),
		scale_botom * thickness,
		scale_hight * thickness,
		32,
		GetColorU8( 255, 0, 0, 0 ),
		GetColorU8( 255, 0, 0, 0 )
		);

	m_pAxisY_Tip = new BasicFig_Cone( 
		Vector3D( 0, 0, 0 ),
		scale_botom * thickness,
		scale_hight * thickness,
		32,
		GetColorU8( 0, 255, 0, 0 ),
		GetColorU8( 0, 255, 0, 0 )
		);

	m_pAxisZ_Tip = new BasicFig_Cone( 
		Vector3D( 0, 0, 0 ),
		scale_botom * thickness,
		scale_hight * thickness,
		32,
		GetColorU8( 0, 0, 255, 0 ),
		GetColorU8( 0, 0, 255, 0 )
		);

	// ��{�}�`�̈ʒu�����̐ݒ�
	MATRIX Msft;
	MATRIX Mrot;
	MATRIX Mwrk;

	// ���̌`������Ă���A���̕����ɓ|���B
	// �� ������֎��̒����������V�t�g���玲�����։�]

	// �w��
	Msft = MGetTranslate( Vector3D( 0, axis_x_length, 0 ).toVECTOR() );
	Mrot = MGetRotZ( -DX_PI_F/2 ); // X�������͔��΂ɂȂ�悤�ɂ���

	Mwrk = MMult( Msft, Mrot );
	m_pAxisX_Tip->setDefault(Mwrk);
	m_pAxisX_Bar->setDefault(Mrot);

	// �x��
	Msft = MGetTranslate( Vector3D( 0, axis_y_length, 0 ).toVECTOR() );
	Mrot = MGetIdent();

	Mwrk = MMult( Msft, Mrot );
	m_pAxisY_Tip->setDefault(Mwrk);
	m_pAxisY_Bar->setDefault(Mrot);

	// �y��
	Msft = MGetTranslate( Vector3D( 0, axis_z_length, 0 ).toVECTOR() );
	Mrot = MGetRotX( DX_PI_F/2 );

	Mwrk = MMult( Msft, Mrot );
	m_pAxisZ_Tip->setDefault(Mwrk);
	m_pAxisZ_Bar->setDefault(Mrot);

};

void CoordinateAxisModel::setMatrix( const MATRIX &Mat )
{
	// �e��{�}�`��setMatrix���邾��
	m_pAxisX_Tip->setMatrix( Mat );
	m_pAxisX_Bar->setMatrix( Mat );
	m_pAxisY_Tip->setMatrix( Mat );
	m_pAxisY_Bar->setMatrix( Mat );
	m_pAxisZ_Tip->setMatrix( Mat );
	m_pAxisZ_Bar->setMatrix( Mat );

};

void CoordinateAxisModel::Render()
{
	// �e��{�}�`��Render���邾��
	m_pAxisX_Tip->Render();
	m_pAxisX_Bar->Render();
	m_pAxisY_Tip->Render();
	m_pAxisY_Bar->Render();
	m_pAxisZ_Tip->Render();
	m_pAxisZ_Bar->Render();

};

// ��́A���̃N���X�̓���m�F
// �\���\���H
// �ό`�\���H
// �� ����

// �~���N���X�̍쐬
// * �l����K�v������Ƃ���΁A�@���x�N�g���̌v�Z���ǂ�����Ă�邩�B
// �� �����i�����j

// �L�����N�^���[�J�����W���f���i���p�}�`�N���X�j�̍쐬
// �� 21:09 �����B

// ���f�����L�����N�^�̃��[�J�����W�ɕ`�悳����


// �����͐}���قɍs���Ȃ��Ă�����...
// �L���쐬��簐i������......









