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
	else if( pEntity->isMatchPrvState( Break::Instance() ) )
	{
		// ダッシュからの切返し後であれば、ブレーキ後の起き上がりのアニメーションを再生する
		pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::BreakingAfter);
		pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::Standing, 10.0 );
		pEntity->m_pAnimMgr->setPitch(20.0); // ピッチを調整

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
	if( pEntity->m_pVirCntrl->m_dStickL_len > 0 )
	{
		// pEntity->ChangeState( Running::Instance() );
		//pEntity->ChangeState(SurfaceMove::Instance());
		pEntity->ChangeState( Run::Instance() );
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
			//pEntity->ChangeState(SurfaceMove::Instance());
			pEntity->ChangeState( Run::Instance() );
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


// #### Break ステートのメソッド ########################################################################
Break* Break::Instance()
{
	static Break instance;
	return &instance;
}

void Break::Enter(PlayerCharacterEntity* pEntity)
{
	// ブレーキ状態のタイマオン
	pEntity->StopWatchOn();

	// アニメーションの設定
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Breaking, 0.0); // ブレーキのアニメーションを設定
	
	float AnimTotalTime = pEntity->m_pAnimMgr->getCurAnimLength();
	
	//   turnに遷移するタイミングでは、既にブレーキアニメーションの再生が終わっている必要がある。
	//   ブレーキに入る最低速度（＆ブレーキの減速度）から、ブレーキ継続する最小時間を計算し、それ以下にする必要がある。
	double BrakingDulation = 0.08;
	float PlayPitch = (float)(AnimTotalTime/BrakingDulation);
	
	pEntity->m_pAnimMgr->setPitch(PlayPitch);
}

void Break::StateTranceDetect(PlayerCharacterEntity* pEntity)
{
	// 廃棄予定のメソッド
}

void Break::Calculate(PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar)
{
	PhyVar.init(); // 初期化

	// 新しいステートへの遷移、キャラクタの物理量の更新は、
	// すべてこのメソッドの中でやるようにする。

	// ## キャラクタのUpdate()での物理量の更新処理を回避するための処理
	// → すべてのステートで、ステート内で物理量を更新するように変更したら、ここの処理は削除すること。。。

	// 速度変化なし
	PhyVar.UseVelVar = false;
	PhyVar.Force = Vector3D(0, 0, 0);

	// キャラクタ向きの手動設定を有効
	PhyVar.UseHeading = true;
	PhyVar.Heading = pEntity->Heading();

	// 位置はデフォルト更新（現在の速度に従い更新）
	PhyVar.UsePosVar = false;

	// ##「ブレーキ」ステートの挙動

	// 速度の向きはキャラクタ向きで代用しようかとも思ったが、（どんどん小さくなっていく速度ベクトルよりは安定であるため）
	// 地平面に傾きがある場合は速度向き＝キャラクタ向きが成り立たない可能性があることに気がついたので、止めた。

	// 速度の大きさを方向を分離
	double   VelSiz = pEntity->Velocity().len();
	Vector3D VelDir = pEntity->Velocity() / VelSiz;

	// 速度の大きさを更新
	double deceleration = 60.0f * pEntity->TimeElaps() * PlayerCharacterEntity::m_dConfigScaling; // ブレーキ中の減速度
	//double deceleration = 10.0f * pEntity->TimeElaps() * PlayerCharacterEntity::m_dConfigScaling; // ブレーキ中の減速度
	double NewVelSiz = MoveTowards(VelSiz, 0, deceleration);

	// 新しい速度ベクトルを再構築
	Vector3D NewVel = NewVelSiz * VelDir;

	// 更新された速度をEntityにセット
	pEntity->setVelocity(NewVel);

	// ## 「ブレーキ」ステートのキャンセル判定

	// スティックの入力があること
	if (!pEntity->MoveInput().isZero())
	{
		// スティックの傾き方向とキャラクタ向きの角度を取得
		double AngleBetween = Angle3D(pEntity->MoveInput(), pEntity->Heading());

		//スティックの傾き方向と(ブレーキ開始時の※)キャラクタ向きとの角が110°より小さくなる
		if (AngleBetween < DX_PI * (11.0 / 18.0))
		{
			//→キャンセルして、「走り／歩き」ステートに遷移。
			//pEntity->ChangeState(SurfaceMove::Instance());
			pEntity->ChangeState(Run::Instance());
			return;
		}
	}


	// ##「ブレーキ」ステートの終了判定

	// キャラクタの速度が０になったら
	if (pEntity->Velocity().sqlen() == 0.0)
	{
		if (pEntity->MoveInput().isZero())
		{ // スティック入力なし
		
			// 「アイドル」（待機）ステートへ遷移
			pEntity->ChangeState(Standing::Instance());
			return;
		}
		else
		{ // スティック入力あり
		
			// [キャラクタ方向を反転]
			Vector3D newHead = -1 * pEntity->Heading();
			pEntity->setHeading(newHead);

			PhyVar.Heading = pEntity->Heading(); // ※ すべてのステートで、ステート内で物理量を更新するように変更したら、ここの処理は削除すること。。。

			//「切返し」ステートへ遷移
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
	// 遠心力による姿勢の傾きの解除
	pEntity->m_pAnimMgr->setBankAngle(0.0);
}

// #### Turn ステートのメソッド ########################################################################
const double Turn::TurnDulation = 0.4; // 切返しステートの継続時間
//const double Turn::TurnSpeed = 1.5 * DX_PI;   // 切返し時の旋回速度
// 2018/01/07 チューニング中
const double Turn::TurnSpeed    = 4.0 * DX_PI;   // 切返し時の旋回速度

Turn* Turn::Instance()
{
	static Turn instance;
	return &instance;
}

void Turn::Enter(PlayerCharacterEntity* pEntity)
{
	// タイマオン
	pEntity->StopWatchOn();

	// 切返し動作のアニメーションを再生
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::TurnFixHead);
	pEntity->m_pAnimMgr->ReserveAnim(PlayerCharacterEntity::TurnFinalFly, 0.0); // ブレンドするとモーション破綻する問題あるため

	// TurnDulation 時間内にアニメーション再生が完了するように再生ピッチを調整
	float AnimTotalTime = 16.0 + 5.0; // TurnFullの時間 + TurnFinalFlyの時間。ハードコーディングでスミマセン。
	// ↑本来なら、AnimUniqueInfo にアニメーションの長さを取得するメソッドをもたせるべきである。
	float PlayPitch = (float)(AnimTotalTime / TurnDulation);
	pEntity->m_pAnimMgr->setPitch(PlayPitch);

}

void Turn::StateTranceDetect(PlayerCharacterEntity* pEntity)
{
	// 廃棄予定のメソッド
}

void Turn::Calculate(PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar)
{
	PhyVar.init(); // 初期化

	// ## キャラクタのUpdate()での物理量の更新処理を回避するための処理
	// → すべてのステートで、ステート内で物理量を更新するように変更したら、ここの処理は削除すること。。。

	// 速度変化なし
	PhyVar.UseVelVar = false;
	PhyVar.Force = Vector3D(0, 0, 0);

	// キャラクタ向きの手動設定を有効
	PhyVar.UseHeading = true;
	PhyVar.Heading = pEntity->Heading();

	// 位置はデフォルト更新（現在の速度に従い更新）
	PhyVar.UsePosVar = false;

	// ## 「切返し」ステートの挙動
	if (!(pEntity->MoveInput().isZero()))
	{ // スティック入力あり
		
		assert(pEntity->Heading().y == 0.0);

		double dbg = pEntity->Heading().normalize()*pEntity->MoveInput().normalize();
		assert(dbg >-1.0);

		// キャラクタ向きの更新
		double turnSpeed = TurnSpeed * pEntity->TimeElaps();
		Vector3D newHead = RotateTowards3D(pEntity->Heading().normalize(), pEntity->MoveInput().normalize(), turnSpeed);
		
		assert(newHead.y == 0.0);

		// newHead.y > 0 になると動作が破綻するかもしれん、ということで、強制的に0にする処理を入れて様子をみる...
		if (newHead.y != 0)
		{
			newHead.y = 0;
			newHead = newHead.normalize();
		}

		// くそ、うまく動かない。
		// 修正したアルゴリズムがおかしいのか...
		// RotateTowards3D のなかで、yベクトルの0チェックをするようにしてみるか...

		// とりあえず、これで破綻は発生しないようになった。
		// RotateTowards3D のアルゴリズム自体を見直したほうがいい。
		
		pEntity->setHeading(newHead);

		PhyVar.Heading = pEntity->Heading(); // ※ すべてのステートで、ステート内で物理量を更新するように変更したら、ここの処理は削除すること。。。

		// キャラクタ速度の大きさを計算
		double SpeedAcceleration = 20.0f * pEntity->TimeElaps() * PlayerCharacterEntity::m_dConfigScaling; 
		double newSpeed = MoveTowards(pEntity->Velocity().len(), Run::runSpeed, SpeedAcceleration);

		Vector3D newVel = newSpeed * newHead;

		assert(newVel.len() < 1000.0);

		pEntity->setVelocity(newVel);

	}
	else
	{ // スティック入力なし

		// SpeedDeceleration
		double SpeedDeceleration = 15.0f * pEntity->TimeElaps() * PlayerCharacterEntity::m_dConfigScaling;
		double newSpeed = MoveTowards(pEntity->Velocity().len(), 0.0f, SpeedDeceleration);

		Vector3D newVel = newSpeed * pEntity->Heading();
		pEntity->setVelocity(newVel);

	}

	// ## 「切返し」ステートの終了判定
	if (pEntity->getStopWatchTime() > TurnDulation)
	{ // 切返しタイマ満了
		// 「走り／歩き」ステートに遷移
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
	// 遠心力による姿勢の傾きの解除
	pEntity->m_pAnimMgr->setBankAngle(0.0);
}




// #### SurfaceMove ステートのメソッド ########################################################################

// ##### 定数
//const double ThresholdSpeedRunToWark = 35.0*35.0; // Running<->Warking の速度の閾値（平方値）
const double SurfaceMove::ThresholdSpeedRunToWark = 50.0*50.0; // Running<->Warking の速度の閾値（平方値）
// 走り始め直後に進行方向を反転させた場合に切返し動作を行わないようにチューニング

const double SurfaceMove::ThresholdSticktiltRunToWark = 0.6;   // Running<->Warking のスティック傾きの閾値

const double SurfaceMove::MaxCentripetalForce = 500.0*10;   // 旋回時の最大向心力

const double SurfaceMove::MaxVelocity      = 65.0; // キャラクターの最大速度（スティックをmaxまで倒した時の最大速度）

const double SurfaceMove::ViscousRsisInert = 30.0;  // 慣性推進時の粘性抵抗係数
const double SurfaceMove::ViscousRsisAccel = 30.0;  // 加速時の粘性抵抗係数
// 2016/08/18
// 走り始め直後に進行方向を反転させた場合に切返し動作を行わないようにチューニング
// 慣性推進時と加速時の粘性抵抗を同じにする。
// 切返し動作時の加速力に合せて値を小さくチューニングした
// 2016/09/25
// ちょっと、加速が悪すぎる 20 → 30 にしてみる


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
		//pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Running  );
		pEntity->m_pAnimMgr->setAnimExStartTime( PlayerCharacterEntity::Running, 0.5 );

		/*
		// サブステートの評価を行う
		if( (pEntity->m_pVirCntrl->m_dStickL_len > ThresholdSticktiltRunToWark) 
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
		*/

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

void SurfaceMove::StateTranceDetect(PlayerCharacterEntity* pEntity)
{
	// m_bJmpChrgUsageFlg OFF
	static const double JmpChrgWaitTime = 0.1; // ジャンプ開始までの待ち時間

	// m_bJmpChrgUsageFlg ON
	static const double JmpChrgMaxTime = 0.3;  // ジャンプチャージの最大時間


	// #### ジャンプ関連
	if (pEntity->m_bJmpChrgUsageFlg)
	{ // m_bJmpChrgUsageFlg ON (ボタンの押し込み時間に応じで、ジャンプ力調整する機能)
		if (pEntity->m_bJmpChrgFlg)
		{ // ジャンプチャージ中
			// ButA が離された or ジャンプチャージ の最大時間を超過した
			if (!pEntity->m_pVirCntrl->ButA.isPushed()
				|| pEntity->getStopWatchTime() > JmpChrgMaxTime)
			{
				// → StateをJumpに更新
				//（Jump State のEnterの中でタイマ値からジャンプのサイズを計算する）
				pEntity->ChangeState(Jump::Instance());
			}

		}
		else if (pEntity->m_pVirCntrl->ButA.isNowPush())
		{ // ButA がこの瞬間に押された
			pEntity->m_bJmpChrgFlg = true; // JmpChrgFlgを上げる
			pEntity->StopWatchOn();        // タイマーセット
		}
	}
	else
	{ // m_bJmpChrgUsageFlg OFF
		if (pEntity->m_bJmpChrgFlg)
		{ // ジャンプチャージ中
			// ButA が離された or ジャンプチャージ の最大時間を超過した
			if (pEntity->getStopWatchTime() > JmpChrgWaitTime)
			{
				// StateをJumpに更新
				pEntity->ChangeState(Jump::Instance());
			}

		}
		else if (pEntity->m_pVirCntrl->ButA.isNowPush())
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



	/*
	// 速度ベクトル（規格化済み）と移動方向ベクトルの内積値がこの値以下であれば、切返しと判定する。
	//static const double InnerProductForStartTurn = 0.0;
	static const double InnerProductForStartTurn = -cos( DX_PI_F/6.0 );

	//if( pEntity->m_eMoveLevel == PlayerCharacterEntity::MvLvRunning )
	if( pEntity->Velocity().sqlen() >= ThresholdSpeedRunToWark )
	{ // 移動レベルが、Runningならば
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

	// 切返し動作をSuperMario64HD準拠化実験

	if (pEntity->Velocity().sqlen() >= ThresholdSpeedRunToWark)
	{
		// ホントは速度条件が違うが、とりあえず
		if(pEntity->MoveInput().isZero())
		{ 
			pEntity->ChangeState(Break::Instance());
			return;
		}

		// スティックの傾き方向とキャラクタ向きの角度を取得
		double AngleBetween = Angle3D(pEntity->MoveInput(), pEntity->Heading());

		//スティックの傾き方向と(ブレーキ開始時の※)キャラクタ向きとの角が110°より小さくなる
		if (AngleBetween >= DX_PI * (11.0f / 18.0f))
		{
			pEntity->ChangeState(Break::Instance());
			return;
		}
	}

	// #### Standingへ遷移 その他 MoveLeve制御
	static const double ThresholdSpeedForStop   = 5.0*5.0;   // Standing に遷移する速度の閾値（平方値）
	//static const double ThresholdSpeedRunToWark = 35.0*35.0; // Running<->Warking の速度の閾値（平方値）// SurfaceMove ステート クラスのメンバに変更
	//static const double ThresholdSticktiltRunToWark = 0.6;   // Running<->Warking のスティック傾きの閾値 // // SurfaceMove ステート クラスのメンバに変更

	pEntity->DBG_m_dDBG=pEntity->SpeedSq();


	if( (pEntity->m_pVirCntrl->m_dStickL_len > ThresholdSticktiltRunToWark)  )
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
		&& pEntity->m_pVirCntrl->m_dStickL_len==0 )
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

	static const double sqMaxCentripetalForce = MaxCentripetalForce*MaxCentripetalForce;

	static const double SensitivityCoefForTurning = 10; // 旋回時の入力に対する反応の良さ。

	PhyVar.init(); // 初期化

	// スティックの傾きの方向からEntityの移動方向を計算する
	Vector3D vStickTiltFromCam = pEntity->MoveInput();

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

	// 向心力の上限値を計算（スティックの傾きの大きさに比例するようにする）
	double LimitCentripetalForce = MaxCentripetalForce * pEntity->m_pVirCntrl->m_dStickL_len;
	//double LimitCentripetalForce = MaxCentripetalForce;

	// DBG用に変数退避
	DBG_m_vSteeringForce = vSteeringForce;

	// ##### 旋回時の挙動改善
	double DriveForce = 0;

	if( pEntity->m_eMoveLevel == PlayerCharacterEntity::MvLvWalking )
	//if( pEntity->Velocity().sqlen() < ThresholdSpeedRunToWark )
	{ // 速度が小さ場合は旋回挙動を適用しない
		
		Vector3D Force = vSteeringForce - eta * (pEntity->Velocity()) ;
		PhyVar.Force = Force ;

		assert( PhyVar.Force.y==0 );

		// DBG
		pEntity->DBG_m_bTurnWithouReduceSpeed = false;

	}
	else
	{
		// 向心力の決定
		m_dCentripetalForce = 0; // 向心力をクリア

		// 推進力の計算
		//   スティックが進行方向とは逆方向の場合は推進力 FDrive を切返しの動作の開始条件である
		//   速度閾値以上にならないように調整
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
	
		// 次のタイムステップの速度・位置を計算
		// * ルンゲクッタ法適用
		// * 旋回運動が不安定なのを解消したいだけなので、
		//   DriveForce, CentripetalForce はこのステップで固定

		Vector3D vK, vL;
		Vector3D vSumK, vSumL;
		Vector3D vVel = pEntity->Velocity(), vNxtVel;
		Vector3D vUpper = pEntity->Uppder();

		// DBG
		DBG_m_bCentripetalForceIsBounded = false; // 初期化

		// K1の計算
		vL = calculateForce( vVel, vUpper, vArrangeSteeringForce, DriveForce, eta, LimitCentripetalForce, m_dCentripetalForce )/pEntity->Mass();
		vK = vVel;
		vSumK = vK;
		vSumL = vL;

		// K2の計算
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, LimitCentripetalForce, m_dCentripetalForce )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K3の計算
		vNxtVel = vVel+0.5*pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, LimitCentripetalForce, m_dCentripetalForce )/pEntity->Mass();
		vK = vNxtVel;
		vSumK += 2*vK;
		vSumL += 2*vL;

		// K4の計算
		vNxtVel = vVel+pEntity->TimeElaps()*vL;
		vL = calculateForce( vNxtVel, vUpper, vArrangeSteeringForce, DriveForce, eta, LimitCentripetalForce, m_dCentripetalForce )/pEntity->Mass();
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

		// DBG
		pEntity->DBG_m_bTurnWithouReduceSpeed = true;

	}

	// #### デバック用に退避

	// DBG_m_vDriveForceForVel と DBG_m_vCentripetalForce の退避
	Vector3D vHeading = pEntity->Heading();
	Vector3D vSide    = vHeading % pEntity->Uppder();
	pEntity->DBG_m_vCentripetalForce = m_dCentripetalForce * vSide;
	pEntity->DBG_m_vDriveForceForVel = DriveForce * vHeading;

	// DBG_m_vSteeringForce の退避
	pEntity->DBG_m_vSteeringForce = DBG_m_vSteeringForce;

	// DBG_m_bCentripetalForceIsBounded の退避
	// ローカル DBG_m_bCentripetalForceIsBounded は SurfaceMove::calculateForce 内で収集している。
	pEntity->DBG_m_bCentripetalForceIsBounded = DBG_m_bCentripetalForceIsBounded; 

	// DBG_m_bTurnWithouReduceSpeed は処理内で退避


	return;
}

// SurfaceMove::Calculateの補助関数
// 2016/09/04
//   走り始めにEntity向きが振動する対策として、
//   CentripetalForce　ではなく、vSteeringForce を渡し、
//   その都度 CentripetalForce を計算する用に変更。
// 2017/04/22
//   いくらかリファクタリング
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
	// Entity速度(Input)から操舵力の向きを再計算
	Vector3D vHeading = vVel.normalize();
	Vector3D vSide    = vHeading % vUpper;

	// vArrangeSteeringForce から CentripetalForce を計算
	double CentripetalForce = vArrangeSteeringForce * vSide ;

	// カットオフ処理
	// DBG_m_bCentripetalForceIsBounded の設定はデバッグ用
	if( cutoff( CentripetalForce, LimitCentripetalForce) ) DBG_m_bCentripetalForceIsBounded = true;

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
		//pEntity->m_pAnimMgr->setPitch((float)((14.0/12.0)*MaxVelocity)); // ハードコーディングはマズイのだ
		// → これだとちょっとやり過ぎだな... あと、マリオではあってるかもしれないけど、このキャラクターだと合わないかも...

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
//const double OneEightyDegreeTurn::MaxVelocity      = 65.0;  // キャラクターの最大速度（スティックをmaxまで倒した時の最大速度）
//const double OneEightyDegreeTurn::ViscousRsisTurn   = 40.0;  // 粘性抵抗係数
//const double OneEightyDegreeTurn::ViscousRsisBreak  = 25.0;  // 粘性抵抗係数

const double OneEightyDegreeTurn::BreakPreDulation = 0.1;  // ブレーキ初期（SUB_BREAK_PRE）の継続時間
const double OneEightyDegreeTurn::TurnRotDulation  = 0.3;  // 切返し回転（SUB_TURN_ROT）の継続時間
const double OneEightyDegreeTurn::TurnFlyDulation  = 0.1;  // 切返し発射（SUB_TURN_FLY）の継続時間

const double OneEightyDegreeTurn::InnerProductForStartTurn = 0.0; // 速度ベクトル（規格化済み）と移動方向ベクトルの内積値がこの値以下であれば、切返しと判定する。

const double OneEightyDegreeTurn::TurningForceSize = 65.0 * 90.0 ;  // 切出し時の加速力大きさ
const double OneEightyDegreeTurn::BrakingForceSize = 65.0 * 10.0 * 3.5 ;  // ブレーキ中の制動力の大きさ
// 問題は、この大きさをどのように決めるか？？
// 粘性抵抗 * 最高速度 が加速力なのでこれに合わせる

const double OneEightyDegreeTurn::SqSlowDownEnough = 5.0 * 5.0;  // ブレーキ状態(SUB_BREAK_STAND/SUB_BREAK_TURN)の終了条件で使用。


OneEightyDegreeTurn* OneEightyDegreeTurn::Instance()
{
	static OneEightyDegreeTurn instance;
	return &instance;
}

void OneEightyDegreeTurn::Enter( PlayerCharacterEntity* pEntity )
{
	// サブステートの初期化
	m_eSubState = SUB_BREAK_PRE;

	// 切返し動作を開始した時の速度方向を記録
	m_vVelDirBeginning = pEntity->Velocity().normalize();

	// ブレーキ状態のタイマオン
	pEntity->StopWatchOn();

	// アニメーションの設定
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Breaking, 0.0 ); // ブレーキのアニメーションを設定
	//pEntity->m_pAnimMgr->setPitch( 20.0 );

	// BrakingDulation 時間内にアニメーション再生が完了するように再生ピッチを調整する
	float AnimTotalTime = pEntity->m_pAnimMgr->getCurAnimLength(); 
	//float PlayPitch = (float)(AnimTotalTime/BrakingDulation) + 1.0;
	float PlayPitch = (float)(AnimTotalTime/BreakPreDulation) ; // +1.0を消してみる。なんで盛ってあるんだろう？
	pEntity->m_pAnimMgr->setPitch(PlayPitch);

	// DBG_m_SubStateDurations を初期化（全て0にセットする）
	DBG_m_SubStateDurations.assign( 6, 0 );

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
	if( m_eSubState == SUB_BREAK_PRE
	 || m_eSubState == SUB_BREAK_TURN 
     || m_eSubState == SUB_BREAK_STAND )
	{ // ブレーキ状態

		// スティックの傾き方向が（切返し開始時の）速度方向に倒されたら（戻されたら）、 
		// 切返しをキャンセルして走りの状態に戻る。
		Vector3D MoveDir = pEntity->MoveInput().normalize();
		if( m_vVelDirBeginning*MoveDir > InnerProductForStartTurn )
		{
			pEntity->ChangeState( SurfaceMove::Instance() );
			return ;
		}

		// Substateの[Enter]でサブステートの評価（runかworkか）を行うこと★

	}

	// #### サブ状態に関しての状態遷移処理

	// 23:44 今日は、これくらいにするか。→ もう寝ましょう！

	if( m_eSubState == SUB_BREAK_PRE )
	{ // ブレーキ初期
		// 【終了条件】このサブ状態に遷移してから一定時間経過後
		if( pEntity->getStopWatchTime() > BreakPreDulation )
		{
			// 【遷移先Stateの判定】
			// 終了時のスティックの傾きの状態で、次の遷移状態を決定する。
			if( pEntity->m_pVirCntrl->m_dStickL_len==0 )
			{
				// スティックの傾きが 0
				// → 切返しなしブレーキ(SUB_BREAK_STAND)へ。
				m_eSubState = SUB_BREAK_STAND;
			}
			else
			{
				// スティックの傾きが 0 でない
				// → 切返しありブレーキ(SUB_BREAK_TURN) へ。
				m_eSubState = SUB_BREAK_TURN;
			}

			// 切出し動作で"発射"する方として、切出し動作開始した時のスティックの向き（規格化）に設定
			m_vTurnDestination = pEntity->MoveInput().normalize();

			// 継続時間を時間記録
			DBG_m_SubStateDurations[SUB_BREAK_PRE] = pEntity->getStopWatchTime();

			// タイマ初期化
			pEntity->StopWatchOn();

		}

	}
	else if( m_eSubState == SUB_BREAK_STAND )
	{ // 切返しなしブレーキ
		// 【終了条件】速度が0になるまで。
		if( pEntity->Velocity().sqlen() < SqSlowDownEnough )
		{
			// 切返し動作状態（OneEightyDegreeTurn）から抜けて基本動作状態（SurfaceMove）へ遷移
			pEntity->ChangeState( Standing::Instance() );

			// 継続時間を時間記録
			DBG_m_SubStateDurations[SUB_BREAK_STAND] = pEntity->getStopWatchTime();

			return ;
		}
	}
	else if( m_eSubState == SUB_BREAK_TURN )
	{ // 切返しありブレーキ
		// 【終了条件】速度が0になるまで。

		// 2016/11/27
		// 単純に速度の絶対値で判定する方式では、ブレーキ力による加速により
		// 速度が切返し方向に向いてしまうとサブ状態から抜けれなくなってしまう
		// ことがあるため、ブレーキ方向との内積値が負値になるように条件を変更する。

		//if( pEntity->Velocity().sqlen() < SqSlowDownEnough )
		if( pEntity->Velocity() * m_vVelDirBeginning < 0 )
		{
			// 切返し回転（SUB_TURN_ROT）サブ状態へ遷移
			m_eSubState = SUB_TURN_ROT;

			// 継続時間を時間記録
			DBG_m_SubStateDurations[SUB_BREAK_TURN] = pEntity->getStopWatchTime();

			// タイマオン
			pEntity->StopWatchOn();

			// （ブレーキ後の）切返し開始時の速度を記憶
			m_vVelEnterTurning = pEntity->Velocity();

			// 切返し動作のアニメーションを再生
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::TurnFirst );

			// TurningDulation 時間内にアニメーション再生が完了するように再生ピッチを調整
			float AnimTotalTime = pEntity->m_pAnimMgr->getCurAnimLength(); 
			//float PlayPitch = (float)(AnimTotalTime/TurningDulation) + 1.0;
			float PlayPitch = (float)(AnimTotalTime/TurnRotDulation) ;
			pEntity->m_pAnimMgr->setPitch(PlayPitch);

			return ;

		}
	}
	else if( m_eSubState == SUB_TURN_ROT )
	{ // 切返し回転
		// 【終了条件】このサブ状態に遷移してから一定時間経過後
		if( pEntity->getStopWatchTime() > TurnRotDulation )
		{
			// 切返し発射（SUB_TURN_FLY）サブ状態へ遷移
			m_eSubState = SUB_TURN_FLY;

			// 継続時間を時間記録
			DBG_m_SubStateDurations[SUB_TURN_ROT] = pEntity->getStopWatchTime();

			// タイマオン
			pEntity->StopWatchOn();

			// 切返ししながら飛び出すアニメーションを再生
			pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::TurnLatter );
			pEntity->m_pAnimMgr->ReserveAnim( PlayerCharacterEntity::TurnFinalFly, 2.0 );

			// TurnFlyDulation 時間内にアニメーション再生が完了するように再生ピッチを調整
			//float AnimTotalTime = pEntity->m_pAnimMgr->getCurAnimLength(); 
			float AnimTotalTime = 9.0 + 5.0 ; // TurnLatterの時間 + TurnFinalFlyの時間。ハードコーディングでスミマセン。
			float PlayPitch = (float)(AnimTotalTime/TurnFlyDulation) ;
			pEntity->m_pAnimMgr->setPitch(PlayPitch);

		}
	}
	else if( m_eSubState == SUB_TURN_FLY )
	{ // 切返し発射
		// 【終了条件】このサブ状態に遷移してから一定時間経過後
		if( pEntity->getStopWatchTime() > TurnFlyDulation )
		{
			// 切返しラスト（SUB_TURN_FIN）サブ状態へ遷移
			m_eSubState = SUB_TURN_FIN;

			// 継続時間を時間記録
			DBG_m_SubStateDurations[SUB_TURN_FLY] = pEntity->getStopWatchTime();

			// タイマオン
			pEntity->StopWatchOn();

			return;
		}
	}
	else if( m_eSubState == SUB_TURN_FIN )
	{ // 切返しラスト（速度方向の調整に使用）
		// 【終了条件】速度を調整するためのみのサブ状態なので直ぐに次の状態に遷移する。
		// 切返し動作状態（OneEightyDegreeTurn）から抜けて基本動作状態（SurfaceMove）へ遷移
		pEntity->ChangeState( SurfaceMove::Instance() );

		// 継続時間を時間記録
		DBG_m_SubStateDurations[SUB_TURN_FIN] = pEntity->getStopWatchTime();

		return;
	}

	// #### デバッグ用
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
	// ブレーキ力、切返し時の推進力は、
	// Entityの終速度と、粘性抵抗係数（=終速度への到達時間）より計算する。

	// 切返し完了時の最大速度を設定
	static double EndSpeed = 0.5 * (SurfaceMove::MaxVelocity + sqrt(SurfaceMove::ThresholdSpeedRunToWark) );

	PhyVar.init(); // 初期化

	if( m_eSubState == SUB_BREAK_PRE
	 || m_eSubState == SUB_BREAK_TURN 
     || m_eSubState == SUB_BREAK_STAND )
	{ // ブレーキ動作
		// 速度が十分小さくなるまでブレーキをかける。

		// ブレーキ力を計算
		Vector3D vBreakingForce = -1 * BrakingForceSize * m_vVelDirBeginning;

		// 粘性抵抗モデルからEntityに働く力を計算
		PhyVar.Force = vBreakingForce - SurfaceMove::ViscousRsisAccel * (pEntity->Velocity());

		// 向きを固定
		PhyVar.Heading = m_vVelDirBeginning;
		PhyVar.UseHeading = true;

		return;

	}
	else if( m_eSubState == SUB_TURN_ROT )
	{ // 切返し回転動作
		// 切り返しの方向に加速する。同時に向きを切返し方向へ回転。

		// ##### Entityに働く力を計算


		// 切返し完了時の目標速度
		Vector3D vTargetVelTurnEnd =
			EndSpeed
			* pEntity->m_pVirCntrl->m_dStickL_len
			* m_vTurnDestination;

		// 切返しを時間内に終速度まで加速するようにする
		double TurnRemainingTime = TurnRotDulation+TurnFlyDulation;
		PhyVar.VelVar = (vTargetVelTurnEnd-m_vVelEnterTurning) / TurnRemainingTime ;
		PhyVar.UseVelVar  = true;

		// ##### Entity向きの回転量を計算
		// * TurningDulation で向きの回転が完了するように、回転速度を調整

		// 現在のHeadingと、目的の方向のベクトルのなす角を計算
		double angle = atan2( 
			pEntity->Side()    * m_vTurnDestination, 
			pEntity->Heading() * m_vTurnDestination );

		// 得られた角度が負値の場合は、逆向きからの角度に変換（これにより、回転方向が固定される）
		if( angle < 0 )
		{
			angle = 2*DX_PI_F + angle;
		}

		// 現在の回転位置と、回転完了までの残り時間より、今回のタイムステップにおける回転量を計算
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
	else if( m_eSubState == SUB_TURN_FLY )
	{ // 切返し発射動作
		// キャラクタ向きの回転はしない。それ以外の動作は切返し回転(SUB_TURN_ROT)と同じ。

		// ##### Entityに働く力を計算

		// 切返し完了時の目標速度
		Vector3D vTargetVelTurnEnd =
			EndSpeed
			* pEntity->m_pVirCntrl->m_dStickL_len
			* m_vTurnDestination;

		// 切返しを時間内に終速度まで加速するようにする
		double TurnRemainingTime = TurnRotDulation+TurnFlyDulation;
		PhyVar.VelVar = (vTargetVelTurnEnd-m_vVelEnterTurning) / TurnRemainingTime ;
		PhyVar.UseVelVar  = true;

		// ##### Entity向きの回転量を計算

		// Entityの向きを切返し方向に向ける。
		// （ノーメンテだと、まだ速度が切返し方向へ向いていない場合があり、Entityが変な動きをするので）

		// 新しいHeadingベクトルを設定し完了
		PhyVar.Heading = m_vTurnDestination;
		PhyVar.UseHeading = true;
		

	}
	else if( m_eSubState == SUB_TURN_FIN )
	{ // 切返しラスト（速度方向の調整に使用）

		// 切り返し動作の完了時に
		// 直接速度方向を切り返し方向に向けるように補正する

		// ##### Entityに働く力を計算

		// 目標速度として、切り返し方向に補正する
		Vector3D vTargetVelTurnEnd = (pEntity->Velocity()*m_vTurnDestination)*m_vTurnDestination;

		// この処理ステップで目標速度に更新されるように加速度を調整する
		double TurnRemainingTime = pEntity->TimeElaps();
		PhyVar.VelVar = (vTargetVelTurnEnd-pEntity->Velocity()) / TurnRemainingTime ;
		PhyVar.UseVelVar  = true;

		// ##### Entity向きの回転量を計算

		// 新しいHeadingベクトルを設定し完了
		PhyVar.Heading = m_vTurnDestination;
		PhyVar.UseHeading = true;


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
	case OneEightyDegreeTurn::SUB_BREAK_PRE:    // ブレーキ初期
		SubStateString = "SUB_BREAK_PRE";
		break;
	case OneEightyDegreeTurn::SUB_BREAK_STAND:  // 切返しなしブレーキ
		SubStateString = "SUB_BREAK_STAND";
		break;
	case OneEightyDegreeTurn::SUB_BREAK_TURN:   // 切返しありブレーキ
		SubStateString = "SUB_BREAK_TURN";
		break;
	case OneEightyDegreeTurn::SUB_TURN_ROT:     // 切返し回転
		SubStateString = "SUB_TURN_ROT";
		break;
	case OneEightyDegreeTurn::SUB_TURN_FLY:     // 切返し発射
		SubStateString = "SUB_TURN_FLY";
		break;
	case OneEightyDegreeTurn::SUB_TURN_FIN:   // 切返しラスト（速度方向の調整に使用）
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
	// 遠心力による姿勢の傾きの解除
	pEntity->m_pAnimMgr->setBankAngle( 0.0 );
}

// 各サブ状態の継続時間を出力する
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


// #### Run ステートのメソッド ########################################################################
const double Run::runSpeed = 7.0 * PlayerCharacterEntity::m_dConfigScaling;  // 走りの最高速度（スティックを最大限に倒したときにｷｬﾗｸﾀが出せる最高速度）
//const double Run::runSpeed = 7.0 * 9.0;  // 走りの最高速度（スティックを最大限に倒したときにｷｬﾗｸﾀが出せる最高速度）
// ↑おそらくこれが原因だが。どうするか。

Run* Run::Instance()
{
	static Run instance;
	return &instance;
}

void Run::Enter(PlayerCharacterEntity* pEntity)
{
	// アニメーションの設定。
	// 走り出しを自然にするようにアニメーション開始位置とブレンド実施
	pEntity->m_pAnimMgr->setAnim(PlayerCharacterEntity::Walking, 5.0);

}

void Run::StateTranceDetect(PlayerCharacterEntity* pEntity)
{
	// 廃棄予定のメソッド
}

void Run::Calculate(PlayerCharacterEntity* pEntity, PhysicalQuantityVariation& PhyVar)
{
	PhyVar.init(); // 初期化

	// ## キャラクタのUpdate()での物理量の更新処理を回避するための処理

	// キャラクタのUpdate()内で速度の更新処理を行わないようにする。（→State内でsetVelocityで設定する）
	PhyVar.UseVelVar = false;
	PhyVar.Force = Vector3D(0, 0, 0);

	// キャラクタ向きは PhyVar.Heading の値を設定する。
	PhyVar.UseHeading = true;
	PhyVar.Heading = pEntity->Heading(); // ここでも設定しておかないと、途中で、状態遷移したときに、headingﾍﾞｸﾄﾙが消失する。

	// 位置はデフォルト更新（現在の速度に従い更新）
	PhyVar.UsePosVar = false;
	
	// 速度ﾍﾞｸﾄﾙを大きさと方向を分離
	double   VelSiz = pEntity->Velocity().len();
	// Vector3D VelDir = pEntity->Velocity() / VelSiz; <-これ、つかってねーな。
 
	if (!(pEntity->MoveInput().isZero()))
	{ // スティック入力あり

		// スティックからの入力方向が、キャラクタの向きと反対方向を向いていた場合。
		// 厳密にはには スティックからの入力方向 と キャラクタの向き の角度が、110°よりも大きかった場合。
		static double ThresholdAngle = DX_PI * (11.0f / 18.0f); // π × 110/180 ラジアン = 110°
		if (Angle3D(pEntity->MoveInput(), pEntity->Heading()) > ThresholdAngle)
		{
			// キャラクタの移動速度が、一定値以上の場合は急ブレーキ状態に遷移してreturn。
			if (VelSiz > runSpeed * 0.56)
			{
				pEntity->ChangeState(Break::Instance());
				return;
			}

			// もし、キャラクタ移動速度が、0の場合は、キャラクタの向きをスティックの入力方向へセットする。
			else if(VelSiz == 0)
			{
				pEntity->setHeading(pEntity->MoveInput().normalize());
			}
		}

		// ## 走り動作によるキャラクタの物理パラメータの更新を実施

		// キャラクタ向きを、スティックの入力方向へ回転させる。（回転速度：turnSpeedで）
		double turnSpeed = Turn::TurnSpeed * pEntity->TimeElaps();
		Vector3D newHead = RotateTowards3D(pEntity->Heading().normalize(), pEntity->MoveInput().normalize(), turnSpeed);
		assert(newHead.y == 0.0); // 正常性chk
		pEntity->setHeading(newHead);

		// # キャラクタ速度の大きさの更新

		// BoundedInterpolationに入力する、現在速度→加速度の導出関数の配列の定義
		static vector<double> Bounds{ 1.3, 3.0, 6.0 }; // C++11風の初期化方法。このやり方使えるのか？
		static vector<double> Values{ 16.0, 12.0, 8.0, 6.0 };
		
		// 加速度（or減速度）の計算
		double targetSpeed = runSpeed * pEntity->MoveInput().len();
		double acceration = targetSpeed >= VelSiz ? BoundedInterpolation(Bounds, Values, VelSiz / PlayerCharacterEntity::m_dConfigScaling) : 16.0; // ★VelSizをスケーリングしないと、思うように動かないよ。
		//double acceration = targetSpeed >= VelSiz ? BoundedInterpolation(Bounds, Values, VelSiz ) : 16.0 ; // ★VelSizをスケーリングしないと、思うように動かないよ。
		acceration *= PlayerCharacterEntity::m_dConfigScaling; // SM64HDの世界からのスケーリング

		// 接地している地表に、「再マッピング」している。
		// → 現状では平面状しか動かないので、今はオミットする。

		// 速度の更新
		double NewVelSiz = MoveTowards(VelSiz, targetSpeed, acceration*pEntity->TimeElaps());
		pEntity->setVelocity(NewVelSiz*newHead);

	}
	else
	{ // スティックからの入力が0の場合。
		if (VelSiz > runSpeed * 0.66)
		{
			// 移動速度が一定速度以上であれば、急ブレーキState(StopState)に遷移してreturn。
			// - その時のしきい値は、スティックの入力が有りのとき(0.56f)と0の時(0.66f)で差分が有ることに注意。
			pEntity->ChangeState(Break::Instance());
			return;
		}
		else
		{
			// そうでない場合は待機状態に遷移してreturn。
			// 移動速度が完全に0になっていなくても、待機状態に遷移するため、待機状態の動作で、減速処理があると言うことか？
			pEntity->ChangeState(Standing::Instance());
			return;
		}

	}

	// 物理量を、キャラクタのUpdate()内で正しく更新させるための処理。
	PhyVar.Heading = pEntity->Heading(); // ※ すべてのステートで、ステート内で物理量を更新するように変更したら、ここの処理は削除すること。。。

	// アニメーション関係の処理はそのまま移植。

	// ## 「歩き」or「走り」のアニメーションの設定

	if (pEntity->m_pVirCntrl->m_dStickL_len > 0.5)
	{ // スティックの入力の大きさが 0.5 より大きければ、
		
		// 「走り」のアニメーションが再生されていなければ、再生する。
		if (pEntity->m_pAnimMgr->getCurAnimID() != PlayerCharacterEntity::Running)
		{
			pEntity->m_pAnimMgr->setAnim(
				PlayerCharacterEntity::Running,
				8.0,
				false,
				true); // 位相を保ちながら切り替え（ブレンド）
		}

	}
	else
	{ // スティックの入力の大きさが 0.5 より小さければ、
		//「歩き」のアニメーションが再生されていなければ、再生する。
		if (pEntity->m_pAnimMgr->getCurAnimID() != PlayerCharacterEntity::Walking)
		{
			pEntity->m_pAnimMgr->setAnim(
				PlayerCharacterEntity::Walking,
				8.0,
				false,
				true); // 位相を保ちながら切り替え（ブレンド）
		}
	}

	// ｷｬﾗｸﾀの移動速度に応じたアニメーション再生速度になるように、再生ピッチを調整する。
	double speed = pEntity->Speed();

	// Running と Warking で再生ピッチがわける
	if (pEntity->m_pAnimMgr->getCurAnimID() == PlayerCharacterEntity::Running)
	{
		pEntity->m_pAnimMgr->setPitch((float)speed);
	}
	else if (pEntity->m_pAnimMgr->getCurAnimID() == PlayerCharacterEntity::Walking)
	{
		pEntity->m_pAnimMgr->setPitch((float)((14.0 / 12.0)*speed)); // ハードコーディングはマズイのだ
	}

	// 旋回による体の傾き（バンク）の設定
	static const double LikeGravity = 500.0 * 10 * 5; // バンク角の計算に使用。重力に相当する。
	double CentripetalForce = VelSiz * Turn::TurnSpeed; // 走り動作のSM64HD準拠化でバンク角の設定→向心力を使用できなくなるので、角速度（旋回速度）＋現在の速度から計算する必要がある。
	static Smoother<double> BankAngleSmoother(6, 0);  // 円滑化
	double bankangle = atan2(CentripetalForce, LikeGravity); // *重力と遠心力によりバンク角の計算
	pEntity->m_pAnimMgr->setBankAngle(BankAngleSmoother.Update(-bankangle));


}

void Run::Render(PlayerCharacterEntity* pEntity)
{
	;
};


void Run::Exit(PlayerCharacterEntity* pEntity)
{

	// 遠心力による姿勢の傾きの解除
	pEntity->m_pAnimMgr->setBankAngle(0.0);
}

// よし、準拠化完了？
// いや、未だだ。
// 20:12 コーディング完了。これで動くだろうか？
// 動いてくれ！
// なんか動かねー。