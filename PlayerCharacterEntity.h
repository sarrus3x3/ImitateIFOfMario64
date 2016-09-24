
#include "Vector3D.h"
#include "Vector2D.h"

// インクルードするとコンパイルエラー
//#include "State.h"
//#include "AnimationManager.h"
//#include "VirtualController.h"

#pragma once

class State;
class AnimationManager;
struct AnimUniqueInfo;
class VirtualController;

// #### 物理量の変化を定義する構造体（State::Calculateの結果渡し） ####
struct PhysicalQuantityVariation
{
	Vector3D Force;   // 力

	bool UseVelVar;   // VelVar 使用するflg
	Vector3D VelVar;  // 速度の変位（単位時間あたり）

	bool UsePosVar;   // PosVar 使用するflg
	Vector3D PosVar;  // 位置の変位（単位時間あたり）

	bool UseHeading;  // Entityの向きを直接設定する
	Vector3D Heading; // Entityの向きを直接設定する

	// 初期化メソッド
	void init()
	{
		Force  = Vector3D(0,0,0);
		UseVelVar = false;
		VelVar = Vector3D(0,0,0);
		UsePosVar = false;
		PosVar = Vector3D(0,0,0);
		UseHeading = false;
		Heading = Vector3D(0,0,0);
	};
};

// ################## PlayerCharacterEntity #######################
class PlayerCharacterEntity
{
private:
	// #### GameWorld 関連 ####
	double m_dTimeElapsed;           // タイムスライス（Updateで更新）
	LONGLONG m_lGameStepCounter;     // ゲーム開始してからのタイムステップ数をカウント（Updateで更新）（ゲームステップで１回のみ更新する状態の更新要否の判定に使用する）

	// #### ステート管理関連 ####
	State* m_pCurrentState;    // 現在の State
	State* m_pPreviousState;   // １つ前の State

public:
	// #### ステートに紐付いてEntityの状態を保持するメンバ変数 ####
	// ジャンプチャージ
	bool   m_bJmpChrgFlg;	   // ジャンプチャージ中か否かのflg

	// 移動レベル
	enum MoveLevelID
	{
		MvLvWalking = 0,
		MvLvRunning = 1
	} m_eMoveLevel;

private:
	// #### 物理情報 ####
	Vector3D m_vPos;            // Entityの位置
	Vector3D m_vVelocity;       // Entityの速度
	Vector3D m_vHeading;        // Entityの向き
	Vector3D m_vSide;           // Entityのローカル座標における横軸正方向
	Vector3D m_vUpper;			// Entityのローカル座標における上方向
	double   m_dMass;           // Entityの質量
	double   m_dBoundingRadius; // Entityのバウンディング半径

	// #### Collision関連 ####

public:
	// #### コンストラクタ・デストラクタ ####
	PlayerCharacterEntity(
		Vector3D vPos,
		Vector3D vVelocity,
		Vector3D vHeading,
		Vector3D vSide,
		double   dMass,
		double   dBoundingRadius,
		VirtualController* pVirCntrl
		); 
	// ~PlayerCharacterEntity(); // ★いつか作る

	// #### GameWorld 関連 ####
	VirtualController* m_pVirCntrl;  // 仮想コントローラーへのポインタ

	// #### Anmation管理 ####
	AnimationManager* m_pAnimMgr; // AnimationManagerのインスタンスへのポインタ

	// #### メソッド ####
	virtual void Update(double time_elapsed); // Entityの状態を更新
	virtual void Render(); // Entityを描画
	void ChangeState( State* ); // 保持しているStateを更新する



	// #### Anmation識別enumを定義 ####
	enum AnimationID
	{
		NoAnim   = 0,  // アニメーション設定なし※他のアニメーション再生後には設定できない？（全てのアニメーションをデタッチしても、アニメーションが初期化されないDXライブラリの仕様？）
		Standing = 1,  // 立ち
		Walking  = 2,  // 歩き
		Running  = 3,  // 走り
		Jumping  = 4,  // ジャンプ
		Jump_PreMotion = 5, // ジャンプ前の"溜め"のモーション
		Jump_Ascent    = 6, // ジャンプ上昇中
		Jump_Descent   = 7, // ジャンプ下降中
		Jump_Landing   = 8,  // ジャンプ後の着地
		Jump_Landing_Short = 9,  // ジャンプ後の着地（センター位置がもっとも下がるところまで。ジャンプ着地→走り出しのアニメーションで使用するため）
		Breaking = 10,  // ダッシュからの切返し状態の"ブレーキ中"のモーション
		Turning  = 11,  // ダッシュからの切返し状態の"切返し中"のモーション
		BreakAndTurn  = 12, // モーション004全体
		BreakingAfter = 13   // ダッシュからの切返しで、急ブレーキ後に切り返さず立ち状態に戻る時の、ブレーキからの起き上がりモーション
	};

	static const int m_iAnimIDMax=14;

	// #### アニメーション固有情報管理クラス
	// 全てのアニメーション固有情報が格納されたコンテナを管理するためのシングルトン
	class AnimUniqueInfoManager
	{
	private:
		AnimUniqueInfoManager();

		// コピーコンストラクタ、代入演算子を private に
		AnimUniqueInfoManager(const AnimUniqueInfoManager&);
		AnimUniqueInfoManager& operator=(const AnimUniqueInfoManager&);

	public:
		// シングルトン
		static AnimUniqueInfoManager* Instance();

		// 全てのアニメーション固有情報が格納されたコンテナ
		AnimUniqueInfo* m_pAnimUniqueInfoContainer;
	
	};

	// #### アクセサ類 ####
	// 参照系
	double   TimeElaps()const{return m_dTimeElapsed;}
	Vector3D Pos()const{return m_vPos;}
	Vector3D Velocity()const{return m_vVelocity;}
	double   Mass()const{return m_dMass;}
	Vector3D Side()const{return m_vSide;}
	Vector3D Uppder()const{return m_vUpper;}
	double   Speed()const{return m_vVelocity.len();}
	double   SpeedSq()const{return m_vVelocity.sqlen();}
	Vector3D Heading()const{return m_vHeading;}
	double   BRadius()const{return m_dBoundingRadius;}

	// 与えられたステートがPrvStateを一致するかをチェック（一致する→true）
	bool isMatchPrvState( State *pCheckState ){ return pCheckState==m_pPreviousState; }

	// 設定系
	void    setTimeElaps( double telaps ){ m_dTimeElapsed=telaps; }
	void    setVelocity( Vector3D newVel ){ m_vVelocity=newVel; };
	void    setPos( Vector3D newPos ){ m_vPos=newPos; };

	// #### タイマー類 ####
private:
	double m_dStopWatchCounter;
	
public:
	void   StopWatchOn(){ m_dStopWatchCounter=0; }; // ストップウォッチオン
	double getStopWatchTime(){ return m_dStopWatchCounter; }; // ストップウォッチの測定時間を取得

	// #### 補助メソッド ####
	
	// スティックの傾きの方向からEntityの移動方向を計算する
	Vector3D calcMovementDirFromStick();


	// #### 暫定 ####
public:
	bool m_bTouchGroundFlg;  // 接地flg
	bool m_bJmpChrgUsageFlg; // ジャンプチャージでジャンプ力を調整するIFを使用するFlg

	double DBG_m_dDBG;

	Vector3D DBG_m_vStickPos;              // （Entity平面上に投影した）スティックの傾きの位置
	void DBG_renderMovementDirFromStick(); // デバッグ用 （Entity平面上に投影した）スティックの傾きの位置を描画

};