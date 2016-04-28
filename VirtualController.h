
#include <vector>

#include "DxLib.h"

#include "Vector3D.h"
#include "Vector2D.h"

#pragma once

// コントローラー（ジョイパッド）管理クラス
class VirtualController
{
private:
	// コントローラーの描画で使用する情報
	double controllerscale;
	double ButtonRad; // ボタンの半径
	Vector2D ControllerBodyTL;  // コントローラー筐体の左上角の位置
	Vector2D ControllerBodyBR;  // コントローラー筐体の右下角の位置
	Vector2D ControllerBotA;    // ボタンAの位置（下）
	Vector2D ControllerBotB;    // ボタンBの位置（右）
	Vector2D ControllerBotY;    // ボタンYの位置（左）
	Vector2D ControllerBotX;    // ボタンXの位置（上）
	std::vector<Vector2D> CrossKeyOutLine;  // 十字キーの輪郭

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

public:

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

	// コントローラーの入力状態を保持する変数
	Button ButA;
	Button ButB;
	Button ButX;
	Button ButY;
	int Virti; // 上、下の方向キーの情報
	int Horiz; // 右、左の方向キーの情報

	// コンストラクタ
	VirtualController();

	// コントローラー状態（入力状態）を更新する
	void Update();

	// コントローラーを描画する
	void Render( Vector2D pos );


};

