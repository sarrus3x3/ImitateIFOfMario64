#include <string>

// ################## �J��������N���X #######################

class CameraWorkManager
{
private:
	CameraWorkManager();

	// �R�s�[�R���X�g���N�^�A������Z�q�� private ��
	CameraWorkManager(const CameraWorkManager&);
	CameraWorkManager& operator=(const CameraWorkManager&);

public:
	// ### �J�������[�h ###
	// �@ RotateCamOnGazePoint : �����_�𒆐S�ɃJ��������]
	// �A TrackingMovingTarget : �^�[�Q�b�g��ǔ�����
	// �J�������[�h����enum
	enum CameraModeID
	{
		RotateCamOnGazePoint=0,
		TrackingMovingTarget=1
	};

	static const int m_iCameraModeMax = 2;

private:
	// ### �����o ###
	CameraModeID m_CurCamMode;

	bool m_bGazingAtTargetPos; // �J������ m_TargetPos �̕������݂邩�̃t���O

	double m_dTilt; // ���_�̌X�� 
	double m_dHead; // ���_�̕����p

	Vector3D m_vFinalCamDist;   // �i�ŏI�I�ɐݒ肳���j�J�����̌��������x�N�g��
	Vector3D m_vFinalTargetPos; // �i�ŏI�I�ɐݒ肳���j�J�����̒����_
	Vector3D m_vFinalCamPos;    // �i�ŏI�I�ɐݒ肳���j�J�����̈ʒu

	Vector3D m_TargetPos;       // �ݒ�\�ȃJ�����̒����_
	double   m_TrgtHight;       // ��ʑ̂̍����i�����݂�悤�Ɂj

	// �@ RotateCamOnGazePoint �Ŏg�p
	double m_dCamDistFrmFP;  // �J�����̃L�����N�^�[����̋���

	// �A TrackingMovingTarget �Ŏg�p
	double m_dCamHight;          // �J�����̍���
	double m_dDstCamToTrgtDef;   // �J�����Ɣ�ʑ̂̋����i�^�ォ�猩���j
	double m_dSqDstCamToTrgtDef; // DstCamToTrgtDef * DstCamToTrgtDef

	// ### ���\�b�h ###
	// �J�������[�h����Update�֐���p��
	void Update_RotateCamOnGazePoint( double timeslice );
	void Update_TrackingMovingTarget( double timeslice );

	// �⏕���\�b�h
	// �@ RotateCamOnGazePoint �p
	void getKeyInput();
	void getMouseWheelInput( double timeslice );

public:
	// �V���O���g��
	static CameraWorkManager* Instance();

	// �J������Ԃ��X�V
	void Update( double timeslice ); // CamMode�ǉ��v

	// DX���C�u�����̃J�����Z�b�g�֐������s
	void setCamera();

	// �J�������[�h��ݒ�
	void setCameraMode( CameraModeID camID ){ m_CurCamMode=camID; };

	// �^�[�Q�b�g��ݒ�
	void setTarget( Vector3D tergetPos ){ m_TargetPos=tergetPos; };

	// ���݂̃J�������[�h���𕶎���ŕԋp
	std::string getCurCamModeName(); // CamMode�ǉ��v

	// �J�����ʒu�̎擾
	Vector3D getCamPos(){ return m_vFinalCamPos; };
};
