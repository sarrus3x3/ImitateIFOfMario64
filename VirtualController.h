
#include <vector>

#include "DxLib.h"

#include "Vector3D.h"
#include "Vector2D.h"

#pragma once

// コントローラー（ジョイパッド）管理クラス
class VirtualController
{
public:
	// ##### コンフィグ値
	// 初期化は、VirtualController.cpp で
	static const int    iANALOG_STICK_TILT_UP_LIMIT;           // GetJoypadXInputState で取得するアナログスティックの傾きの最大値
	static const double dANALOG_STICK_TILT_IGNORE_THRESHOLD;   // アナログスティックの傾きが十分に小さくなったときに0とみなす閾値
	
	// ##### 入力デバイスモード

	// 入力デバイスモード（CameraWorkManagerを参考に）
	enum InputDeviceID
	{
		ID_KeyBord = 0, // ←初期値
		ID_GamePad = 1
	};

	// 入力デバイス種別
	InputDeviceID m_eInpuDeviceMode;

	// 入力デバイス判定機能（ゲームパッドが接続されていれば、入力デバイスをゲームパッドにセットする）
	void CheckAndSetGamePadMode();

private:
	// ##### コントローラーの描画で使用する情報
	double controllerscale;
	double ButtonRad; // ボタンの半径
	Vector2D ControllerBodyTL;  // コントローラー筐体の左上角の位置
	Vector2D ControllerBodyBR;  // コントローラー筐体の右下角の位置
	Vector2D ControllerBotA;    // ボタンAの位置（下）
	Vector2D ControllerBotB;    // ボタンBの位置（右）
	Vector2D ControllerBotY;    // ボタンYの位置（左）
	Vector2D ControllerBotX;    // ボタンXの位置（上）

	// 十字キー関連
	Vector2D CrossKeyCenter;     // 十字キーの中心
	double   CrossKeyAreaRadius; // 十字キー領域半径
	std::vector<Vector2D> CrossKeyOutLine;  // 十字キーの輪郭
	std::vector<Vector2D> CrossKeyTriangleUp;     // 十字キー上キー矢印
	std::vector<Vector2D> CrossKeyTriangleDown;   // 十字キー下キー矢印
	std::vector<Vector2D> CrossKeyTriangleLeft;   // 十字キー左キー矢印
	std::vector<Vector2D> CrossKeyTriangleRight;  // 十字キー右キー矢印

	int CntBodyColor;  // 本体の色
	int CrossKeyColor; // 十字キーの色
	int BotAColor;     // Aボタンの色
	int BotBColor;     // Bボタンの色
	int BotXColor;     // Yボタンの色
	int BotYColor;     // Xボタンの色

	// 十字キーの押下状態の補助情報
	int UpPushCnt;
	int DownPushCnt;
	int LeftPushCnt;
	int RightPushCnt;

	// ##### 内部メソッド
	void UpdateAsKeyBord(); // KeyBordの場合のUpdate
	void UpdateAsGamePad(); // GamePadの場合のUpdate

	// スティックの傾きが十分に小さい場合は 0 にカットオフする
	void RoundingTinyStickTilt();


public:

	// アナログスティック状態描画のためクラス
	class RenderStickTrajectory
	{
	public:
		// 軌跡格納
		std::vector<Vector2D> TrajectoryList; 
		int CurIndex; // TrajectoryListにおける現在のindex
		
		//Point2D CentRender; // 描画中心
		//int CanvasSize;

		Vector2D m_vCenterPos; // 描画中心
		double   m_dRadius;

		// コンストラクタ
		RenderStickTrajectory() : TrajectoryList(5), CurIndex(0) {};
		RenderStickTrajectory( Vector2D CntPos, double Radius ) : 
			m_vCenterPos( CntPos ), 
			m_dRadius( Radius ), 
			TrajectoryList(5), 
			CurIndex(0) {};


		// 描画
		void Render( Vector2D CurStickPos, Vector2D RendPos );

	} m_RenderStickL, m_RenderStickR;
	
	class Button
	{
	public:
		// このメンバへのアクセススコープを親クラス（VirtualController）に限定することができない
		int ButCounter;

		//コンストラクタ
		Button() : ButCounter(0) {};

		// ボタンは押されている
		bool isPushed()
		{
			if( ButCounter>0 ) return true;
			return false;
		};

		// ボタンはまさにこのタイミングで押された
		bool isNowPush()
		{
			if( ButCounter==1 ) return true;
			return false;
		};
	};

	// ##### コントローラーの入力状態を保持する変数

	// ロジクールコントローラの生情報
	XINPUT_STATE m_XinputState;
	
	// ボタン類
	Button ButA;
	Button ButB;
	Button ButX;
	Button ButY;

	Button ButLB; // LB
	Button ButRB; // RB

	// 十字キー
	int Virti; // 上、下の方向キーの情報
	int Horiz; // 右、左の方向キーの情報

	// アナログスティック状態
	Vector2D m_vStickL; // アナログスティック左
	Vector2D m_vStickR; // アナログスティック右
	double m_dStickL_len; // アナログスティック左の傾き大きさ （Updateで更新、無視閾値適用済み）
	double m_dStickR_len; // アナログスティック右の傾き大きさ （同上）

	// ＊スティック傾き情報に関する留意事項＊
	// ・大きさ最大値=1.0に規格化、ただし、m_vStickL.lem()<1 こともある。（入力自体の大きさがリミット32767を超えることがあるため）

	// コンストラクタ
	VirtualController();

	// コントローラー状態（入力状態）を更新する
	void Update( double time_elapsed );

	// コントローラーを描画する
	void Render( Vector2D pos );

	// ##### デバッグ用

	// m_XinputState の状態をデバック出力
	void DBG_ShowXinputState();


	// ##### 動作検証のための自動操作機能
	// 動作の検証用に、事前に設計した操作(入力)を流し込む機能

	//自動操作機能のON / OFFを設定するメソッド
	void AutoControlOnOff();
	//・Onのときは初期化
	//- TimeSinceAutoControlStart の 0クリ

	//自動操作機能の有効状態を管理するためのフラグ
	bool m_bAutoControlFlg;

	//自動操作をタイムススライスで更新
	void UpdateAutoControl(double TimeElaps);

	//自動操作開始してからの経過時間
	double m_dTimeSinceAutoControlStart;

	// 各変数の初期化を忘れずに。


};

