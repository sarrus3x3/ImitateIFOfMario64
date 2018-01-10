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
	else if( pEntity->isMatchPrvState( Break::Instance() ) )
	{
		// �_�b�V������̐ؕԂ���ł���΁A�u���[�L��̋N���オ��̃A�j���[�V�������Đ�����
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::BreakingAfter);
		pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::Standing, 10.0 );
		pEntity->m_pAnimMgr->setPitch(20.0); // �s�b�`�𒲐�

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
	if( pEntity->m_pVirCntrl->m_dStickL_len > 0 )
	{
		// pEntity->ChangeState( Running::Instance() );
		//pEntity->ChangeState(SurfaceMove::Instance());
		pEntity->ChangeState( Run::Instance() );
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
			//pEntity->ChangeState(SurfaceMove::Instance());
			pEntity->ChangeState( Run::Instance() );
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


// #### Break �X�e�[�g�̃��\�b�h ########################################################################
Break* Break::Instance()
{
	static Break instance;
	return &instance;
}

void Break::Enter(PlayerCharacterEntity* pEntity)
{
	// �u���[�L��Ԃ̃^�C�}�I��
	pEntity->StopWatchOn();

	// �A�j���[�V�����̐ݒ�
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Breaking, 0.0); // �u���[�L�̃A�j���[�V������ݒ�
	
	float AnimTotalTime = pEntity->m_pAnimMgr->getCurAnimLength();
	
	//   turn�ɑJ�ڂ���^�C�~���O�ł́A���Ƀu���[�L�A�j���[�V�����̍Đ����I����Ă���K�v������B
	//   �u���[�L�ɓ���Œᑬ�x�i���u���[�L�̌����x�j����A�u���[�L�p������ŏ����Ԃ��v�Z���A����ȉ��ɂ���K�v������B
	double BrakingDulation = 0.08;
	float PlayPitch = (float)(AnimTotalTime/BrakingDulation);
	
	pEntity->m_pAnimMgr->setPitch(PlayPitch);
}

void Break::StateTranceDetect(PlayerCharacterEntity* pEntity)
{
	// �p���\��̃��\�b�h
}

void Break::Calculate(PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar)
{
	PhyVar.init(); // ������

	// �V�����X�e�[�g�ւ̑J�ځA�L�����N�^�̕����ʂ̍X�V�́A
	// ���ׂĂ��̃��\�b�h�̒��ł��悤�ɂ���B

	// ## �L�����N�^��Update()�ł̕����ʂ̍X�V������������邽�߂̏���
	// �� ���ׂẴX�e�[�g�ŁA�X�e�[�g���ŕ����ʂ��X�V����悤�ɕύX������A�����̏����͍폜���邱�ƁB�B�B

	// ���x�ω��Ȃ�
	PhyVar.UseVelVar = false;
	PhyVar.Force = Vector3D(0, 0, 0);

	// �L�����N�^�����̎蓮�ݒ��L��
	PhyVar.UseHeading = true;
	PhyVar.Heading = pEntity->Heading();

	// �ʒu�̓f�t�H���g�X�V�i���݂̑��x�ɏ]���X�V�j
	PhyVar.UsePosVar = false;

	// ##�u�u���[�L�v�X�e�[�g�̋���

	// ���x�̌����̓L�����N�^�����ő�p���悤���Ƃ��v�������A�i�ǂ�ǂ񏬂����Ȃ��Ă������x�x�N�g�����͈���ł��邽�߁j
	// �n���ʂɌX��������ꍇ�͑��x�������L�����N�^���������藧���Ȃ��\�������邱�ƂɋC�������̂ŁA�~�߂��B

	// ���x�̑傫��������𕪗�
	double   VelSiz = pEntity->Velocity().len();
	Vector3D VelDir = pEntity->Velocity() / VelSiz;

	// ���x�̑傫�����X�V
	double deceleration = 60.0f * pEntity->TimeElaps() * PlayerCharacterEntity::m_dConfigScaling; // �u���[�L���̌����x
	//double deceleration = 10.0f * pEntity->TimeElaps() * PlayerCharacterEntity::m_dConfigScaling; // �u���[�L���̌����x
	double NewVelSiz = MoveTowards(VelSiz, 0, deceleration);

	// �V�������x�x�N�g�����č\�z
	Vector3D NewVel = NewVelSiz * VelDir;

	// �X�V���ꂽ���x��Entity�ɃZ�b�g
	pEntity->setVelocity(NewVel);

	// ## �u�u���[�L�v�X�e�[�g�̃L�����Z������

	// �X�e�B�b�N�̓��͂����邱��
	if (!pEntity->MoveInput().isZero())
	{
		// �X�e�B�b�N�̌X�������ƃL�����N�^�����̊p�x���擾
		double AngleBetween = Angle3D(pEntity->MoveInput(), pEntity->Heading());

		//�X�e�B�b�N�̌X��������(�u���[�L�J�n���́�)�L�����N�^�����Ƃ̊p��110����菬�����Ȃ�
		if (AngleBetween < DX_PI * (11.0 / 18.0))
		{
			//���L�����Z�����āA�u����^�����v�X�e�[�g�ɑJ�ځB
			//pEntity->ChangeState(SurfaceMove::Instance());
			pEntity->ChangeState(Run::Instance());
			return;
		}
	}


	// ##�u�u���[�L�v�X�e�[�g�̏I������

	// �L�����N�^�̑��x���O�ɂȂ�����
	if (pEntity->Velocity().sqlen() == 0.0)
	{
		if (pEntity->MoveInput().isZero())
		{ // �X�e�B�b�N���͂Ȃ�
		
			// �u�A�C�h���v�i�ҋ@�j�X�e�[�g�֑J��
			pEntity->ChangeState(Standing::Instance());
			return;
		}
		else
		{ // �X�e�B�b�N���͂���
		
			// [�L�����N�^�����𔽓]]
			Vector3D newHead = -1 * pEntity->Heading();
			pEntity->setHeading(newHead);

			PhyVar.Heading = pEntity->Heading(); // �� ���ׂẴX�e�[�g�ŁA�X�e�[�g���ŕ����ʂ��X�V����悤�ɕύX������A�����̏����͍폜���邱�ƁB�B�B

			//�u�ؕԂ��v�X�e�[�g�֑J��
			pEntity->ChangeState(Turn::Instance());
			return;
		} 
	}

}

void Break::Render(PlayerCharacterEntity* pEntity)
{
	;
};


void Break::Exit(PlayerCharacterEntity* pEntity)
{
	// ���S�͂ɂ��p���̌X���̉���
	pEntity->m_pAnimMgr->setBankAngle(0.0);
}

// #### Turn �X�e�[�g�̃��\�b�h ########################################################################
const double Turn::TurnDulation = 0.4; // �ؕԂ��X�e�[�g�̌p������
//const double Turn::TurnSpeed = 1.5 * DX_PI;   // �ؕԂ����̐��񑬓x
// 2018/01/07 �`���[�j���O��
const double Turn::TurnSpeed    = 4.0 * DX_PI;   // �ؕԂ����̐��񑬓x

Turn* Turn::Instance()
{
	static Turn instance;
	return &instance;
}

void Turn::Enter(PlayerCharacterEntity* pEntity)
{
	// �^�C�}�I��
	pEntity->StopWatchOn();

	// �ؕԂ�����̃A�j���[�V�������Đ�
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::TurnFixHead);
	pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::TurnFinalFly, 0.0); // �u�����h����ƃ��[�V�����j�]�����肠�邽��

	// TurnDulation ���ԓ��ɃA�j���[�V�����Đ�����������悤�ɍĐ��s�b�`�𒲐�
	float AnimTotalTime = 16.0 + 5.0; // TurnFull�̎��� + TurnFinalFly�̎��ԁB�n�[�h�R�[�f�B���O�ŃX�~�}�Z���B
	// ���{���Ȃ�AAnimUniqueInfo �ɃA�j���[�V�����̒������擾���郁�\�b�h����������ׂ��ł���B
	float PlayPitch = (float)(AnimTotalTime / TurnDulation);
	pEntity->m_pAnimMgr->setPitch(PlayPitch);

}

void Turn::StateTranceDetect(PlayerCharacterEntity* pEntity)
{
	// �p���\��̃��\�b�h
}

void Turn::Calculate(PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar)
{
	PhyVar.init(); // ������

	// ## �L�����N�^��Update()�ł̕����ʂ̍X�V������������邽�߂̏���
	// �� ���ׂẴX�e�[�g�ŁA�X�e�[�g���ŕ����ʂ��X�V����悤�ɕύX������A�����̏����͍폜���邱�ƁB�B�B

	// ���x�ω��Ȃ�
	PhyVar.UseVelVar = false;
	PhyVar.Force = Vector3D(0, 0, 0);

	// �L�����N�^�����̎蓮�ݒ��L��
	PhyVar.UseHeading = true;
	PhyVar.Heading = pEntity->Heading();

	// �ʒu�̓f�t�H���g�X�V�i���݂̑��x�ɏ]���X�V�j
	PhyVar.UsePosVar = false;

	// ## �u�ؕԂ��v�X�e�[�g�̋���
	if (!(pEntity->MoveInput().isZero()))
	{ // �X�e�B�b�N���͂���
		
		assert(pEntity->Heading().y == 0.0);

		double dbg = pEntity->Heading().normalize()*pEntity->MoveInput().normalize();
		assert(dbg >-1.0);

		// �L�����N�^�����̍X�V
		double turnSpeed = TurnSpeed * pEntity->TimeElaps();
		Vector3D newHead = RotateTowards3D(pEntity->Heading().normalize(), pEntity->MoveInput().normalize(), turnSpeed);
		
		assert(newHead.y == 0.0);

		// newHead.y > 0 �ɂȂ�Ɠ��삪�j�]���邩�������A�Ƃ������ƂŁA�����I��0�ɂ��鏈�������ėl�q���݂�...
		if (newHead.y != 0)
		{
			newHead.y = 0;
			newHead = newHead.normalize();
		}

		// �����A���܂������Ȃ��B
		// �C�������A���S���Y�������������̂�...
		// RotateTowards3D �̂Ȃ��ŁAy�x�N�g����0�`�F�b�N������悤�ɂ��Ă݂邩...

		// �Ƃ肠�����A����Ŕj�]�͔������Ȃ��悤�ɂȂ����B
		// RotateTowards3D �̃A���S���Y�����̂����������ق��������B
		
		pEntity->setHeading(newHead);

		PhyVar.Heading = pEntity->Heading(); // �� ���ׂẴX�e�[�g�ŁA�X�e�[�g���ŕ����ʂ��X�V����悤�ɕύX������A�����̏����͍폜���邱�ƁB�B�B

		// �L�����N�^���x�̑傫�����v�Z
		double SpeedAcceleration = 20.0f * pEntity->TimeElaps() * PlayerCharacterEntity::m_dConfigScaling; 
		double newSpeed = MoveTowards(pEntity->Velocity().len(), Run::runSpeed, SpeedAcceleration);

		Vector3D newVel = newSpeed * newHead;

		assert(newVel.len() < 1000.0);

		pEntity->setVelocity(newVel);

	}
	else
	{ // �X�e�B�b�N���͂Ȃ�

		// SpeedDeceleration
		double SpeedDeceleration = 15.0f * pEntity->TimeElaps() * PlayerCharacterEntity::m_dConfigScaling;
		double newSpeed = MoveTowards(pEntity->Velocity().len(), 0.0f, SpeedDeceleration);

		Vector3D newVel = newSpeed * pEntity->Heading();
		pEntity->setVelocity(newVel);

	}

	// ## �u�ؕԂ��v�X�e�[�g�̏I������
	if (pEntity->getStopWatchTime() > TurnDulation)
	{ // �ؕԂ��^�C�}����
		// �u����^�����v�X�e�[�g�ɑJ��
		//pEntity->ChangeState(SurfaceMove::Instance());
		pEntity->ChangeState(Run::Instance());
		return;
	}

	return;
}

void Turn::Render(PlayerCharacterEntity* pEntity)
{
	;
};


void Turn::Exit(PlayerCharacterEntity* pEntity)
{
	// ���S�͂ɂ��p���̌X���̉���
	pEntity->m_pAnimMgr->setBankAngle(0.0);
}




// #### SurfaceMove �X�e�[�g�̃��\�b�h ########################################################################

// ##### �萔
//const double ThresholdSpeedRunToWark = 35.0*35.0; // Running<->Warking �̑��x��臒l�i�����l�j
const double SurfaceMove::ThresholdSpeedRunToWark = 50.0*50.0; // Running<->Warking �̑��x��臒l�i�����l�j
// ����n�ߒ���ɐi�s�����𔽓]�������ꍇ�ɐؕԂ�������s��Ȃ��悤�Ƀ`���[�j���O

const double SurfaceMove::ThresholdSticktiltRunToWark = 0.6;   // Running<->Warking �̃X�e�B�b�N�X����臒l

const double SurfaceMove::MaxCentripetalForce = 500.0*10;   // ���񎞂̍ő���S��

const double SurfaceMove::MaxVelocity      = 65.0; // �L�����N�^�[�̍ő呬�x�i�X�e�B�b�N��max�܂œ|�������̍ő呬�x�j

const double SurfaceMove::ViscousRsisInert = 30.0;  // �������i���̔S����R�W��
const double SurfaceMove::ViscousRsisAccel = 30.0;  // �������̔S����R�W��
// 2016/08/18
// ����n�ߒ���ɐi�s�����𔽓]�������ꍇ�ɐؕԂ�������s��Ȃ��悤�Ƀ`���[�j���O
// �������i���Ɖ������̔S����R�𓯂��ɂ���B
// �ؕԂ����쎞�̉����͂ɍ����Ēl���������`���[�j���O����
// 2016/09/25
// ������ƁA�������������� 20 �� 30 �ɂ��Ă݂�


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
		//pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running  );
		pEntity->m_pAnimMgr->setAnimExStartTime( PlayerCharacterEntity::Running, 0.5 );

		/*
		// �T�u�X�e�[�g�̕]�����s��
		if( (pEntity->m_pVirCntrl->m_dStickL_len > ThresholdSticktiltRunToWark) 
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
		*/

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

void SurfaceMove::StateTranceDetect(PlayerCharacterEntity* pEntity)
{
	// m_bJmpChrgUsageFlg OFF
	static const double JmpChrgWaitTime = 0.1; // �W�����v�J�n�܂ł̑҂�����

	// m_bJmpChrgUsageFlg ON
	static const double JmpChrgMaxTime = 0.3;  // �W�����v�`���[�W�̍ő厞��


	// #### �W�����v�֘A
	if (pEntity->m_bJmpChrgUsageFlg)
	{ // m_bJmpChrgUsageFlg ON (�{�^���̉������ݎ��Ԃɉ����ŁA�W�����v�͒�������@�\)
		if (pEntity->m_bJmpChrgFlg)
		{ // �W�����v�`���[�W��
			// ButA �������ꂽ or �W�����v�`���[�W �̍ő厞�Ԃ𒴉߂���
			if (!pEntity->m_pVirCntrl->ButA.isPushed()
				|| pEntity->getStopWatchTime() > JmpChrgMaxTime)
			{
				// �� State��Jump�ɍX�V
				//�iJump State ��Enter�̒��Ń^�C�}�l����W�����v�̃T�C�Y���v�Z����j
				pEntity->ChangeState(Jump::Instance());
			}

		}
		else if (pEntity->m_pVirCntrl->ButA.isNowPush())
		{ // ButA �����̏u�Ԃɉ����ꂽ
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlg���グ��
			pEntity->StopWatchOn();        // �^�C�}�[�Z�b�g
		}
	}
	else
	{ // m_bJmpChrgUsageFlg OFF
		if (pEntity->m_bJmpChrgFlg)
		{ // �W�����v�`���[�W��
			// ButA �������ꂽ or �W�����v�`���[�W �̍ő厞�Ԃ𒴉߂���
			if (pEntity->getStopWatchTime() > JmpChrgWaitTime)
			{
				// State��Jump�ɍX�V
				pEntity->ChangeState(Jump::Instance());
			}

		}
		else if (pEntity->m_pVirCntrl->ButA.isNowPush())
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



	/*
	// ���x�x�N�g���i�K�i���ς݁j�ƈړ������x�N�g���̓��ϒl�����̒l�ȉ��ł���΁A�ؕԂ��Ɣ��肷��B
	//static const double InnerProductForStartTurn = 0.0;
	static const double InnerProductForStartTurn = -cos( DX_PI_F/6.0 );

	//if( pEntity->m_eMoveLevel == PlayerCharacterEntity::MvLvRunning )
	if( pEntity->Velocity().sqlen() >= ThresholdSpeedRunToWark )
	{ // �ړ����x�����ARunning�Ȃ��
		Vector3D VelDir  = pEntity->Velocity().normalize();
		Vector3D MoveDir = pEntity->MoveInput().normalize();
		Vector3D EstiDir = (pEntity->MoveInput()-VelDir).normalize();
		//if( VelDir*MoveDir <= InnerProductForStartTurn )
		if( MoveDir*VelDir<=0 && EstiDir*VelDir<=InnerProductForStartTurn )
		{
			pEntity->ChangeState( Break::Instance() );
			return ;
		}
	}
	*/

	// �ؕԂ������SuperMario64HD����������

	if (pEntity->Velocity().sqlen() >= ThresholdSpeedRunToWark)
	{
		// �z���g�͑��x�������Ⴄ���A�Ƃ肠����
		if(pEntity->MoveInput().isZero())
		{ 
			pEntity->ChangeState(Break::Instance());
			return;
		}

		// �X�e�B�b�N�̌X�������ƃL�����N�^�����̊p�x���擾
		double AngleBetween = Angle3D(pEntity->MoveInput(), pEntity->Heading());

		//�X�e�B�b�N�̌X��������(�u���[�L�J�n���́�)�L�����N�^�����Ƃ̊p��110����菬�����Ȃ�
		if (AngleBetween >= DX_PI * (11.0f / 18.0f))
		{
			pEntity->ChangeState(Break::Instance());
			return;
		}
	}

	// #### Standing�֑J�� ���̑� MoveLeve����
	static const double ThresholdSpeedForStop   = 5.0*5.0;   // Standing �ɑJ�ڂ��鑬�x��臒l�i�����l�j
	//static const double ThresholdSpeedRunToWark = 35.0*35.0; // Running<->Warking �̑��x��臒l�i�����l�j// SurfaceMove �X�e�[�g �N���X�̃����o�ɕύX
	//static const double ThresholdSticktiltRunToWark = 0.6;   // Running<->Warking �̃X�e�B�b�N�X����臒l // // SurfaceMove �X�e�[�g �N���X�̃����o�ɕύX

	pEntity->DBG_m_dDBG=pEntity->SpeedSq();


	if( (pEntity->m_pVirCntrl->m_dStickL_len > ThresholdSticktiltRunToWark)  )
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
		&& pEntity->m_pVirCntrl->m_dStickL_len==0 )
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

	static const double sqMaxCentripetalForce = MaxCentripetalForce*MaxCentripetalForce;

	static const double SensitivityCoefForTurning = 10; // ���񎞂̓��͂ɑ΂��锽���̗ǂ��B

	PhyVar.init(); // ������

	// �X�e�B�b�N�̌X���̕�������Entity�̈ړ��������v�Z����
	Vector3D vStickTiltFromCam = pEntity->MoveInput();

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

	// ���S�͂̏���l���v�Z�i�X�e�B�b�N�̌X���̑傫���ɔ�Ⴗ��悤�ɂ���j
	double LimitCentripetalForce = MaxCentripetalForce * pEntity->m_pVirCntrl->m_dStickL_len;
	//double LimitCentripetalForce = MaxCentripetalForce;

	// DBG�p�ɕϐ��ޔ�
	DBG_m_vSteeringForce = vSteeringForce;

	// ##### ���񎞂̋������P
	double DriveForce = 0;

	if( pEntity->m_eMoveLevel == PlayerCharacterEntity::MvLvWalking )
	//if( pEntity->Velocity().sqlen() < ThresholdSpeedRunToWark )
	{ // ���x�������ꍇ�͐��񋓓���K�p���Ȃ�
		
		Vector3D Force = vSteeringForce - eta * (pEntity->Velocity()) ;
		PhyVar.Force = Force ;

		assert( PhyVar.Force.y==0 );

		// DBG
		pEntity->DBG_m_bTurnWithouReduceSpeed = false;

	}
	else
	{
		// ���S�͂̌���
		m_dCentripetalForce = 0; // ���S�͂��N���A

		// ���i�͂̌v�Z
		//   �X�e�B�b�N���i�s�����Ƃ͋t�����̏ꍇ�͐��i�� FDrive ��ؕԂ��̓���̊J�n�����ł���
		//   ���x臒l�ȏ�ɂȂ�Ȃ��悤�ɒ���
		DriveForce = vSteeringForce.len();

		if( vStickTiltFromCam * pEntity->Heading() <= 0 )
		{
			double ThresholdForce = eta * ThresholdSpeedRunToWark * 0.8 ;
			if( DriveForce > ThresholdForce )
			{
				DriveForce = ThresholdForce;
			}
		}

		//Vector3D vDriveForce = DriveForce * pEntity->Heading();
	
		// ���̃^�C���X�e�b�v�̑��x�E�ʒu���v�Z
		// * �����Q�N�b�^�@�K�p
		// * ����^�����s����Ȃ̂����������������Ȃ̂ŁA
		//   DriveForce, CentripetalForce �͂��̃X�e�b�v�ŌŒ�

		Vector3D vK, vL;
		Vector3D vSumK, vSumL;
		Vector3D vVel = pEntity->Velocity(), vNxtVel;
		Vector3D vUpper = pEntity->Uppder();

		// DBG
		DBG_m_bCentripetalForceIsBounded = false; // ������

		// K1�̌v�Z
		vL = calculateForce( vVel, vUpper, vArrangeSteeringForce, DriveForce, eta, LimitCentripetalForce, m_dCentripetalForce )/pEntity->Mass();
		vK = vVel;
		vSumK = vK;
		vSumL = vL;

		// K2�̌v�Z
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, LimitCentripetalForce, m_dCentripetalForce )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K3�̌v�Z
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, LimitCentripetalForce, m_dCentripetalForce )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K4�̌v�Z
		vNxtVel = vVel+pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, LimitCentripetalForce, m_dCentripetalForce )/pEntity->Mass();
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

		// DBG
		pEntity->DBG_m_bTurnWithouReduceSpeed = true;

	}

	// #### �f�o�b�N�p�ɑޔ�

	// DBG_m_vDriveForceForVel �� DBG_m_vCentripetalForce �̑ޔ�
	Vector3D vHeading = pEntity->Heading();
	Vector3D vSide    = vHeading % pEntity->Uppder();
	pEntity->DBG_m_vCentripetalForce = m_dCentripetalForce * vSide;
	pEntity->DBG_m_vDriveForceForVel = DriveForce * vHeading;

	// DBG_m_vSteeringForce �̑ޔ�
	pEntity->DBG_m_vSteeringForce = DBG_m_vSteeringForce;

	// DBG_m_bCentripetalForceIsBounded �̑ޔ�
	// ���[�J�� DBG_m_bCentripetalForceIsBounded �� SurfaceMove::calculateForce ���Ŏ��W���Ă���B
	pEntity->DBG_m_bCentripetalForceIsBounded = DBG_m_bCentripetalForceIsBounded; 

	// DBG_m_bTurnWithouReduceSpeed �͏������őޔ�


	return;
}

// SurfaceMove::Calculate�̕⏕�֐�
// 2016/09/04
//   ����n�߂�Entity�������U������΍�Ƃ��āA
//   CentripetalForce�@�ł͂Ȃ��AvSteeringForce ��n���A
//   ���̓s�x CentripetalForce ���v�Z����p�ɕύX�B
// 2017/04/22
//   �����炩���t�@�N�^�����O
Vector3D SurfaceMove::calculateForce( 
		Vector3D vVel, 
		Vector3D vUpper,
		Vector3D vArrangeSteeringForce,
		double DriveForce,
		double eta,
		double LimitCentripetalForce,
		double &outCentripetalForce
		)
{
	// Entity���x(Input)���瑀�Ǘ͂̌������Čv�Z
	Vector3D vHeading = vVel.normalize();
	Vector3D vSide    = vHeading % vUpper;

	// vArrangeSteeringForce ���� CentripetalForce ���v�Z
	double CentripetalForce = vArrangeSteeringForce * vSide ;

	// �J�b�g�I�t����
	// DBG_m_bCentripetalForceIsBounded �̐ݒ�̓f�o�b�O�p
	if( cutoff( CentripetalForce, LimitCentripetalForce) ) DBG_m_bCentripetalForceIsBounded = true;

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
		//pEntity->m_pAnimMgr->setPitch((float)((14.0/12.0)*MaxVelocity)); // �n�[�h�R�[�f�B���O�̓}�Y�C�̂�
		// �� ���ꂾ�Ƃ�����Ƃ��߂�����... ���ƁA�}���I�ł͂����Ă邩������Ȃ����ǁA���̃L�����N�^�[���ƍ���Ȃ�����...

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
//const double OneEightyDegreeTurn::MaxVelocity      = 65.0;  // �L�����N�^�[�̍ő呬�x�i�X�e�B�b�N��max�܂œ|�������̍ő呬�x�j
//const double OneEightyDegreeTurn::ViscousRsisTurn   = 40.0;  // �S����R�W��
//const double OneEightyDegreeTurn::ViscousRsisBreak  = 25.0;  // �S����R�W��

const double OneEightyDegreeTurn::BreakPreDulation = 0.1;  // �u���[�L�����iSUB_BREAK_PRE�j�̌p������
const double OneEightyDegreeTurn::TurnRotDulation  = 0.3;  // �ؕԂ���]�iSUB_TURN_ROT�j�̌p������
const double OneEightyDegreeTurn::TurnFlyDulation  = 0.1;  // �ؕԂ����ˁiSUB_TURN_FLY�j�̌p������

const double OneEightyDegreeTurn::InnerProductForStartTurn = 0.0; // ���x�x�N�g���i�K�i���ς݁j�ƈړ������x�N�g���̓��ϒl�����̒l�ȉ��ł���΁A�ؕԂ��Ɣ��肷��B

const double OneEightyDegreeTurn::TurningForceSize = 65.0 * 90.0 ;  // �؏o�����̉����͑傫��
const double OneEightyDegreeTurn::BrakingForceSize = 65.0 * 10.0 * 3.5 ;  // �u���[�L���̐����͂̑傫��
// ���́A���̑傫�����ǂ̂悤�Ɍ��߂邩�H�H
// �S����R * �ō����x �������͂Ȃ̂ł���ɍ��킹��

const double OneEightyDegreeTurn::SqSlowDownEnough = 5.0 * 5.0;  // �u���[�L���(SUB_BREAK_STAND/SUB_BREAK_TURN)�̏I�������Ŏg�p�B


OneEightyDegreeTurn* OneEightyDegreeTurn::Instance()
{
	static OneEightyDegreeTurn instance;
	return &instance;
}

void OneEightyDegreeTurn::Enter( PlayerCharacterEntity* pEntity )
{
	// �T�u�X�e�[�g�̏�����
	m_eSubState = SUB_BREAK_PRE;

	// �ؕԂ�������J�n�������̑��x�������L�^
	m_vVelDirBeginning = pEntity->Velocity().normalize();

	// �u���[�L��Ԃ̃^�C�}�I��
	pEntity->StopWatchOn();

	// �A�j���[�V�����̐ݒ�
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Breaking, 0.0 ); // �u���[�L�̃A�j���[�V������ݒ�
	//pEntity->m_pAnimMgr->setPitch( 20.0 );

	// BrakingDulation ���ԓ��ɃA�j���[�V�����Đ�����������悤�ɍĐ��s�b�`�𒲐�����
	float AnimTotalTime = pEntity->m_pAnimMgr->getCurAnimLength(); 
	//float PlayPitch = (float)(AnimTotalTime/BrakingDulation) + 1.0;
	float PlayPitch = (float)(AnimTotalTime/BreakPreDulation) ; // +1.0�������Ă݂�B�Ȃ�Ő����Ă���񂾂낤�H
	pEntity->m_pAnimMgr->setPitch(PlayPitch);

	// DBG_m_SubStateDurations ���������i�S��0�ɃZ�b�g����j
	DBG_m_SubStateDurations.assign( 6, 0 );

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
	if( m_eSubState == SUB_BREAK_PRE
	 || m_eSubState == SUB_BREAK_TURN 
     || m_eSubState == SUB_BREAK_STAND )
	{ // �u���[�L���

		// �X�e�B�b�N�̌X���������i�ؕԂ��J�n���́j���x�����ɓ|���ꂽ��i�߂��ꂽ��j�A 
		// �ؕԂ����L�����Z�����đ���̏�Ԃɖ߂�B
		Vector3D MoveDir = pEntity->MoveInput().normalize();
		if( m_vVelDirBeginning*MoveDir > InnerProductForStartTurn )
		{
			pEntity->ChangeState( SurfaceMove::Instance() );
			return ;
		}

		// Substate��[Enter]�ŃT�u�X�e�[�g�̕]���irun��work���j���s�����Ɓ�

	}

	// #### �T�u��ԂɊւ��Ă̏�ԑJ�ڏ���

	// 23:44 �����́A���ꂭ�炢�ɂ��邩�B�� �����Q�܂��傤�I

	if( m_eSubState == SUB_BREAK_PRE )
	{ // �u���[�L����
		// �y�I�������z���̃T�u��ԂɑJ�ڂ��Ă����莞�Ԍo�ߌ�
		if( pEntity->getStopWatchTime() > BreakPreDulation )
		{
			// �y�J�ڐ�State�̔���z
			// �I�����̃X�e�B�b�N�̌X���̏�ԂŁA���̑J�ڏ�Ԃ����肷��B
			if( pEntity->m_pVirCntrl->m_dStickL_len==0 )
			{
				// �X�e�B�b�N�̌X���� 0
				// �� �ؕԂ��Ȃ��u���[�L(SUB_BREAK_STAND)�ցB
				m_eSubState = SUB_BREAK_STAND;
			}
			else
			{
				// �X�e�B�b�N�̌X���� 0 �łȂ�
				// �� �ؕԂ�����u���[�L(SUB_BREAK_TURN) �ցB
				m_eSubState = SUB_BREAK_TURN;
			}

			// �؏o�������"����"������Ƃ��āA�؏o������J�n�������̃X�e�B�b�N�̌����i�K�i���j�ɐݒ�
			m_vTurnDestination = pEntity->MoveInput().normalize();

			// �p�����Ԃ����ԋL�^
			DBG_m_SubStateDurations[SUB_BREAK_PRE] = pEntity->getStopWatchTime();

			// �^�C�}������
			pEntity->StopWatchOn();

		}

	}
	else if( m_eSubState == SUB_BREAK_STAND )
	{ // �ؕԂ��Ȃ��u���[�L
		// �y�I�������z���x��0�ɂȂ�܂ŁB
		if( pEntity->Velocity().sqlen() < SqSlowDownEnough )
		{
			// �ؕԂ������ԁiOneEightyDegreeTurn�j���甲���Ċ�{�����ԁiSurfaceMove�j�֑J��
			pEntity->ChangeState( Standing::Instance() );

			// �p�����Ԃ����ԋL�^
			DBG_m_SubStateDurations[SUB_BREAK_STAND] = pEntity->getStopWatchTime();

			return ;
		}
	}
	else if( m_eSubState == SUB_BREAK_TURN )
	{ // �ؕԂ�����u���[�L
		// �y�I�������z���x��0�ɂȂ�܂ŁB

		// 2016/11/27
		// �P���ɑ��x�̐�Βl�Ŕ��肷������ł́A�u���[�L�͂ɂ������ɂ��
		// ���x���ؕԂ������Ɍ����Ă��܂��ƃT�u��Ԃ��甲����Ȃ��Ȃ��Ă��܂�
		// ���Ƃ����邽�߁A�u���[�L�����Ƃ̓��ϒl�����l�ɂȂ�悤�ɏ�����ύX����B

		//if( pEntity->Velocity().sqlen() < SqSlowDownEnough )
		if( pEntity->Velocity() * m_vVelDirBeginning < 0 )
		{
			// �ؕԂ���]�iSUB_TURN_ROT�j�T�u��Ԃ֑J��
			m_eSubState = SUB_TURN_ROT;

			// �p�����Ԃ����ԋL�^
			DBG_m_SubStateDurations[SUB_BREAK_TURN] = pEntity->getStopWatchTime();

			// �^�C�}�I��
			pEntity->StopWatchOn();

			// �i�u���[�L��́j�ؕԂ��J�n���̑��x���L��
			m_vVelEnterTurning = pEntity->Velocity();

			// �ؕԂ�����̃A�j���[�V�������Đ�
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::TurnFirst );

			// TurningDulation ���ԓ��ɃA�j���[�V�����Đ�����������悤�ɍĐ��s�b�`�𒲐�
			float AnimTotalTime = pEntity->m_pAnimMgr->getCurAnimLength(); 
			//float PlayPitch = (float)(AnimTotalTime/TurningDulation) + 1.0;
			float PlayPitch = (float)(AnimTotalTime/TurnRotDulation) ;
			pEntity->m_pAnimMgr->setPitch(PlayPitch);

			return ;

		}
	}
	else if( m_eSubState == SUB_TURN_ROT )
	{ // �ؕԂ���]
		// �y�I�������z���̃T�u��ԂɑJ�ڂ��Ă����莞�Ԍo�ߌ�
		if( pEntity->getStopWatchTime() > TurnRotDulation )
		{
			// �ؕԂ����ˁiSUB_TURN_FLY�j�T�u��Ԃ֑J��
			m_eSubState = SUB_TURN_FLY;

			// �p�����Ԃ����ԋL�^
			DBG_m_SubStateDurations[SUB_TURN_ROT] = pEntity->getStopWatchTime();

			// �^�C�}�I��
			pEntity->StopWatchOn();

			// �ؕԂ����Ȃ����яo���A�j���[�V�������Đ�
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::TurnLatter );
			pEntity->m_pAnimMgr->ReserveAnim( PlayerCharacterEntity::TurnFinalFly, 2.0 );

			// TurnFlyDulation ���ԓ��ɃA�j���[�V�����Đ�����������悤�ɍĐ��s�b�`�𒲐�
			//float AnimTotalTime = pEntity->m_pAnimMgr->getCurAnimLength(); 
			float AnimTotalTime = 9.0 + 5.0 ; // TurnLatter�̎��� + TurnFinalFly�̎��ԁB�n�[�h�R�[�f�B���O�ŃX�~�}�Z���B
			float PlayPitch = (float)(AnimTotalTime/TurnFlyDulation) ;
			pEntity->m_pAnimMgr->setPitch(PlayPitch);

		}
	}
	else if( m_eSubState == SUB_TURN_FLY )
	{ // �ؕԂ�����
		// �y�I�������z���̃T�u��ԂɑJ�ڂ��Ă����莞�Ԍo�ߌ�
		if( pEntity->getStopWatchTime() > TurnFlyDulation )
		{
			// �ؕԂ����X�g�iSUB_TURN_FIN�j�T�u��Ԃ֑J��
			m_eSubState = SUB_TURN_FIN;

			// �p�����Ԃ����ԋL�^
			DBG_m_SubStateDurations[SUB_TURN_FLY] = pEntity->getStopWatchTime();

			// �^�C�}�I��
			pEntity->StopWatchOn();

			return;
		}
	}
	else if( m_eSubState == SUB_TURN_FIN )
	{ // �ؕԂ����X�g�i���x�����̒����Ɏg�p�j
		// �y�I�������z���x�𒲐����邽�߂݂̂̃T�u��ԂȂ̂Œ����Ɏ��̏�ԂɑJ�ڂ���B
		// �ؕԂ������ԁiOneEightyDegreeTurn�j���甲���Ċ�{�����ԁiSurfaceMove�j�֑J��
		pEntity->ChangeState( SurfaceMove::Instance() );

		// �p�����Ԃ����ԋL�^
		DBG_m_SubStateDurations[SUB_TURN_FIN] = pEntity->getStopWatchTime();

		return;
	}

	// #### �f�o�b�O�p
	if( m_eSubState == SUB_TURN_ROT || m_eSubState == SUB_TURN_FLY )
	{
		pEntity->DBG_m_vTurnDestination = m_vTurnDestination;
	}
	else
	{
		pEntity->DBG_m_vTurnDestination = Vector3D( 0,0,0 );
	}


}

void OneEightyDegreeTurn::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	// �u���[�L�́A�ؕԂ����̐��i�͂́A
	// Entity�̏I���x�ƁA�S����R�W���i=�I���x�ւ̓��B���ԁj���v�Z����B

	// �ؕԂ��������̍ő呬�x��ݒ�
	static double EndSpeed = 0.5 * (SurfaceMove::MaxVelocity + sqrt(SurfaceMove::ThresholdSpeedRunToWark) );

	PhyVar.init(); // ������

	if( m_eSubState == SUB_BREAK_PRE
	 || m_eSubState == SUB_BREAK_TURN 
     || m_eSubState == SUB_BREAK_STAND )
	{ // �u���[�L����
		// ���x���\���������Ȃ�܂Ńu���[�L��������B

		// �u���[�L�͂��v�Z
		Vector3D vBreakingForce = -1 * BrakingForceSize * m_vVelDirBeginning;

		// �S����R���f������Entity�ɓ����͂��v�Z
		PhyVar.Force = vBreakingForce - SurfaceMove::ViscousRsisAccel * (pEntity->Velocity());

		// �������Œ�
		PhyVar.Heading = m_vVelDirBeginning;
		PhyVar.UseHeading = true;

		return;

	}
	else if( m_eSubState == SUB_TURN_ROT )
	{ // �ؕԂ���]����
		// �؂�Ԃ��̕����ɉ�������B�����Ɍ�����ؕԂ������։�]�B

		// ##### Entity�ɓ����͂��v�Z


		// �ؕԂ��������̖ڕW���x
		Vector3D vTargetVelTurnEnd =
			EndSpeed
			* pEntity->m_pVirCntrl->m_dStickL_len
			* m_vTurnDestination;

		// �ؕԂ������ԓ��ɏI���x�܂ŉ�������悤�ɂ���
		double TurnRemainingTime = TurnRotDulation+TurnFlyDulation;
		PhyVar.VelVar = (vTargetVelTurnEnd-m_vVelEnterTurning) / TurnRemainingTime ;
		PhyVar.UseVelVar  = true;

		// ##### Entity�����̉�]�ʂ��v�Z
		// * TurningDulation �Ō����̉�]����������悤�ɁA��]���x�𒲐�

		// ���݂�Heading�ƁA�ړI�̕����̃x�N�g���̂Ȃ��p���v�Z
		double angle = atan2( 
			pEntity->Side()    * m_vTurnDestination, 
			pEntity->Heading() * m_vTurnDestination );

		// ����ꂽ�p�x�����l�̏ꍇ�́A�t��������̊p�x�ɕϊ��i����ɂ��A��]�������Œ肳���j
		if( angle < 0 )
		{
			angle = 2*DX_PI_F + angle;
		}

		// ���݂̉�]�ʒu�ƁA��]�����܂ł̎c�莞�Ԃ��A����̃^�C���X�e�b�v�ɂ������]�ʂ��v�Z
		double RemainingTime = TurnRotDulation - pEntity->getStopWatchTime();
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
	else if( m_eSubState == SUB_TURN_FLY )
	{ // �ؕԂ����˓���
		// �L�����N�^�����̉�]�͂��Ȃ��B����ȊO�̓���͐ؕԂ���](SUB_TURN_ROT)�Ɠ����B

		// ##### Entity�ɓ����͂��v�Z

		// �ؕԂ��������̖ڕW���x
		Vector3D vTargetVelTurnEnd =
			EndSpeed
			* pEntity->m_pVirCntrl->m_dStickL_len
			* m_vTurnDestination;

		// �ؕԂ������ԓ��ɏI���x�܂ŉ�������悤�ɂ���
		double TurnRemainingTime = TurnRotDulation+TurnFlyDulation;
		PhyVar.VelVar = (vTargetVelTurnEnd-m_vVelEnterTurning) / TurnRemainingTime ;
		PhyVar.UseVelVar  = true;

		// ##### Entity�����̉�]�ʂ��v�Z

		// Entity�̌�����ؕԂ������Ɍ�����B
		// �i�m�[�����e���ƁA�܂����x���ؕԂ������֌����Ă��Ȃ��ꍇ������AEntity���ςȓ���������̂Łj

		// �V����Heading�x�N�g����ݒ肵����
		PhyVar.Heading = m_vTurnDestination;
		PhyVar.UseHeading = true;
		

	}
	else if( m_eSubState == SUB_TURN_FIN )
	{ // �ؕԂ����X�g�i���x�����̒����Ɏg�p�j

		// �؂�Ԃ�����̊�������
		// ���ڑ��x������؂�Ԃ������Ɍ�����悤�ɕ␳����

		// ##### Entity�ɓ����͂��v�Z

		// �ڕW���x�Ƃ��āA�؂�Ԃ������ɕ␳����
		Vector3D vTargetVelTurnEnd = (pEntity->Velocity()*m_vTurnDestination)*m_vTurnDestination;

		// ���̏����X�e�b�v�ŖڕW���x�ɍX�V�����悤�ɉ����x�𒲐�����
		double TurnRemainingTime = pEntity->TimeElaps();
		PhyVar.VelVar = (vTargetVelTurnEnd-pEntity->Velocity()) / TurnRemainingTime ;
		PhyVar.UseVelVar  = true;

		// ##### Entity�����̉�]�ʂ��v�Z

		// �V����Heading�x�N�g����ݒ肵����
		PhyVar.Heading = m_vTurnDestination;
		PhyVar.UseHeading = true;


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
	case OneEightyDegreeTurn::SUB_BREAK_PRE:    // �u���[�L����
		SubStateString = "SUB_BREAK_PRE";
		break;
	case OneEightyDegreeTurn::SUB_BREAK_STAND:  // �ؕԂ��Ȃ��u���[�L
		SubStateString = "SUB_BREAK_STAND";
		break;
	case OneEightyDegreeTurn::SUB_BREAK_TURN:   // �ؕԂ�����u���[�L
		SubStateString = "SUB_BREAK_TURN";
		break;
	case OneEightyDegreeTurn::SUB_TURN_ROT:     // �ؕԂ���]
		SubStateString = "SUB_TURN_ROT";
		break;
	case OneEightyDegreeTurn::SUB_TURN_FLY:     // �ؕԂ�����
		SubStateString = "SUB_TURN_FLY";
		break;
	case OneEightyDegreeTurn::SUB_TURN_FIN:   // �ؕԂ����X�g�i���x�����̒����Ɏg�p�j
		SubStateString = "SUB_TURN_FIN";
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

// �e�T�u��Ԃ̌p�����Ԃ��o�͂���
void OneEightyDegreeTurn::DBG_expSubStateDurations( int &c )
{
	int width = 15;
	DrawFormatString( 0, width*c, 0xffffff, "SUB_BREAK_PRE  :%f", DBG_m_SubStateDurations[SUB_BREAK_PRE] ); 
	c++;
	DrawFormatString( 0, width*c, 0xffffff, "SUB_BREAK_STAND:%f", DBG_m_SubStateDurations[SUB_BREAK_STAND] ); 
	c++;
	DrawFormatString( 0, width*c, 0xffffff, "SUB_BREAK_TURN :%f", DBG_m_SubStateDurations[SUB_BREAK_TURN] ); 
	c++;
	DrawFormatString( 0, width*c, 0xffffff, "SUB_TURN_ROT   :%f", DBG_m_SubStateDurations[SUB_TURN_ROT] ); 
	c++;
	DrawFormatString( 0, width*c, 0xffffff, "SUB_TURN_FLY   :%f", DBG_m_SubStateDurations[SUB_TURN_FLY] ); 
	c++;
	DrawFormatString( 0, width*c, 0xffffff, "SUB_TURN_FIN   :%f", DBG_m_SubStateDurations[SUB_TURN_FIN] ); 
	c++;

};


// #### Run �X�e�[�g�̃��\�b�h ########################################################################
const double Run::runSpeed = 7.0 * PlayerCharacterEntity::m_dConfigScaling;  // ����̍ō����x�i�X�e�B�b�N���ő���ɓ|�����Ƃ��ɷ�׸����o����ō����x�j
//const double Run::runSpeed = 7.0 * 9.0;  // ����̍ō����x�i�X�e�B�b�N���ő���ɓ|�����Ƃ��ɷ�׸����o����ō����x�j
// �������炭���ꂪ���������B�ǂ����邩�B

Run* Run::Instance()
{
	static Run instance;
	return &instance;
}

void Run::Enter(PlayerCharacterEntity* pEntity)
{
	// �A�j���[�V�����̐ݒ�B
	// ����o�������R�ɂ���悤�ɃA�j���[�V�����J�n�ʒu�ƃu�����h���{
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Walking, 5.0);

}

void Run::StateTranceDetect(PlayerCharacterEntity* pEntity)
{
	// �p���\��̃��\�b�h
}

void Run::Calculate(PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar)
{
	PhyVar.init(); // ������

	// ## �L�����N�^��Update()�ł̕����ʂ̍X�V������������邽�߂̏���

	// �L�����N�^��Update()���ő��x�̍X�V�������s��Ȃ��悤�ɂ���B�i��State����setVelocity�Őݒ肷��j
	PhyVar.UseVelVar = false;
	PhyVar.Force = Vector3D(0, 0, 0);

	// �L�����N�^������ PhyVar.Heading �̒l��ݒ肷��B
	PhyVar.UseHeading = true;
	PhyVar.Heading = pEntity->Heading(); // �����ł��ݒ肵�Ă����Ȃ��ƁA�r���ŁA��ԑJ�ڂ����Ƃ��ɁAheading�޸�ق���������B

	// �ʒu�̓f�t�H���g�X�V�i���݂̑��x�ɏ]���X�V�j
	PhyVar.UsePosVar = false;
	
	// ���x�޸�ق�傫���ƕ����𕪗�
	double   VelSiz = pEntity->Velocity().len();
	// Vector3D VelDir = pEntity->Velocity() / VelSiz; <-����A�����Ăˁ[�ȁB
 
	if (!(pEntity->MoveInput().isZero()))
	{ // �X�e�B�b�N���͂���

		// �X�e�B�b�N����̓��͕������A�L�����N�^�̌����Ɣ��Ε����������Ă����ꍇ�B
		// �����ɂ͂ɂ� �X�e�B�b�N����̓��͕��� �� �L�����N�^�̌��� �̊p�x���A110�������傫�������ꍇ�B
		static double ThresholdAngle = DX_PI * (11.0f / 18.0f); // �� �~ 110/180 ���W�A�� = 110��
		if (Angle3D(pEntity->MoveInput(), pEntity->Heading()) > ThresholdAngle)
		{
			// �L�����N�^�̈ړ����x���A���l�ȏ�̏ꍇ�͋}�u���[�L��ԂɑJ�ڂ���return�B
			if (VelSiz > runSpeed * 0.56)
			{
				pEntity->ChangeState(Break::Instance());
				return;
			}

			// �����A�L�����N�^�ړ����x���A0�̏ꍇ�́A�L�����N�^�̌������X�e�B�b�N�̓��͕����փZ�b�g����B
			else if(VelSiz == 0)
			{
				pEntity->setHeading(pEntity->MoveInput().normalize());
			}
		}

		// ## ���蓮��ɂ��L�����N�^�̕����p�����[�^�̍X�V�����{

		// �L�����N�^�������A�X�e�B�b�N�̓��͕����։�]������B�i��]���x�FturnSpeed�Łj
		double turnSpeed = Turn::TurnSpeed * pEntity->TimeElaps();
		Vector3D newHead = RotateTowards3D(pEntity->Heading().normalize(), pEntity->MoveInput().normalize(), turnSpeed);
		assert(newHead.y == 0.0); // ���퐫chk
		pEntity->setHeading(newHead);

		// # �L�����N�^���x�̑傫���̍X�V

		// BoundedInterpolation�ɓ��͂���A���ݑ��x�������x�̓��o�֐��̔z��̒�`
		static vector<double> Bounds{ 1.3, 3.0, 6.0 }; // C++11���̏��������@�B���̂����g����̂��H
		static vector<double> Values{ 16.0, 12.0, 8.0, 6.0 };
		
		// �����x�ior�����x�j�̌v�Z
		double targetSpeed = runSpeed * pEntity->MoveInput().len();
		double acceration = targetSpeed >= VelSiz ? BoundedInterpolation(Bounds, Values, VelSiz / PlayerCharacterEntity::m_dConfigScaling) : 16.0; // ��VelSiz���X�P�[�����O���Ȃ��ƁA�v���悤�ɓ����Ȃ���B
		//double acceration = targetSpeed >= VelSiz ? BoundedInterpolation(Bounds, Values, VelSiz ) : 16.0 ; // ��VelSiz���X�P�[�����O���Ȃ��ƁA�v���悤�ɓ����Ȃ���B
		acceration *= PlayerCharacterEntity::m_dConfigScaling; // SM64HD�̐��E����̃X�P�[�����O

		// �ڒn���Ă���n�\�ɁA�u�ă}�b�s���O�v���Ă���B
		// �� ����ł͕��ʏ󂵂������Ȃ��̂ŁA���̓I�~�b�g����B

		// ���x�̍X�V
		double NewVelSiz = MoveTowards(VelSiz, targetSpeed, acceration*pEntity->TimeElaps());
		pEntity->setVelocity(NewVelSiz*newHead);

	}
	else
	{ // �X�e�B�b�N����̓��͂�0�̏ꍇ�B
		if (VelSiz > runSpeed * 0.66)
		{
			// �ړ����x����葬�x�ȏ�ł���΁A�}�u���[�LState(StopState)�ɑJ�ڂ���return�B
			// - ���̎��̂������l�́A�X�e�B�b�N�̓��͂��L��̂Ƃ�(0.56f)��0�̎�(0.66f)�ō������L�邱�Ƃɒ��ӁB
			pEntity->ChangeState(Break::Instance());
			return;
		}
		else
		{
			// �����łȂ��ꍇ�͑ҋ@��ԂɑJ�ڂ���return�B
			// �ړ����x�����S��0�ɂȂ��Ă��Ȃ��Ă��A�ҋ@��ԂɑJ�ڂ��邽�߁A�ҋ@��Ԃ̓���ŁA��������������ƌ������Ƃ��H
			pEntity->ChangeState(Standing::Instance());
			return;
		}

	}

	// �����ʂ��A�L�����N�^��Update()���Ő������X�V�����邽�߂̏����B
	PhyVar.Heading = pEntity->Heading(); // �� ���ׂẴX�e�[�g�ŁA�X�e�[�g���ŕ����ʂ��X�V����悤�ɕύX������A�����̏����͍폜���邱�ƁB�B�B

	// �A�j���[�V�����֌W�̏����͂��̂܂܈ڐA�B

	// ## �u�����vor�u����v�̃A�j���[�V�����̐ݒ�

	if (pEntity->m_pVirCntrl->m_dStickL_len > 0.5)
	{ // �X�e�B�b�N�̓��͂̑傫���� 0.5 ���傫����΁A
		
		// �u����v�̃A�j���[�V�������Đ�����Ă��Ȃ���΁A�Đ�����B
		if (pEntity->m_pAnimMgr->getCurAnimID() != PlayerCharacterEntity::Running)
		{
			pEntity->m_pAnimMgr->setAnim(
				PlayerCharacterEntity::Running,
				8.0,
				false,
				true); // �ʑ���ۂ��Ȃ���؂�ւ��i�u�����h�j
		}

	}
	else
	{ // �X�e�B�b�N�̓��͂̑傫���� 0.5 ��菬������΁A
		//�u�����v�̃A�j���[�V�������Đ�����Ă��Ȃ���΁A�Đ�����B
		if (pEntity->m_pAnimMgr->getCurAnimID() != PlayerCharacterEntity::Walking)
		{
			pEntity->m_pAnimMgr->setAnim(
				PlayerCharacterEntity::Walking,
				8.0,
				false,
				true); // �ʑ���ۂ��Ȃ���؂�ւ��i�u�����h�j
		}
	}

	// ��׸��̈ړ����x�ɉ������A�j���[�V�����Đ����x�ɂȂ�悤�ɁA�Đ��s�b�`�𒲐�����B
	double speed = pEntity->Speed();

	// Running �� Warking �ōĐ��s�b�`���킯��
	if (pEntity->m_pAnimMgr->getCurAnimID() == PlayerCharacterEntity::Running)
	{
		pEntity->m_pAnimMgr->setPitch((float)speed);
	}
	else if (pEntity->m_pAnimMgr->getCurAnimID() == PlayerCharacterEntity::Walking)
	{
		pEntity->m_pAnimMgr->setPitch((float)((14.0 / 12.0)*speed)); // �n�[�h�R�[�f�B���O�̓}�Y�C�̂�
	}

	// ����ɂ��̂̌X���i�o���N�j�̐ݒ�
	static const double LikeGravity = 500.0 * 10 * 5; // �o���N�p�̌v�Z�Ɏg�p�B�d�͂ɑ�������B
	double CentripetalForce = VelSiz * Turn::TurnSpeed; // ���蓮���SM64HD�������Ńo���N�p�̐ݒ聨���S�͂��g�p�ł��Ȃ��Ȃ�̂ŁA�p���x�i���񑬓x�j�{���݂̑��x����v�Z����K�v������B
	static Smoother<double> BankAngleSmoother(6, 0);  // �~����
	double bankangle = atan2(CentripetalForce, LikeGravity); // *�d�͂Ɖ��S�͂ɂ��o���N�p�̌v�Z
	pEntity->m_pAnimMgr->setBankAngle(BankAngleSmoother.Update(-bankangle));


}

void Run::Render(PlayerCharacterEntity* pEntity)
{
	;
};


void Run::Exit(PlayerCharacterEntity* pEntity)
{

	// ���S�͂ɂ��p���̌X���̉���
	pEntity->m_pAnimMgr->setBankAngle(0.0);
}

// �悵�A�����������H
// ����A�������B
// 20:12 �R�[�f�B���O�����B����œ������낤���H
// �����Ă���I
// �Ȃ񂩓����ˁ[�B