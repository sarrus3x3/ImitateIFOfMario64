#include <vector>

#include "DxLib.h"

// ��{���C�u����
#include "MyUtilities.h"
#include "Vector3D.h"
#include "Vector2D.h"

#define DBG_MEASURE_TIME

// �A�j���[�V�����̕������Z�i���̖т�h�炷�Ƃ��j�֘A�N���X

// ���֐߃{�[���̃��[�V��������N���X
class MultiJointBoneMotionControl
{
public:
	int m_iModelHandle;          // ���f���̃n���h��

	int m_iJointSize;            // ���֐ߐ�
	std::vector<int> m_lFrameIndexList;  // �����𐧌䂵�����{�[���i�t���[���j�̔ԍ����X�g�i���[�g�֐߂���A�֐߂̐ڑ����ɂȂ��Ă��邱�Ɓj

	MATRIX*   m_pDefaultTransMatList;    // ���X�g�F�Ώۃ{�[���̃f�t�H���g�̍��W�ϊ��s��i�����������s�ړ�������0�ɂ������́j
	Vector3D* m_pDefaultBoneDirList;     // ���X�g�F�Ώۃ{�[���̃f�t�H���g�̌���

	static const VECTOR OrignVec; // ���_�x�N�g��

public:
	// �R���X�g���N�^
	MultiJointBoneMotionControl( 
		int    ModelHandle,
		std::vector<int> FrameIndexList,
		int    JointSize   // ���[�g�֐߂�JointSize�Ɋ܂߂Ȃ��Bi.e. �������Z�Őݒ肷��JointSize����-1�������̂�ݒ肷�邱�ƁB
		);

	void setBoneDirPosMain( 
		Vector3D SpecifyBonePosForWorldCoord,  // �{�[�����������������̃x�N�g�� v�i���[���h���W�j
		Vector3D BoneRootPosForWorldCoord,     // ���{�[���̕t�����ʒu p �i���[���h���W�j
		int      TargetFrameIndex,             // ���{�[���̃t���[���ԍ�
		int      ParentFrameIndex,             // �e�{�[���̃t���[���ԍ�
		MATRIX   &DefaultTransMat,             // �Ώۃ{�[����[�f�t�H���g]�̍��W�ϊ��s��i�����������s�ړ�������0�ɂ������́j
		Vector3D DefltBoneDirForPrntLocCoord   // �e�t���[���̃��[�J�����W�ɂ�����[�f�t�H���g]�̎��{�[���̕��� u 
		);

	// ������JointPosList�i���[���h���W�j�ɏ]���A�{�[���i�t���[���j��ݒ肷��
	// JointPosList �̃T�C�Y�́Am_iJointSize�B 
	void setBoneAsJointList( Vector3D *pJointPosList ); 

	// ���W�ϊ��s����f�t�H���g�ɖ߂�
	void Reset();

// ############ �f�o�b�N�p�@�\ ############
	void DBG_RenewModelHandles( int newModelHandle ){ m_iModelHandle=newModelHandle; };

};


// �����󑽏d�A���΂˃��f���̕������Z
class StraightMultiConnectedSpringModel
{
private:
	static const Vector3D m_vVertiDir;  // ���������x�N�g���istatic const�j

	// ���f���n���h��
	int m_iModelHandle;

	// #### �p�����[�^
	static double m_dTimeElapsedPhys; // �������Z�̃^�C���X�e�b�v�B�Q�[���̃^�C���X�e�b�v�ƓƗ��ɐݒ�\

	double m_dMass;    // ���_�̏d�ʁi�Œ�j
	double m_dViscous; // �S����R�i�Œ�j
	double m_dGravity; // �d�͒萔
	double m_dSpring;  // �o�l�萔�i�x�[�X�j
	double m_dNaturalFactor; // ���R���Z�o�����ł̕␳�W��

	int m_iJointSize; // ���_��
	std::vector<int>    m_iFrameIndexList;   // �t���[��index���X�g�i�܂��A��d�����ɂȂ邯�ǂ������B�債���T�C�Y����Ȃ����B�j
	std::vector<double> m_dSpringList;       // �o�l�萔�̃��X�g
	std::vector<double> m_dNaturalList;      // �o�l�̎��R���̃��X�g

	// #### �����p���\�b�h

	// i�Ԗڂ̎��_�ɓ����͂��v�Z�i�� m �Ŋ��������́j
	Vector3D ForceWorksToMassPoint( int i, Vector3D *pPosList, Vector3D *pVecList );

	void UpdateMain(double time_elapsed);         // �������Z���{�i��t�������j
	void UpdateByEuler(double time_elapsed); 	  // �I�C���[�@�ɂ�鐔�l�v�Z
	void UpdateByRungeKutta(double time_elapsed); // �����Q�N�b�^�@�i�S���j�ɂ�鐔�l�v�Z

public:
	// ### �����ϐ�
	Vector3D *m_pPosList;  // ���_�̈ʒu���X�g ��[0]�̓��[�g�֐߂ł���ʒu�Œ�i�t���[���ʒu����擾�����j
	Vector3D *m_pVelList;  // ���_�̑��x���X�g

	// ### �A�N�Z�T
	int    getJointSize(){ return m_iJointSize; };
	void   setTimeElapsedPhys( double time_elapsed ){ m_dTimeElapsedPhys = time_elapsed; };
	double getTimeElapsedPhys( ){ return m_dTimeElapsedPhys; };

public:

	// �R���X�g���N�^
	// �t���[�����X�gindex��n�����
	StraightMultiConnectedSpringModel(
		int    ModelHandle,
		std::vector<int> FrameIndexList,
		int    JointSize,
		double Mass,
		double Viscous,
		double Gravity,
		double Spring,
		double NaturalFactor
		);

	// time_elapsed��m_dTimeElapsedPhys����v�Z���āA�K�v�ȉ񐔂���UpdateMain�����s����
	// �߂�l�FUpdateMain ���s������
	int  Update(double time_elapsed); 

	void DebugRender(); // �`��i�f�o�b�O�p�j

	void setJointPosAsFrame(); // �W���C���g�ʒu���f�t�H���g�l�ɖ߂�

// ############ �f�o�b�N�p�@�\ ############
	void DBG_RenewModelHandles( int newModelHandle ){ m_iModelHandle=newModelHandle; };

#ifdef DBG_MEASURE_TIME
	// UpdateMain 1�񂠂���̕��Ϗ������ԁi�b�j
	double DBG_m_dAverageTimeForUpdate;
	MeasureFPS DBG_m_MeasureFPS;
#endif

};

