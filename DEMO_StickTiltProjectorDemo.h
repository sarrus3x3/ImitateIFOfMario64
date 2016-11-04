//#include <functional>

#include "DxLib.h"

#include "..\\ImitateIFOfMario64\\Vector3D.h"
#include "..\\ImitateIFOfMario64\\Vector2D.h"
#include "..\\ImitateIFOfMario64\\SolidObjects.h"


// 2016/09/15
// �X�e�B�b�N�̌X�����L�����N�^���ʂɓ��e����
// �܊pclass�������񂾂���A�\�[�X�ƃw�b�_��ʃt�@�C���ɂ��悤���H

// 2016/09/17 14:36
// �N���X������....


// ��`
// �R���X�g���N�^�Ƀp�����[�^��^���āA��`�̒��_�z��𐶐����邾��
class SectorFigure2D
{
public:
	// ���_�z��
	Vector2D *m_pVertexes;

	// ���_��
	int m_iVertexNum;

	// �`�悷��Ƃ��̔{��
	static double m_dRenderScale;

	// �`�悷�鎞�̐F
	int m_iRenderColor;

	// �`�悷�鎞�̉�]��
	double m_dRotation;

	// ### �R���X�g���N�^
	// �@ �J�n�p�ƏI���p�i���W�A���j��^����ꂽ��A���_���S�ɐ�`�𐶐�����
	SectorFigure2D( double Radius, double StartAng, double EndAng, int DivNum );

	// �A �J�n�p�ƏI���p���x�N�g���ŗ^���邱�Ƃ��ł���
	SectorFigure2D( double Radius, Vector2D StartVec, Vector2D EndVec, int DivNum );

	// �f�X�g���N�^
	~SectorFigure2D();

	// ���ʃL�����o�X��ɕ`�悷��@�\����������
	void RenderOnCanvas( Vector2D RenderPos );

	// �`��p�|���S���̐���
	void GeneratePolygons();

private:

	// �|���S������
	int m_iPolygonNum;

	// �`��p�̃|���S���W���i�|���S���O�p�`�̒��_�����ԂɈ�̔z��ɋl�߂Ă���j
	Vector2D* m_pPolyVertex;


};

// �X�N���[�����ʃN���X
	// �`��I�u�W�F�N�g
		// �����i�g�j
		// �X�N���[����̃L�����N�^�ʒu
		// �X�e�B�b�N�̉��͈͉~
	// Render
	// ���W�ϊ��s��i���[�J�������[���h�j
class ScreenPlane
{
private:
	// #### �`��I�u�W�F�N�g ####

	// �����i�g�j
	LineSegment m_oFrameT;  // ��g
	LineSegment m_oFrameB;	// ���g
	LineSegment m_oFrameL;	// ���g
	LineSegment m_oFrameR;	// �E�g

	LineSegment m_oAxisX; // x��
	LineSegment m_oAxisY; // y��
	
	LineRing    m_oStickRing; // �X�e�B�b�N�̉��͈͉~

	// �X�N���[����̃L�����N�^�ʒu
	Vector3D    m_vCntStickRing;

public:
	// ���[���h���W�ւ̕ϊ��s��
	MATRIX m_mLocalToWorldMatrix;
	
	// #### ���\�b�h ####

	// �R���X�g���N�^
	ScreenPlane( double Width, double Hight, double Radius );

	// �I�u�W�F�N�g�̒��_���v�Z
	void setVertex();

	// �`��
	void Render();

	// �X�N���[����̃L�����N�^�ʒu�ݒ�i�����[�J�����W�j
	void setCntStickRing( Vector3D cnt ){ m_vCntStickRing = cnt; };
};


class StickTiltProjectorDemo
{
public:

	// ���f���̃X�e�[�^�X
	Vector3D m_vModelCamPos;          // �����p�J�����̈ʒu
	Vector3D m_vModelCamGazePoint;    // �����p�J�����̏œ_
	double   m_dModelCamNearClipDist; // �����p�J�����̃N���b�v����
	Vector3D m_vPlayerPos;            // �i���삷��j�L�����N�^�̈ʒu

	Vector3D m_vStickTiltOnScreen; // �X�e�B�b�N�̌X���̕����ix-z���ʏ�j

	static double SectorRadius;
	static double AngeSize;

	// ���������̃X�e�B�b�N�X���̎n�_
	Vector2D m_vVertiStickTiltDirBgn;

	// ���������̃X�e�B�b�N�X���̏I�_
	Vector2D m_vVertiStickTiltDirEnd;

	// ���������̃X�e�B�b�N�X���̎n�_
	Vector2D m_vHorizStickTiltDirBgn;

	// ���������̃X�e�B�b�N�X���̏I�_
	Vector2D m_vHorizStickTiltDirEnd;

	// 3D���f�����
	int m_iModelCamHandle;

	// �X�N���[���I�u�W�F�N�g
	ScreenPlane m_ScPlane;

	// �X�N���[����ɕ`�悷��O���b�h�͈̔�
	double m_dGridRangeOnScreen;

	// �R���X�g���N�^
	StickTiltProjectorDemo(
		Vector3D vModelCamPos,          // �����p�J�����̈ʒu
		Vector3D vModelCamGazePoint,    // �����p�J�����̏œ_
		double   dModelCamNearClipDist, // �����p�J�����̃N���b�v����
		Vector3D vPlayerPos             // �i���삷��j�L�����N�^�̈ʒu
		);

	// �J�����̃r���[�s����擾����
	MATRIX getViewMatrix(){ return mModelCamViewMat; }; // �r���[�s��̓J�����̃��[�J�����W�s��̋t�s��ł���B

	// �v�Z���ꂽEntity�̐i�s�������擾����
	// �� UpdateGrids() �̒��Ōv�Z����邱�Ƃɒ���
	Vector3D getEntityMoveDir(){ return (m_vArrowEndEntPln - m_vArrowBgnEntPln).normalize(); };

	// ##### Update �n �֐� #####

	// ���f���X�e�[�^�X������W�ϊ��s����X�V
	void UpdateTransMats();

	// ���f���X�e�[�^�X����􉽓I���W�ʒu���X�V
	void UpdateGeoPoss();

	// �X�N���[����̉~�ƁA�����xz���ʏ�ɓ��e�����}�`�̔z����X�V����
	void UpdateScreenCircleProjection();

	// �I���W�i���̃X�e�B�b�N�̌X���̐�` �̏�����
	void UpdateSectorOrgStickTiltDir();
	
	// �i�X�e�B�b�N�̌X������j�v�Z���ꂽ�L�����N�^�̐i�s�����i��}�������`�j�̏�����
	void UpdateSectorCharactrMoveDir();

	// ���o�p�O���b�h�̏�����
	void UpdateGrids();


	// ##### Draw �n �֐� #####

	// �J�������f����`��
	void RenderModelCamera();

	// �X�N���[����`��
	void RenderScreen();

	// �e��⏕����`��
	void RenderAuxiliaryLines();

	// �@ �X�N���[����̃X�e�B�b�N�̋O�Ղ�`��
	void RenderStickTrackOnScreen();

	// �A �@��xz���ʏ�ɓ��e�����}�`�i�ȉ~�j
	void RenderStickTrackProjection();

	// �B �A�𓊉e���ʏ�Ő��K���������~�̕`��
	void RenderStickTrackProjectionNormalize();

	// #### �ω��ʂ̐�^��\��

	// ���f�����ɕ`�悷��R�����}�`�A�f�B�X�v���C�p��`���f���A�̕`��
	void RenderSectors();

	// �I���W�i���̃X�e�B�b�N�̌X���̕ω��ʂ�\����^��`��
	void RenderOrgStickTiltDirVariation( Vector2D RenderPos );

	// �i�X�e�B�b�N�̌X���j�v�Z���ꂽ�L�����N�^�̐i�s�����̕ω��ʂ�\����^��`��
	void RenderCharactrMoveDirVariation( Vector2D RenderPos );

	// #### �ό`�̃O���b�h��\��

	// �X�N���[����̃O���b�h��`��
	void RenderGridOnScreen();

	// Entity���ʂɓ��e�����O���b�h��`��
	void RenderGridGrandPrj();

	// �X�N���[����ɁA�X�e�B�b�N�̌X�������ɖ���`��
	//void RenderArrowToStickTiltOnScreen();
	// �� RenderGridOnScreen �̒��ŕ`�悷��

	// �L�����N�^���ʏ�ɁA�X�e�B�b�N�̌X�������ɖ���`��
	//void RenderArrowToStickTiltOnEntityPlane();
	// �� RenderGridGrandPrj �̒��ŕ`�悷��

private:
	// �X�e�B�b�N�̋O�Ղ̕�����
	static int m_iStickTrackDivNum;

	// �@ �X�N���[����̃X�e�B�b�N�̋O��
	LineFreeCycle m_StickTrackOnScreen;

	// �A �@��xz���ʏ�ɓ��e�����}�`�i�ȉ~�j
	LineFreeCycle m_StickTrackProjection;

	// �B �A�𓊉e���ʏ�Ő��K���������~
	LineFreeCycle m_StickTrackProjectionNormalize;


	// ��`�̕�����
	static int m_iSectorDivNum;


	// ### ��^

	// ## ���f�����ɕ`�悷��R�����}�`�A�f�B�X�v���C�p��`���f��

	// ���������̐�`�i�X�N���[����j
	PlaneConvexFill* m_pVertiSectorOnScreen;

	// ���������̐�`�ixz���ʂɓ��e�j
	PlaneConvexFill* m_pVertiSectorProjection;

	// ���������̐�`�i�X�N���[����j
	PlaneConvexFill* m_pHorizSectorOnScreen;

	// ���������̐�`�ixz���ʂɓ��e�j
	PlaneConvexFill* m_pHorizSectorProjection;

	// ## �I���W�i���̃X�e�B�b�N�̌X��

	// ���������̐�`
	SectorFigure2D* m_pVertiSectorOrgStickTiltDir;

	// ���������̐�`
	SectorFigure2D* m_pHorizSectorOrgStickTiltDir;

	// ## �i�X�e�B�b�N�̌X���j�v�Z���ꂽ�L�����N�^�̐i�s����

	// ���������̐�`
	SectorFigure2D* m_pVertiSectorCharactrMoveDir;

	// ���������̐�`
	SectorFigure2D* m_pHorizSectorCharactrMoveDir;


	// ### �O���b�h

	// �I���W�i���̃O���b�h
	GroundGrid* m_pGridOriginal;

	// �X�N���[����̃O���b�h
	GroundGrid* m_pGridOnScreen;

	// Entity���ʂɓ��e�����O���b�h
	GroundGrid* m_pGridGrandPrj;

	// �X�N���[����ɕ`�悷��A�X�e�B�b�N�̌X�������̖��
	Arrow3D* m_pArrowToStickTiltOnScreen;

	// �L�����N�^���ʏ�ɕ`�悷��A�X�e�B�b�N�̌X�������̖��
	Arrow3D* m_pArrowToStickTiltOnEntityPlane;

	// ���̏��iArrow3D�Ɉ�̉������Ă����΂悩�����j
	Vector3D m_vArrowBgnScreen, m_vArrowEndScreen;
	Vector3D m_vArrowBgnEntPln, m_vArrowEndEntPln;

	// #### �e����W�ϊ��s��

	// �����p�J�����̃��[�J�������[���h���W�ϊ��s��
	MATRIX mModelCamLocalToWorld;

	// �����p�J�����̃��[�J�������[���h���W�ϊ��s��̋t�s��
	MATRIX mModelCamWorldToLocal;

	// �X�N���[���̃��[�J�������[���h���W�ϊ��s��
	MATRIX mScreenLocalToWorld;

	// �X�N���[���̃��[�J�������[���h���W�ϊ��s��̋t�s��
	MATRIX mScreenWorldToLocal;

	// �����p�J�����̊�{�ϊ��s��
	MATRIX MatConf;

	// �X�N���[�����[�J�����W���L�����N�^���ʂփX�N���[���������ۂ��Ȃ��獄�̕ϊ�����s��icalcStickTiltPos_RigidTrans�Ŏg�p�j
	MATRIX mScreenPosRigidTrans;

	// �����p�J�����̃r���[�s��i�����p�J�������f���Ǝ��ۂ̃J�����̕������Ⴄ���߁AmModelCamWorldToLocal�͂��̂܂܎g���Ȃ��j
	MATRIX mModelCamViewMat;


	// #### �e��􉽓I�ʒu

	// �X�N���[����̃L�����N�^�ʒu�i�X�N���[�����[�J�����W�ɂ�����j
	Vector3D m_vEntiPosAsScreenForScLocal;

	// �X�N���[����̃L�����N�^�ʒu�i���[���h���W�ɂ�����j
	Vector3D m_vEntiPosAsScreenForWorld;

	// �X�N���[����́A�X�N���[���̒��S�ʒu�i���[���h���W�ɂ�����j
	Vector3D m_vScreenCntPosOnScreen;

	// �X�N���[���̒��S�ʒu���L�����N�^���ʂɓ��e�����ʒu
	Vector3D m_vScreenCntPosOnEntityPlane;

	// �֐��̊�
	//std::function<void(Vector3D,Vector3D&,Vector3D&)> calcStickTiltPosProjection;

	// �֐��|�C���^
	void (StickTiltProjectorDemo::*fpCalcStickTiltPosProjection) (
		Vector3D vStickTiltPosForScLocal, // [IN] �X�N���[�����[�J�����W�ɂ�����X�e�B�b�N�̌X�������ʒu
		Vector3D &vStickTiltPosForWorld,  // [OUT] ���[���h���W�ɂ�����X�e�B�b�N�̌X�������ʒu
		Vector3D &vStickTiltPosProjection // [OUT] xz���ʏ�ɓ��e�����X�e�B�b�N�̌X�������ʒu�i���[���h���W�ɂ�����j
		);

	// �X�e�B�b�N�̌X���̕����ix-z���ʏ�j�� ���[��x-z���ʏ�֓��e�������W�֕ϊ�����֐���p�ӂ���

	// * �ˉe�ϊ� HomogTrans �i�z���O���t�B�ϊ��j
	//   ���ŁB��ʃX�N���[���ɓ��e�����X�e�B�b�N�̌X��������Entity�̕��ʂɓ��e����
	void calcStickTiltPos_HomogTrans(
		Vector3D vStickTiltPosForScLocal, // [IN] �X�N���[�����[�J�����W�ɂ�����X�e�B�b�N�̌X�������ʒu
		Vector3D &vStickTiltPosForWorld,  // [OUT] ���[���h���W�ɂ�����X�e�B�b�N�̌X�������ʒu
		Vector3D &vStickTiltPosProjection // [OUT] xz���ʏ�ɓ��e�����X�e�B�b�N�̌X�������ʒu�i���[���h���W�ɂ�����j
		);

	// * ���̕ϊ� RigidTrans
	//   ���ǔŁB������̌��������������ڂƍ����A�p�x��ۑ����ĕϊ�����B
	void calcStickTiltPos_RigidTrans(
		Vector3D vStickTiltPosForScLocal, // [IN] �X�N���[�����[�J�����W�ɂ�����X�e�B�b�N�̌X�������ʒu
		Vector3D &vStickTiltPosForWorld,  // [OUT] ���[���h���W�ɂ�����X�e�B�b�N�̌X�������ʒu
		Vector3D &vStickTiltPosProjection // [OUT] xz���ʏ�ɓ��e�����X�e�B�b�N�̌X�������ʒu�i���[���h���W�ɂ�����j
		);


};

