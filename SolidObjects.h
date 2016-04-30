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
		COLOR_U8  DifColor, // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor  // ���̒��_�X�y�L�����J���[
		);

	// �`��
	void Render();

};


