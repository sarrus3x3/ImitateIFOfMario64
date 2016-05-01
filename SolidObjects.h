#include "DxLib.h"

#include "Vector3D.h"
#include "Vector2D.h"

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
		int       TexturesHandle,   // �e�N�X�`���摜�̃n���h��
		COLOR_U8  DifColor, // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor  // ���̒��_�X�y�L�����J���[
		);

	// �`��
	void Render();

};

// �e�L�X�`����\��t�����鋅
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

	Vector3D* m_pRawVertexPos; // VERTEX������ɒ��S�ʒu�V�t�g�ɑΉ����邽�߃I���W�i���̒��_�ʒu����ێ�����
	Vector3D  m_vCntPos;       // ���̒��S�ʒu 
	MATERIALPARAM m_Material;         // �}�e���A���p�����[�^
	MATERIALPARAM m_MaterialDefault;  // �}�e���A���p�����[�^�i�f�t�H���g�j

public:
	// �R���X�g���N�^
	TextureSphere3D( 
		Vector3D  CntPos,           // �����S�̈ʒu
		double    Radius,           // ���̔��a
		bool      Outward,          // true:�O�����Afalse:������
		int       DivNumLongi,      // �o�x�����̕�����
		int       DivNumLati,       // �ܓx�����̕�����
		int       TexturesHandle,   // �e�N�X�`���摜�̃n���h��
		COLOR_U8  DifColor, // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor  // ���̒��_�X�y�L�����J���[
		);

	// �`��
	void Render();

	// ���S�ʒu�̐ݒ�i�X�V�j
	void setCenterPos( Vector3D CntPos );

};

// �n�ʂɕ��ᎆ�͗l��`�悷��@�\��ǉ�
class GroundGrid
{
private:
	double m_dGridRange; // �O���b�h�͈�
	int    m_iGridNum;   // �O���b�h��
	// ���_�𒆐S�ɁAx���Ay�� : [-m_dGridRange, m_dGridRange] �͈̔͂ɃO���b�h��`�悷��

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
		int    GridNum 		// �O���b�h��
		);

	// �`��
	void Render();

};