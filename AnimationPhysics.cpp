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

void MultiJointBoneMotionControl::setBoneAsJointList( Vector3D *pJointPosList )
{
	// ���[�g�֐߂̃��[���h���W�i�Œ�j���擾����B�i�e�t���[��index����Ȃ����H�j
	//Vector3D RootJointPosForWorldCoord = MV1GetFramePosition( m_iModelHandle, m_lFrameIndexList[1] ); // ���[�g�֐߂̔ԍ���1

	for( int i=0; i<m_iJointSize; i++ )
	{
		setBoneDirPosMain(
			pJointPosList[i+1],
			pJointPosList[i],
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
double StraightMultiConnectedSpringModel::m_dTimeElapsedPhys = 0.01; // 0.0001 ���炢�ɂ���ƈ���

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
	m_dNaturalList( JointSize )
	//m_vPosList( JointSize ),
	//m_vVelList( JointSize, Vector3D(0,0,0) )
{
	// m_pPosList, m_pVelList �̃������m��
	m_pPosList = new Vector3D[m_iJointSize];
	m_pVelList = new Vector3D[m_iJointSize];

	// m_vPosList �̏�����
	for( int i=0; i<m_iJointSize; i++ ){
		// �t���[���ʒu�ŏ�����
		m_pPosList[i] = MV1GetFramePosition( m_iModelHandle, m_iFrameIndexList[i+1] );
	}

	for( int i=0; i<(m_iJointSize-1); i++ ){
		// m_dNaturalList �̏�����
		// m_dNaturalFactor �ŁA�d�͂ŐL�т����Ƀf�t�H���g�̔��̌`��ɋ߂Â��悤�Ƀ`���[������B
		m_dNaturalList[i] = m_dNaturalFactor * (m_pPosList[i+1]-m_pPosList[i]).len();

		// m_dSpringList �̏������i�o�l�̎��R���ɔ��j
		m_dSpringList[i] = m_dNaturalList[i] * m_dSpring;
	}

};

// �������Z���{
void StraightMultiConnectedSpringModel::UpdateMain(double time_elapsed)
{
	UpdateByEuler(time_elapsed);
	//UpdateByRungeKutta(time_elapsed);
};

// �I�C���[�@�ɂ�鐔�l�v�Z
void StraightMultiConnectedSpringModel::UpdateByEuler(double time_elapsed)
{
	// �VPos,Vel�̃������m��
	static Vector3D *pNewPosList = new Vector3D[m_iJointSize];
	static Vector3D *pNewVelList = new Vector3D[m_iJointSize];

	// r0 �̈ʒu���A�t���[���ʒu�ɍX�V
	m_pPosList[0]  = MV1GetFramePosition( m_iModelHandle, m_iFrameIndexList[1] );
	pNewPosList[0] = m_pPosList[0];

	// �^���������ɏ]���A���x�E�ʒu���X�V
	for( int i=1; i<m_iJointSize; i++ )
	{
		// ���x���X�V
		Vector3D Accel = ForceWorksToMassPoint(i, m_pPosList, m_pVelList ) / m_dMass;

		pNewVelList[i] = time_elapsed * Accel + m_pVelList[i];
		//if( pNewVelList[i].sqlen() > 100.0*100.0 ) pNewVelList[i] = 100 * pNewVelList[i].normalize();

		// �ʒu���X�V
		pNewPosList[i] = time_elapsed * m_pVelList[i] + m_pPosList[i];
		//pNewPosList[i] = time_elapsed * pNewVelList[i] + m_pPosList[i];

	}

	// �V���x�E�ʒu��L����
	swap( m_pPosList, pNewPosList );
	swap( m_pVelList, pNewVelList );
}; 

// �����Q�N�b�^�@�i�S���j�ɂ�鐔�l�v�Z
void StraightMultiConnectedSpringModel::UpdateByRungeKutta(double time_elapsed)
{
	// �����Q�N�b�^�@���z K1-K4 �L���p�̃������m��
	static Vector3D *pCurK = new Vector3D[m_iJointSize];
	static Vector3D *pCurL = new Vector3D[m_iJointSize];

	// ���X�e�b�v�̏d�ݕt�����ρi�̓r���v�Z���ʁj
	static Vector3D *pSumK = new Vector3D[m_iJointSize];
	static Vector3D *pSumL = new Vector3D[m_iJointSize];

	// �VPos,Vel�̃������m��
	static Vector3D *pNewPosList = new Vector3D[m_iJointSize];
	static Vector3D *pNewVelList = new Vector3D[m_iJointSize];

	// r0 �̈ʒu���A�t���[���ʒu�ɍX�V
	m_pPosList[0]  = MV1GetFramePosition( m_iModelHandle, m_iFrameIndexList[1] );
	pNewPosList[0] = m_pPosList[0];

	// �Ȃ��ʒu���ꂪ�N����̂��낤�H

	// #### K1�̌v�Z
	for( int i=1; i<m_iJointSize; i++ )
	{
		pCurK[i] = ForceWorksToMassPoint(i, m_pPosList, m_pVelList ) / m_dMass;
		pCurL[i] = m_pVelList[i];
		pSumK[i] = pCurK[i]; // �������̂��߉��Z�ł͂Ȃ��㏑��
		pSumL[i] = pCurL[i];
	}

	// #### K2�̌v�Z

	// Pos, Vec �� 0.5 * Dt* K �V�t�g�������z����쐬
	for( int i=1; i<m_iJointSize; i++ )
	{
		pNewPosList[i] = m_pPosList[i] + 0.5 * time_elapsed * pCurL[i];
		pNewVelList[i] = m_pVelList[i] + 0.5 * time_elapsed * pCurK[i];
	}

	// �b��Pos,Vec����K2���v�Z
	for( int i=1; i<m_iJointSize; i++ )
	{
		pCurK[i] = ForceWorksToMassPoint(i, pNewPosList, pNewVelList ) / m_dMass;
		pCurL[i] = pNewVelList[i];
		pSumK[i] += 2*pCurK[i];
		pSumL[i] += 2*pCurL[i];
	}

	// #### K3�̌v�Z

	// Pos, Vec �� 0.5 * Dt* K �V�t�g�������z����쐬
	for( int i=1; i<m_iJointSize; i++ )
	{
		pNewPosList[i] = m_pPosList[i] + 0.5 * time_elapsed * pCurL[i];
		pNewVelList[i] = m_pVelList[i] + 0.5 * time_elapsed * pCurK[i];
	}

	// �b��Pos,Vec����K2���v�Z
	for( int i=1; i<m_iJointSize; i++ )
	{
		pCurK[i] = ForceWorksToMassPoint(i, pNewPosList, pNewVelList ) / m_dMass;
		pCurL[i] = pNewVelList[i];
		pSumK[i] += 2*pCurK[i];
		pSumL[i] += 2*pCurL[i];
	}

	// #### K4�̌v�Z

	// Pos, Vec �� Dt * K �V�t�g�������z����쐬
	for( int i=1; i<m_iJointSize; i++ )
	{
		pNewPosList[i] = m_pPosList[i] + time_elapsed * pCurL[i];
		pNewVelList[i] = m_pVelList[i] + time_elapsed * pCurK[i];
	}

	// �b��Pos,Vec����K2���v�Z
	for( int i=1; i<m_iJointSize; i++ )
	{
		pCurK[i] = ForceWorksToMassPoint(i, pNewPosList, pNewVelList ) / m_dMass;
		pCurL[i] = pNewVelList[i];
		pSumK[i] += pCurK[i];
		pSumL[i] += pCurL[i];
	}

	// #### ���̃X�e�b�v�̈ʒu�E���x�����߂�
	for( int i=1; i<m_iJointSize; i++ )
	{
		m_pVelList[i] += time_elapsed * (1.0/6.0) * pSumK[i];
		m_pPosList[i] += time_elapsed * (1.0/6.0) * pSumL[i];
	}
};

// i�Ԗڂ̎��_�ɓ����͂��v�Z�i�� m �Ŋ��������́j
Vector3D StraightMultiConnectedSpringModel::ForceWorksToMassPoint
	( int i, Vector3D *pPosList, Vector3D *pVecList )
{
	Vector3D Force(0,0,0);

	// ���_i�`���_i+1�Ԃ̃o�l����󂯂��
	if( i+1<m_iJointSize )
	{
		// �o�l�̐L��
		double Growth = (pPosList[i+1]-pPosList[i]).len() - m_dNaturalList[i]; // �o�l�̐L��
		Force += m_dSpringList[i] * Growth * (pPosList[i+1]-pPosList[i]).normalize();
	}

	// ���_i-1�`���_i�Ԃ̃o�l����󂯂��
	double Growth = (pPosList[i]-pPosList[i-1]).len() - m_dNaturalList[i-1]; // �o�l�̐L��
	Force += -m_dSpringList[i-1] * Growth * (pPosList[i]-pPosList[i-1]).normalize();
		
	// �S����R
	Force += -m_dViscous * pVecList[i];

	// ���ʂŊ���
	//Force /= m_dMass;

	// �d��
	Force += m_dMass * m_dGravity * m_vVertiDir;

	return Force;
};

// time_elapsed��m_dTimeElapsedPhys����v�Z���āA�K�v�ȉ񐔂���UpdateMain�����s����
int StraightMultiConnectedSpringModel::Update(double time_elapsed)
{
	static double fr = 0; // �^�C���X�e�b�v�̒[��

	int counter=0; // UpdateMain ���s�񐔂̃J�E���^
	
	fr += time_elapsed;

#ifdef DBG_MEASURE_TIME
	// ���ԑ��菈��
	LONGLONG BeginTime = GetNowHiPerformanceCount();
#endif

	while(1)
	{
		if( fr<m_dTimeElapsedPhys ) break;
		fr -= m_dTimeElapsedPhys;
		UpdateMain(m_dTimeElapsedPhys);
		counter++;
	}

#ifdef DBG_MEASURE_TIME
	// ���ԑ��菈��
	LONGLONG EndTime = GetNowHiPerformanceCount();
	double Average = (double)(EndTime-BeginTime);
	//Average *= 0.000001;
	Average /= counter;
	DBG_m_dAverageTimeForUpdate = DBG_m_MeasureFPS.Update(Average);
#endif

	return counter;
};

// �W���C���g�ʒu���f�t�H���g�l�ɖ߂�
void StraightMultiConnectedSpringModel::setJointPosAsFrame()
{
	// m_vPosList �̏�����
	for( int i=0; i<m_iJointSize; i++ ){
		// �t���[���ʒu�ŏ�����
		m_pPosList[i] = MV1GetFramePosition( m_iModelHandle, m_iFrameIndexList[i+1] );

		// ���x��0�ɁB
		m_pVelList[i] = Vector3D( 0,0,0 );

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
			m_pPosList[i].toVECTOR(), 
			1.0f, 
			8, 
			GetColor( 255,   0, 0 ),   // �ԐF 
			GetColor( 255, 255, 255 ), 
			TRUE ) ; 

		// �{�[���i�W���C���g�ڑ��j�����`��
		if( i+1<m_iJointSize )
		{
			DrawLine3D( 
				m_pPosList[i].toVECTOR(), 
				m_pPosList[i+1].toVECTOR(), 
				GetColor( 255, 0, 0 ) ); // �ԐF
		}

	}


	

};
