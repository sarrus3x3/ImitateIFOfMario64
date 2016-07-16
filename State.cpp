#include "State.h"
#include "AnimationManager.h"
#include "VirtualController.h"
#include "MyUtilities.h"
#include "SolidObjects.h"
#include "CameraWorkManager.h"

#include <cassert>

//static const double EPS = 1e-5;
static const double EPS = 0.5;

void State::Rotate( 
		double RotateVelSpeed, 
		PlayerCharacterEntity* pEntity, 
		PhysicalQuantityVariation& PhyVar )
{
	// ���L�[��������Ă���ΐ���x�N�g����ݒ�
	double KeyHoriz = -1.0 * (double)pEntity->m_pVirCntrl->Horiz;
	PhyVar.DstVar = KeyHoriz * RotateVelSpeed * pEntity->Side();
};

// #### Dammy �X�e�[�g�̃��\�b�h ########################################################################
Dammy* Dammy::Instance()
{
	static Dammy instance;
	return &instance;
}

void Dammy::Enter( PlayerCharacterEntity* pEntity )
{
	// �A�j���[�V������ݒ�
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running);
}

void Dammy::StateTranceDetect( PlayerCharacterEntity* pEntity )
{
	;
}

void Dammy::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	PhyVar.init(); // ������

	return;
}

void Dammy::Render(PlayerCharacterEntity* pEntity )
{
	;
};


void Dammy::Exit( PlayerCharacterEntity* pEntity )
{
	;
}

// #### Standing �X�e�[�g�̃��\�b�h ########################################################################
Standing* Standing::Instance()
{
	static Standing instance;
	return &instance;
}

void Standing::Enter( PlayerCharacterEntity* pEntity )
{
	// �A�j���[�V������ݒ�
	if( pEntity->isMatchPrvState( Jump::Instance() ) )
	{
		// �W�����v��ł���΁A���n�̃A�j���[�V�������Đ�����
		pEntity->m_pAnimMgr->setPitch(20.0);
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_Landing);
		pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::Standing);
	}
	else
	{
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Standing, 5.0 );
		pEntity->m_pAnimMgr->setPitch( 20.0 );
	}

	// m_bJmpChrgFlg ��������
	pEntity->m_bJmpChrgFlg = false; 
}

void Standing::StateTranceDetect( PlayerCharacterEntity* pEntity )
{
	// �R���g���[���[���͂���������AState��Running�ɕύX
	if( pEntity->m_pVirCntrl->m_dTiltQuantStickL > 0 )
	{
		// pEntity->ChangeState( Running::Instance() );
		pEntity->ChangeState( SurfaceMove::Instance() );
	}

	// m_bJmpChrgUsageFlg OFF
	static const double JmpChrgWaitTime = 0.1; // �W�����v�J�n�܂ł̑҂�����
	
	// m_bJmpChrgUsageFlg ON
	static const double JmpChrgMaxTime = 0.3;  // �W�����v�`���[�W�̍ő厞��

	// #### �W�����v�֘A
	if( pEntity->m_bJmpChrgUsageFlg )
	{
		if( pEntity->m_bJmpChrgFlg )
		{ // �W�����v�`���[�W��
			// ButA �������ꂽ or �W�����v�`���[�W �̍ő厞�Ԃ𒴉߂���
			if( !pEntity->m_pVirCntrl->ButA.isPushed() 
				|| pEntity->getStopWatchTime() > JmpChrgMaxTime )
			{
				// �� State��Jump�ɍX�V
				//�iJump State ��Enter�̒��Ń^�C�}�l����W�����v�̃T�C�Y���v�Z����j
				pEntity->ChangeState( Jump::Instance() );
			}

		}
		else if( pEntity->m_pVirCntrl->ButA.isNowPush() )
		{ // ButA �����̏u�Ԃɉ����ꂽ
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlg���グ��
			pEntity->StopWatchOn();        // �^�C�}�[�Z�b�g
		}
	}
	else
	{
		if( pEntity->m_bJmpChrgFlg )
		{ // �W�����v�`���[�W��
			// ButA �������ꂽ or �W�����v�`���[�W �̍ő厞�Ԃ𒴉߂���
			if( pEntity->getStopWatchTime() > JmpChrgWaitTime )
			{
				// State��Jump�ɍX�V
				pEntity->ChangeState( Jump::Instance() );
			}

		}
		else if( pEntity->m_pVirCntrl->ButA.isNowPush() )
		{ // ButA �����̏u�Ԃɉ����ꂽ
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlg���グ��
			pEntity->StopWatchOn();        // �^�C�}�[�Z�b�g

			// �W�����v�O�̃A�j���[�V������ݒ�
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_PreMotion);
			float AnimTotalTime = pEntity->m_pAnimMgr->getMotionTotalTime(); // JmpChrgWaitTime ���ԓ��ɃA�j���[�V�����Đ�����������悤�ɍĐ��s�b�`�𒲐�
			float PlayPitch = (float)(AnimTotalTime/JmpChrgWaitTime);
			pEntity->m_pAnimMgr->setPitch(PlayPitch);
		}
	}

}

void Standing::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	static const double ViscousResistance =  40.0; // �S����R�W���iRunning�̔S����R���傫���Ƃ��Ă���j
	static const double RotateVelSpeed    =   6.0; // ���񑬓x

	PhyVar.init(); // ������

	// ���S�ɑ��x��0�ɗ��������Ă��Ȃ����߁A�����͌p��
	PhyVar.Force = -ViscousResistance * pEntity->Velocity() ;

	// �~�܂��Ă��Ă�����͉\
	Rotate( RotateVelSpeed, pEntity, PhyVar ); 
}

void Standing::Render(PlayerCharacterEntity* pEntity )
{
	;
};


void Standing::Exit( PlayerCharacterEntity* pEntity )
{
	;
}


// #### Running �X�e�[�g�̃��\�b�h ########################################################################
Running* Running::Instance()
{
	static Running instance;
	return &instance;
}

void Running::Enter( PlayerCharacterEntity* pEntity )
{
		// �A�j���[�V������ݒ�
	if( pEntity->isMatchPrvState( Jump::Instance() ) )
	{
		// �W�����v��ł���΁A���n�̃A�j���[�V�������Đ�����
		pEntity->m_pAnimMgr->setPitch(40.0);
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_Landing_Short);
		//pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running, 20.0, false );
		pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::Running, 5.0, false );
	}
	else
	{
		// ����o�������R�ɂ���悤�ɃA�j���[�V�����J�n�ʒu�ƃu�����h���{
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running, 10.0 );
	}

	// m_bJmpChrgFlg ��������
	pEntity->m_bJmpChrgFlg = false; 

	// MoveLevel��ݒ�
	pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvRunning;
}

void Running::StateTranceDetect( PlayerCharacterEntity* pEntity )
{
	// m_bJmpChrgUsageFlg OFF
	static const double JmpChrgWaitTime = 0.1; // �W�����v�J�n�܂ł̑҂�����

	// m_bJmpChrgUsageFlg ON
	static const double JmpChrgMaxTime = 0.3;  // �W�����v�`���[�W�̍ő厞��

	// #### �W�����v�֘A
	if( pEntity->m_bJmpChrgUsageFlg )
	{
		if( pEntity->m_bJmpChrgFlg )
		{ // �W�����v�`���[�W��
			// ButA �������ꂽ or �W�����v�`���[�W �̍ő厞�Ԃ𒴉߂���
			if( !pEntity->m_pVirCntrl->ButA.isPushed() 
				|| pEntity->getStopWatchTime() > JmpChrgMaxTime )
			{
				// �� State��Jump�ɍX�V
				//�iJump State ��Enter�̒��Ń^�C�}�l����W�����v�̃T�C�Y���v�Z����j
				pEntity->ChangeState( Jump::Instance() );
			}

		}
		else if( pEntity->m_pVirCntrl->ButA.isNowPush() )
		{ // ButA �����̏u�Ԃɉ����ꂽ
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlg���グ��
			pEntity->StopWatchOn();        // �^�C�}�[�Z�b�g
		}
	}
	else
	{
		if( pEntity->m_bJmpChrgFlg )
		{ // �W�����v�`���[�W��
			// ButA �������ꂽ or �W�����v�`���[�W �̍ő厞�Ԃ𒴉߂���
			if( pEntity->getStopWatchTime() > JmpChrgWaitTime )
			{
				// State��Jump�ɍX�V
				pEntity->ChangeState( Jump::Instance() );
			}

		}
		else if( pEntity->m_pVirCntrl->ButA.isNowPush() )
		{ // ButA �����̏u�Ԃɉ����ꂽ
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlg���グ��
			pEntity->StopWatchOn();        // �^�C�}�[�Z�b�g

			// �W�����v�O�̃A�j���[�V������ݒ�
			//pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_PreMotion, 5.0, false );
			//float AnimTotalTime = pEntity->m_pAnimMgr->getMotionTotalTime(); // JmpChrgWaitTime ���ԓ��ɃA�j���[�V�����Đ�����������悤�ɍĐ��s�b�`�𒲐�
			//float PlayPitch = (float)(AnimTotalTime/JmpChrgWaitTime);
			//pEntity->m_pAnimMgr->setPitch(PlayPitch);
		}
	}
	
	// #### Standing�֑J�� ���̑� MoveLeve����
	static const double ThresholdSpeedForStop   = 5.0*5.0; // Standing �X�e�[�g�ɑJ�ڂ��鑬�x��臒l
	static const double ThresholdSpeedRunToWark = 35.0*35.0;
	if( !(pEntity->m_pVirCntrl->Virti > 0)  )
	{
		if( pEntity->SpeedSq() < ThresholdSpeedForStop )
		{ // �R���g���[���[���͂��Ȃ��A���x���\���ɏ������Ȃ�����AState��Standing�ɕύX
			pEntity->ChangeState( Standing::Instance() );
			return ;
		}
		else if( pEntity->SpeedSq() < ThresholdSpeedRunToWark && pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvWalking )
		{ // ���x��臒l��菬�����Ȃ�����A�A�j���[�V������warking�ɕύX����B�Đ����ɐݒ肷��s�b�`���ύX����
			pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking; // MoveLeve���X�V
			// �A�j���[�V�������X�V
			pEntity->m_pAnimMgr->setAnim( 
				PlayerCharacterEntity::Walking, 
				8.0, 
				false, 
				true );
		}
	}
	else
	{ // �ĂуA�N�Z���[�V�������ꂽ�ꍇ��Move���x����߂��K�v������
		if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvRunning )
		{
			pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvRunning; // MoveLeve���X�V
			// �A�j���[�V�������X�V
			pEntity->m_pAnimMgr->setAnim( 
				PlayerCharacterEntity::Running, 
				8.0, 
				false, 
				true );
		}
	}


}

void Running::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	// �����萔���`
	static const double DrivingForce      =2000.0;        // ���i��
	static const double ViscousResistance =  40.0;        // �S����R�W��
	static const double ViscousResistanceInertia = 10.0;  // �S����R�W���i�������i���j
	//static const double TurningAngle      = 0.25*DX_PI;   // ���L�[���͎��̐��i�͂̌X��
	static const double RotateVelSpeed    =   3.0;        // ���񑬓x

	PhyVar.init(); // ������

	Vector3D vDrivingForce;

	// ������L�[��������Ă����
	if( pEntity->m_pVirCntrl->Virti > 0 )
	{
		vDrivingForce = DrivingForce * pEntity->Heading() - ViscousResistance * pEntity->Velocity() ;
	}
	else
	{
		vDrivingForce = -1 * ViscousResistanceInertia * pEntity->Velocity() ;
	}

	PhyVar.Force = vDrivingForce ;

	// ���L�[��������Ă���ΐ���x�N�g����ݒ�
	Rotate( RotateVelSpeed, pEntity, PhyVar ); 
	
}

void Running::Render(PlayerCharacterEntity* pEntity )
{
	// Animation�ɑ��x�ɉ������Đ��s�b�`���Z�b�g����
	double speed = pEntity->Speed();

	// Running �� Warking �ōĐ��s�b�`���킯��i�n�ʂ̕����������R�ɂȂ�悤�j
	if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvRunning )		
	{
		pEntity->m_pAnimMgr->setPitch((float)speed);
	}
	else if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvWalking )	
	{
		pEntity->m_pAnimMgr->setPitch((float)((14.0/12.0)*speed)); // �n�[�h�R�[�f�B���O�̓}�Y�C�̂�
	}
};

void Running::Exit( PlayerCharacterEntity* pEntity )
{
	;
}


// #### Jump �X�e�[�g�̃��\�b�h ########################################################################
Jump* Jump::Instance()
{
	static Jump instance;
	return &instance;
}

void Jump::Enter( PlayerCharacterEntity* pEntity )
{
	// �W�����v�T�C�Y�����߂�{�^������������
	static const double PressTimeLength_SmallJump = 0.1;
	static const double PressTimeLength_MiddlJump = 0.2;

	// �Ȃ�قǁAEnter�ň��Ղɏ����x�ݒ�͍l�����ق��������B
	//�i�����ʂ̍X�V��Calculate�ɏW�񂷂�\���ɂ��Ă���̂ŁB�j
	// �����̌������猾���΁ACalculate ��[����]���v�Z���ė^����悤�ɂ���̂�����
	//   �������A���̂��߂ɏ�����Ԃ������܂��̂��H
	// �� ���������\���ɂ��邩...
	// �E �W�����v�T�C�Y��ێ�����ϐ���Entity���ɒǉ�
	// �E �T�u�X�e�[�g�񋓌^���`�iJimp.h�j
	// �E �T�u�X�e�[�g��ێ�����ϐ���Entity���ɒǉ� ���ǂ������������H�H�H�H�i�^�́H�H�j
	// �E Calculate �̏��߂Ŕ���H
	// �V���O���g���ł���Ă邩�疳��������̂��ȁH�H�H�H
	// �� �V���O���g���ɂ��Ȃ��ƁA��Ԃ��ς�邽�тɁA
	//    new������del������ŏ����̃I�[�o�[�w�b�h�傫���Ȃ邱�ƌ��O�B
	//    metagame�ł����߂̃C���X�^���X���łȂ񂩓������x���Ȃ����L�������邵�B
	// �� �����̊Ԃ́AEntity��1�����o�Ă��Ȃ����A
	//    State�Ƀ����o���������Ă��܂��Ă����Ȃ��̂ł́H�H
	// �i������ނ́jEntity�������o�Ă��鎞��State�C���X�^���X�Ǘ��A�V���O���g�����g��������ׂ����͉ۑ聚

	if( pEntity->m_bJmpChrgUsageFlg )
	{
		// �^�C�}�l����W�����v�̃T�C�Y���v�Z����
		if( pEntity->getStopWatchTime() < PressTimeLength_SmallJump )
		{ // ���W�����v
			m_eJumpSize = Small;
		}
		else if( pEntity->getStopWatchTime() < PressTimeLength_MiddlJump )
		{ // ���W�����v
			m_eJumpSize = Middl;
		}
		else
		{ // ��W�����v
			m_eJumpSize = Large;
		}
	}
	else
	{
		m_eJumpSize = Middl; 
	}

	// ���̌��Calculate�ŁA�����I�ɃW�����v���s�����߂̃t���O���グ��
	m_bInitImplusivForce = true;

	// �����t���O�� false �ɃZ�b�g
	m_bDescenting = false;

	// �A�j���[�V������ݒ�
	pEntity->m_pAnimMgr->setPitch(20.0);
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_Ascent);
}

void Jump::StateTranceDetect( PlayerCharacterEntity* pEntity )
{
	// ���n�̏������L�q 
	if( pEntity->m_bTouchGroundFlg )
	{
		if( pEntity->m_pVirCntrl->Virti > 0 )
		{
			// State �ύX
			pEntity->ChangeState( Running::Instance() );
			return;
		}
		else
		{
			// State �ύX
			pEntity->ChangeState( Standing::Instance() );
			return;
		}
	}
	// Collision�֘A�����𐷂荞�񂾎��ɁA���̕ӂ̏����͂ǂ��@�\���S������ׂ����H�H��

	// �����J�n����
	if( pEntity->Velocity().y < 0 )
	{
		// �����t���O���グ��
		m_bDescenting = true;

		// �����p�̃A�j���[�V�������Z�b�g
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_Descent);
	}
}

void Jump::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	// �W�����v�T�C�Y��y�������x
	static const double IniVelY_SmallJump = 25;
	static const double IniVelY_MiddlJump = 150;
	static const double IniVelY_LargeJump = 100;
	static const double GravityAccel = 500;

	PhyVar.init(); // ������

	if( m_bInitImplusivForce )
	{ // State�ɓ��������߂�Calculate�Ō��͂������A�W�����v�̏�����^����

		double InitVelY;
		// �W�����v�T�C�Y�ɂ�菉�����ς��
		switch( m_eJumpSize )
		{
		case Small:
			InitVelY = IniVelY_SmallJump;
			break;
		case Middl:
			InitVelY = IniVelY_MiddlJump;
			break;
		case Large:
			InitVelY = IniVelY_LargeJump;
			break;
		}

		// ������^���邽�߂̌��͂��v�Z
		double ImplusivForce;
		ImplusivForce = ( InitVelY / pEntity->TimeElaps() ) * pEntity->Mass();
		PhyVar.Force = Vector3D( 0, ImplusivForce, 0 ); // �^��ɃW�����v���Ă���i���̎��̑���̏�Ԃɉ����āA�����������������������B�j

		m_bInitImplusivForce = false;

		// �ڒnflg��OFF�ɂ���
		pEntity->m_bTouchGroundFlg = false;

		return;
	}
	else
	{ // ���R�����̉^�����L�q

		// y���������ɁA�d�͉����x���Z�b�g
		double GravForce = GravityAccel * pEntity->Mass();
		PhyVar.Force = Vector3D( 0, -GravForce, 0 ); 
		// �� ��ŁA�����^���̋L�q�́Astate�̒��ōs��Ȃ��悤�ɕύX����ׂ��B�i�v�����j

		return;

	}
}

void Jump::Render(PlayerCharacterEntity* pEntity )
{
	;
};


void Jump::Exit( PlayerCharacterEntity* pEntity )
{
	;
}



// #### SurfaceMove �X�e�[�g�̃��\�b�h ########################################################################
SurfaceMove* SurfaceMove::Instance()
{
	static SurfaceMove instance;
	return &instance;
}

void SurfaceMove::Enter( PlayerCharacterEntity* pEntity )
{
		// �A�j���[�V������ݒ�
	if( pEntity->isMatchPrvState( Jump::Instance() ) )
	{
		// �W�����v��ł���΁A���n�̃A�j���[�V�������Đ�����
		pEntity->m_pAnimMgr->setPitch(40.0);
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_Landing_Short);
		//pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::SurfaceMove, 20.0, false );
		pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::Running, 5.0, false );
	}
	else
	{
		// ����o�������R�ɂ���悤�ɃA�j���[�V�����J�n�ʒu�ƃu�����h���{
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Walking, 5.0 );
	}

	// m_bJmpChrgFlg ��������
	pEntity->m_bJmpChrgFlg = false; 

	// MoveLevel��ݒ�
	pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking;
}

void SurfaceMove::StateTranceDetect( PlayerCharacterEntity* pEntity )
{
	// m_bJmpChrgUsageFlg OFF
	static const double JmpChrgWaitTime = 0.1; // �W�����v�J�n�܂ł̑҂�����

	// m_bJmpChrgUsageFlg ON
	static const double JmpChrgMaxTime = 0.3;  // �W�����v�`���[�W�̍ő厞��



	// #### �W�����v�֘A
	if( pEntity->m_bJmpChrgUsageFlg )
	{ // m_bJmpChrgUsageFlg ON (�{�^���̉������ݎ��Ԃɉ����ŁA�W�����v�͒�������@�\)
		if( pEntity->m_bJmpChrgFlg )
		{ // �W�����v�`���[�W��
			// ButA �������ꂽ or �W�����v�`���[�W �̍ő厞�Ԃ𒴉߂���
			if( !pEntity->m_pVirCntrl->ButA.isPushed() 
				|| pEntity->getStopWatchTime() > JmpChrgMaxTime )
			{
				// �� State��Jump�ɍX�V
				//�iJump State ��Enter�̒��Ń^�C�}�l����W�����v�̃T�C�Y���v�Z����j
				pEntity->ChangeState( Jump::Instance() );
			}

		}
		else if( pEntity->m_pVirCntrl->ButA.isNowPush() )
		{ // ButA �����̏u�Ԃɉ����ꂽ
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlg���グ��
			pEntity->StopWatchOn();        // �^�C�}�[�Z�b�g
		}
	}
	else
	{ // m_bJmpChrgUsageFlg OFF
		if( pEntity->m_bJmpChrgFlg )
		{ // �W�����v�`���[�W��
			// ButA �������ꂽ or �W�����v�`���[�W �̍ő厞�Ԃ𒴉߂���
			if( pEntity->getStopWatchTime() > JmpChrgWaitTime )
			{
				// State��Jump�ɍX�V
				pEntity->ChangeState( Jump::Instance() );
			}

		}
		else if( pEntity->m_pVirCntrl->ButA.isNowPush() )
		{ // ButA �����̏u�Ԃɉ����ꂽ
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlg���グ��
			pEntity->StopWatchOn();        // �^�C�}�[�Z�b�g

			// �W�����v�O�̃A�j���[�V������ݒ�
			//pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_PreMotion, 5.0, false );
			//float AnimTotalTime = pEntity->m_pAnimMgr->getMotionTotalTime(); // JmpChrgWaitTime ���ԓ��ɃA�j���[�V�����Đ�����������悤�ɍĐ��s�b�`�𒲐�
			//float PlayPitch = (float)(AnimTotalTime/JmpChrgWaitTime);
			//pEntity->m_pAnimMgr->setPitch(PlayPitch);
		}
	}
	
	// #### Standing�֑J�� ���̑� MoveLeve����
	static const double ThresholdSpeedForStop   = 5.0*5.0;   // Standing �ɑJ�ڂ��鑬�x��臒l�i�����l�j
	static const double ThresholdSpeedRunToWark = 35.0*35.0; // Running<->Warking �̑��x��臒l�i�����l�j
	static const double ThresholdSticktiltRunToWark = 0.6;   // Running<->Warking �̃X�e�B�b�N�X����臒l

	pEntity->DBG_m_dDBG=pEntity->SpeedSq();


	if( (pEntity->m_pVirCntrl->m_dTiltQuantStickL > ThresholdSticktiltRunToWark)  )
	{ // �A�i���O�X�e�B�b�N�X����Warking臒l�ȏ�
		// �ĂуA�N�Z���[�V�������ꂽ�ꍇ��Move���x����߂��K�v������
		if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvRunning )
		{
			pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvRunning; // MoveLeve���X�V
			// �A�j���[�V�������X�V
			pEntity->m_pAnimMgr->setAnim( 
				PlayerCharacterEntity::Running, 
				8.0, 
				false, 
				true );
		}
	}
	else
	{ // �A�i���O�X�X�e�B�b�N�X���L��
		
		//dbgval++;
		if( pEntity->SpeedSq() < ThresholdSpeedRunToWark 
			&& pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvWalking )
		{ // ���x��臒l��菬�����Ȃ�����A�A�j���[�V������warking�ɕύX����B�Đ����ɐݒ肷��s�b�`���ύX����
			pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking; // MoveLeve���X�V
			// �A�j���[�V�������X�V �� animation�̍X�V�������ł��Ȃ�...
			pEntity->m_pAnimMgr->setAnim( 
				PlayerCharacterEntity::Walking, 
				8.0, 
				false, 
				true );
		}
	}

	// �R���g���[���[���͂��Ȃ��A���x���\���ɏ������Ȃ�����AState��Standing�ɕύX
	if( pEntity->SpeedSq() < ThresholdSpeedForStop 
		&& pEntity->m_pVirCntrl->m_dTiltQuantStickL==0 )
	{ 
		pEntity->ChangeState( Standing::Instance() );
		return ;
	}

	// �A�j���[�V�����֌W�����́Arender()�ցB
	// �� �Ǝv�������A�T�uState�J�ڂ̃g���K��render�̒��ł����킩��Ȃ��B
	//    �������ł��Ȃ����Ȃ����A���̂܂܂ł�����...


}

void SurfaceMove::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	// �����萔���`
	//static const double DrivingForce      =2000.0;        // ���i��
	//static const double ViscousResistance =  40.0;        // �S����R�W��
	//static const double ViscousResistanceInertia = 40.0; //10.0;  // �S����R�W���i�������i���j
	//static const double TurningAngle      = 0.25*DX_PI;   // ���L�[���͎��̐��i�͂̌X��
	//static const double RotateVelSpeed    =   3.0;        // ���񑬓x

	static const double MaxVelocity      = 65.0; // �L�����N�^�[�̍ő呬�x�i�X�e�B�b�N��max�܂œ|�������̍ő呬�x�j
	static const double ViscousRsisInert = 10.0;  // �������i���̔S����R�W��
	static const double ViscousRsisAccel = 80.0;  // �������̔S����R�W��
	
	static const double MaxCentripetalForce =500.0*10;   // ���񎞂̍ő���S��
	static const double sqMaxCentripetalForce = MaxCentripetalForce*MaxCentripetalForce;

	static const double SensitivityCoefForTurning = 10; // ���񎞂̓��͂ɑ΂��锽���̗ǂ��B

	PhyVar.init(); // ������

	// 16:45
	// ##### �X�e�B�b�N�̌X���̕�������AEntity�ɓ������Ǘ͂̕������v�Z����
	// �E���[���h���W���g�����Ƃɂ�鐸�x�򉻂�h�����߁A�v�Z��Entity�̃��[�J�����W�ōs���悤�ɏC��
	
	/*
	// �X�N���[�����Entity�ʒu���v�Z
	Vector3D EntiPosForScreen = ConvWorldPosToScreenPos( pEntity->Pos().toVECTOR() );
	assert( EntiPosForScreen.z >= 0.0 && EntiPosForScreen.z <= 1.0 );

	// �f�o�b�N
	DBG_m_vEntiPosOnScreen.x = EntiPosForScreen.x;
	DBG_m_vEntiPosOnScreen.y = EntiPosForScreen.y;

	// �X�N���[����ɃX�e�B�b�N�̌X�������̈ʒu�����߂�
	Vector3D vStickTile( 0,0,0);
	vStickTile.x = pEntity->m_pVirCntrl->m_vStickL.x;
	vStickTile.y = -pEntity->m_pVirCntrl->m_vStickL.y;
	Vector3D StickTiltPosForScreen = EntiPosForScreen + vStickTile;

	// �X�e�B�b�N�̌X�������̈ʒuEntity�̒n���ʂɓ��e���邵���ʒu�����߂�
	StickTiltPosForScreen.z = 0.0;
	Vector3D BgnPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	StickTiltPosForScreen.z = 1.0;
	Vector3D EndPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	Vector3D vCrossPos;
	int rtn = calcCrossPointWithXZPlane( BgnPos, EndPos, vCrossPos ); // ����́A�����ɂ̓L�����N�^�����ʂł͂Ȃ��̂ŁA�W�����v����ƕs���m�B�C�����K�v��
	vCrossPos.y = 0; // ���ՂȑΏ��@

	// ���Ǘ͂̌���
	Vector3D vSteeringForceDir = ( vCrossPos - pEntity->Pos() ).normalize();
	if( rtn < 0 )
	{ // �J�����̌����̔��Α��Œn���ʂƌ����ꍇ
		vSteeringForceDir *= -1; // ���Ό����ɐݒ�
	}

	*/

	// ���݂̃J�����̃r���[�s���ޔ��iEntity�̐i�s�w�����������߂�v�Z�ŁA�r���[�s���Entity�̃��[�J�����W�ł̂��̂ɐݒ肷�邽�߁j
	MATRIX MSaveViewMat = GetCameraViewMatrix();

	// �J�����̃r���[�s���Entity�̃��[�J�����W�ł̂��̂ɐݒ�
	SetCameraViewMatrix( CameraWorkManager::Instance()->m_MViewLocal );

	static const Vector3D vPosOrign = Vector3D( 0,0,0 );

	// �X�N���[�����Entity�ʒu���v�Z
	Vector3D EntiPosForScreen = ConvWorldPosToScreenPos( vPosOrign.toVECTOR() );
	assert( EntiPosForScreen.z >= 0.0 && EntiPosForScreen.z <= 1.0 );

	// �f�o�b�N
	DBG_m_vEntiPosOnScreen.x = EntiPosForScreen.x;
	DBG_m_vEntiPosOnScreen.y = EntiPosForScreen.y;

	// �X�N���[����ɃX�e�B�b�N�̌X�������̈ʒu�����߂�
	Vector3D vStickTile = vPosOrign;
	vStickTile.x = pEntity->m_pVirCntrl->m_vStickL.x;
	vStickTile.y = -pEntity->m_pVirCntrl->m_vStickL.y;
	Vector3D StickTiltPosForScreen = EntiPosForScreen + vStickTile;

	// �X�e�B�b�N�̌X�������̈ʒuEntity�̒n���ʂɓ��e���邵���ʒu�����߂�
	StickTiltPosForScreen.z = 0.0;
	Vector3D BgnPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	StickTiltPosForScreen.z = 1.0;
	Vector3D EndPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	Vector3D vCrossPos;
	int rtn = calcCrossPointWithXZPlane( BgnPos, EndPos, vCrossPos ); // ����́A�����ɂ̓L�����N�^�����ʂł͂Ȃ��̂ŁA�W�����v����ƕs���m�B�C�����K�v��
	vCrossPos.y = 0; // ���ՂȑΏ��@

	// ���Ǘ͂̌���
	Vector3D vSteeringForceDir = ( vCrossPos ).normalize();
	if( rtn < 0 )
	{ // �J�����̌����̔��Α��Œn���ʂƌ����ꍇ
		vSteeringForceDir *= -1; // ���Ό����ɐݒ�
	}

	// �J�����̃r���[�s������ɖ߂�
	SetCameraViewMatrix( MSaveViewMat );


	// �傫���̓I���W�i���̃X�e�B�b�N�̌X�����g�p
	Vector3D vStickTiltFromCam = pEntity->m_pVirCntrl->m_vStickL.toVector3D().len() * vSteeringForceDir;
	//Vector3D vStickTiltFromCam = vSteeringForceDir;

	// �f�o�b�N�p�ɋL��
	DBG_m_vStickPos = 10 * vStickTiltFromCam + pEntity->Pos();

	// �� For DBG

	// 17:16 Coding��
	// 17:29 �c�O�A�����Ȃ�

	// 17:43 �f�o�b�N��

	// * �X�e�B�b�N�̌X���i=Input�j����A�I���x���v�Z
	Vector3D TerminalVel = MaxVelocity * vStickTiltFromCam;
	
	// * �ō����x�ƌ��ݑ��x���r���A�����^������Ԏ��̔��肵�āA$\eta$������
	double eta;
	if( TerminalVel.sqlen() > pEntity->Velocity().sqlen() )
	{
		eta = ViscousRsisAccel; // �������
	}
	else
	{
		eta = ViscousRsisInert; // �������i��
	}
	
	//eta = ViscousRsisAccel; // ��DBG

	
	// * $F_{Drive}$ �����܂�B
	Vector3D vSteeringForce = eta * TerminalVel;
	

	// ##### ���񎞂̋������P

	m_dCentripetalForce = 0; // ���S�͂��N���A
	
	if(pEntity->Velocity().len() < 25.0)
	{ // ���x�������ꍇ�͐��񋓓���K�p���Ȃ�
		Vector3D Force = vSteeringForce - eta * (pEntity->Velocity()) ;
		PhyVar.Force = Force ;

		assert( PhyVar.Force.y==0 );

	}
	else
	{
		// ���S�͂̌���
		double DriveForce = vSteeringForce.len();
		Vector3D vDriveForce = DriveForce * pEntity->Heading();
	
		double CentripetalForce = (SensitivityCoefForTurning * vSteeringForce) * (pEntity->Side());
		if( fabs(CentripetalForce) > MaxCentripetalForce )
		{
			double sgn =  (double)( (CentripetalForce>0) - (CentripetalForce<0) );
			CentripetalForce = sgn * MaxCentripetalForce;
		}
		//CentripetalForce = MaxCentripetalForce; //��DBG

		m_dCentripetalForce = CentripetalForce; // ���S�͂��L��
	

		// ���̃^�C���X�e�b�v�̑��x�E�ʒu���v�Z
		// * �����Q�N�b�^�@�K�p
		// * ����^�����s����Ȃ̂����������������Ȃ̂ŁA
		//   DriveForce, CentripetalForce �͂��̃X�e�b�v�ŌŒ�

		Vector3D vK, vL;
		Vector3D vSumK, vSumL;
		Vector3D vVel = pEntity->Velocity(), vNxtVel;
		Vector3D vUpper = pEntity->Uppder();

		// K1�̌v�Z
		vL = calculateForce( vVel, vUpper, DriveForce, CentripetalForce, eta )/pEntity->Mass();
		vK = vVel;
		vSumK = vK;
		vSumL = vL;

		// K2�̌v�Z
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, DriveForce, CentripetalForce, eta )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K3�̌v�Z
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, DriveForce, CentripetalForce, eta )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K4�̌v�Z
		vNxtVel = vVel+pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, DriveForce, CentripetalForce, eta )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += vK;
		vSumL += vL;

		// ���̃X�e�b�v�̈ʒu�E���x�i�ψʁj�����߂�
		PhyVar.VelVar = (1.0/6.0) * vSumL;
		PhyVar.PosVar = (1.0/6.0) * vSumK;

		// ����p�̌v�Z�����Ă��炤���߂Ƀt���O�𗧂Ă�
		PhyVar.UseVelVar = true;
		PhyVar.UsePosVar = true;

		assert( PhyVar.VelVar.y==0 );

	}

	return;
}

// SurfaceMove::Calculate�̕⏕�֐�
Vector3D SurfaceMove::calculateForce( 
		Vector3D vVel, 
		Vector3D vUpper,
		double DriveForce,
		double CentripetalForce,
		double eta )
{
	// Entity���x(Input)���瑀�Ǘ͂̌������Čv�Z
	Vector3D vHeading = vVel.normalize();
	Vector3D vSide = VCross( vHeading.toVECTOR(), vUpper.toVECTOR() );
	Vector3D vSteering = DriveForce * vHeading + CentripetalForce * vSide;

	// �S�����f���ɏ]��Entity�ɓ����͂��v�Z���ԋp
	return vSteering - eta * vVel ;

};


void SurfaceMove::Render(PlayerCharacterEntity* pEntity )
{
	static const double LikeGravity = 500.0 * 10 * 5; // �o���N�p�̌v�Z�Ɏg�p�B�d�͂ɑ�������B
	//static const double LikeGravity = 500.0 * 10 * 1; // �o���N�p�̌v�Z�Ɏg�p�B�d�͂ɑ�������B

	// Animation�ɑ��x�ɉ������Đ��s�b�`���Z�b�g����
	double speed = pEntity->Speed();

	// ���S�͂Ƀo���N���o
	static Smoother<double> BankAngleSmoother( 6, 0 );  // �~����
	double bankangle = atan2( m_dCentripetalForce, LikeGravity ); // *�d�͂Ɖ��S�͂ɂ��o���N�p�̌v�Z
	pEntity->m_pAnimMgr->setBankAngle( BankAngleSmoother.Update(-bankangle) );
	/*
	double bankangle = atan2( m_dCentripetalForce, LikeGravity ); // *�d�͂Ɖ��S�͂ɂ��o���N�p�̌v�Z
	pEntity->m_pAnimMgr->setBankAngle( -bankangle );
	*/

	// Running �� Warking �ōĐ��s�b�`���킯��i�n�ʂ̕����������R�ɂȂ�悤�j
	if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvRunning )		
	{
		pEntity->m_pAnimMgr->setPitch((float)speed);
	}
	else if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvWalking )	
	{
		pEntity->m_pAnimMgr->setPitch((float)((14.0/12.0)*speed)); // �n�[�h�R�[�f�B���O�̓}�Y�C�̂�
	}

	// �f�o�b�N�p
	// �iEntity���ʏ�ɓ��e�����j�X�e�B�b�N�̌X���̈ʒu��`��
	static PlaneRing RingIns( 
		0.6, 0.4, 16, 
		GetColorU8(255,   0,   0, 0 ),
		GetColorU8(255, 255, 255, 0 ) );
	RingIns.setCenterPos( DBG_m_vStickPos );
	RingIns.Render(); 

	// �X�N���[�����Entity�ʒu��`��
	DrawCircle( (int)DBG_m_vEntiPosOnScreen.x, (int)DBG_m_vEntiPosOnScreen.y, 5, GetColor(0,255,0) );

	// ���S�͏o��
	//�s��
	int colmun= 0;
	int width = 15;

	// Entity�̑��x��\��
	DrawFormatString( 0, width*colmun, 0xffffff, "m_dCentripetalForce:%8f", m_dCentripetalForce ); 
	colmun++;

	DrawFormatString( 0, width*colmun, 0xffffff, "EntiPosOnScreen:%8f, %8f", DBG_m_vEntiPosOnScreen.x, DBG_m_vEntiPosOnScreen.y ); 
	colmun++;

	// �X�e�B�b�N�̌X�����o��
	Vector2D vStickTile;
	vStickTile.x = pEntity->m_pVirCntrl->m_vStickL.x;
	vStickTile.y = -pEntity->m_pVirCntrl->m_vStickL.y;
	DrawFormatString( 0, width*colmun, 0xffffff, "EntiPosOnScreen:%8f, %8f", vStickTile.x, vStickTile.y ); 
	colmun++;


};

void SurfaceMove::Exit( PlayerCharacterEntity* pEntity )
{
	// ���S�͂ɂ��p���̌X���̉���
	pEntity->m_pAnimMgr->setBankAngle( 0.0 );
}
