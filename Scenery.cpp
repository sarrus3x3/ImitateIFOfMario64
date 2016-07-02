#include "Scenery.h"

// �R���X�g���N�^�i�p�`�p�`�ƃf�[�^����͂��Ă����j
JupiterSystemDiorama::JupiterSystemDiorama() :
	m_mLocalToWorldMatrix( MGetIdent() ), // �}�g���N�X�̏�����
	m_dSatelliteRotateAngleList( m_iSatelliteNum, 0 ),
	m_dSatelliteOrbitalAngleList( m_iSatelliteNum, 0 ),
	m_dPrimalyRotateAngle( 0 )
{
	// ######### �e��ϐ��̏��������� #########

	// #### �����p�����[�^ ####
	// Desktop\�f��\�f���e�N�X�`��\�ؐ��Ɖq��

	// �启���a
	m_dPrimalyStarRadius = 8.93650000;

	// �启���]����(rotation period)
	m_dPrimalyRotationPeriod = 0.4135;

	// �q�����a�i�z��j
	m_pSatelliteStarRadius = new double[m_iSatelliteNum];
	m_pSatelliteStarRadius[SATELLITE_IO      ] = 0.22766667;
	m_pSatelliteStarRadius[SATELLITE_EUROPA  ] = 0.19510000;
	m_pSatelliteStarRadius[SATELLITE_GANYMEDE] = 0.32890000;
	m_pSatelliteStarRadius[SATELLITE_CALLISTO] = 0.30128750;
	/*
	m_pSatelliteStarRadius[SATELLITE_IO      ] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_EUROPA  ] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_GANYMEDE] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_CALLISTO] = 3.0;
	*/

	// �q���O�����a�i�z��j
	m_pSatelliteOrbitalRadius = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalRadius[SATELLITE_IO      ] =  52.71250000;
	m_pSatelliteOrbitalRadius[SATELLITE_EUROPA  ] =  83.87925000;
	m_pSatelliteOrbitalRadius[SATELLITE_GANYMEDE] = 133.80150000;
	m_pSatelliteOrbitalRadius[SATELLITE_CALLISTO] = 235.33862500;

	// �q�����]�����i�z��j
	m_pSatelliteRotationPeriod = new double[m_iSatelliteNum];
	m_pSatelliteRotationPeriod[SATELLITE_IO      ] = 1.769137786;
	m_pSatelliteRotationPeriod[SATELLITE_EUROPA  ] = 3.551181041;
	m_pSatelliteRotationPeriod[SATELLITE_GANYMEDE] = 7.15455296;
	m_pSatelliteRotationPeriod[SATELLITE_CALLISTO] = 16.6890184;

	// �q�����]�����i�z��j�iorbital period�j
	m_pSatelliteOrbitalPeriod = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalPeriod[SATELLITE_IO      ] = 1.769137786;
	m_pSatelliteOrbitalPeriod[SATELLITE_EUROPA  ] = 3.551181041;
	m_pSatelliteOrbitalPeriod[SATELLITE_GANYMEDE] = 7.15455296;
	m_pSatelliteOrbitalPeriod[SATELLITE_CALLISTO] = 16.6890184;

	// #### TextureSphere3D�^�̘f���I�u�W�F�N�g

	// �ؐ��I�u�W�F�N�g
	m_pPrimalyObj = new TextureSphere3D(
				Vector3D( 0, 0, 0),
				m_dPrimalyStarRadius,
				true,
				32,
				32,
				LoadGraph( "JupiterSystemDiorama\\jupiter.jpg" ),
				TextureSphere3D::OBJECT_NOSPECULAR
			);

	// �q���I�u�W�F�N�g�i�z��j
	//   �z��ō\�z�i���m�ہj���āAvector�^�̃����o�ɑ��������@���Ƃ�
	//   ���������N���X�̔z��̏��������@
	//     http://brain.cc.kogakuin.ac.jp/~kanamaru/lecture/C++2/09/09-01.html
	TextureSphere3D Tmp[] = {
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_IO      ],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\0_io_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_EUROPA  ],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\1_europa_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_GANYMEDE],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\2_ganymede_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_CALLISTO],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\3_callisto_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			)
	};

	m_cSatelliteObjList.assign( &Tmp[0], &Tmp[m_iSatelliteNum] ); // vector�^�̃����o�ɑ��

	// �q���O���̐��փI�u�W�F�N�g�i�z��j
	LineRing Tmp2[] = {
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_IO      ],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_EUROPA  ],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_GANYMEDE],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_CALLISTO],
			32,
			GetColor( 255, 255, 255 )
			)
	};

	m_cSatelliteOrbitalObjList.assign( &Tmp2[0], &Tmp2[m_iSatelliteNum] ); // vector�^�̃����o�ɑ��

	// ### ���]���x�E���]���x���v�Z���Ċi�[
	
	// �启
	m_dPrimalyRotationSpeed = 2*DX_PI_F/(double)m_dPrimalyRotationPeriod;

	// �q��
	m_pSatelliteRotationSpeed = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalSpeed  = new double[m_iSatelliteNum];
	for( int i=0; i<m_iSatelliteNum; i++ )
	{
		m_pSatelliteRotationSpeed[i] = 2*DX_PI_F/(double)m_pSatelliteRotationPeriod[i];
		m_pSatelliteOrbitalSpeed[i]  = 2*DX_PI_F/(double)m_pSatelliteOrbitalPeriod[i];
	}

};

void JupiterSystemDiorama::setVertex()
{
	static MATRIX WorkMat;

	// #### ���]�p�A���]�p����A���[�J�����W�ɂ�����ʒu���v�Z

	m_pPrimalyObj->resetVertex();

	// �启 - ���]
	WorkMat = MGetRotY( (float)m_dPrimalyRotateAngle );

	// �启 - ���[���h���W�ɕϊ�
	WorkMat = MMult( WorkMat, m_mLocalToWorldMatrix );

	// Vertex�ɔ��f
	m_pPrimalyObj->MatTransVertex( WorkMat );


	// �q��
	for( int i=0; i<m_iSatelliteNum; i++ )
	{ 
		m_cSatelliteObjList[i].resetVertex();

		// ���]
		WorkMat = MGetRotY( (float)m_dSatelliteRotateAngleList[i] );

		// ���]
		Vector2D OrbitalPos2D( 1, 0 );
		OrbitalPos2D = m_pSatelliteOrbitalRadius[i] * OrbitalPos2D.rot(m_dSatelliteOrbitalAngleList[i]);
		//Vector3D OrbitalPos3D = OrbitalPos2D.toVector3D();
		
		WorkMat.m[3][0] = OrbitalPos2D.x;
		WorkMat.m[3][2] = OrbitalPos2D.y;

		// ���[���h���W�ɕϊ�
		WorkMat = MMult( WorkMat, m_mLocalToWorldMatrix ); // ���W�ϊ��s�����������Ƃ��́A��ɍ�p���������s��� MMult �̍����ɒu�����ƁB

		// Vertex�ɔ��f
		m_cSatelliteObjList[i].MatTransVertex( WorkMat );

		// �O����
		m_cSatelliteOrbitalObjList[i].resetVertex();
		m_cSatelliteOrbitalObjList[i].MatTransVertex( m_mLocalToWorldMatrix );

	}

};

void JupiterSystemDiorama::Update( double TimeElapse )
{
	// ���]�p�A���]�p���X�V
	m_dPrimalyRotateAngle += TimeElapse*m_dPrimalyRotationSpeed;

	for( int i=0; i<m_iSatelliteNum; i++ )
	{
		m_dSatelliteRotateAngleList[i]  += TimeElapse*m_pSatelliteRotationSpeed[i];
		m_dSatelliteOrbitalAngleList[i] -= TimeElapse*m_pSatelliteOrbitalSpeed[i];
	}
};

void JupiterSystemDiorama::Render()
{
	// �e�I�u�W�F�N�g�̕`�惁�\�b�h���ĂԂ���

	// �启
	m_pPrimalyObj->Render();

	// �q��
	for( int i=0; i<m_iSatelliteNum; i++ )
	{ 
		m_cSatelliteObjList[i].Render();

		// �O����\��
		m_cSatelliteOrbitalObjList[i].Render();

	}
	
};
