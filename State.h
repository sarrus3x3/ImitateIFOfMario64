
#include "PlayerCharacterEntity.h"

// #### State の基底クラス ####
class State
{
public:

	virtual ~State(){}

	// ステートに入った際に実行される
	virtual void Enter(PlayerCharacterEntity*)=0;

	// 入力情報等を評価しState更新の判定・実行をする
	virtual void StateTranceDetect(PlayerCharacterEntity*)=0;

	// EntityのUpdateで実行される
	// このStateにおいてEntityに働く力を計算する。
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& )=0;

	// EntityのRenderで実行される
	virtual void Render(PlayerCharacterEntity*)=0;

	// ステートから出る際に実行される
	virtual void Exit(PlayerCharacterEntity*)=0;

protected:

	// #### Calculate のサブメソッド ####
	
	// 旋回
	void Rotate( 
		double RotateVelSpeed, 
		PlayerCharacterEntity* pEntity, 
		PhysicalQuantityVariation& PhyVar );

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

};

// #### Standing ステート ####
class Standing : public State
{
private:
	Standing(){}

	// コピーコンストラクタ、代入演算子を private に
	Standing(const Standing&);
	Standing& operator=(const Standing&);

	// メンバ

public:

	// シングルトン
	static Standing* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );

};

// #### Running ステート ####
class Running : public State
{
private:
	Running(){}

	// コピーコンストラクタ、代入演算子を private に
	Running(const Running&);
	Running& operator=(const Running&);

	// メンバ

public:

	// シングルトン
	static Running* Instance();
  
	virtual void Enter(PlayerCharacterEntity* );
	virtual void StateTranceDetect(PlayerCharacterEntity*);
	virtual void Calculate(PlayerCharacterEntity*, PhysicalQuantityVariation& );
	virtual void Render(PlayerCharacterEntity*);
	virtual void Exit(PlayerCharacterEntity* );

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