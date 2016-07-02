#include "DxLib.h"

#include "Vector3D.h"
#include "Vector2D.h"

// ############### �~����`�悷��class ###############
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

// ############### �ׂ������b�V���̕��� ###############
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

// ############### �����̂�`�悷��class�A�摜�\��t���̎��� ###############
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
		int       TexturesHandle,   // �e�N�X�`���摜�̃n���h��
		COLOR_U8  DifColor, // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor  // ���̒��_�X�y�L�����J���[
		);

	// �`��
	void Render();

};

// ############### �e�L�X�`����\��t�����鋅 ###############
// �y���ӎ����z
// �E�p�m���}�����Ŕw�ʂɕ`�悷�邽�߁ARender��Z�o�b�t�@��OFF�ɂ��Ă���B
//   �`�掞�̓p�m���}������ԏ��߂ɕ`�悷�邱�ƁB�i�������Ȃ��ƁA������O�ɕ`�悳�ꂽ�}�`���B��Ă��܂��I�j
class TextureSphere3D
{
private:
	VERTEX3D* m_pVertex; // �|���S���W����ێ�
	int m_iPolygonNum;   // �|���S����

	// �e�N�X�`���摜�̃n���h��
	int m_iTexturesHandle;

	Vector3D* m_pRawVertexPos; // �I���W�i���̒��_�ʒu����ێ�����
	Vector3D* m_pRawVertexNrm; // �I���W�i���̒��_�@���x�N�g������ێ����� �� �I�u�W�F�N�g�̉�]���ɁA�@��
	Vector3D  m_vCntPos;       // ���̒��S�ʒu 
	MATERIALPARAM m_Material;         // �}�e���A���p�����[�^
	MATERIALPARAM m_MaterialDefault;  // �}�e���A���p�����[�^�i�f�t�H���g�j

	bool m_bOutward; // �O�������̃t���O

public:
	// �I�u�W�F�N�g�^�C�v
	enum ObjectTypeID
	{
		OBJECT_SKYDOME,    // �X�J�C�h�[���i�p�m���}���j: �G�~�b�V�u���̂�
		OBJECT_NOSPECULAR  // ����Ȃ� : �X�y�L����OFF
	};
private:
	ObjectTypeID m_eObjectType; // �I�u�W�F�N�g�^�C�v��ێ�

public:
	// �R���X�g���N�^
	TextureSphere3D( 
		Vector3D  CntPos,           // �����S�̈ʒu
		double    Radius,           // ���̔��a
		bool      Outward,          // �\�ʂ̌���  true:�O�����Afalse:������
		int       DivNumLongi,      // �o�x�����̕�����
		int       DivNumLati,       // �ܓx�����̕�����
		int       TexturesHandle,   // �e�N�X�`���摜�̃n���h��
		ObjectTypeID ObjectType     // �I�u�W�F�N�g�^�C�v
		);

	// m_pVertex �� m_pRawVertexPos �ŏ�����
	void resetVertex();

	// �`��
	void Render();

	// �ݒ肵�����S�ʒu�� m_pRawVertexPos ���� m_pVertex ���i�āj�v�Z
	void setCenterPos( Vector3D CntPos );

	// m_pRawVertexPos ���I�C���[��]�FY����]��Z����]��X����] �̏��ɍ�p
	// �� ���̊֐����Ă񂾂����ł́Am_pVertex �ɔ��f����Ȃ��̂ŁAsetCenterPos �ōX�V���邱�ƁB
	void rotEuler( double angX, double angZ, double angY );

	// �^����ꂽ�s�� Mat �� m_pVertex ��ϊ�
	void MatTransVertex( const MATRIX &Mat );


};

// ############### �n�ʂɕ��ᎆ�͗l��`�悷��@�\��ǉ� ###############
class GroundGrid
{
private:
	double m_dGridRange; // �O���b�h�͈�
	int    m_iGridNum;   // �O���b�h��
	// ���_�𒆐S�ɁAx���Ay�� : [-m_dGridRange, m_dGridRange] �͈̔͂ɃO���b�h��`�悷��
	int    m_iColor;     // �O���b�h�̐F

	// �����̍\����
	struct LINE
	{
		Vector3D to;
		Vector3D from;
	};

	LINE* m_pVertiGrid; // �c����gird
	LINE* m_pHorizGrid; // ������gird

public:
	// �R���X�g���N�^
	GroundGrid( 
		double GridRange,	// �O���b�h�͈�
		int    GridNum,		// �O���b�h��
		int    Color		// �O���b�h�̐F
		);

	// �`��
	void Render();

};

// ############### ���ʗւ��� ###############
class PlaneRing
{
private:
	VERTEX3D* m_pVertex; // �|���S���W����ێ�
	int m_iPolygonNum;   // �|���S����

	Vector3D* m_pRawVertexPos; // VERTEX������ɒ��S�ʒu�V�t�g�ɑΉ����邽�߃I���W�i���̒��_�ʒu����ێ�����
	Vector3D  m_vCntPos;       // ���̒��S�ʒu 
	MATERIALPARAM m_Material;         // �}�e���A���p�����[�^
	MATERIALPARAM m_MaterialDefault;  // �}�e���A���p�����[�^�i�f�t�H���g�j

public:
	// �R���X�g���N�^
	PlaneRing( 
		double    Radius,           // �ւ̔��a�i���a�j
		double    Width,			// �ւ̕� 
		int       DivNum,           // ������
		COLOR_U8  DifColor,			// ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor			// ���̒��_�X�y�L�����J���[
		);

	// �`��
	void Render();

	// ���S�ʒu�̐ݒ�i�X�V�j
	void setCenterPos( Vector3D CntPos );

};

// ############### ���ւ��� ###############
class LineRing
{
private:
	int       m_iVertexNum;    // ���_��
	VECTOR*   m_pVECTORs;      // ���_�W����ێ�
	Vector3D* m_pRawVertexPos; // VERTEX������ɒ��S�ʒu�V�t�g�ɑΉ����邽�߃I���W�i���̒��_�ʒu����ێ�����
	MATERIALPARAM m_Material;         // �}�e���A���p�����[�^
	MATERIALPARAM m_MaterialDefault;  // �}�e���A���p�����[�^�i�f�t�H���g�j

	unsigned int m_iColor;  // ���̐F

public:

	// �R���X�g���N�^
	LineRing( 
		double    Radius,           // �ւ̔��a
		int       DivNum,           // ������
		unsigned int Color          // ���̐F
		);

	// m_pVertex �� m_pRawVertexPos �ŏ�����
	void resetVertex();

	// �`��
	void Render();

	// �^����ꂽ�s�� Mat �� m_pVertex ��ϊ�
	void MatTransVertex( const MATRIX &Mat );

};

// ############### ���� ###############
class LineSegment
{
private:
	int       m_iVertexNum;    // ���_��
	VECTOR*   m_pVECTORs;      // ���_�W����ێ�
	Vector3D* m_pRawVertexPos; // VERTEX������ɒ��S�ʒu�V�t�g�ɑΉ����邽�߃I���W�i���̒��_�ʒu����ێ�����
	MATERIALPARAM m_Material;         // �}�e���A���p�����[�^
	MATERIALPARAM m_MaterialDefault;  // �}�e���A���p�����[�^�i�f�t�H���g�j

	unsigned int m_iColor;  // ���̐F

public:

	// �R���X�g���N�^
	LineSegment( 
		Vector3D bgn,  // �n�_
		Vector3D end,  // �I�_
		unsigned int Color  // ���̐F
		);

	// m_pVertex �� m_pRawVertexPos �ŏ�����
	void resetVertex();

	// �`��
	void Render();

	// �^����ꂽ�s�� Mat �� m_pVertex ��ϊ�
	void MatTransVertex( const MATRIX &Mat );

};
