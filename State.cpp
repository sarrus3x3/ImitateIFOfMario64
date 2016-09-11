#include "State.h"
#include "AnimationManager.h"
#include "VirtualController.h"
#include "MyUtilities.h"
//#include "SolidObjects.h"
//#include "CameraWorkManager.h"

#include <cassert>

//static const double EPS = 1e-5;
static const double EPS = 0.5;

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
	else if( pEntity->isMatchPrvState( OneEightyDegreeTurn::Instance() ) )
	{
		// ダッシュからの切返し後であれば、ブレーキ後の起き上がりのアニメーションを再生する
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::BreakingAfter);
		pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::Standing, 10.0 );
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
}

void Standing::Render(PlayerCharacterEntity* pEntity )
{
	;
};


void Standing::Exit( PlayerCharacterEntity* pEntity )
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
			pEntity->ChangeState( SurfaceMove::Instance() );
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

// ##### 定数
//const double ThresholdSpeedRunToWark = 35.0*35.0; // Running<->Warking の速度の閾値（平方値）
const double SurfaceMove::ThresholdSpeedRunToWark = 50.0*50.0; // Running<->Warking の速度の閾値（平方値）
// 走り始め直後に進行方向を反転させた場合に切返し動作を行わないようにチューニング

const double SurfaceMove::ThresholdSticktiltRunToWark = 0.6;   // Running<->Warking のスティック傾きの閾値

const double SurfaceMove::MaxCentripetalForce = 500.0*10;   // 旋回時の最大向心力


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

		// MoveLevelを設定
		pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking;

	}
	else if(pEntity->isMatchPrvState( OneEightyDegreeTurn::Instance() ))
	{
		// ダッシュからの切返しの後であれば、走りのアニメーションを継続する。
		//pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running, 5.0, false );
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running  );

		// あー、切返しキャンセルした場合はブレンドしたい → 切返し動作のサブステートをEntityに持たせる必要がある
		// → 面倒くさいからいや…。ブレンドしても大丈夫だろ

		// サブステートの評価を行う
		if( (pEntity->m_pVirCntrl->m_dTiltQuantStickL > ThresholdSticktiltRunToWark) 
			|| pEntity->SpeedSq() < ThresholdSpeedRunToWark )
		{ // アナログスティック傾きがWarking閾値以上 or Entityのスピードが十分大きい
			// サブステートを走りにセット
			pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvRunning; // MoveLeveを更新
			// アニメーションを更新
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running, 5.0, false );
		}
		else
		{ 
			// それ以外はサブステートを歩きにセット
			pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking; // MoveLeveを更新
			// アニメーションを更新
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Walking, 5.0, false );
		}

	}
	else
	{
		// 走り出しを自然にするようにアニメーション開始位置とブレンド実施
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Walking, 5.0 );

		// MoveLevelを設定
		pEntity->m_eMoveLevel = PlayerCharacterEntity::MvLvWalking;

	}

	// m_bJmpChrgFlg を初期化
	pEntity->m_bJmpChrgFlg = false; 

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


	// #### ダッシュからの切返しの遷移判定
	static const double InnerProductForStartTurn = 0.0; // 速度ベクトル（規格化済み）と移動方向ベクトルの内積値がこの値以下であれば、切返しと判定する。
	
	//if( pEntity->m_eMoveLevel == PlayerCharacterEntity::MvLvRunning )
	if( pEntity->Velocity().sqlen() >= ThresholdSpeedRunToWark )
	{ // 移動レベルが、Runningならば
		Vector3D VelDir  = pEntity->Velocity().normalize();
		Vector3D MoveDir = pEntity->calcMovementDirFromStick().normalize();
		if( VelDir*MoveDir <= InnerProductForStartTurn )
		{
			pEntity->ChangeState( OneEightyDegreeTurn::Instance() );
			return ;
		}
	}


	// #### Standingへ遷移 その他 MoveLeve制御
	static const double ThresholdSpeedForStop   = 5.0*5.0;   // Standing に遷移する速度の閾値（平方値）
	//static const double ThresholdSpeedRunToWark = 35.0*35.0; // Running<->Warking の速度の閾値（平方値）// SurfaceMove ステート クラスのメンバに変更
	//static const double ThresholdSticktiltRunToWark = 0.6;   // Running<->Warking のスティック傾きの閾値 // // SurfaceMove ステート クラスのメンバに変更

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
	static const double MaxVelocity      = 65.0; // キャラクターの最大速度（スティックをmaxまで倒した時の最大速度）
	
	static const double ViscousRsisInert = 40.0;  // 慣性推進時の粘性抵抗係数
	static const double ViscousRsisAccel = 40.0;  // 加速時の粘性抵抗係数
	// 2016/08/18
	// 走り始め直後に進行方向を反転させた場合に切返し動作を行わないようにチューニング
	// 慣性推進時と加速時の粘性抵抗を同じにする。
	// 切返し動作時の加速力に合せて値を小さくチューニングした

	static const double sqMaxCentripetalForce = MaxCentripetalForce*MaxCentripetalForce;

	static const double SensitivityCoefForTurning = 10; // 旋回時の入力に対する反応の良さ。

	PhyVar.init(); // 初期化

	// スティックの傾きの方向からEntityの移動方向を計算する
	Vector3D vStickTiltFromCam = pEntity->calcMovementDirFromStick();

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
	
	// * 推進力の決定
	Vector3D vSteeringForce = eta * TerminalVel;
	Vector3D vArrangeSteeringForce = SensitivityCoefForTurning * vSteeringForce;

	// DBG
	DBG_m_vSteeringForce = vSteeringForce;

	// ##### 旋回時の挙動改善

	if( pEntity->m_eMoveLevel == PlayerCharacterEntity::MvLvWalking )
	//if( pEntity->Velocity().sqlen() < ThresholdSpeedRunToWark )
	{ // 速度が小さ場合は旋回挙動を適用しない
		
		Vector3D Force = vSteeringForce - eta * (pEntity->Velocity()) ;
		PhyVar.Force = Force ;

		assert( PhyVar.Force.y==0 );

	}
	else
	{
		// 向心力の決定
		m_dCentripetalForce = 0; // 向心力をクリア

		// 推進力の計算
		//   スティックが進行方向とは逆方向の場合は推進力 FDrive を切返しの動作の開始条件である
		//   速度閾値以上にならないように調整
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
	
		// 次のタイムステップの速度・位置を計算
		// * ルンゲクッタ法適用
		// * 旋回運動が不安定なのを解消したいだけなので、
		//   DriveForce, CentripetalForce はこのステップで固定

		Vector3D vK, vL;
		Vector3D vSumK, vSumL;
		Vector3D vVel = pEntity->Velocity(), vNxtVel;
		Vector3D vUpper = pEntity->Uppder();

		// K1の計算
		vL = calculateForce( vVel, vUpper, vArrangeSteeringForce, DriveForce, eta, m_dCentripetalForce )/pEntity->Mass();
		vK = vVel;
		vSumK = vK;
		vSumL = vL;

		// K2の計算
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, m_dCentripetalForce )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K3の計算
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, m_dCentripetalForce )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K4の計算
		vNxtVel = vVel+pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, m_dCentripetalForce )/pEntity->Mass();
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
// 2016/09/04
//   走り始めにEntity向きが振動する対策として、
//   CentripetalForce　ではなく、vSteeringForce を渡し、
//   その都度 CentripetalForce を計算する用に変更。
Vector3D SurfaceMove::calculateForce( 
		Vector3D vVel, 
		Vector3D vUpper,
		Vector3D vArrangeSteeringForce,
		double DriveForce,
		double eta,
		double &outCentripetalForce
		)
{
	// Entity速度(Input)から操舵力の向きを再計算
	Vector3D vHeading = vVel.normalize();
	Vector3D vSide = VCross( vHeading.toVECTOR(), vUpper.toVECTOR() );

	// vArrangeSteeringForce から CentripetalForce を計算
	double CentripetalForce = vArrangeSteeringForce * vSide ;
	// カットオフ処理
	if( fabs(CentripetalForce) > MaxCentripetalForce )
	{
		double sgn =  (double)( (CentripetalForce>0) - (CentripetalForce<0) );
		CentripetalForce = sgn * MaxCentripetalForce;
	}

	// 最終的な操舵力を計算
	Vector3D vSteering = DriveForce * vHeading + CentripetalForce * vSide;

	// 計算した向心力を格納
	outCentripetalForce = CentripetalForce;

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

	// Running と Warking で再生ピッチがわける（地面の歩き方が自然になるよう）
	if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvRunning )		
	{
		pEntity->m_pAnimMgr->setPitch((float)speed);
	}
	else if( pEntity->m_eMoveLevel!=PlayerCharacterEntity::MvLvWalking )	
	{
		pEntity->m_pAnimMgr->setPitch((float)((14.0/12.0)*speed)); // ハードコーディングはマズイのだ
	}

	// ###### デバック用出力 

	// （Entity平面上に投影した）スティックの傾きの位置を描画
	pEntity->DBG_renderMovementDirFromStick();

	// 向心力出力
	//行数
	int colmun= 0;
	int width = 15;

	// Entityの速度を表示
	DrawFormatString( 0, width*colmun, 0xffffff, "m_dCentripetalForce:%8f", m_dCentripetalForce ); 
	colmun++;

	// スティックの傾き情報出力
	Vector2D vStickTile;
	vStickTile.x = pEntity->m_pVirCntrl->m_vStickL.x;
	vStickTile.y = -pEntity->m_pVirCntrl->m_vStickL.y;
	DrawFormatString( 0, width*colmun, 0xffffff, "EntiPosOnScreen:%8f, %8f", vStickTile.x, vStickTile.y ); 
	colmun++;

	// Entityのへ操舵力を出力
	DrawFormatString( 0, width*colmun, 0xffffff, "vSteeringForce:%8f, %8f", DBG_m_vSteeringForce.x, DBG_m_vSteeringForce.z ); 
	colmun++;


};

void SurfaceMove::Exit( PlayerCharacterEntity* pEntity )
{
	// 遠心力による姿勢の傾きの解除
	pEntity->m_pAnimMgr->setBankAngle( 0.0 );
}


// #### OneEightyDegreeTurn ステートのメソッド ########################################################################

// ##### 定数
const double OneEightyDegreeTurn::MaxVelocity      = 65.0;  // キャラクターの最大速度（スティックをmaxまで倒した時の最大速度）
//const double OneEightyDegreeTurn::ViscousRsisTurn  = 80.0;  // 粘性抵抗係数
const double OneEightyDegreeTurn::ViscousRsisTurn   = 40.0;  // 粘性抵抗係数
const double OneEightyDegreeTurn::ViscousRsisBreak  = 25.0;  // 粘性抵抗係数
//const double OneEightyDegreeTurn::TurningDulation  = 7.0/(OneEightyDegreeTurn::ViscousRsisTurn);
const double OneEightyDegreeTurn::TurningDulation  = 0.3;
const double OneEightyDegreeTurn::SlowDownEnough   = 5.0;   // SurfaceMove→停止 の速度閾値と同じにしておく
const double OneEightyDegreeTurn::InnerProductForStartTurn = 0.0; // 速度ベクトル（規格化済み）と移動方向ベクトルの内積値がこの値以下であれば、切返しと判定する。


OneEightyDegreeTurn* OneEightyDegreeTurn::Instance()
{
	static OneEightyDegreeTurn instance;
	return &instance;
}

void OneEightyDegreeTurn::Enter( PlayerCharacterEntity* pEntity )
{
	// サブステートの初期化
	m_eSubState = SUB_BREAKING;

	// 切返し動作を開始した時の速度方向を記録
	m_vVelDirBeginning = pEntity->Velocity().normalize();

	// アニメーションの設定
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Breaking, 0.0 ); // ブレーキのアニメーションを設定
	pEntity->m_pAnimMgr->setPitch( 20.0 );

}

void OneEightyDegreeTurn::StateTranceDetect( PlayerCharacterEntity* pEntity )
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
	
	// #### ブレーキ中にキャンセル
	if( m_eSubState == SUB_BREAKING )
	{ // ブレーキ状態

		// スティックの傾き方向が（切返し開始時の）速度方向に倒されたら（戻されたら）、 
		// 切返しをキャンセルして走りの状態に戻る。
		Vector3D MoveDir = pEntity->calcMovementDirFromStick().normalize();
		if( m_vVelDirBeginning*MoveDir > InnerProductForStartTurn )
		{
			pEntity->ChangeState( SurfaceMove::Instance() );
			return ;
		}

		// Substateの[Enter]でサブステートの評価（runかworkか）を行うこと★

	}

	// #### ブレーキ・サブ状態から切返し・サブ状態への遷移判定
	static const double SqSlowDownEnough = SlowDownEnough * SlowDownEnough;

	if( m_eSubState == SUB_BREAKING )
	{ // ブレーキ状態

		// 十分に減速したか？
		if( pEntity->Velocity().sqlen() < SqSlowDownEnough )
		{
			// スティックの傾きが 0 だった場合は、その後に切返しをキャンセルして 停止の状態に遷移。
			if( pEntity->m_pVirCntrl->m_dTiltQuantStickL==0 )
			{ 
				pEntity->ChangeState( Standing::Instance() );
				return ;
			}

			// 切返し状態に遷移
			m_eSubState = SUB_TURNING;

			// タイマオン
			pEntity->StopWatchOn();

			// 切返し動作のアニメーションを再生
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Turning );

			// TurningDulation 時間内にアニメーション再生が完了するように再生ピッチを調整
			//float AnimTotalTime = pEntity->m_pAnimMgr->getMotionTotalTime(); 
			float AnimTotalTime = 41 - 14; //  getMotionTotalTime だと、切り出した場合のアニメーションの再生時間を取得できない
			float PlayPitch = (float)(AnimTotalTime/TurningDulation) + 1.0;
			pEntity->m_pAnimMgr->setPitch(PlayPitch);

			return ;

		}

	}

	// #### 切返し・サブ状態がタイマ満了になったらSurfaceMove Stateに遷移
	if( m_eSubState == SUB_TURNING )
	{ // 切返し状態
		if( pEntity->getStopWatchTime() > TurningDulation )
		{
			pEntity->ChangeState( SurfaceMove::Instance() );

			return;
		}
	}

}

void OneEightyDegreeTurn::Calculate( PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar )
{
	// ブレーキ力、切返し時の推進力は、
	// Entityの終速度と、粘性抵抗係数（=終速度への到達時間）より計算する。

	PhyVar.init(); // 初期化

	// サブ・ステートにより動作をわける
	if( m_eSubState == SUB_BREAKING )
	{ // ブレーキ状態
		// 速度が十分小さくなるまでブレーキをかける。

		// ブレーキ力を計算
		Vector3D vBreakingForce = -1 * ViscousRsisBreak * MaxVelocity * m_vVelDirBeginning;

		// 粘性抵抗モデルからEntityに働く力を計算
		PhyVar.Force = vBreakingForce - ViscousRsisBreak * (pEntity->Velocity());

		// 向きを固定
		PhyVar.Heading = m_vVelDirBeginning;
		PhyVar.UseHeading = true;

		return;

	}
	else if( m_eSubState == SUB_TURNING )
	{ // 切返し状態
		// 切り返しの方向に加速する。同時に向きを切返し方向へ回転。

		// ##### Entityに働く力を計算

		// スティックの傾きの方向からEntityの移動方向を計算する
		Vector3D vStickTiltFromCam = pEntity->calcMovementDirFromStick();
		
		Vector3D vTurnDestination; // 回転して向ける方向

		// 取得したスティックの傾きの方向が 0 でなければその方向を規格化して設定。
		// 0 の場合は、切返し動作に入った時の進行方向と真逆の方向にセット
		if( vStickTiltFromCam.sqlen() > 0 )
		{
			vTurnDestination = vStickTiltFromCam.normalize();
		}
		else
		{
			vTurnDestination = -1 * m_vVelDirBeginning;
		}

		// * スティックの傾き（=Input）から、終速度を計算
		Vector3D TerminalVel = MaxVelocity * vTurnDestination;

		// 切返し時の推進力を計算
		Vector3D vTurnningForce = ViscousRsisTurn * TerminalVel;

		// 粘性抵抗モデルからEntityに働く力を計算
		PhyVar.Force = vTurnningForce - ViscousRsisTurn * (pEntity->Velocity());

		// ##### Entity向きの回転量を計算
		// * TurningDulation で向きの回転が完了するように、回転速度を調整

		// 現在のHeadingと、目的の方向のベクトルのなす角を計算
		double angle = atan2( 
			pEntity->Side()    * vTurnDestination, 
			pEntity->Heading() * vTurnDestination );

		// 得られた角度が負値の場合は、逆向きからの角度に変換（これにより、回転方向が固定される）
		if( angle < 0 )
		{
			angle = 2*DX_PI_F + angle;
		}

		// 現在の回転位置と、回転完了までの残り時間より、今回のタイムステップにおける回転量を計算
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
		// ↑RemainingTime ～ 0 で挙動がおかしくなる可能性がある

		// MGetRotAxisをつかい、現在のHeadinをUpperを軸に、今回のタイムステップにおける回転量だけ回転させる
		MATRIX RotMat = MGetRotAxis( pEntity->Uppder().toVECTOR(), RotQuant );
		Vector3D vNewHedding = VTransform( pEntity->Heading().toVECTOR(), RotMat );

		// 新しいHeadingベクトルを設定し完了
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

	// ###### デバック用出力 

	// （Entity平面上に投影した）スティックの傾きの位置を描画
	pEntity->DBG_renderMovementDirFromStick();

	//行数
	int colmun= 0;
	int width = 15;

	// サブ状態を描画
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
	// 遠心力による姿勢の傾きの解除
	pEntity->m_pAnimMgr->setBankAngle( 0.0 );
}

