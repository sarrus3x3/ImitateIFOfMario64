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
	// 横キーが押されていれば旋回ベクトルを設定
	double KeyHoriz = -1.0 * (double)pEntity->m_pVirCntrl->Horiz;
	PhyVar.DstVar = KeyHoriz * RotateVelSpeed * pEntity->Side();
};

// #### Dammy ステートのメソッド ########################################################################
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

// #### Standing ステートのメソッド ########################################################################
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
	if( pEntity->m_pVirCntrl->m_dTiltQuantStickL > 0 )
	{
		// pEntity->ChangeState( Running::Instance() );
		pEntity->ChangeState( SurfaceMove::Instance() );
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


// #### Running ステートのメソッド ########################################################################
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


// #### Jump ステートのメソッド ########################################################################
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



// #### SurfaceMove ステートのメソッド ########################################################################
SurfaceMove* SurfaceMove::Instance()
{
	static SurfaceMove instance;
	return &instance;
}

void SurfaceMove::Enter( PlayerCharacterEntity* pEntity )
{
		// アニメーションを設定
	if( pEntity->isMatchPrvState( Jump::Instance() ) )
	{
		// ジャンプ後であれば、着地のアニメーションを再生する
		pEntity->m_pAnimMgr->setPitch(40.0);
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Jump_Landing_Short);
		//pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::SurfaceMove, 20.0, false );
		pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::Running, 5.0, false );
	}
	else
	{
		// 走り出しを自然にするようにアニメーション開始位置とブレンド実施
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Walking, 5.0 );
	}

	// m_bJmpChrgFlg を初期化
	pEntity->m_bJmpChrgFlg = false; 

	// MoveLevelを設定
	pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking;
}

void SurfaceMove::StateTranceDetect( PlayerCharacterEntity* pEntity )
{
	// m_bJmpChrgUsageFlg OFF
	static const double JmpChrgWaitTime = 0.1; // ジャンプ開始までの待ち時間

	// m_bJmpChrgUsageFlg ON
	static const double JmpChrgMaxTime = 0.3;  // ジャンプチャージの最大時間



	// #### ジャンプ関連
	if( pEntity->m_bJmpChrgUsageFlg )
	{ // m_bJmpChrgUsageFlg ON (ボタンの押し込み時間に応じで、ジャンプ力調整する機能)
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
	{ // m_bJmpChrgUsageFlg OFF
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
	static const double ThresholdSpeedForStop   = 5.0*5.0;   // Standing に遷移する速度の閾値（平方値）
	static const double ThresholdSpeedRunToWark = 35.0*35.0; // Running<->Warking の速度の閾値（平方値）
	static const double ThresholdSticktiltRunToWark = 0.6;   // Running<->Warking のスティック傾きの閾値

	pEntity->DBG_m_dDBG=pEntity->SpeedSq();


	if( (pEntity->m_pVirCntrl->m_dTiltQuantStickL > ThresholdSticktiltRunToWark)  )
	{ // アナログスティック傾きがWarking閾値以上
		// 再びアクセレーションされた場合はMoveレベルを戻す必要がある
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
	else
	{ // アナログススティック傾き有り
		
		//dbgval++;
		if( pEntity->SpeedSq() < ThresholdSpeedRunToWark 
			&& pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvWalking )
		{ // 速度が閾値より小さくなったら、アニメーションをwarkingに変更する。再生時に設定するピッチも変更する
			pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking; // MoveLeveを更新
			// アニメーションを更新 ← animationの更新をここでやるなよ...
			pEntity->m_pAnimMgr->setAnim( 
				PlayerCharacterEntity::Walking, 
				8.0, 
				false, 
				true );
		}
	}

	// コントローラー入力がなく、速度が十分に小さくなったら、StateをStandingに変更
	if( pEntity->SpeedSq() < ThresholdSpeedForStop 
		&& pEntity->m_pVirCntrl->m_dTiltQuantStickL==0 )
	{ 
		pEntity->ChangeState( Standing::Instance() );
		return ;
	}

	// アニメーション関係処理は、render()へ。
	// → と思ったが、サブState遷移のトリガはrenderの中でしかわからない。
	//    無理やりできなくもないが、このままでいいか...


}

void SurfaceMove::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	// 物理定数を定義
	//static const double DrivingForce      =2000.0;        // 推進力
	//static const double ViscousResistance =  40.0;        // 粘性抵抗係数
	//static const double ViscousResistanceInertia = 40.0; //10.0;  // 粘性抵抗係数（慣性推進時）
	//static const double TurningAngle      = 0.25*DX_PI;   // 横キー入力時の推進力の傾き
	//static const double RotateVelSpeed    =   3.0;        // 旋回速度

	static const double MaxVelocity      = 65.0; // キャラクターの最大速度（スティックをmaxまで倒した時の最大速度）
	static const double ViscousRsisInert = 10.0;  // 慣性推進時の粘性抵抗係数
	static const double ViscousRsisAccel = 80.0;  // 加速時の粘性抵抗係数
	
	static const double MaxCentripetalForce =500.0*10;   // 旋回時の最大向心力
	static const double sqMaxCentripetalForce = MaxCentripetalForce*MaxCentripetalForce;

	static const double SensitivityCoefForTurning = 10; // 旋回時の入力に対する反応の良さ。

	PhyVar.init(); // 初期化

	// 16:45
	// ##### スティックの傾きの方向から、Entityに働く操舵力の方向を計算する
	// ・ワールド座標を使うことによる精度劣化を防ぐため、計算をEntityのローカル座標で行うように修正
	
	/*
	// スクリーン上のEntity位置を計算
	Vector3D EntiPosForScreen = ConvWorldPosToScreenPos( pEntity->Pos().toVECTOR() );
	assert( EntiPosForScreen.z >= 0.0 && EntiPosForScreen.z <= 1.0 );

	// デバック
	DBG_m_vEntiPosOnScreen.x = EntiPosForScreen.x;
	DBG_m_vEntiPosOnScreen.y = EntiPosForScreen.y;

	// スクリーン上にスティックの傾き方向の位置を求める
	Vector3D vStickTile( 0,0,0);
	vStickTile.x = pEntity->m_pVirCntrl->m_vStickL.x;
	vStickTile.y = -pEntity->m_pVirCntrl->m_vStickL.y;
	Vector3D StickTiltPosForScreen = EntiPosForScreen + vStickTile;

	// スティックの傾き方向の位置Entityの地平面に投影するした位置を求める
	StickTiltPosForScreen.z = 0.0;
	Vector3D BgnPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	StickTiltPosForScreen.z = 1.0;
	Vector3D EndPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	Vector3D vCrossPos;
	int rtn = calcCrossPointWithXZPlane( BgnPos, EndPos, vCrossPos ); // これは、厳密にはキャラクタ水平面ではないので、ジャンプすると不正確。修正が必要★
	vCrossPos.y = 0; // 安易な対処法

	// 操舵力の決定
	Vector3D vSteeringForceDir = ( vCrossPos - pEntity->Pos() ).normalize();
	if( rtn < 0 )
	{ // カメラの向きの反対側で地平面と交わる場合
		vSteeringForceDir *= -1; // 反対向きに設定
	}

	*/

	// 現在のカメラのビュー行列を退避（Entityの進行指示方向を求める計算で、ビュー行列をEntityのローカル座標でのものに設定するため）
	MATRIX MSaveViewMat = GetCameraViewMatrix();

	// カメラのビュー行列をEntityのローカル座標でのものに設定
	SetCameraViewMatrix( CameraWorkManager::Instance()->m_MViewLocal );

	static const Vector3D vPosOrign = Vector3D( 0,0,0 );

	// スクリーン上のEntity位置を計算
	Vector3D EntiPosForScreen = ConvWorldPosToScreenPos( vPosOrign.toVECTOR() );
	assert( EntiPosForScreen.z >= 0.0 && EntiPosForScreen.z <= 1.0 );

	// デバック
	DBG_m_vEntiPosOnScreen.x = EntiPosForScreen.x;
	DBG_m_vEntiPosOnScreen.y = EntiPosForScreen.y;

	// スクリーン上にスティックの傾き方向の位置を求める
	Vector3D vStickTile = vPosOrign;
	vStickTile.x = pEntity->m_pVirCntrl->m_vStickL.x;
	vStickTile.y = -pEntity->m_pVirCntrl->m_vStickL.y;
	Vector3D StickTiltPosForScreen = EntiPosForScreen + vStickTile;

	// スティックの傾き方向の位置Entityの地平面に投影するした位置を求める
	StickTiltPosForScreen.z = 0.0;
	Vector3D BgnPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	StickTiltPosForScreen.z = 1.0;
	Vector3D EndPos = ConvScreenPosToWorldPos( StickTiltPosForScreen.toVECTOR() );
	Vector3D vCrossPos;
	int rtn = calcCrossPointWithXZPlane( BgnPos, EndPos, vCrossPos ); // これは、厳密にはキャラクタ水平面ではないので、ジャンプすると不正確。修正が必要★
	vCrossPos.y = 0; // 安易な対処法

	// 操舵力の決定
	Vector3D vSteeringForceDir = ( vCrossPos ).normalize();
	if( rtn < 0 )
	{ // カメラの向きの反対側で地平面と交わる場合
		vSteeringForceDir *= -1; // 反対向きに設定
	}

	// カメラのビュー行列を元に戻す
	SetCameraViewMatrix( MSaveViewMat );


	// 大きさはオリジナルのスティックの傾きを使用
	Vector3D vStickTiltFromCam = pEntity->m_pVirCntrl->m_vStickL.toVector3D().len() * vSteeringForceDir;
	//Vector3D vStickTiltFromCam = vSteeringForceDir;

	// デバック用に記憶
	DBG_m_vStickPos = 10 * vStickTiltFromCam + pEntity->Pos();

	// ★ For DBG

	// 17:16 Coding完
	// 17:29 残念、動かない

	// 17:43 デバック完

	// * スティックの傾き（=Input）から、終速度を計算
	Vector3D TerminalVel = MaxVelocity * vStickTiltFromCam;
	
	// * 最高速度と現在速度を比較し、加速／慣性状態時の判定して、$\eta$を決定
	double eta;
	if( TerminalVel.sqlen() > pEntity->Velocity().sqlen() )
	{
		eta = ViscousRsisAccel; // 加速状態
	}
	else
	{
		eta = ViscousRsisInert; // 慣性推進時
	}
	
	//eta = ViscousRsisAccel; // ★DBG

	
	// * $F_{Drive}$ が決まる。
	Vector3D vSteeringForce = eta * TerminalVel;
	

	// ##### 旋回時の挙動改善

	m_dCentripetalForce = 0; // 向心力をクリア
	
	if(pEntity->Velocity().len() < 25.0)
	{ // 速度が小さ場合は旋回挙動を適用しない
		Vector3D Force = vSteeringForce - eta * (pEntity->Velocity()) ;
		PhyVar.Force = Force ;

		assert( PhyVar.Force.y==0 );

	}
	else
	{
		// 向心力の決定
		double DriveForce = vSteeringForce.len();
		Vector3D vDriveForce = DriveForce * pEntity->Heading();
	
		double CentripetalForce = (SensitivityCoefForTurning * vSteeringForce) * (pEntity->Side());
		if( fabs(CentripetalForce) > MaxCentripetalForce )
		{
			double sgn =  (double)( (CentripetalForce>0) - (CentripetalForce<0) );
			CentripetalForce = sgn * MaxCentripetalForce;
		}
		//CentripetalForce = MaxCentripetalForce; //★DBG

		m_dCentripetalForce = CentripetalForce; // 向心力を記憶
	

		// 次のタイムステップの速度・位置を計算
		// * ルンゲクッタ法適用
		// * 旋回運動が不安定なのを解消したいだけなので、
		//   DriveForce, CentripetalForce はこのステップで固定

		Vector3D vK, vL;
		Vector3D vSumK, vSumL;
		Vector3D vVel = pEntity->Velocity(), vNxtVel;
		Vector3D vUpper = pEntity->Uppder();

		// K1の計算
		vL = calculateForce( vVel, vUpper, DriveForce, CentripetalForce, eta )/pEntity->Mass();
		vK = vVel;
		vSumK = vK;
		vSumL = vL;

		// K2の計算
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, DriveForce, CentripetalForce, eta )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K3の計算
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, DriveForce, CentripetalForce, eta )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K4の計算
		vNxtVel = vVel+pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, DriveForce, CentripetalForce, eta )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += vK;
		vSumL += vL;

		// 次のステップの位置・速度（変位）を求める
		PhyVar.VelVar = (1.0/6.0) * vSumL;
		PhyVar.PosVar = (1.0/6.0) * vSumK;

		// それ用の計算をしてもらうためにフラグを立てる
		PhyVar.UseVelVar = true;
		PhyVar.UsePosVar = true;

		assert( PhyVar.VelVar.y==0 );

	}

	return;
}

// SurfaceMove::Calculateの補助関数
Vector3D SurfaceMove::calculateForce( 
		Vector3D vVel, 
		Vector3D vUpper,
		double DriveForce,
		double CentripetalForce,
		double eta )
{
	// Entity速度(Input)から操舵力の向きを再計算
	Vector3D vHeading = vVel.normalize();
	Vector3D vSide = VCross( vHeading.toVECTOR(), vUpper.toVECTOR() );
	Vector3D vSteering = DriveForce * vHeading + CentripetalForce * vSide;

	// 粘性モデルに従いEntityに働く力を計算し返却
	return vSteering - eta * vVel ;

};


void SurfaceMove::Render(PlayerCharacterEntity* pEntity )
{
	static const double LikeGravity = 500.0 * 10 * 5; // バンク角の計算に使用。重力に相当する。
	//static const double LikeGravity = 500.0 * 10 * 1; // バンク角の計算に使用。重力に相当する。

	// Animationに速度に応じた再生ピッチをセットする
	double speed = pEntity->Speed();

	// 遠心力にバンク演出
	static Smoother<double> BankAngleSmoother( 6, 0 );  // 円滑化
	double bankangle = atan2( m_dCentripetalForce, LikeGravity ); // *重力と遠心力によりバンク角の計算
	pEntity->m_pAnimMgr->setBankAngle( BankAngleSmoother.Update(-bankangle) );
	/*
	double bankangle = atan2( m_dCentripetalForce, LikeGravity ); // *重力と遠心力によりバンク角の計算
	pEntity->m_pAnimMgr->setBankAngle( -bankangle );
	*/

	// Running と Warking で再生ピッチがわける（地面の歩き方が自然になるよう）
	if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvRunning )		
	{
		pEntity->m_pAnimMgr->setPitch((float)speed);
	}
	else if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvWalking )	
	{
		pEntity->m_pAnimMgr->setPitch((float)((14.0/12.0)*speed)); // ハードコーディングはマズイのだ
	}

	// デバック用
	// （Entity平面上に投影した）スティックの傾きの位置を描画
	static PlaneRing RingIns( 
		0.6, 0.4, 16, 
		GetColorU8(255,   0,   0, 0 ),
		GetColorU8(255, 255, 255, 0 ) );
	RingIns.setCenterPos( DBG_m_vStickPos );
	RingIns.Render(); 

	// スクリーン上のEntity位置を描画
	DrawCircle( (int)DBG_m_vEntiPosOnScreen.x, (int)DBG_m_vEntiPosOnScreen.y, 5, GetColor(0,255,0) );

	// 向心力出力
	//行数
	int colmun= 0;
	int width = 15;

	// Entityの速度を表示
	DrawFormatString( 0, width*colmun, 0xffffff, "m_dCentripetalForce:%8f", m_dCentripetalForce ); 
	colmun++;

	DrawFormatString( 0, width*colmun, 0xffffff, "EntiPosOnScreen:%8f, %8f", DBG_m_vEntiPosOnScreen.x, DBG_m_vEntiPosOnScreen.y ); 
	colmun++;

	// スティックの傾き情報出力
	Vector2D vStickTile;
	vStickTile.x = pEntity->m_pVirCntrl->m_vStickL.x;
	vStickTile.y = -pEntity->m_pVirCntrl->m_vStickL.y;
	DrawFormatString( 0, width*colmun, 0xffffff, "EntiPosOnScreen:%8f, %8f", vStickTile.x, vStickTile.y ); 
	colmun++;


};

void SurfaceMove::Exit( PlayerCharacterEntity* pEntity )
{
	// 遠心力による姿勢の傾きの解除
	pEntity->m_pAnimMgr->setBankAngle( 0.0 );
}
