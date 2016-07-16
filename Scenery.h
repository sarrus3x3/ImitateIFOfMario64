#include<vector>

#include "SolidObjects.h"

// ##### �ؐ��q���n�̃W�I���} #######################################
class JupiterSystemDiorama
{
private:
	// #### TextureSphere3D�^�̘f���I�u�W�F�N�g

	// �ؐ��I�u�W�F�N�g
	TextureSphere3D *m_pPrimalyObj;

	// �q���I�u�W�F�N�g�i�z��j
	std::vector<TextureSphere3D> m_cSatelliteObjList;

	// �q���O���̐��փI�u�W�F�N�g�i�z��j
	std::vector<LineRing> m_cSatelliteOrbitalObjList;

	// #### �����p�����[�^ ####

	// �启���a
	double m_dPrimalyStarRadius;

	// �启���]����(rotation period)
	double m_dPrimalyRotationPeriod;

public:
	// �q����
	static const int m_iSatelliteNum = 4;

	// �q�����i�񋓌^�j
	enum SatelliteID
	{
		SATELLITE_IO       = 0,
		SATELLITE_EUROPA   = 1,
		SATELLITE_GANYMEDE = 2,
		SATELLITE_CALLISTO = 3
	};

private:
	// �q�����a�i�z��j
	double *m_pSatelliteStarRadius;

	// �q���O�����a�i�z��j
	double *m_pSatelliteOrbitalRadius;

	// �q�����]�����i�P�ʁF���j�i�z��j
	double *m_pSatelliteRotationPeriod;

	// �q�����]�����i�P�ʁF���j�i�z��j
	double *m_pSatelliteOrbitalPeriod;

	// �启���]���x
	double m_dPrimalyRotationSpeed;

	// �q�����]���x�i���W�A���^���j�i�z��j
	double *m_pSatelliteRotationSpeed;

	// �q�����]���x�i���W�A���^���j�i�z��j
	double *m_pSatelliteOrbitalSpeed;


	// #### ��ԃp�����[�^ ####

	// �启���]�p
	double m_dPrimalyRotateAngle;

	// �q�����]�p�i�z��j
	std::vector<double> m_dSatelliteRotateAngleList;

	// �q�����]�p�i�z��j
	std::vector<double> m_dSatelliteOrbitalAngleList;

public:
	// ���[���h���W�ւ̕ϊ��s��
	MATRIX m_mLocalToWorldMatrix;

	// #### ���\�b�h ####

	// �R�X�g���N�^
	JupiterSystemDiorama();

	// �q���ʒu�X�V
	void Update( double TimeElapse );

	// �I�u�W�F�N�g�̒��_���ʒu�v�Z
	//   Render()�ŕ`�悷��O�ɁA
	//   �O���ʒu�Ȃǂ̌n��Ԃƕϊ��s�񂩂�e�I�u�W�F�N�g�̒��_�ʒu���v�Z����B
	//   Render()�ƕ������Ă���̂́A�n��ԁE�ϊ��s��̍X�V�Ȃ����ɏ����y�����������߁B
	void setVertex();

	// �`��
	void Render();

	// ���e�X�g���Ă݂��ق��������ł��傤�B

};

// ##### ���V����_���W���� #######################################
// * ����̔w�i��1�Z�b�g�Ƃ��āB�i���̃_���W�������ۂ����̂͏���ł��B�n�ʂ���O��Ă���������͂��܂���j
// * �R���X�g���N�^�Ń}�b�v��`bitmap��ǂݍ��݁A�_���W�����̌`���F������B
// * ��������̃f���p��"�ۑ�"�̋����ݒ�ł���悤�Ɂi�g���@�\�I�j

class FloatingDungeon
{
public:
	// �_���W�������\������Z���i�^�C���j�̑���
	int m_iCellNum;

	// �_���W�����̃v���p�e�B
	double m_dCellSize;      // * �Z���i�^�C���j�̃T�C�Y�i1�ӂ̒����A�^�C���͐����`�j
	double m_dCellThickness; // * �Z���i�^�C���j�̌���


	// �|���S���֌W
	VERTEX3D* m_pVertex; // �|���S���W����ێ�
	int m_iPolygonNum;   // �|���S����
	vector<Vector3D> m_RawVertexPosList; // �I���W�i���̒��_�ʒu����ێ�����
	vector<Vector3D> m_RawVertexNrmList; // �I���W�i���̒��_�@���x�N�g������ێ����� �� �I�u�W�F�N�g�̉�]���ɁA�@��


	// #### ���\�b�h ####

	// �R���X�g���N�^
	// * �Z���i�^�C���j�̃T�C�Y�i1�ӂ̒����A�^�C���͐����`�j
	// * �Z���i�^�C���j�̌���
	// * �}�b�v��`�r�b�g�}�b�v�̃p�X�i�n���h���ł͂Ȃ��A�p�X�j
	FloatingDungeon( double CellSize, double CellThickness, char *FileName );
	
	// m_pVertex �� m_pRawVertexPos �ŏ�����
	void resetVertex();

	// �`��
	void Render();
};
