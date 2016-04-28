#include "State.h"
#include "AnimationManager.h"
#include "VirtualController.h"

//static const double EPS = 1e-5;
static const double EPS = 0.5;

void State::Rotate( 
		double RotateVelSpeed, 
		PlayerCharacterEntity* pEntity, 
		PhysicalQuantityVariation& PhyVar )
{
	// 横キーが押されていれば旋回ベクトルを設定
	double KeyHoriz = -1.0 * (double)pEntity->m_pVirCntrl->Horiz;
	PhyVar.DstVar = KeyHoriz * RotateVelSpeed * pEntity->Side();
};

// #### Dammy ステートのメソッド ####
Dammy* Dammy::Instance()
{
	static Dammy instance;
	return &instance;
}

void Dammy::Enter( PlayerCharacterEntity* pEntity )
{
	// アニメーションを設定
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running);
}

void Dammy::StateTranceDetect( PlayerCharacterEntity* pEntity )
{
	;
}

void Dammy::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	PhyVar.init(); // 初期化

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

// #### Standing ステートのメソッド ####
Standing* Standing::Instance()
{
	static Standing instance;
	return &instance;
}

void Standing::Enter( PlayerCharacterEntity* pEntity )
{
	// アニメーションを設定
	if( pEntity->isMatchPrvState( Jump::Instance() ) )
	{
		// ジャンプ後であれば、着地のアニメーションを再生する
		pEntity->m_pAnimMgr->setPitch(20.0);
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_Landing);
		pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::Standing);
	}
	else
	{
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Standing, 5.0 );
		pEntity->m_pAnimMgr->setPitch( 20.0 );
	}

	// m_bJmpChrgFlg を初期化
	pEntity->m_bJmpChrgFlg = false; 
}

void Standing::StateTranceDetect( PlayerCharacterEntity* pEntity )
{
	// コントローラー入力があったら、StateをRunningに変更
	if( pEntity->m_pVirCntrl->Virti > 0 )
	{
		pEntity->ChangeState( Running::Instance() );
	}

	// m_bJmpChrgUsageFlg OFF
	static const double JmpChrgWaitTime = 0.1; // ジャンプ開始までの待ち時間
	
	// m_bJmpChrgUsageFlg ON
	static const double JmpChrgMaxTime = 0.3;  // ジャンプチャージの最大時間

	// #### ジャンプ関連
	if( pEntity->m_bJmpChrgUsageFlg )
	{
		if( pEntity->m_bJmpChrgFlg )
		{ // ジャンプチャージ中
			// ButA が離された or ジャンプチャージ の最大時間を超過した
			if( !pEntity->m_pVirCntrl->ButA.isPushed() 
				|| pEntity->getStopWatchTime() > JmpChrgMaxTime )
			{
				// → StateをJumpに更新
				//（Jump State のEnterの中でタイマ値からジャンプのサイズを計算する）
				pEntity->ChangeState( Jump::Instance() );
			}

		}
		else if( pEntity->m_pVirCntrl->ButA.isNowPush() )
		{ // ButA がこの瞬間に押された
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlgを上げる
			pEntity->StopWatchOn();        // タイマーセット
		}
	}
	else
	{
		if( pEntity->m_bJmpChrgFlg )
		{ // ジャンプチャージ中
			// ButA が離された or ジャンプチャージ の最大時間を超過した
			if( pEntity->getStopWatchTime() > JmpChrgWaitTime )
			{
				// StateをJumpに更新
				pEntity->ChangeState( Jump::Instance() );
			}

		}
		else if( pEntity->m_pVirCntrl->ButA.isNowPush() )
		{ // ButA がこの瞬間に押された
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlgを上げる
			pEntity->StopWatchOn();        // タイマーセット

			// ジャンプ前のアニメーションを設定
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_PreMotion);
			float AnimTotalTime = pEntity->m_pAnimMgr->getMotionTotalTime(); // JmpChrgWaitTime 時間内にアニメーション再生が完了するように再生ピッチを調整
			float PlayPitch = (float)(AnimTotalTime/JmpChrgWaitTime);
			pEntity->m_pAnimMgr->setPitch(PlayPitch);
		}
	}

}

void Standing::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	static const double ViscousResistance =  40.0; // 粘性抵抗係数（Runningの粘性抵抗より大きくとっている）
	static const double RotateVelSpeed    =   6.0; // 旋回速度

	PhyVar.init(); // 初期化

	// 完全に速度が0に落ちきっていないため、減速は継続
	PhyVar.Force = -ViscousResistance * pEntity->Velocity() ;

	// 止まっていても旋回は可能
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


// #### Running ステートのメソッド ####
Running* Running::Instance()
{
	static Running instance;
	return &instance;
}

void Running::Enter( PlayerCharacterEntity* pEntity )
{
		// アニメーションを設定
	if( pEntity->isMatchPrvState( Jump::Instance() ) )
	{
		// ジャンプ後であれば、着地のアニメーションを再生する
		pEntity->m_pAnimMgr->setPitch(40.0);
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_Landing_Short);
		//pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running, 20.0, false );
		pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::Running, 5.0, false );
	}
	else
	{
		// 走り出しを自然にするようにアニメーション開始位置とブレンド実施
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running, 10.0 );
	}

	// m_bJmpChrgFlg を初期化
	pEntity->m_bJmpChrgFlg = false; 

	// MoveLevelを設定
	pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvRunning;
}

void Running::StateTranceDetect( PlayerCharacterEntity* pEntity )
{
	// m_bJmpChrgUsageFlg OFF
	static const double JmpChrgWaitTime = 0.1; // ジャンプ開始までの待ち時間

	// m_bJmpChrgUsageFlg ON
	static const double JmpChrgMaxTime = 0.3;  // ジャンプチャージの最大時間

	// #### ジャンプ関連
	if( pEntity->m_bJmpChrgUsageFlg )
	{
		if( pEntity->m_bJmpChrgFlg )
		{ // ジャンプチャージ中
			// ButA が離された or ジャンプチャージ の最大時間を超過した
			if( !pEntity->m_pVirCntrl->ButA.isPushed() 
				|| pEntity->getStopWatchTime() > JmpChrgMaxTime )
			{
				// → StateをJumpに更新
				//（Jump State のEnterの中でタイマ値からジャンプのサイズを計算する）
				pEntity->ChangeState( Jump::Instance() );
			}

		}
		else if( pEntity->m_pVirCntrl->ButA.isNowPush() )
		{ // ButA がこの瞬間に押された
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlgを上げる
			pEntity->StopWatchOn();        // タイマーセット
		}
	}
	else
	{
		if( pEntity->m_bJmpChrgFlg )
		{ // ジャンプチャージ中
			// ButA が離された or ジャンプチャージ の最大時間を超過した
			if( pEntity->getStopWatchTime() > JmpChrgWaitTime )
			{
				// StateをJumpに更新
				pEntity->ChangeState( Jump::Instance() );
			}

		}
		else if( pEntity->m_pVirCntrl->ButA.isNowPush() )
		{ // ButA がこの瞬間に押された
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlgを上げる
			pEntity->StopWatchOn();        // タイマーセット

			// ジャンプ前のアニメーションを設定
			//pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_PreMotion, 5.0, false );
			//float AnimTotalTime = pEntity->m_pAnimMgr->getMotionTotalTime(); // JmpChrgWaitTime 時間内にアニメーション再生が完了するように再生ピッチを調整
			//float PlayPitch = (float)(AnimTotalTime/JmpChrgWaitTime);
			//pEntity->m_pAnimMgr->setPitch(PlayPitch);
		}
	}
	
	// #### Standingへ遷移 その他 MoveLeve制御
	static const double ThresholdSpeedForStop   = 5.0*5.0; // Standing ステートに遷移する速度の閾値
	static const double ThresholdSpeedRunToWark = 35.0*35.0;
	if( !(pEntity->m_pVirCntrl->Virti > 0)  )
	{
		if( pEntity->SpeedSq() < ThresholdSpeedForStop )
		{ // コントローラー入力がなく、速度が十分に小さくなったら、StateをStandingに変更
			pEntity->ChangeState( Standing::Instance() );
			return ;
		}
		else if( pEntity->SpeedSq() < ThresholdSpeedRunToWark && pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvWalking )
		{ // 速度が閾値より小さくなったら、アニメーションをwarkingに変更する。再生時に設定するピッチも変更する
			pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking; // MoveLeveを更新
			// アニメーションを更新
			pEntity->m_pAnimMgr->setAnim( 
				PlayerCharacterEntity::Walking, 
				8.0, 
				false, 
				true );
		}
	}
	else
	{ // 再びアクセレーションされた場合はMoveレベルを戻す必要がある
		if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvRunning )
		{
			pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvRunning; // MoveLeveを更新
			// アニメーションを更新
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
	// 物理定数を定義
	static const double DrivingForce      =2000.0;        // 推進力
	static const double ViscousResistance =  40.0;        // 粘性抵抗係数
	static const double ViscousResistanceInertia = 10.0;  // 粘性抵抗係数（慣性推進時）
	//static const double TurningAngle      = 0.25*DX_PI;   // 横キー入力時の推進力の傾き
	static const double RotateVelSpeed    =   3.0;        // 旋回速度

	PhyVar.init(); // 初期化

	Vector3D vDrivingForce;

	// 上方向キーが押されていれば
	if( pEntity->m_pVirCntrl->Virti > 0 )
	{
		vDrivingForce = DrivingForce * pEntity->Heading() - ViscousResistance * pEntity->Velocity() ;
	}
	else
	{
		vDrivingForce = -1 * ViscousResistanceInertia * pEntity->Velocity() ;
	}

	PhyVar.Force = vDrivingForce ;

	// 横キーが押されていれば旋回ベクトルを設定
	Rotate( RotateVelSpeed, pEntity, PhyVar ); 
	
}

void Running::Render(PlayerCharacterEntity* pEntity )
{
	// Animationに速度に応じた再生ピッチをセットする
	double speed = pEntity->Speed();

	// Running と Warking で再生ピッチがわける（地面の歩き方が自然になるよう）
	if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvRunning )		
	{
		pEntity->m_pAnimMgr->setPitch((float)speed);
	}
	else if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvWalking )	
	{
		pEntity->m_pAnimMgr->setPitch((float)((14.0/12.0)*speed)); // ハードコーディングはマズイのだ
	}
};

void Running::Exit( PlayerCharacterEntity* pEntity )
{
	;
}


// #### Jump ステートのメソッド ####
Jump* Jump::Instance()
{
	static Jump instance;
	return &instance;
}

void Jump::Enter( PlayerCharacterEntity* pEntity )
{
	// ジャンプサイズを決めるボタンを押す長さ
	static const double PressTimeLength_SmallJump = 0.1;
	static const double PressTimeLength_MiddlJump = 0.2;

	// なるほど、Enterで安易に初速度設定は考えたほうがいい。
	//（物理量の更新はCalculateに集約する構造にしているので。）
	// ↑その原則から言えば、Calculate で[撃力]を計算して与えるようにするのがいい
	//   しかし、そのために初期状態を持ちまわるのか？
	// → そういう構造にするか...
	// ・ ジャンプサイズを保持する変数をEntity側に追加
	// ・ サブステート列挙型を定義（Jimp.h）
	// ・ サブステートを保持する変数をEntity側に追加 ←どう持たせるつもり？？？？（型は？？）
	// ・ Calculate の初めで判定？
	// シングルトンでやってるから無理があるのかな？？？？
	// → シングルトンにしないと、状態が変わるたびに、
	//    newしたりdelしたりで処理のオーバーヘッド大きくなること懸念。
	//    metagameでも初めのインスタンス化でなんか動きが遅くなった記憶があるし。
	// → 当分の間は、Entityは1つしか出てこないし、
	//    Stateにメンバを実装してしまっても問題ないのでは？？
	// （同じ種類の）Entityが複数出てくる時のStateインスタンス管理、シングルトンを使い続けるべきかは課題★

	if( pEntity->m_bJmpChrgUsageFlg )
	{
		// タイマ値からジャンプのサイズを計算する
		if( pEntity->getStopWatchTime() < PressTimeLength_SmallJump )
		{ // 小ジャンプ
			m_eJumpSize = Small;
		}
		else if( pEntity->getStopWatchTime() < PressTimeLength_MiddlJump )
		{ // 中ジャンプ
			m_eJumpSize = Middl;
		}
		else
		{ // 大ジャンプ
			m_eJumpSize = Large;
		}
	}
	else
	{
		m_eJumpSize = Middl; 
	}

	// この後のCalculateで、物理的にジャンプを行うためのフラグを上げる
	m_bInitImplusivForce = true;

	// 落下フラグを false にセット
	m_bDescenting = false;

	// アニメーションを設定
	pEntity->m_pAnimMgr->setPitch(20.0);
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_Ascent);
}

void Jump::StateTranceDetect( PlayerCharacterEntity* pEntity )
{
	// 着地の処理を記述 
	if( pEntity->m_bTouchGroundFlg )
	{
		if( pEntity->m_pVirCntrl->Virti > 0 )
		{
			// State 変更
			pEntity->ChangeState( Running::Instance() );
			return;
		}
		else
		{
			// State 変更
			pEntity->ChangeState( Standing::Instance() );
			return;
		}
	}
	// Collision関連処理を盛り込んだ時に、この辺の処理はどう機能分担させるべきか？？★

	// 落下開始判定
	if( pEntity->Velocity().y < 0 )
	{
		// 落下フラグを上げる
		m_bDescenting = true;

		// 落下用のアニメーションをセット
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_Descent);
	}
}

void Jump::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	// ジャンプサイズのy軸初速度
	static const double IniVelY_SmallJump = 25;
	static const double IniVelY_MiddlJump = 150;
	static const double IniVelY_LargeJump = 100;
	static const double GravityAccel = 500;

	PhyVar.init(); // 初期化

	if( m_bInitImplusivForce )
	{ // Stateに入った初めのCalculateで撃力を加え、ジャンプの初速を与える

		double InitVelY;
		// ジャンプサイズにより初速が変わる
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

		// 初速を与えるための撃力を計算
		double ImplusivForce;
		ImplusivForce = ( InitVelY / pEntity->TimeElaps() ) * pEntity->Mass();
		PhyVar.Force = Vector3D( 0, ImplusivForce, 0 ); // 真上にジャンプしている（その時の走りの状態に応じて、方向をつけた方がいいかも。）

		m_bInitImplusivForce = false;

		// 接地flgをOFFにする
		pEntity->m_bTouchGroundFlg = false;

		return;
	}
	else
	{ // 自由落下の運動を記述

		// y軸下方向に、重力加速度をセット
		double GravForce = GravityAccel * pEntity->Mass();
		PhyVar.Force = Vector3D( 0, -GravForce, 0 ); 
		// ★ 後で、落下運動の記述は、stateの中で行わないように変更するべき。（要検討）

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
