#include "AnimationPhysics.h"

// ################# MultiJointBoneMotionControl #################

// ������static const�̏�����
const VECTOR MultiJointBoneMotionControl::OrignVec = { 0, 0, 0 };

// #### �R���X�g���N�^ ####
MultiJointBoneMotionControl::MultiJointBoneMotionControl( 
		int    ModelHandle,
		std::vector<int> FrameIndexList,
		int    JointSize
		) :
	m_iModelHandle(ModelHandle),
	m_lFrameIndexList( FrameIndexList ),
	m_iJointSize( JointSize )
{
	// �R���e�i��p��
	m_pDefaultTransMatList = new MATRIX[m_iJointSize];
	m_pDefaultBoneDirList  = new Vector3D[m_iJointSize];

	for( int i=0; i<m_iJointSize; i++)
	{
		// �Ώۃ{�[���̃f�t�H���g�̍��W�ϊ��s�� ��ۑ�
		m_pDefaultTransMatList[i] = MV1GetFrameLocalMatrix( m_iModelHandle, m_lFrameIndexList[i+1] );

		// �Ώۃ{�[���̃f�t�H���g�̌��� ���v�Z���ĕۑ�
		MATRIX ChildLocalMat;
		ChildLocalMat = MV1GetFrameLocalMatrix( m_iModelHandle, m_lFrameIndexList[i+2] ); 
		m_pDefaultBoneDirList[i] = VTransform( OrignVec, ChildLocalMat );

		// �Ώۃ{�[���̃f�t�H���g�̍��W�ϊ��s�� �̕��s�ړ������� 0 �ɂ���B
		for( int j=0; j<3; j++ ){ m_pDefaultTransMatList[i].m[3][j] = 0.0; }

	}

};

void MultiJointBoneMotionControl::setBoneAsJointList( std::vector<Vector3D> &JointPosList )
{
	// ���[�g�֐߂̃��[���h���W�i�Œ�j���擾����B�i�e�t���[��index����Ȃ����H�j
	//Vector3D RootJointPosForWorldCoord = MV1GetFramePosition( m_iModelHandle, m_lFrameIndexList[1] ); // ���[�g�֐߂̔ԍ���1

	for( int i=0; i<m_iJointSize; i++ )
	{
		/*
		Vector3D BoneRootPosForWorldCoord;
		if( i>0 ) BoneRootPosForWorldCoord = JointPosList[i-1];
		else      BoneRootPosForWorldCoord = RootJointPosForWorldCoord;
		*/

		setBoneDirPosMain(
			JointPosList[i+1],
			JointPosList[i],
			m_lFrameIndexList[i+1],
			m_lFrameIndexList[i],
			m_pDefaultTransMatList[i],
			m_pDefaultBoneDirList[i]
		);
	}

};

void MultiJointBoneMotionControl::setBoneDirPosMain( 
		Vector3D SpecifyBonePosForWorldCoord,  // �{�[�����������������̃x�N�g�� v�i���[���h���W�j
		Vector3D BoneRootPosForWorldCoord,     // ���{�[���̕t�����ʒu p �i���[���h���W�j
		int      TargetFrameIndex,             // ���{�[���̃t���[���ԍ�
		int      ParentFrameIndex,             // �e�{�[���̃t���[���ԍ�
		MATRIX   &DefaultTransMat,             // �Ώۃ{�[����[�f�t�H���g]�̍��W�ϊ��s��i�����������s�ړ�������0�ɂ������́j
		Vector3D DefltBoneDirForPrntLocCoord   // �e�t���[���̃��[�J�����W�ɂ�����[�f�t�H���g]�̎��{�[���̕��� u 
	)
{
	static MATRIX TmpMat; // ��Ɨp�s��

	// �@ v �� p ���A�t���[�����䃍�[�J�����W�ɂ�����{�[�����������������x�N�g�� v'(=SpecifyDirLoc) �����߂�
	
	// v ����� p ���t���[�����䃍�[�J�����W�ɕϊ�
	TmpMat = MV1GetFrameLocalWorldMatrix ( m_iModelHandle, ParentFrameIndex ); // �e�{�[���̃��[�J�������[���g�ϊ��s�� �擾
	TmpMat = MInverse( TmpMat ) ; // �t�s����v�Z
	Vector3D SpecifyBonePosForPrntLocCoord = VTransform( SpecifyBonePosForWorldCoord.toVECTOR(), TmpMat ); 
	Vector3D BoneRootPosForPrntLocCoord    = VTransform( BoneRootPosForWorldCoord.toVECTOR()   , TmpMat ); 

	// �{�[�����������������x�N�g�� v' ���v�Z
	VECTOR SpecifyDirLoc = 
		(SpecifyBonePosForPrntLocCoord-BoneRootPosForPrntLocCoord).normalize().toVECTOR();

	// �A M�~u = v �ƂȂ�A��]�s�� R ���v�Z����B
	MATRIX RotMat = MGetRotVec2( DefltBoneDirForPrntLocCoord.toVECTOR(), SpecifyDirLoc );

	// �B ���t���[���̕t�������ʒu p �ɂȂ�悤�ɁAM �� p �������s�ړ����� M' �𐶐�
	//MATRIX TransMatShiftP = MAdd( 
	//	MGetTranslate( BoneRootPosForPrntLocCoord.toVECTOR() ), DefaultTransMat );
	MATRIX TransMatShiftP = DefaultTransMat;
	TransMatShiftP.m[3][0] = (float)BoneRootPosForPrntLocCoord.x;
	TransMatShiftP.m[3][1] = (float)BoneRootPosForPrntLocCoord.y;
	TransMatShiftP.m[3][2] = (float)BoneRootPosForPrntLocCoord.z;


	// �C �t���[���̐V�������W�ϊ��s��Ƃ��āAR�~M' ���Z�b�g����B
	TmpMat = MMult( RotMat, TransMatShiftP );
	MV1SetFrameUserLocalMatrix( m_iModelHandle, TargetFrameIndex, TmpMat );

};

void MultiJointBoneMotionControl::Reset()
{
	for( int i=0; i<m_iJointSize; i++)
	{
		// ���W�ϊ��s������ɖ߂�
		MV1ResetFrameUserLocalMatrix( m_iModelHandle, m_lFrameIndexList[i+1] );
	}
};

// ################# StraightMultiConnectedSpringModel #################


// ������static const�̏�����
const Vector3D StraightMultiConnectedSpringModel::m_vVertiDir( 0.0, -1.0, 0.0 );

// �R���X�g���N�^
//   �e��ϐ�����
StraightMultiConnectedSpringModel::StraightMultiConnectedSpringModel(
		int    ModelHandle,
		std::vector<int> FrameIndexList,
		int    JointSize,
		double Mass,
		double Viscous,
		double Gravity,
		double Spring,
		double NaturalFactor
		) :
	m_iModelHandle( ModelHandle ),
	m_iFrameIndexList( FrameIndexList ),
	m_iJointSize( JointSize ),
	m_dMass( Mass ),
	m_dViscous( Viscous ),
	m_dGravity( Gravity ),
	m_dSpring( Spring ),
	m_dNaturalFactor( NaturalFactor ),
	m_dSpringList( JointSize ),
	m_dNaturalList( JointSize ),
	m_vPosList( JointSize ),
	m_vVelList( JointSize, Vector3D(0,0,0) )
{
	// m_vPosList �̏�����
	for( int i=0; i<m_iJointSize; i++ ){
		// �t���[���ʒu�ŏ�����
		m_vPosList[i] = MV1GetFramePosition( m_iModelHandle, m_iFrameIndexList[i+1] );
	}

	for( int i=0; i<(m_iJointSize-1); i++ ){
		// m_dNaturalList �̏�����
		// m_dNaturalFactor �ŁA�d�͂ŐL�т����Ƀf�t�H���g�̔��̌`��ɋ߂Â��悤�Ƀ`���[������B
		m_dNaturalList[i] = m_dNaturalFactor * (m_vPosList[i+1]-m_vPosList[i]).len();

		// m_dSpringList �̏������i�o�l�̎��R���ɔ��j
		m_dSpringList[i] = m_dNaturalList[i] * m_dSpring;
	}

};

// �������Z���{
void StraightMultiConnectedSpringModel::Update(double time_elapsed)
{
	static std::vector<Vector3D> ForceList(m_iJointSize);

	// r0 �̈ʒu���A�t���[���ʒu�ɍX�V
	m_vPosList[0] = MV1GetFramePosition( m_iModelHandle, m_iFrameIndexList[1] );

	// �e���_�ɓ����� F_i ���v�Z
	for( int i=1; i<m_iJointSize; i++ )
	{
		// ������
		ForceList[i] = Vector3D( 0, 0, 0 );

		// ���_i�`���_i+1�Ԃ̃o�l����󂯂��
		if( i+1<m_iJointSize )
		{
			// �o�l�̐L��
			double Growth = (m_vPosList[i+1]-m_vPosList[i]).len() - m_dNaturalList[i]; // �o�l�̐L��
			ForceList[i] += m_dSpringList[i] * Growth * (m_vPosList[i+1]-m_vPosList[i]).normalize();
		}

		// ���_i∁E�`���_i�Ԃ̃o�l����󂯂��
		double Growth = (m_vPosList[i]-m_vPosList[i-1]).len() - m_dNaturalList[i-1]; // �o�l�̐L��
		ForceList[i] += -m_dSpringList[i-1] * Growth * (m_vPosList[i]-m_vPosList[i-1]).normalize();
		
		// �S����R
		ForceList[i] += -m_dViscous * m_vVelList[i];

		// �d��
		ForceList[i] += m_dMass * m_dGravity * m_vVertiDir;

	}

	// �^���������ɏ]���A���x�E�ʒu���X�V
	for( int i=1; i<m_iJointSize; i++ )
	{
		Vector3D Accel = ForceList[i] / m_dMass;

		// ���x���X�V
		m_vVelList[i] += time_elapsed * Accel;
		if( m_vVelList[i].sqlen() > 100.0*100.0 ) m_vVelList[i] = 100 * m_vVelList[i].normalize();

		// �ʒu���X�V
		m_vPosList[i] += m_vVelList[i] * time_elapsed;

	}

};

// �W���C���g�ʒu���f�t�H���g�l�ɖ߂�
void StraightMultiConnectedSpringModel::setJointPosAsFrame()
{
	// m_vPosList �̏�����
	for( int i=0; i<m_iJointSize; i++ ){
		// �t���[���ʒu�ŏ�����
		m_vPosList[i] = MV1GetFramePosition( m_iModelHandle, m_iFrameIndexList[i+1] );

		// ���x��0�ɁB
		m_vVelList[i] = Vector3D( 0,0,0 );

	}
};

void StraightMultiConnectedSpringModel::DebugRender()
{
	for( int i=0; i+1<m_iJointSize; i++ )
	{
		/*
		// �~���ŕ\������
		DrawCone3D( 
			m_vPosList[i+1].toVECTOR(), 
			m_vPosList[i].toVECTOR(), 
			0.5f,
			4,
			GetColor( 255,   0, 0 ),   // �ԐF 
			GetColor( 255, 255, 255 ), 
			TRUE ) ;
			*/

		// �W���C���g�ɋ���`�悵�A
		DrawSphere3D( 
			m_vPosList[i].toVECTOR(), 
			1.0f, 
			8, 
			GetColor( 255,   0, 0 ),   // �ԐF 
			GetColor( 255, 255, 255 ), 
			TRUE ) ; 

		// �{�[���i�W���C���g�ڑ��j�����`��
		if( i+1<m_iJointSize )
		{
			DrawLine3D( 
				m_vPosList[i].toVECTOR(), 
				m_vPosList[i+1].toVECTOR(), 
				GetColor( 255, 0, 0 ) ); // �ԐF
		}

	}


	

};
