
#include "PlayerCharacterEntity.h"

// #### State の基底クラス ####
class State
{
public:

	virtual ~State(){}

	// ステートに入った際に実行される
	virtual void Enter(PlayerCharacterEntity*)=0;

	// ステートから出る際に実行される
	virtual void Exit(PlayerCharacterEntity*)=0;

	// State遷移に関する処理をする（入力情報等を評価しState更新の判定・実行をする）
	virtual void StateTranceDetect(PlayerCharacterEntity*)=0;

	// Entityの運動を処理する（このStateにおいてEntityに働く力を計算する）
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& )=0;

	// Entityの描画を処理する
	virtual void Render(PlayerCharacterEntity*)=0;

	// 自分のStateの名前を取得する
	virtual string getStateName()=0;


protected:
	
};

// #### ダミーステート（デバッグ用に使用） ####
class Dammy : public State
{
private:
	Dammy(){}

	// コピーコンストラクタ、代入演算子を private に
	Dammy(const Dammy&);
	Dammy& operator=(const Dammy&);

	// メンバ

public:

	// シングルトン
	static Dammy* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );
	virtual string getStateName(){ return "Dammy"; };
};

// #### Standing ステート ####
class Standing : public State
{
private:
	Standing(){}

	// コピーコンストラクタ、代入演算子を private に
	Standing(const Standing&);
	Standing& operator=(const Standing&);

public:

	// シングルトン
	static Standing* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );
	virtual string getStateName(){ return "Standing"; };


};

// #### Jump ステート ####
class Jump : public State
{
private:
	Jump(){}

	// コピーコンストラクタ、代入演算子を private に
	Jump(const Jump&);
	Jump& operator=(const Jump&);

	// #### メンバ ####

	// ジャンプサイズの識別ID
	enum JumpSizeID
	{
		Small = 0,
		Middl = 1,
		Large = 2
	};
	
	JumpSizeID m_eJumpSize;

	// Stateに入った初めのCalculateでジャンプするための撃力を加えるためのフラグ
	bool m_bInitImplusivForce; 

	bool m_bDescenting;      // 落下フラグ

public:

	// シングルトン
	static Jump* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );
	virtual string getStateName(){ return "Jump"; };


};

// ブレーキステート（切返し動作）
class Break : public State
{
private:
	Break() {}

	// コピーコンストラクタ、代入演算子を private に
	Break(const Break&);
	Break& operator=(const Break&);

	// #### メンバ ####
	float m_fAnim_BreaktoTurn_PlayPitch;
	float m_fAnim_BreaktoStop_PlayPitch;

public:

	// シングルトン
	static Break* Instance();

	virtual void Enter(PlayerCharacterEntity*);
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation&);
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity*);
	virtual string getStateName() { return "Break"; };
};

// 切返しステート（切返し動作）
class Turn : public State
{
private:
	Turn() {}

	// コピーコンストラクタ、代入演算子を private に
	Turn(const Turn&);
	Turn& operator=(const Turn&);


public:
	// #### 動作パラメータ
	static const double TurnDulation;  // 切返しステートの継続時間
	static const double TurnSpeed;     // 切返し時の旋回速度

	// シングルトン
	static Turn* Instance();

	virtual void Enter(PlayerCharacterEntity*);
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation&);
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity*);
	virtual string getStateName() { return "Turn"; };
};



// #### SurfaceMove ステート ####
class SurfaceMove : public State
{
private:
	SurfaceMove(){}

	// コピーコンストラクタ、代入演算子を private に
	SurfaceMove(const SurfaceMove&);
	SurfaceMove& operator=(const SurfaceMove&);

	// ##### グローバル定数
	static const double ThresholdSticktiltRunToWark;   // Running<->Warking のスティック傾きの閾値
	static const double MaxCentripetalForce;   // 旋回時の最大向心力
	static const double ViscousRsisInert;  // 慣性推進時の粘性抵抗係数

public:
	static const double ThresholdSpeedRunToWark;  // Running<->Warking の速度の閾値（平方値）
	static const double MaxVelocity; // キャラクターの最大速度（スティックをmaxまで倒した時の最大速度）
	static const double ViscousRsisAccel;  // 加速時の粘性抵抗係数


	// ##### メンバ変数
private:
	double m_dCentripetalForce; // 向心力の大きさ （遠心力によるバンク演出のために使用）
	
	// 以下はデバック用に保持
	//Vector3D DBG_m_vDriveForceForVel;      // 速度方向の推進力 ※ルンゲクッタ法を使用しているため正確ではない
	bool DBG_m_bCentripetalForceIsBounded; // 向心力が上限に達したかのflg ※ルンゲクッタ法を使用しているため正確ではない
	bool DBG_m_bTurnWithouReduceSpeed;     // 速度を落とさず旋回かのflg

public:

	// シングルトン
	static SurfaceMove* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );
	virtual string getStateName(){ return "SurfaceMove"; };

	// 補助
	//static Vector3D calculateForce( 
	Vector3D calculateForce( 
		Vector3D vVel, 
		Vector3D vUpper,
		Vector3D vArrangeSteeringForce,
		double DriveForce,
		double eta,
		double LimitCentripetalForce,
		double &CentripetalForce // 計算した向心力を返す
		);

	// #### デバック用 ####

	Vector3D DBG_m_vSteeringForce;

};

// #### OneEightyDegreeTurn ステート ####
// ダッシュからの切り返し、急方向転換。= 180°度方向転換
class OneEightyDegreeTurn : public State
{
private:
	OneEightyDegreeTurn() : DBG_m_SubStateDurations( vector<float>(6,0) )
	{}

	// コピーコンストラクタ、代入演算子を private に
	OneEightyDegreeTurn(const OneEightyDegreeTurn&);
	OneEightyDegreeTurn& operator=(const OneEightyDegreeTurn&);

	// ##### 定数
	
	// サブ状態の継続時間
	static const double BreakPreDulation;  // ブレーキ初期（SUB_BREAK_PRE）の継続時間
	static const double TurnRotDulation;   // 切返し回転（SUB_TURN_ROT）の継続時間
	static const double TurnFlyDulation;   // 切返し発射（SUB_TURN_FLY）の継続時間

	// 動作の物理モデルパラメータ
	static const double SqSlowDownEnough;  // ブレーキ状態(SUB_BREAK_STAND/SUB_BREAK_TURN)の終了条件で使用。

	static const double TurningForceSize;  // 切出し時の加速力大きさ
	static const double BrakingForceSize;  // ブレーキ中の制動力の大きさ

	static const double InnerProductForStartTurn; // 速度ベクトル（規格化済み）と移動方向ベクトルの内積値がこの値以下であれば、切返しと判定する。

	// メンバ
	Vector3D m_vVelDirBeginning; // OneEightyDegreeTurnにEnterした時の速度方向
	//Vector3D m_vStickTiltBeginning; // OneEightyDegreeTurnにEnterした時のスティックの傾きを記憶。（切返しで"発射"方向を計算する時に使用）→改善実装止めた。（切返し加速時の物理計算改善したら不自然じゃなくなったので）
	Vector3D m_vTurnDestination; // 切返し動作で"発射"する方向。切出し動作開始した時のスティックの向き（規格化）が設定
	Vector3D m_vVelEnterTurning; // 切返し開始時の速度を記憶。（切返し中の加速度の計算に使用）


	// サブ状態
	enum SubStateID
	{
		//SUB_BREAKING = 0,
		//SUB_TURNING  = 1,
		SUB_BREAK_PRE   = 0, // ブレーキ初期
		SUB_BREAK_STAND = 1, // 切返しなしブレーキ
		SUB_BREAK_TURN  = 2, // 切返しありブレーキ
		SUB_TURN_ROT    = 3, // 切返し回転
		SUB_TURN_FLY    = 4, // 切返し発射
		SUB_TURN_FIN    = 5  // 切返しラスト（速度方向の調整に使用）
	};
	SubStateID m_eSubState;


	double DBG_m_dAngle;
	double DBG_m_dRemainingTime;

	// サブ状態の継続時間の測定
	vector<float> DBG_m_SubStateDurations;   // サブ状態の継続時間を記録する配列

public:

	// シングルトン
	static OneEightyDegreeTurn* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );
	virtual string getStateName(){ return "OneEightyDegreeTurn"; };

	// ##### デバッグ用
	void DBG_expSubStateDurations( int &c ); // 各サブ状態の継続時間を出力する

};


// 走りステート（歩き動作、走り動作）
class Run : public State
{
private:
	Run() {}

	// コピーコンストラクタ、代入演算子を private に
	Run(const Run&);
	Run& operator=(const Run&);

public:
	// #### 動作パラメータ
	static const double runSpeed;  // 走りの最高速度（スティックを最大限に倒したときにｷｬﾗｸﾀが出せる最高速度）
	//static const double turnSpeed; // ｷｬﾗｸﾀの旋回可能な最大速度 → TurnState の TurnSpeed と共用する。

	// シングルトン
	static Run* Instance();

	virtual void Enter(PlayerCharacterEntity*);
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation&);
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity*);
	virtual string getStateName() { return "Run"; };
};

/*

class DraggingViewWindow : public State
{
private:

	DraggingViewWindow(){}

	// コピーコンストラクタ、代入演算子を private に
	DraggingViewWindow(const DraggingViewWindow&);
	DraggingViewWindow& operator=(const DraggingViewWindow&);

	// メンバ
	RealVector m_clickpt;	// ドラッグ開始時にクリックした場所
	bool m_draging;			// ドラッグ中か？
	RealVector m_viewloc;	// 表示画面の位置
	RealVector m_viewvel;	// 表示画面の速度

public:

	// シングルトン
	static DraggingViewWindow* Instance();
  
	virtual void Enter(DrawGraphMgr* );
	virtual void Execute(DrawGraphMgr* );
	virtual void Exit(DrawGraphMgr* );

	virtual void click( Point& );
	virtual void unclk( Point& );

};

class DraggingNodeMove: public State
{
private:

	DraggingNodeMove(){}

	// コピーコンストラクタ、代入演算子を private に
	DraggingNodeMove(const DraggingNodeMove&);
	DraggingNodeMove& operator=(const DraggingNodeMove&);

public:

	// シングルトン
	static DraggingNodeMove* Instance();
  
	virtual void Enter(DrawGraphMgr* );
	virtual void Execute(DrawGraphMgr* );
	virtual void Exit(DrawGraphMgr* );

	virtual void click( Point& );
	virtual void unclk( Point& );

};

// マウスからのクリックを受け止め、各Stateへ振り分けを行う。
class WatingForMouseInpt: public State
{
private:

	WatingForMouseInpt(){}

	// コピーコンストラクタ、代入演算子を private に
	WatingForMouseInpt(const WatingForMouseInpt&);
	WatingForMouseInpt& operator=(const WatingForMouseInpt&);

public:

	// シングルトン
	static WatingForMouseInpt* Instance();
  
	virtual void Enter(DrawGraphMgr* );
	virtual void Execute(DrawGraphMgr* );
	virtual void Exit(DrawGraphMgr* );

	virtual void click( Point& );
	virtual void unclk( Point& );

};

*/

