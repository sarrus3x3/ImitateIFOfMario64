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
	// �B SavedViewMatrix      : �ۑ����Ă������r���[�s��ɐݒ�
	// �J�������[�h����enum
	enum CameraModeID
	{
		RotateCamOnGazePoint=0,
		TrackingMovingTarget=1,
		SetSavedViewMatrix=2
	};

	static const int m_iCameraModeMax = 3;

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
	// Update�֐��i�J�������[�h���ɗp�Ӂj
	void Update_RotateCamOnGazePoint( double timeslice );
	void Update_TrackingMovingTarget( double timeslice );
	void Update_SetSavedViewMatrix();

	// ### �⏕���\�b�h ###
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

	// ���݂̃J�������[�h���擾
	CameraModeID getCameraMode(){ return m_CurCamMode; };

	// �J�������[�h��ݒ�
	void setCameraMode( CameraModeID camID ){ m_CurCamMode=camID; };

	// �^�[�Q�b�g��ݒ�
	void setTarget( Vector3D tergetPos ){ m_TargetPos=tergetPos; };

	// ���݂̃J�������[�h���𕶎���ŕԋp
	std::string getCurCamModeName(); // CamMode�ǉ��v

	// �r���[�s��
	MATRIX m_MViewLocal; // Entity�̃��[�J�����W�ł̃r���[�s��
	MATRIX m_MViewWorld; // ���[���h���W�ł̃r���[�s��

	// ##### �A�N�Z�T�� #####
	Vector3D getCamPos(){ return m_vFinalCamPos; }; // �J�����ʒu�̎擾
	double   getCamDistFrmFP(){ return m_dCamDistFrmFP; };

	// �J�����ʒu�����ʍ��W�n�Őݒ� 
	// �� dist�����܂������Ȃ��̂Œ��ӁI
	//    getMouseWheelInput �̒���static�ϐ��Ō��܂��Ă��܂��Ă���I
	//    �im_dCamDistFrmFP��work�ϐ��ɂȂ��Ă���\�������Ƃ����Ȃ��ƒ���Ȃ��B�j
	void     setCamPosSpherical ( double tilt, double head, double dist )
	{ 
		m_dTilt=tilt;
		m_dHead=head;
		m_dCamDistFrmFP=dist; 
	}; 

	// �J�����ʒu�𒼌����W�n�Őݒ�
	// �� Update�ƕ��p�ł��Ȃ��̂Œ��� 
	//void     setCamFinalPos ( Vector3D Pos ){ m_vFinalCamPos = Pos; }; 
	// �� ���́A�J������Ԃ��r���[�s��Ő��䂵�Ă��邽�ߎg�p�ł��Ȃ�

	// #### �J�����̃r���[�ʒu�̕ۑ��������@�\

	// �J�����̃r���[�s���ۑ�����ϐ�
	MATRIX m_MSaveViewWorld;

	// �J�����̃r���[�s���ۑ��i�O���t�@�C���ւ̏��o�����ϐ��ɕێ��j
	void saveViewMatrix( MATRIX mViewMat ); 
	void saveViewMatrix(){ saveViewMatrix( m_MViewWorld ); }; // �����Ȃ��Ō��݂̃r���[�s��im_MViewWorld�j��ۑ�����B

	// �J�����̃r���[�s����O���t�@�C������ϐ��ɓǂݍ���
	int loadViewMatrix();


};
