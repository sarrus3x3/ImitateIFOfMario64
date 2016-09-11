#include "State.h"
#include "AnimationManager.h"
#include "VirtualController.h"
#include "MyUtilities.h"
//#include "SolidObjects.h"
//#include "CameraWorkManager.h"

#include <cassert>

//static const double EPS = 1e-5;
static const double EPS = 0.5;

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
	else if( pEntity->isMatchPrvState( OneEightyDegreeTurn::Instance() ) )
	{
		// �_�b�V������̐ؕԂ���ł���΁A�u���[�L��̋N���オ��̃A�j���[�V�������Đ�����
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::BreakingAfter);
		pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::Standing, 10.0 );
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
}

void Standing::Render(PlayerCharacterEntity* pEntity )
{
	;
};


void Standing::Exit( PlayerCharacterEntity* pEntity )
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
			pEntity->ChangeState( SurfaceMove::Instance() );
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

// ##### �萔
//const double ThresholdSpeedRunToWark = 35.0*35.0; // Running<->Warking �̑��x��臒l�i�����l�j
const double SurfaceMove::ThresholdSpeedRunToWark = 50.0*50.0; // Running<->Warking �̑��x��臒l�i�����l�j
// ����n�ߒ���ɐi�s�����𔽓]�������ꍇ�ɐؕԂ�������s��Ȃ��悤�Ƀ`���[�j���O

const double SurfaceMove::ThresholdSticktiltRunToWark = 0.6;   // Running<->Warking �̃X�e�B�b�N�X����臒l

const double SurfaceMove::MaxCentripetalForce = 500.0*10;   // ���񎞂̍ő���S��


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

		// MoveLevel��ݒ�
		pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking;

	}
	else if(pEntity->isMatchPrvState( OneEightyDegreeTurn::Instance() ))
	{
		// �_�b�V������̐ؕԂ��̌�ł���΁A����̃A�j���[�V�������p������B
		//pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running, 5.0, false );
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running  );

		// ���[�A�ؕԂ��L�����Z�������ꍇ�̓u�����h������ �� �ؕԂ�����̃T�u�X�e�[�g��Entity�Ɏ�������K�v������
		// �� �ʓ|���������炢��c�B�u�����h���Ă����v����

		// �T�u�X�e�[�g�̕]�����s��
		if( (pEntity->m_pVirCntrl->m_dTiltQuantStickL > ThresholdSticktiltRunToWark) 
			|| pEntity->SpeedSq() < ThresholdSpeedRunToWark )
		{ // �A�i���O�X�e�B�b�N�X����Warking臒l�ȏ� or Entity�̃X�s�[�h���\���傫��
			// �T�u�X�e�[�g�𑖂�ɃZ�b�g
			pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvRunning; // MoveLeve���X�V
			// �A�j���[�V�������X�V
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running, 5.0, false );
		}
		else
		{ 
			// ����ȊO�̓T�u�X�e�[�g������ɃZ�b�g
			pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking; // MoveLeve���X�V
			// �A�j���[�V�������X�V
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Walking, 5.0, false );
		}

	}
	else
	{
		// ����o�������R�ɂ���悤�ɃA�j���[�V�����J�n�ʒu�ƃu�����h���{
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Walking, 5.0 );

		// MoveLevel��ݒ�
		pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking;

	}

	// m_bJmpChrgFlg ��������
	pEntity->m_bJmpChrgFlg = false; 

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


	// #### �_�b�V������̐ؕԂ��̑J�ڔ���
	static const double InnerProductForStartTurn = 0.0; // ���x�x�N�g���i�K�i���ς݁j�ƈړ������x�N�g���̓��ϒl�����̒l�ȉ��ł���΁A�ؕԂ��Ɣ��肷��B
	
	//if( pEntity->m_eMoveLevel == PlayerCharacterEntity::MvLvRunning )
	if( pEntity->Velocity().sqlen() >= ThresholdSpeedRunToWark )
	{ // �ړ����x�����ARunning�Ȃ��
		Vector3D VelDir  = pEntity->Velocity().normalize();
		Vector3D MoveDir = pEntity->calcMovementDirFromStick().normalize();
		if( VelDir*MoveDir <= InnerProductForStartTurn )
		{
			pEntity->ChangeState( OneEightyDegreeTurn::Instance() );
			return ;
		}
	}


	// #### Standing�֑J�� ���̑� MoveLeve����
	static const double ThresholdSpeedForStop   = 5.0*5.0;   // Standing �ɑJ�ڂ��鑬�x��臒l�i�����l�j
	//static const double ThresholdSpeedRunToWark = 35.0*35.0; // Running<->Warking �̑��x��臒l�i�����l�j// SurfaceMove �X�e�[�g �N���X�̃����o�ɕύX
	//static const double ThresholdSticktiltRunToWark = 0.6;   // Running<->Warking �̃X�e�B�b�N�X����臒l // // SurfaceMove �X�e�[�g �N���X�̃����o�ɕύX

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
	static const double MaxVelocity      = 65.0; // �L�����N�^�[�̍ő呬�x�i�X�e�B�b�N��max�܂œ|�������̍ő呬�x�j
	
	static const double ViscousRsisInert = 40.0;  // �������i���̔S����R�W��
	static const double ViscousRsisAccel = 40.0;  // �������̔S����R�W��
	// 2016/08/18
	// ����n�ߒ���ɐi�s�����𔽓]�������ꍇ�ɐؕԂ�������s��Ȃ��悤�Ƀ`���[�j���O
	// �������i���Ɖ������̔S����R�𓯂��ɂ���B
	// �ؕԂ����쎞�̉����͂ɍ����Ēl���������`���[�j���O����

	static const double sqMaxCentripetalForce = MaxCentripetalForce*MaxCentripetalForce;

	static const double SensitivityCoefForTurning = 10; // ���񎞂̓��͂ɑ΂��锽���̗ǂ��B

	PhyVar.init(); // ������

	// �X�e�B�b�N�̌X���̕�������Entity�̈ړ��������v�Z����
	Vector3D vStickTiltFromCam = pEntity->calcMovementDirFromStick();

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
	
	// * ���i�͂̌���
	Vector3D vSteeringForce = eta * TerminalVel;
	Vector3D vArrangeSteeringForce = SensitivityCoefForTurning * vSteeringForce;

	// DBG
	DBG_m_vSteeringForce = vSteeringForce;

	// ##### ���񎞂̋������P

	if( pEntity->m_eMoveLevel == PlayerCharacterEntity::MvLvWalking )
	//if( pEntity->Velocity().sqlen() < ThresholdSpeedRunToWark )
	{ // ���x�������ꍇ�͐��񋓓���K�p���Ȃ�
		
		Vector3D Force = vSteeringForce - eta * (pEntity->Velocity()) ;
		PhyVar.Force = Force ;

		assert( PhyVar.Force.y==0 );

	}
	else
	{
		// ���S�͂̌���
		m_dCentripetalForce = 0; // ���S�͂��N���A

		// ���i�͂̌v�Z
		//   �X�e�B�b�N���i�s�����Ƃ͋t�����̏ꍇ�͐��i�� FDrive ��ؕԂ��̓���̊J�n�����ł���
		//   ���x臒l�ȏ�ɂȂ�Ȃ��悤�ɒ���
		double DriveForce = vSteeringForce.len();

		if( vStickTiltFromCam * pEntity->Heading() <= 0 )
		{
			double ThresholdForce = eta * ThresholdSpeedRunToWark * 0.8 ;
			if( DriveForce > ThresholdForce )
			{
				DriveForce = ThresholdForce;
			}
		}

		Vector3D vDriveForce = DriveForce * pEntity->Heading();
	
		// ���̃^�C���X�e�b�v�̑��x�E�ʒu���v�Z
		// * �����Q�N�b�^�@�K�p
		// * ����^�����s����Ȃ̂����������������Ȃ̂ŁA
		//   DriveForce, CentripetalForce �͂��̃X�e�b�v�ŌŒ�

		Vector3D vK, vL;
		Vector3D vSumK, vSumL;
		Vector3D vVel = pEntity->Velocity(), vNxtVel;
		Vector3D vUpper = pEntity->Uppder();

		// K1�̌v�Z
		vL = calculateForce( vVel, vUpper, vArrangeSteeringForce, DriveForce, eta, m_dCentripetalForce )/pEntity->Mass();
		vK = vVel;
		vSumK = vK;
		vSumL = vL;

		// K2�̌v�Z
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, m_dCentripetalForce )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K3�̌v�Z
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, m_dCentripetalForce )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K4�̌v�Z
		vNxtVel = vVel+pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, m_dCentripetalForce )/pEntity->Mass();
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
// 2016/09/04
//   ����n�߂�Entity�������U������΍�Ƃ��āA
//   CentripetalForce�@�ł͂Ȃ��AvSteeringForce ��n���A
//   ���̓s�x CentripetalForce ���v�Z����p�ɕύX�B
Vector3D SurfaceMove::calculateForce( 
		Vector3D vVel, 
		Vector3D vUpper,
		Vector3D vArrangeSteeringForce,
		double DriveForce,
		double eta,
		double &outCentripetalForce
		)
{
	// Entity���x(Input)���瑀�Ǘ͂̌������Čv�Z
	Vector3D vHeading = vVel.normalize();
	Vector3D vSide = VCross( vHeading.toVECTOR(), vUpper.toVECTOR() );

	// vArrangeSteeringForce ���� CentripetalForce ���v�Z
	double CentripetalForce = vArrangeSteeringForce * vSide ;
	// �J�b�g�I�t����
	if( fabs(CentripetalForce) > MaxCentripetalForce )
	{
		double sgn =  (double)( (CentripetalForce>0) - (CentripetalForce<0) );
		CentripetalForce = sgn * MaxCentripetalForce;
	}

	// �ŏI�I�ȑ��Ǘ͂��v�Z
	Vector3D vSteering = DriveForce * vHeading + CentripetalForce * vSide;

	// �v�Z�������S�͂��i�[
	outCentripetalForce = CentripetalForce;

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

	// Running �� Warking �ōĐ��s�b�`���킯��i�n�ʂ̕����������R�ɂȂ�悤�j
	if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvRunning )		
	{
		pEntity->m_pAnimMgr->setPitch((float)speed);
	}
	else if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvWalking )	
	{
		pEntity->m_pAnimMgr->setPitch((float)((14.0/12.0)*speed)); // �n�[�h�R�[�f�B���O�̓}�Y�C�̂�
	}

	// ###### �f�o�b�N�p�o�� 

	// �iEntity���ʏ�ɓ��e�����j�X�e�B�b�N�̌X���̈ʒu��`��
	pEntity->DBG_renderMovementDirFromStick();

	// ���S�͏o��
	//�s��
	int colmun= 0;
	int width = 15;

	// Entity�̑��x��\��
	DrawFormatString( 0, width*colmun, 0xffffff, "m_dCentripetalForce:%8f", m_dCentripetalForce ); 
	colmun++;

	// �X�e�B�b�N�̌X�����o��
	Vector2D vStickTile;
	vStickTile.x = pEntity->m_pVirCntrl->m_vStickL.x;
	vStickTile.y = -pEntity->m_pVirCntrl->m_vStickL.y;
	DrawFormatString( 0, width*colmun, 0xffffff, "EntiPosOnScreen:%8f, %8f", vStickTile.x, vStickTile.y ); 
	colmun++;

	// Entity�֑̂��Ǘ͂��o��
	DrawFormatString( 0, width*colmun, 0xffffff, "vSteeringForce:%8f, %8f", DBG_m_vSteeringForce.x, DBG_m_vSteeringForce.z ); 
	colmun++;


};

void SurfaceMove::Exit( PlayerCharacterEntity* pEntity )
{
	// ���S�͂ɂ��p���̌X���̉���
	pEntity->m_pAnimMgr->setBankAngle( 0.0 );
}


// #### OneEightyDegreeTurn �X�e�[�g�̃��\�b�h ########################################################################

// ##### �萔
const double OneEightyDegreeTurn::MaxVelocity      = 65.0;  // �L�����N�^�[�̍ő呬�x�i�X�e�B�b�N��max�܂œ|�������̍ő呬�x�j
//const double OneEightyDegreeTurn::ViscousRsisTurn  = 80.0;  // �S����R�W��
const double OneEightyDegreeTurn::ViscousRsisTurn   = 40.0;  // �S����R�W��
const double OneEightyDegreeTurn::ViscousRsisBreak  = 25.0;  // �S����R�W��
//const double OneEightyDegreeTurn::TurningDulation  = 7.0/(OneEightyDegreeTurn::ViscousRsisTurn);
const double OneEightyDegreeTurn::TurningDulation  = 0.3;
const double OneEightyDegreeTurn::SlowDownEnough   = 5.0;   // SurfaceMove����~ �̑��x臒l�Ɠ����ɂ��Ă���
const double OneEightyDegreeTurn::InnerProductForStartTurn = 0.0; // ���x�x�N�g���i�K�i���ς݁j�ƈړ������x�N�g���̓��ϒl�����̒l�ȉ��ł���΁A�ؕԂ��Ɣ��肷��B


OneEightyDegreeTurn* OneEightyDegreeTurn::Instance()
{
	static OneEightyDegreeTurn instance;
	return &instance;
}

void OneEightyDegreeTurn::Enter( PlayerCharacterEntity* pEntity )
{
	// �T�u�X�e�[�g�̏�����
	m_eSubState = SUB_BREAKING;

	// �ؕԂ�������J�n�������̑��x�������L�^
	m_vVelDirBeginning = pEntity->Velocity().normalize();

	// �A�j���[�V�����̐ݒ�
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Breaking, 0.0 ); // �u���[�L�̃A�j���[�V������ݒ�
	pEntity->m_pAnimMgr->setPitch( 20.0 );

}

void OneEightyDegreeTurn::StateTranceDetect( PlayerCharacterEntity* pEntity )
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
	
	// #### �u���[�L���ɃL�����Z��
	if( m_eSubState == SUB_BREAKING )
	{ // �u���[�L���

		// �X�e�B�b�N�̌X���������i�ؕԂ��J�n���́j���x�����ɓ|���ꂽ��i�߂��ꂽ��j�A 
		// �ؕԂ����L�����Z�����đ���̏�Ԃɖ߂�B
		Vector3D MoveDir = pEntity->calcMovementDirFromStick().normalize();
		if( m_vVelDirBeginning*MoveDir > InnerProductForStartTurn )
		{
			pEntity->ChangeState( SurfaceMove::Instance() );
			return ;
		}

		// Substate��[Enter]�ŃT�u�X�e�[�g�̕]���irun��work���j���s�����Ɓ�

	}

	// #### �u���[�L�E�T�u��Ԃ���ؕԂ��E�T�u��Ԃւ̑J�ڔ���
	static const double SqSlowDownEnough = SlowDownEnough * SlowDownEnough;

	if( m_eSubState == SUB_BREAKING )
	{ // �u���[�L���

		// �\���Ɍ����������H
		if( pEntity->Velocity().sqlen() < SqSlowDownEnough )
		{
			// �X�e�B�b�N�̌X���� 0 �������ꍇ�́A���̌�ɐؕԂ����L�����Z������ ��~�̏�ԂɑJ�ځB
			if( pEntity->m_pVirCntrl->m_dTiltQuantStickL==0 )
			{ 
				pEntity->ChangeState( Standing::Instance() );
				return ;
			}

			// �ؕԂ���ԂɑJ��
			m_eSubState = SUB_TURNING;

			// �^�C�}�I��
			pEntity->StopWatchOn();

			// �ؕԂ�����̃A�j���[�V�������Đ�
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Turning );

			// TurningDulation ���ԓ��ɃA�j���[�V�����Đ�����������悤�ɍĐ��s�b�`�𒲐�
			//float AnimTotalTime = pEntity->m_pAnimMgr->getMotionTotalTime(); 
			float AnimTotalTime = 41 - 14; //  getMotionTotalTime ���ƁA�؂�o�����ꍇ�̃A�j���[�V�����̍Đ����Ԃ��擾�ł��Ȃ�
			float PlayPitch = (float)(AnimTotalTime/TurningDulation) + 1.0;
			pEntity->m_pAnimMgr->setPitch(PlayPitch);

			return ;

		}

	}

	// #### �ؕԂ��E�T�u��Ԃ��^�C�}�����ɂȂ�����SurfaceMove State�ɑJ��
	if( m_eSubState == SUB_TURNING )
	{ // �ؕԂ����
		if( pEntity->getStopWatchTime() > TurningDulation )
		{
			pEntity->ChangeState( SurfaceMove::Instance() );

			return;
		}
	}

}

void OneEightyDegreeTurn::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	// �u���[�L�́A�ؕԂ����̐��i�͂́A
	// Entity�̏I���x�ƁA�S����R�W���i=�I���x�ւ̓��B���ԁj���v�Z����B

	PhyVar.init(); // ������

	// �T�u�E�X�e�[�g�ɂ�蓮����킯��
	if( m_eSubState == SUB_BREAKING )
	{ // �u���[�L���
		// ���x���\���������Ȃ�܂Ńu���[�L��������B

		// �u���[�L�͂��v�Z
		Vector3D vBreakingForce = -1 * ViscousRsisBreak * MaxVelocity * m_vVelDirBeginning;

		// �S����R���f������Entity�ɓ����͂��v�Z
		PhyVar.Force = vBreakingForce - ViscousRsisBreak * (pEntity->Velocity());

		// �������Œ�
		PhyVar.Heading = m_vVelDirBeginning;
		PhyVar.UseHeading = true;

		return;

	}
	else if( m_eSubState == SUB_TURNING )
	{ // �ؕԂ����
		// �؂�Ԃ��̕����ɉ�������B�����Ɍ�����ؕԂ������։�]�B

		// ##### Entity�ɓ����͂��v�Z

		// �X�e�B�b�N�̌X���̕�������Entity�̈ړ��������v�Z����
		Vector3D vStickTiltFromCam = pEntity->calcMovementDirFromStick();
		
		Vector3D vTurnDestination; // ��]���Č��������

		// �擾�����X�e�B�b�N�̌X���̕����� 0 �łȂ���΂��̕������K�i�����Đݒ�B
		// 0 �̏ꍇ�́A�ؕԂ�����ɓ��������̐i�s�����Ɛ^�t�̕����ɃZ�b�g
		if( vStickTiltFromCam.sqlen() > 0 )
		{
			vTurnDestination = vStickTiltFromCam.normalize();
		}
		else
		{
			vTurnDestination = -1 * m_vVelDirBeginning;
		}

		// * �X�e�B�b�N�̌X���i=Input�j����A�I���x���v�Z
		Vector3D TerminalVel = MaxVelocity * vTurnDestination;

		// �ؕԂ����̐��i�͂��v�Z
		Vector3D vTurnningForce = ViscousRsisTurn * TerminalVel;

		// �S����R���f������Entity�ɓ����͂��v�Z
		PhyVar.Force = vTurnningForce - ViscousRsisTurn * (pEntity->Velocity());

		// ##### Entity�����̉�]�ʂ��v�Z
		// * TurningDulation �Ō����̉�]����������悤�ɁA��]���x�𒲐�

		// ���݂�Heading�ƁA�ړI�̕����̃x�N�g���̂Ȃ��p���v�Z
		double angle = atan2( 
			pEntity->Side()    * vTurnDestination, 
			pEntity->Heading() * vTurnDestination );

		// ����ꂽ�p�x�����l�̏ꍇ�́A�t��������̊p�x�ɕϊ��i����ɂ��A��]�������Œ肳���j
		if( angle < 0 )
		{
			angle = 2*DX_PI_F + angle;
		}

		// ���݂̉�]�ʒu�ƁA��]�����܂ł̎c�莞�Ԃ��A����̃^�C���X�e�b�v�ɂ������]�ʂ��v�Z
		double RemainingTime = TurningDulation - pEntity->getStopWatchTime();
		double RotQuant;
		if( RemainingTime > pEntity->TimeElaps() )
		{
			RotQuant = -angle * pEntity->TimeElaps() / ( RemainingTime + pEntity->TimeElaps() );
		}
		else
		{
			RotQuant = -angle;
		}
		// ��RemainingTime �` 0 �ŋ��������������Ȃ�\��������

		// MGetRotAxis�������A���݂�Headin��Upper�����ɁA����̃^�C���X�e�b�v�ɂ������]�ʂ�����]������
		MATRIX RotMat = MGetRotAxis( pEntity->Uppder().toVECTOR(), RotQuant );
		Vector3D vNewHedding = VTransform( pEntity->Heading().toVECTOR(), RotMat );

		// �V����Heading�x�N�g����ݒ肵����
		PhyVar.Heading = vNewHedding;
		PhyVar.UseHeading = true;

		DBG_m_dAngle = angle;
		DBG_m_dRemainingTime = RemainingTime;

		return;

	}
	else{ assert(false); }

	
	return;
}

void OneEightyDegreeTurn::Render(PlayerCharacterEntity* pEntity )
{

	// ###### �f�o�b�N�p�o�� 

	// �iEntity���ʏ�ɓ��e�����j�X�e�B�b�N�̌X���̈ʒu��`��
	pEntity->DBG_renderMovementDirFromStick();

	//�s��
	int colmun= 0;
	int width = 15;

	// �T�u��Ԃ�`��
	string SubStateString;
	switch( m_eSubState )
	{
	case OneEightyDegreeTurn::SUB_BREAKING:
		SubStateString = "SUB_BREAKING";
		break;
	case OneEightyDegreeTurn::SUB_TURNING:
		SubStateString = "SUB_TURNING";
		break;
	}

	DrawFormatString( 0, width*colmun, 0xffffff, "SubState:%s", SubStateString.c_str() ); 
	colmun++;

	
	DrawFormatString( 0, width*colmun, 0xffffff, "Angle:%f", DBG_m_dAngle ); 
	colmun++;

	DrawFormatString( 0, width*colmun, 0xffffff, "RemainingTime:%f", DBG_m_dRemainingTime ); 
	colmun++;


};

void OneEightyDegreeTurn::Exit( PlayerCharacterEntity* pEntity )
{
	// ���S�͂ɂ��p���̌X���̉���
	pEntity->m_pAnimMgr->setBankAngle( 0.0 );
}

