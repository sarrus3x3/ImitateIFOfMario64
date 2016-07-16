#include "Scenery.h"


// ##### �ؐ��q���n�̃W�I���} #######################################

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

// ##### ���V����_���W���� #######################################

// �R���X�g���N�^
FloatingDungeon::FloatingDungeon( double CellSize, double CellThickness, char *FileName ) : 
	m_dCellSize( CellSize ), 
	m_dCellThickness( CellThickness )
{
	// �|���S�����ivector�Œ�`�j
	// �������͂ǂ̂悤�Ɋm�ۂ��邩�H
	
	// �}�b�v����`�r�b�g�}�b�v��ǂݍ���
	int BitMapHandle = LoadSoftImage( FileName );

	// �}�b�v����`�r�b�g�}�b�v�̃T�C�Y���擾�i�c���j
	int W, H;
	GetSoftImageSize( BitMapHandle, &W, &H ) ;

	int TotalHorizSidesNum = W*(H+1);  // �^�C���̕ӂŐ����Ȃ��̂̑���
	int TotalSidesNum = TotalHorizSidesNum + H*(W+1); // �^�C���̕ӂ̑���

	// �^�C���̏d�Ȃ�ӂ��������邽�ߕӂ̃��X�g�𐶐�
	// �ʂ̌�������������K�v������
	vector<int> SidesList( TotalSidesNum, 0 );

	// �}�b�v����`�r�b�g�}�b�v����́B
	int  r, g, b, a ;
	for( int i=0; i<W; i++)
	{
		for( int j=0; j<H; j++)
		{
			GetPixelSoftImage( BitMapHandle, j, i, &r, &g, &b, &a ) ;

			// r==0 �łȂ���΁Acell�L��Ɣ��f
			if( r==0 )
			{
				// �ʂ̃|���S���𐶐�
				Vector3D TL, BL, TR, BR, Nrm;

				// �S���_���`
				TR= Vector3D( m_dCellSize*(i+1), 0, m_dCellSize*(j+1) );
				TL= Vector3D( m_dCellSize*(i  ), 0, m_dCellSize*(j+1) );
				BR= Vector3D( m_dCellSize*(i+1), 0, m_dCellSize*(j  ) );
				BL= Vector3D( m_dCellSize*(i  ), 0, m_dCellSize*(j  ) );

				// �@���x�N�g�����`
				Nrm = Vector3D( 0, 1, 0 );

				// ##### m_pRawVertexPos
				// �O�p�`�P
				m_RawVertexPosList.push_back( TL );
				m_RawVertexPosList.push_back( TR );
				m_RawVertexPosList.push_back( BL );

				// �O�p�`�Q
				m_RawVertexPosList.push_back( TR );
				m_RawVertexPosList.push_back( BL );
				m_RawVertexPosList.push_back( BR );

				// ##### m_pRawVertexNrm
				// �O�p�`�P
				m_RawVertexNrmList.push_back( Nrm );
				m_RawVertexNrmList.push_back( Nrm );
				m_RawVertexNrmList.push_back( Nrm );

				// �O�p�`�Q
				m_RawVertexNrmList.push_back( Nrm );
				m_RawVertexNrmList.push_back( Nrm );
				m_RawVertexNrmList.push_back( Nrm );

				// ##### SideList �ɕӂ̊i�[�@���@�d�Ȃ�ӂ�����

				// �^�C���̂S�ӂ̃G���R�[�h
				int T = W*j     + i;
				int B = W*(j+1) + i;
				int L = H*i     + j + TotalHorizSidesNum;
				int R = H*(i+1) + j + TotalHorizSidesNum;
				
				// �r���I�_���a���Ƃ邱�Ƃŏd������ӂ���������B
				// �ӂ̌����������Ɋi�[����
				SidesList[T] =  1 * (int)(SidesList[T]==0);
				SidesList[B] = -1 * (int)(SidesList[B]==0);
				SidesList[L] =  1 * (int)(SidesList[L]==0);
				SidesList[R] = -1 * (int)(SidesList[R]==0);

			}
		}
	}

	// �ӂ̃|���S���𐶐�
	for( int i=0; i<TotalSidesNum; i++ )
	{
		if( SidesList[i] != 0 )
		{ // �ӂ�����
			// �C���f�b�N�X����ӂ̃p�����[�^���f�R�[�h����
			Vector3D BS, BG, ES, EG, Nrm;
			if( i < TotalHorizSidesNum )
			{ // �����ȕ�
				int y = i/W;
				int x = i%W;

				// �ʒu : (x,y) - (x+1,y)
				// ���� : (0,1) * SidesList[i]

				// �S���_���`
				BS = Vector3D( m_dCellSize* x   ,                 0, m_dCellSize*y );
				BG = Vector3D( m_dCellSize* x   , -m_dCellThickness, m_dCellSize*y );
				ES = Vector3D( m_dCellSize*(x+1),                 0, m_dCellSize*y );
				EG = Vector3D( m_dCellSize*(x+1), -m_dCellThickness, m_dCellSize*y );

				// �@���x�N�g�����`
				Nrm = Vector3D( 0, 0, 1 );
				Nrm *= SidesList[i];
			}
			else
			{ // �����ȕ�
				int k = i - TotalHorizSidesNum;
				int x = k/H;
				int y = k%H;

				// �ʒu : (x,y) - (x,y+1)
				// ���� : (1,0) * SidesList[i]

				// �S���_���`
				BS = Vector3D( m_dCellSize* x   ,                 0, m_dCellSize* y    );
				BG = Vector3D( m_dCellSize* x   , -m_dCellThickness, m_dCellSize* y    );
				ES = Vector3D( m_dCellSize* x   ,                 0, m_dCellSize*(y+1) );
				EG = Vector3D( m_dCellSize* x   , -m_dCellThickness, m_dCellSize*(y+1) );

				// �@���x�N�g�����`
				Nrm = Vector3D( 1, 0, 0 );
				Nrm *= SidesList[i];

			}

			// �ӂ̃|���S���𐶐�

			// ##### m_pRawVertexPos
			// �O�p�`�P
			m_RawVertexPosList.push_back( BS );
			m_RawVertexPosList.push_back( ES );
			m_RawVertexPosList.push_back( BG );

			// �O�p�`�Q
			m_RawVertexPosList.push_back( ES );
			m_RawVertexPosList.push_back( BG );
			m_RawVertexPosList.push_back( EG );

			// ##### m_pRawVertexNrm
			// �O�p�`�P
			m_RawVertexNrmList.push_back( Nrm );
			m_RawVertexNrmList.push_back( Nrm );
			m_RawVertexNrmList.push_back( Nrm );

			// �O�p�`�Q
			m_RawVertexNrmList.push_back( Nrm );
			m_RawVertexNrmList.push_back( Nrm );
			m_RawVertexNrmList.push_back( Nrm );

		}
	}
	
	// �}�b�v����`�r�b�g�}�b�v�̏���j��
	DeleteSoftImage( BitMapHandle );

	// m_pVertex �̐���
	int VectexNum = m_RawVertexPosList.size();
	m_iPolygonNum = VectexNum/3;
	m_pVertex       = new VERTEX3D[VectexNum];

	// color �Ǝg��Ȃ��v�f��������
	COLOR_U8 DifColor = GetColorU8( 255, 255, 255, 0 );
	COLOR_U8 SpcColor = GetColorU8( 255, 255, 255, 0 );
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].dif = DifColor;
		m_pVertex[i].spc = SpcColor;
		m_pVertex[i].su  = 0.0f;
		m_pVertex[i].sv  = 0.0f;
	}

	resetVertex();


};

void FloatingDungeon::resetVertex()
{
	int VectexNum = m_iPolygonNum * 3;

	// m_pVertex.pos �� m_pRawVertexPos �ŏ㏑��
	for( int i=0; i<VectexNum; i++ )
	{
		m_pVertex[i].pos  = m_RawVertexPosList[i].toVECTOR();
		m_pVertex[i].norm = m_RawVertexNrmList[i].toVECTOR();
	}
};

// �`��
void FloatingDungeon::Render()
{
	DrawPolygon3D( m_pVertex, m_iPolygonNum, DX_NONE_GRAPH , FALSE ) ;
};


