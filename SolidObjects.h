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

// ############### �Z�p�`�N���X�^�� �A�C�e���p ###############
// class HexagonCrystal
// ���̂����݂��Ȃ����ߍ폜


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
	int    m_iGridLinesNum;  // �\����������̌��i�����������j

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

	// ���_�𒼐ڕҏW���邽�߂̃C���^�[�t�F�X
	Vector3D& editVertexes( int VectorIndex );
	int m_iMaxVectorNum;

	// �`��
	void Render();

};

// ############### ���ʐ}�`�̊��N���X ###############
class BasePlaneFigure
{
protected:
	VERTEX3D* m_pVertex; // �|���S���W����ێ�
	int m_iPolygonNum;   // �|���S����

	Vector3D* m_pRawVertexPos; // VERTEX������ɒ��S�ʒu�V�t�g�ɑΉ����邽�߃I���W�i���̒��_�ʒu����ێ�����
	Vector3D* m_pRawVertexNrm; // �I���W�i���̒��_�@���x�N�g������ێ����� �� �I�u�W�F�N�g�̉�]���ɁA�@��

	MATERIALPARAM m_Material;         // �}�e���A���p�����[�^
	MATERIALPARAM m_MaterialDefault;  // �}�e���A���p�����[�^�i�f�t�H���g�j
	
	// Z�o�b�t�@���g�p���邩�̃t���O
	bool m_bUseZBuffer;

public:
	// �R���X�g���N�^
	BasePlaneFigure( 
		int       PolygonNum,       // �|���S���i�O�p�`�j��
		COLOR_F   EmissivColor,     // �I�u�W�F�N�g�̐F�i���Ȕ����F�j
		bool      UseZBuffer        // Z�o�b�t�@���g�p���邩�H
		);

	// ���_��ҏW <- �@�������͒����ł��Ȃ��̂Œ��ӁB����̓����_�����O�����Ȕ����ɂ���̂ŕs�s���͂Ȃ���...
	Vector3D* editVertexes(){ return m_pRawVertexPos; };

	// �S���_�����擾
	int getAllVertexNum(){ return 3*m_iPolygonNum; };

	// m_pVertex �� m_pRawVertexPos �ŏ�����
	void resetVertex();

	// �`��
	void Render();

	// �^����ꂽ�s�� Mat �� m_pVertex ��ϊ�
	void MatTransVertex( const MATRIX &Mat );


};


// ############### ���ʗւ��� ###############
class PlaneRing : public BasePlaneFigure
{
private:
	Vector3D  m_vCntPos;       // ���̒��S�ʒu 

public:
	// �R���X�g���N�^
	PlaneRing( 
		double    Radius,           // �ւ̔��a�i���a�j
		double    Width,			// �ւ̕� 
		int       DivNum            // ������
		);

	// ���S�ʒu�̐ݒ�i�X�V�j
	void setCenterPos( Vector3D CntPos );

};

// ############### ���ʏ�ʌ`�}�`�E�h��Ԃ� ###############

// BasePlaneFigure �̌p���ɏ�������...

class PlaneConvexFill : public BasePlaneFigure
{
public:
	// �R���X�g���N�^
	PlaneConvexFill( 
		Vector2D  *pVertex2D,       // �ʌ`�}�`�̗֊s���_�̔z��
		int       DivNum,           // �������i���_���j
		COLOR_F   EmissivColor      // �I�u�W�F�N�g�̐F�i���Ȕ����F�j
		);

};



// ############### ���ւ��� ###############
class LineRing
{
private:
	int       m_iVertexNum;    // ���_��
	VECTOR*   m_pVECTORs;      // ���_�W����ێ�
	Vector3D* m_pRawVertexPos; // VERTEX������ɒ��S�ʒu�V�t�g�ɑΉ����邽�߃I���W�i���̒��_�ʒu����ێ�����

	unsigned int m_iColor;  // ���̐F

public:

	// �R���X�g���N�^
	LineRing( 
		double    Radius,           // �ւ̔��a
		int       DivNum,           // ������
		unsigned int Color          // ���̐F
		);

	// ���_��ҏW
	Vector3D* editVertexes(){ return m_pRawVertexPos; };

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

// ############### ���R�H ###############
// �R�����H
class LineFreeCycle
{
private:
	int       m_iVertexNum;    // ���_��
	VECTOR*   m_pVECTORs;      // ���_�W����ێ�
	Vector3D* m_pRawVertexPos; // VERTEX������ɒ��S�ʒu�V�t�g�ɑΉ����邽�߃I���W�i���̒��_�ʒu����ێ�����

	unsigned int m_iColor;  // ���̐F

public:

	// �R���X�g���N�^
	LineFreeCycle( 
		int       DivNum,           // ������
		unsigned int Color          // ���̐F
		);

	// ���_��ҏW
	Vector3D* editVertexes(){ return m_pRawVertexPos; };

	// m_pVertex �� m_pRawVertexPos �ŏ�����
	void resetVertex();

	// �`��
	void Render();

	// �^����ꂽ�s�� Mat �� m_pVertex ��ϊ�
	void MatTransVertex( const MATRIX &Mat );
};

// ############### �R�c��� ###############
class Arrow3D
{
private:
	VERTEX3D* m_pVertex; // �|���S���W����ێ�
	int m_iPolygonNum;   // �|���S����

	MATERIALPARAM m_Material;         // �}�e���A���p�����[�^
	MATERIALPARAM m_MaterialDefault;  // �}�e���A���p�����[�^�i�f�t�H���g�j

	// ���̃p�����[�^
	double m_dArwHight; // ���̖�̕����̍���
	double m_dArwWidth; // ���̖�̕����̕�
	double m_dBdyWidth; // ���̖_�̕����̕�

public:

	// �R���X�g���N�^
	Arrow3D( 
		double    ArrowHight, 
		double    ArrowWidth, 
		double    BodyWidth, 
		COLOR_F   EmissivColor
		);

	// �`��
	void Render( Vector3D vBgn, Vector3D vEnd, Vector3D vUpper );

};

// ###############################################
// ��{�}�`�N���X
// * �~���A�~���A�����̂Ȃǂ̊ȒP�Ȑ}�`�B
//   �t���[�����[�N���`�������N���X�B
//   ������p�����Ċe�}�`�N���X���`����悤�ɂ���B
// ###############################################
class BasicFig_Base
{
protected:
	//==========�����o
	int m_iPolygonNum;     // �|���S����
	int m_iVectexNum;      // ���_��

	VERTEX3D* m_pVertex;   // �|���S���W����ێ�
	Vector3D* m_pOrgVertx; // �I���W�i���̒��_�ʒu����ێ�����
	Vector3D* m_pOrgNorms; // �I���W�i���̒��_�@���x�N�g������ێ����� �� �I�u�W�F�N�g�̉�]���ɖ@�������̒Ǐ]���K�v�ɂȂ邽�߁B

	MATRIX m_MTransMat;    // ���W�ϊ��s��i���\�b�h setMatrix �Őݒ肷��j


public:
	//==========���\�b�h

	// �R���X�g���N�^
	// - �}�`�̓����ɍ��킹�āA�l�X�ȏ��������@�ɂȂ�B
	//--------------------------
	// * ���N���X�̃R���X�g���N�^�̃p�����[�^
	//   - �F
	//   - ���_�� or �|���S����
	// * ���N���X�̃R���X�g���N�^���ōs�����ƁB
	//   - �|�C���^�̃������m�ہB�B
	//   - m_pVertex �֐F�i�{���j�̐ݒ�B
	//   - ��̓I�Ȍ`��̐ݒ�́A���N���X���p�������}�`�N���X�̃R���X�g���N�^���ōs�����ƁB
	BasicFig_Base( 
		int       PolygonNum,       // �|���S���i�O�p�`�j��
		COLOR_U8  DifColor,         // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor          // ���̒��_�X�y�L�����J���[
		);

	// ��{�ό`�̎��{
	// - �I���W�i����vertex�ɓK�p�����i=�}�`�̃f�t�H���g�̎p���j
	//--------------------------
	// * m_pOrgVertx/m_pOrgNorms�ɑ΂��ĕό`�����{
	// * m_pVertex ���X�V�isetMatrix �����s�j�B
	void setDefault( const MATRIX &Mat ) ;

	// ���W�ϊ��s��̐ݒ�
	// - �`�掞�ɓK�p�����ό`
	// - �ʂ̕ϊ���K�p����ƑO�̕ϊ��̓��Z�b�g�����B
	//--------------------------
	// * m_pOrgVertx/m_pOrgNorms �� m_pVertex �֑���B
	// * m_pVertex �� TransMat �̍��W�ϊ������{
	void setMatrix( const MATRIX &Mat ) ;

	// �`��
	//--------------------------
	// * �`��
	void Render() ;


};

// �~���}�`�N���X�i����{�}�`�N���X���p���j
class BasicFig_Column : public BasicFig_Base
{
	// �ʂɎ������o�͂Ȃ��H

public:
	//==========���\�b�h

	// �R���X�g���N�^
	// - �}�`�̋�̓I�Ȍ`��̐ݒ���s��
	BasicFig_Column( 
		Vector3D  CenterPos,       // ��ʂ̉~�`�̒��S�ʒu
		double    Radius,          // ���a
		double    Hight,           // �~���̍���
		int       DivNum,          // �������i����ʂ̉~�`�̕������j
		COLOR_U8  DifColor,        // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor         // ���̒��_�X�y�L�����J���[
		);

};

// �~���}�`�N���X�i����{�}�`�N���X���p���j
class BasicFig_Cone : public BasicFig_Base
{
	// �ʂɎ������o�͂Ȃ��H

public:
	//==========���\�b�h

	// �R���X�g���N�^
	// - �}�`�̋�̓I�Ȍ`��̐ݒ���s��
	BasicFig_Cone( 
		Vector3D  CenterPos,       // ��ʂ̉~�`�̒��S�ʒu
		double    Radius,          // ���a
		double    Hight,           // �~���̍���
		int       DivNum,          // �������i����ʂ̉~�`�̕������j
		COLOR_U8  DifColor,        // ���_�f�B�t���[�Y�J���[
		COLOR_U8  SpcColor         // ���̒��_�X�y�L�����J���[
		);

};

// ���b�`�ȍ��W�����f��
class CoordinateAxisModel
{
private:
	// �����o

	// �\���v�f�ƂȂ��{�}�`
	// * �C���X�^���X�����R���X�g���N�^���Ŏ��s���邽�߁A�|�C���^�Ŏ��悤�ɂ��Ă���B
	BasicFig_Cone   *m_pAxisX_Tip; // �w���̐�[�i���j����
	BasicFig_Column *m_pAxisX_Bar; // �w���̖_����
	BasicFig_Cone   *m_pAxisY_Tip; // �x���̐�[�i���j����
	BasicFig_Column *m_pAxisY_Bar; // �x���̖_����
	BasicFig_Cone   *m_pAxisZ_Tip; // �y���̐�[�i���j����
	BasicFig_Column *m_pAxisZ_Bar; // �y���̖_����

public:

	// �R���X�g���N�^
	//--------------------------
	CoordinateAxisModel(
		double thickness,		// ���W���̑����i�����傫���j
		double axis_x_length,	// �w���̒���
		double axis_y_length,	// �x���̒���
		double axis_z_length	// �y���̒���
		);

	// ���W�ϊ��s��̐ݒ�
	//--------------------------
	void setMatrix( const MATRIX &Mat ) ;

	// �`��
	//--------------------------
	void Render() ;

};