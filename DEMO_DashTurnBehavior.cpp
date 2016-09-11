
#include <cassert>

#include "DEMO_DashTurnBehavior.h"

// ######################## FlipbookPlayer Class ########################

// �R���X�g���N�^
FlipbookPlayer::FlipbookPlayer( 
	string GraphFileName, 
	int FrameNum , 
	int XSize, int YSize ):
		m_iMaxFrame( FrameNum )
{
	// ## m_hGraphArray ���i�[����

	// �b��z��
	int *pTmpArray = new int[FrameNum];

	// # �摜�̃T�C�Y���擾

	// �摜�̃T�C�Y���擾���邽�߈ꎞ�I�Ƀ��[�h
	int hTmpGraph = LoadGraph( GraphFileName.c_str() ); 
	
	// �摜�̃T�C�Y���擾
	int XBaseGrSize, YBaseGrSize;
	GetGraphSize( hTmpGraph, &XBaseGrSize, &YBaseGrSize );

	// ���[�h�����摜�f�[�^���J��
	DeleteGraph( hTmpGraph ); // �B���֐��H

	// # XNum, YNum ���v�Z
	int XDivNum, YDivNum;
	XDivNum = XBaseGrSize/XSize;
	YDivNum = YBaseGrSize/YSize;

	// # LoadDivGraph �ŉ摜�𕪊��ǂݍ���
	LoadDivGraph( GraphFileName.c_str(), FrameNum, XDivNum, YDivNum, XSize, YSize, pTmpArray );

	// # �z��^��vector<int>�^�� m_hGraphArray �Ɋi�[������
	m_hGraphArray.assign( &pTmpArray[0], &pTmpArray[FrameNum] );

	// �b��z����J��
	delete pTmpArray;

	// �i���\�ʓ|�����������ȁj

}

// �A�j���[�V�����̍Đ����s
void FlipbookPlayer::Play( Vector3D Pos, float Width, int Frame )
{
	// �n���ꂽFrane���ő�t���[�������傫����΁A�J��Ԃ��Ƃ��ĉ��߂���
	int PlayFrame = Frame % m_iMaxFrame;
	
	// DX���C�u�����̑g���݊֐��ōĐ����s
	DrawBillboard3D( 
		Pos.toVECTOR(), 
		0.5, 0.5,
		Width,
		0.0,
		m_hGraphArray[PlayFrame],
		TRUE );
}

// ######################## SampleEffectAnimManager Class ########################
// SampleEffectEntity�� one - to one �ŃA�j���[�V�������Ǘ����s��

// �ÓI�����o�̒�`
FlipbookPlayer* SampleEffectAnimManager::m_pFlipbookPlayerIns = NULL; 
int    SampleEffectAnimManager::m_iMaxFrameNum = 10; // �A�j���[�V�����̍ő�t���[����
string SampleEffectAnimManager::m_sGraphFileName = "promi_star_anime240.png";  // �A�j���[�V�����̉摜�f�[�^�̃p�X
int    SampleEffectAnimManager::m_iXSize = 240; // �A�j���[�V�����̂P�R�}��X�T�C�Y
int    SampleEffectAnimManager::m_iYSize = 240; // �A�j���[�V�����̂P�R�}��Y�T�C�Y

// �R���X�g���N�^
SampleEffectAnimManager::SampleEffectAnimManager() :
	m_iCurPlayFrame( 0 ),
	m_bEnd( false ),
	m_bRepeatAnimation( false ),
	m_fAnimWidth( 20.0 )
{
	// �������q�őS�ďo���Ă��܂����B
};

// �f�X�g���N�^
SampleEffectAnimManager::~SampleEffectAnimManager()
{
	// �f�X�g���N�^�ɏo�Ԃ͂Ȃ�
};

// static�����o�̏�����
void SampleEffectAnimManager::Initialize()
{
	// FlipbookPlayer �̃C���X�^���X��
	m_pFlipbookPlayerIns = new FlipbookPlayer( m_sGraphFileName, m_iMaxFrameNum, m_iXSize, m_iYSize );
}

// �A�j���[�V�����̕`��
void SampleEffectAnimManager::Render( Vector3D Pos )
{
	// �Đ��J�E���^�̃C���N��
	m_iCurPlayFrame++;

	if( !(m_iCurPlayFrame<m_iMaxFrameNum) )
	{

		if( m_bRepeatAnimation )
		{
			// ���[�v�Đ������Ȃ�A�ŏ��ɖ߂�
			m_iCurPlayFrame = 0;
		}
		else
		{
			// �����łȂ����m_iMaxFrameNum�𒴂��Ȃ��悤�ɂ���
			m_iCurPlayFrame = m_iMaxFrameNum-1;
			m_bEnd = true; // �Đ��I��������Am_bEnd�t���O���グ��
		}
	}

	// �Đ�
	m_pFlipbookPlayerIns->Play( Pos, m_fAnimWidth, m_iCurPlayFrame );
	
}

// �A�j���[�V�������Đ��I���������H
bool SampleEffectAnimManager::isEnd()
{
	return m_bEnd;
}

// ����m�F�ł���


// ######################## SampleItemAnimManager Class ########################
// SampleItemEntity�� one - to one �ŃA�j���[�V�������Ǘ����s��

// �ÓI�����o�̒�`
int SampleItemAnimManager::m_iModelHandleMaster = -1;  // MMD���f���̃n���h���̌��{
int SampleItemAnimManager::m_iAllModelHandleNum =  0;  // �S���f���n���h����

// �R���X�g���N�^
SampleItemAnimManager::SampleItemAnimManager() :
	//m_fModelScale( 3.0 ) // ���@�΂̏ꍇ�̃X�P�[��
	m_fModelScale( 250.0 ) // �R�C���̏ꍇ�̃X�P�[��
{
	// �����f�����̃J�E���^���C���N��
	m_iAllModelHandleNum++ ;

	// �R�s�[���f���n���h���̐���
	
	// �ŏ��̃C���X�^���X�̂݃I���W�i�����f�����g�p�B�Q�߈ȍ~�̓R�s�[���g�p
	if( m_iAllModelHandleNum <= 1 )
	{
		m_iModelHandle = m_iModelHandleMaster;
	}
	else
	{
		m_iModelHandle = MV1DuplicateModel( m_iModelHandleMaster );
	}

	// �֊s���̒���
	int MaterialNum, i ;
	MaterialNum = MV1GetMaterialNum( m_iModelHandle ) ;		// �}�e���A���̐����擾
	for( i = 0 ; i < MaterialNum ; i ++ )
	{
		// ### �֊s��������
		MV1SetMaterialOutLineDotWidth( m_iModelHandle, i, 0.0f ) ;
		MV1SetMaterialOutLineWidth( m_iModelHandle, i, 0.0f ) ;
	}

}

// �f�X�g���N�^
SampleItemAnimManager::~SampleItemAnimManager()
{
	// ���f���n���h���̊J��
	
	// �ꉞ�A�I���W�i���͏����Ȃ��悤�ɂ��Ă���
	if( m_iModelHandle != m_iModelHandleMaster )
	{
		MV1DeleteModel( m_iModelHandle );
	}

}

// static�����o�̏�����
void SampleItemAnimManager::Initialize()
{
	// MMD���f���̃n���h���̌��{�̐���
	//m_iModelHandleMaster = MV1LoadModel( "..\\mmd_model\\MagicStone\\�⎂�����@��_ver1.00.pmx" ) ;
	m_iModelHandleMaster = MV1LoadModel( "..\\mmd_model\\coin\\coin_ver1.0_masisi.mv1" ) ;
	assert( m_iModelHandleMaster >= 0 );

	// [����]
	// �֊s���𒲐��̏����͈����p����Ȃ��̂ŁA�R�s�[�����X�̃��f���n���h���ɑ΂��Ď��{����K�v������B

}

// �A�j���[�V�����̕`��
void SampleItemAnimManager::Render( Vector3D Pos, Vector3D Head )
{	// �ʒu�␳�p�̍��W�ϊ��s��𐶐�
	MATRIX TransMac;

	// ���f���T�C�Y�̒���
	TransMac = MGetScale( Vector3D( m_fModelScale, m_fModelScale, m_fModelScale ).toVECTOR() );
	
	// Entity�̌����ݒ�p�̍��W�ϊ��s��𐶐�
	// ���{���́Ahead�Aside�Auppper�̊�ꂩ������ݒ�s����v�Z�������B
	//   ���̏�����A�f�t�H���g��Entity�̌������l�����Ȃ��Ƃ����Ȃ��̂ŁA�Ƃ肠�����ۗ�
	Vector2D head2D = Head.toVector2D();               // heading��2D�ϊ�
	double headangle = atan2( head2D.x, head2D.y );    // heading�̉�]�p���擾
	TransMac = MMult( TransMac, MGetRotY( headangle+DX_PI ) );

	// Entity�̈ʒu�ݒ�p�̍��W�ϊ��s��𐶐�
	MATRIX SiftM = MGetIdent();
	SiftM.m[3][0] = (float)Pos.x;
	SiftM.m[3][1] = (float)Pos.y;
	SiftM.m[3][2] = (float)Pos.z;
	TransMac = MMult( TransMac, SiftM );

	// ���W�ϊ��s������f���ɓK�p
	MV1SetMatrix( m_iModelHandle, TransMac );

	// ���f���̕`��
    MV1DrawModel( m_iModelHandle ) ;

};

// ######################## SampleItemEntity Class ########################

// �ÓI�����o�̒�`
double   SampleItemEntity::m_dBoundingRadius = 8.0;
double   SampleItemEntity::m_dCharacterHight = 20.0;
double   SampleItemEntity::m_dSqBoundingRadius = SampleItemEntity::m_dBoundingRadius * SampleItemEntity::m_dBoundingRadius;
bool     SampleItemEntity::m_bExpBoundingRadius = false;
double   SampleItemEntity::m_dRotSpeed = DX_PI;
const Vector2D SampleItemEntity::m_vDefaultHeading2D = Vector2D( 1.0, 0 );

// �R���X�g���N�^
SampleItemEntity::SampleItemEntity( SampleGameWorld *world, Vector3D Pos ) : 
		SampleBaseGameEntity( Pos ),
		m_pWorld( world ),
		m_vHeading( Vector3D( 1.0, 0, 0 ) ),
		m_dRotAngle( 0 ),
		m_bGotten( false )
{
	// m_pAnimMgr �̃C���X�^���X�̐���
	m_pAnimMgr = new SampleItemAnimManager();

};

// �f�X�g���N�^ 
SampleItemEntity::~SampleItemEntity()
{
	// m_pAnimMgr �̌�n��
	delete m_pAnimMgr;

};

// ��Ԃ̍X�V
void SampleItemEntity::Update(double time_elapsed) // �A�C�e�����A���邭���]����
{
	// �A�C�e������]������ �� ��]���x�́A�ÓI�����o�Œ�`����
	m_dRotAngle += m_dRotSpeed * time_elapsed;

	// Headin�x�N�g����m_dRotAngle����]������...
	m_vHeading = m_vDefaultHeading2D.rot( m_dRotAngle ).toVector3D();

	// m_bGotten ���Q�Ƃ��āA�擾�ς݂̏ꍇ�ɁASampleEffectEntity �𐶐��B
	if( m_bGotten )
	{
		// SampleEffectEntity �𐶐����� GameWorld �ɓo�^
		m_pWorld->RegisterSampleEffectEntity( new SampleEffectEntity( m_pWorld, m_vPos ) );

		// ���ȏI������
		m_bExtinct = true;
	}

};

	// �`��
void SampleItemEntity::Render()
{
	m_pAnimMgr->Render( m_vPos, m_vHeading );
};

// �v���[���[�L�����N�^�Ƃ̐ڐG���o�� hasGotten �t���O���グ��
bool SampleItemEntity::TouchDetection( Vector3D CharaPos )
{
	// �ڐG���a�̂Q����v�Z
	double sqlen = m_dBoundingRadius * m_dBoundingRadius;

	Vector3D vDiff = m_vPos-CharaPos;
	
	if( (vDiff.y>0) && (m_dCharacterHight>vDiff.y) 
		&& (vDiff.toVector2D().sqlen()<m_dSqBoundingRadius) )
	{
		// m_bGotten ���グ��
		m_bGotten = true;
		
		return true;
	}

	return false;
};


// ######################## SampleEffectEntity Class ########################

// �R���X�g���N�^
SampleEffectEntity::SampleEffectEntity( SampleGameWorld *world, Vector3D Pos ) : 
		SampleBaseGameEntity( Pos ),
		m_pWorld( world )
{
	// m_pAnimMgr �̃C���X�^���X�̐���
	m_pAnimMgr = new SampleEffectAnimManager();

};

// �f�X�g���N�^ 
SampleEffectEntity::~SampleEffectEntity()
{
	// m_pAnimMgr �̌�n��
	delete m_pAnimMgr;

};

// ��Ԃ̍X�V
void SampleEffectEntity::Update(double time_elapsed) // �A�C�e�����A���邭���]����
{
	// �A�j���[�V�����̍Đ����I��������A���ȏI������B
	if( m_pAnimMgr->isEnd() )
	{
		m_bExtinct = true;
	}

};

// �`��
void SampleEffectEntity::Render()
{
	m_pAnimMgr->Render( m_vPos );
};

// ######################## SampleGameWorld Class ########################

// �R���X�g���N�^
SampleGameWorld::SampleGameWorld()
{}

// SampleItemEntity ��o�^
void SampleGameWorld::RegisterSampleItemEntity( SampleItemEntity* pItem )
{
	// m_pIttemEntityList �ɓo�^
	m_pIttemEntityList.push_back( pItem );

	// m_pAllEntityList �ɓo�^
	m_pAllEntityList.push_back( pItem );

};

// SampleEffectEntity ��o�^
void SampleGameWorld::RegisterSampleEffectEntity( SampleEffectEntity* pEffect )
{
	// m_pAllEntityList �ɓo�^
	m_pAllEntityList.push_back( pEffect );

};

// ������
void SampleGameWorld::Initialize()
{
	SampleEffectAnimManager::Initialize();
	SampleItemAnimManager::Initialize();
};

// �X�V
void SampleGameWorld::Update( double time_elapsed, Vector3D CharaPos )
{

	for( list<SampleItemEntity*>::iterator it=m_pIttemEntityList.begin();
		 it!=m_pIttemEntityList.end(); )
	{
		// �C�e���[�g����STL��list����v�f�����S�ɍ폜������@
		// -> http://marupeke296.com/TIPS_No12_ListElementErase.html
		if((*it)->isExtinct())
		{
			// �폜����
			it = m_pIttemEntityList.erase( it );
			continue;
		}

		// Entity�̍X�V
		(*it)->TouchDetection( CharaPos );

		// �Ō�ɃC�e���[�^���C���N��
		it++; 

	}

	for( list<SampleBaseGameEntity*>::iterator it=m_pAllEntityList.begin(); 
		 it!=m_pAllEntityList.end(); )
	{
		// �C�e���[�g����STL��list����v�f�����S�ɍ폜������@ 
		// -> http://marupeke296.com/TIPS_No12_ListElementErase.html
		if((*it)->isExtinct())
		{
			// �폜����
			it = m_pAllEntityList.erase( it );
			continue;
		}

		// Entity�̍X�V
		(*it)->Update( time_elapsed );

		// �Ō�ɃC�e���[�^���C���N��
		it++; 
	}


};

// �`��
void SampleGameWorld::Render()
{
	list<SampleBaseGameEntity*>::iterator it;
	for( it=m_pAllEntityList.begin(); it!=m_pAllEntityList.end(); it++ )
	{
		(*it)->Render();
	}

};


// �A�C�e���z�u��`�r�b�g�}�b�v��ǂݍ��݃t�B�[���h�ɃA�C�e���z�u���s��
void SampleGameWorld::SetItemsToWorld( double Separation , double ItemHight, char *BitMapFileName )
{
	// �}�b�v����`�r�b�g�}�b�v��ǂݍ���
	int BitMapHandle = LoadSoftImage( BitMapFileName );

	// �}�b�v����`�r�b�g�}�b�v�̃T�C�Y���擾�i�c���j
	int W, H;
	GetSoftImageSize( BitMapHandle, &W, &H ) ;

	// �}�b�v����`�r�b�g�}�b�v����́B
	int  r, g, b, a ;
	for( int i=0; i<W; i++)
	{
		for( int j=0; j<H; j++)
		{
			GetPixelSoftImage( BitMapHandle, j, i, &r, &g, &b, &a ) ;

			// r==0 �łȂ���΁A�A�C�e���L��Ɣ��f
			if( r==0 )
			{
				Vector3D Pos;
				Pos.y = ItemHight;
				Pos.x = i*Separation + 0.5*Separation;
				Pos.z = j*Separation + 0.5*Separation;

				RegisterSampleItemEntity( new SampleItemEntity( this, Pos ) );
			}
		}
	}

};