#include <fstream> 
#include <sstream>  //カンマで区切るために必要
#include <string>

#include "VirtualController.h"

#include "MyUtilities.h" // コントローラー描画のためのツール使用

const int    VirtualController::iANALOG_STICK_TILT_UP_LIMIT = 32767;        // GetJoypadXInputState で取得するアナログスティックの傾きの最大値
const double VirtualController::dANALOG_STICK_TILT_IGNORE_THRESHOLD = 0.1; // アナログスティックの傾きが十分に小さくなったときに0とみなす閾値 

// ################## VirtualController クラスのメンバ・メソッドの定義 #######################

VirtualController::VirtualController() : 
	Virti(0), 
	Horiz(0), 
	UpPushCnt(0), 
	DownPushCnt(0), 
	LeftPushCnt(0), 
	RightPushCnt(0),
	m_eInpuDeviceMode( ID_KeyBord ),
	m_bAutoControlFlg(false),
	m_dTimeSinceAutoControlStart(0),
	m_eRecodeReplayState(ID_DoNothing),
	m_dRecodeReplayElapsed(0)

{
	// 内容を設定
	controllerscale = 8.0;

	// 十字キーの中心
	CrossKeyCenter = controllerscale * Vector2D( 2.0, 2.0 );

	// 十字キー領域半径
	CrossKeyAreaRadius = controllerscale * 1.5;

	// 十字キーの輪郭
	CrossKeyOutLine.resize(12);
	CrossKeyOutLine[0]  = controllerscale * Vector2D( 1.5, 0.5 );
	CrossKeyOutLine[1]  = controllerscale * Vector2D( 1.5, 1.5 );
	CrossKeyOutLine[2]  = controllerscale * Vector2D( 0.5, 1.5 );
	CrossKeyOutLine[3]  = controllerscale * Vector2D( 0.5, 2.5 );
	CrossKeyOutLine[4]  = controllerscale * Vector2D( 1.5, 2.5 );
	CrossKeyOutLine[5]  = controllerscale * Vector2D( 1.5, 3.5 );
	CrossKeyOutLine[6]  = controllerscale * Vector2D( 2.5, 3.5 );
	CrossKeyOutLine[7]  = controllerscale * Vector2D( 2.5, 2.5 );
	CrossKeyOutLine[8]  = controllerscale * Vector2D( 3.5, 2.5 );
	CrossKeyOutLine[9]  = controllerscale * Vector2D( 3.5, 1.5 );
	CrossKeyOutLine[10] = controllerscale * Vector2D( 2.5, 1.5 );
	CrossKeyOutLine[11] = controllerscale * Vector2D( 2.5, 0.5 );
	
	// 十字キー上キー矢印
	CrossKeyTriangleUp.resize(3);
	CrossKeyTriangleUp[0] = CrossKeyOutLine[1];
	CrossKeyTriangleUp[1] = CrossKeyOutLine[10];
	CrossKeyTriangleUp[2] = 0.5*(CrossKeyOutLine[0]+CrossKeyOutLine[11]);

	// 十字キー下キー矢印
	CrossKeyTriangleDown.resize(3);
	CrossKeyTriangleDown[0] = CrossKeyOutLine[4];
	CrossKeyTriangleDown[1] = CrossKeyOutLine[7];
	CrossKeyTriangleDown[2] = 0.5*(CrossKeyOutLine[5]+CrossKeyOutLine[6]);

	// 十字キー左キー矢印
	CrossKeyTriangleLeft.resize(3);
	CrossKeyTriangleLeft[0] = CrossKeyOutLine[1];
	CrossKeyTriangleLeft[1] = CrossKeyOutLine[4];
	CrossKeyTriangleLeft[2] = 0.5*(CrossKeyOutLine[2]+CrossKeyOutLine[3]);

	// 十字キー右キー矢印
	CrossKeyTriangleRight.resize(3);
	CrossKeyTriangleRight[0] = CrossKeyOutLine[10];
	CrossKeyTriangleRight[1] = CrossKeyOutLine[7];
	CrossKeyTriangleRight[2] = 0.5*(CrossKeyOutLine[9]+CrossKeyOutLine[8]);

	// コントローラー筐体
	ControllerBodyTL = controllerscale * Vector2D( 0.0, 0.0 );
	ControllerBodyBR = controllerscale * Vector2D( 12.0, 6.5 );

	// ボタン
	double CrossToBottLen = 8.0;
	Vector2D BotShift( CrossToBottLen-4.0, 0.0 ); 
	ControllerBotA	= controllerscale * ( BotShift + Vector2D( 6.0, 3.0 ));
	ControllerBotB	= controllerscale * ( BotShift + Vector2D( 7.0, 2.0 ));
	ControllerBotY	= controllerscale * ( BotShift + Vector2D( 5.0, 2.0 ));
	ControllerBotX	= controllerscale * ( BotShift + Vector2D( 6.0, 1.0 ));
	
	ButtonRad = controllerscale * 0.5; // ボタンの半径

	// アナログスティック
	m_RenderStickL.m_dRadius = controllerscale * 1.5;
	m_RenderStickR.m_dRadius = controllerscale * 1.5; 

	double len = 3.5*0.5*sqrt(2.0);
	Vector2D Shift;
	Shift =  Vector2D( 2.0, 2.0 );
	Shift += Vector2D( len, len );
	m_RenderStickL.m_vCenterPos = controllerscale * Shift;
	
	Shift =  Vector2D( 2.0, 2.0 );
	Shift += Vector2D( CrossToBottLen, 0.0 );
	Shift += Vector2D( -len, len );
	m_RenderStickR.m_vCenterPos = controllerscale * Shift;

	// 色の設定
	CntBodyColor = GetColor( 220, 220, 220 );
	CrossKeyColor = GetColor( 105, 105, 105 );
	BotAColor = GetColor( 255,   0,   0 );
	BotBColor = GetColor( 255, 255,   0 );
	BotXColor = GetColor(   0,   0, 255 );
	BotYColor = GetColor(   0, 255,   0 );

};

void VirtualController::Update( double time_elapsed )
{
	switch( m_eInpuDeviceMode )
	{
	case ID_KeyBord:
		UpdateAsKeyBord();
		break;
	case ID_GamePad:
		UpdateAsGamePad();
		if (m_bAutoControlFlg) UpdateAutoControl(time_elapsed);
		break;
	}

	// コントローラ操作の記録＆再生機能 の処理
	switch (m_eRecodeReplayState)
	{
	case ID_Recoding:
		Update_RecodeControl(time_elapsed);
		break;
	case ID_Replaying:
		Update_ReplayRecodedControl(time_elapsed);
		break;
	}

}

void VirtualController::UpdateAsKeyBord()
{
	// 全てのキー状態を取得
	char KeyStateBuf[ 256 ] ;
	GetHitKeyStateAll( KeyStateBuf ) ;

	// 十字キーの処理
	if( KeyStateBuf[KEY_INPUT_UP]     ){ UpPushCnt++;    }
	else{ UpPushCnt=0;    } 
	if( KeyStateBuf[KEY_INPUT_DOWN]   ){ DownPushCnt++;  }
	else{ DownPushCnt=0;  }
	if( KeyStateBuf[KEY_INPUT_LEFT]   ){ LeftPushCnt++;  }
	else{ LeftPushCnt=0;  }
	if( KeyStateBuf[KEY_INPUT_RIGHT]  ){ RightPushCnt++; }
	else{ RightPushCnt=0; }

	// 垂直方向
	if( KeyStateBuf[KEY_INPUT_UP] && KeyStateBuf[KEY_INPUT_DOWN] )
	{
		if( UpPushCnt  ==1 ){ Virti= 1; }
		if( DownPushCnt==1 ){ Virti=-1; }
	}
	else if( KeyStateBuf[KEY_INPUT_UP]   ){ Virti= 1; }
	else if( KeyStateBuf[KEY_INPUT_DOWN] ){ Virti=-1; }
	else{ Virti = 0; }

	// 水平方向
	if( KeyStateBuf[KEY_INPUT_LEFT] && KeyStateBuf[KEY_INPUT_RIGHT] )
	{
		if( RightPushCnt==1 ){ Horiz= 1; }
		if( LeftPushCnt ==1 ){ Horiz=-1; }
	}
	else if( KeyStateBuf[KEY_INPUT_RIGHT]){ Horiz= 1; }
	else if( KeyStateBuf[KEY_INPUT_LEFT] ){ Horiz=-1; }
	else{ Horiz = 0; }

	// ボタン
	// ButA
	if( KeyStateBuf[KEY_INPUT_SPACE] ){ ButA.ButCounter++; }
	else{ ButA.ButCounter=0; }

	// ButB
	if( KeyStateBuf[KEY_INPUT_LCONTROL] ){ ButB.ButCounter++; }
	else{ ButB.ButCounter=0; }

	// ButX
	if( KeyStateBuf[KEY_INPUT_X] ){ ButX.ButCounter++; }
	else{ ButX.ButCounter=0; }

	// ButY
	if( KeyStateBuf[KEY_INPUT_Y] ){ ButY.ButCounter++; }
	else{ ButY.ButCounter=0; }

};

void VirtualController::UpdateAsGamePad()
{
	// ----------------------------------------------------
	// ロジクールゲームパッドとの論理ボタン番号の対応を
	// ※ スーファミ配列 | ロジクール配列
	//     A（赤）| B(赤) :12
	//     B（黄）| A(緑) :13
	//     X（青）| Y(黄) :15
	//     Y（緑）| X(青) :14
	//     LB:8
	//     RB:9
	//     十字上:0
	//     十字下:1
	//     十字左:2
	//     十字右:3
	// ----------------------------------------------------


	// ##### コントローラ状態を更新
	GetJoypadXInputState( DX_INPUT_PAD1, &m_XinputState ) ;

	// ##### ボタン類の更新
	// A（赤）| B(赤) :12
	// ButA
	if( m_XinputState.Buttons[12] ){ ButA.ButCounter++; }
	else{ ButA.ButCounter=0; }

	// B（黄）| A(緑) :13
	// ButB
	if( m_XinputState.Buttons[13] ){ ButB.ButCounter++; }
	else{ ButB.ButCounter=0; }

	// X（青）| Y(黄) :15
	// ButX
	if( m_XinputState.Buttons[15] ){ ButX.ButCounter++; }
	else{ ButX.ButCounter=0; }

	// Y（緑）| X(青) :14
	// ButY
	if( m_XinputState.Buttons[14] ){ ButY.ButCounter++; }
	else{ ButY.ButCounter=0; }

	//     LB:8
	if( m_XinputState.Buttons[8] ){ ButLB.ButCounter++; }
	else{ ButLB.ButCounter=0; }

	//     RB:9
	if( m_XinputState.Buttons[9] ){ ButRB.ButCounter++; }
	else{ ButRB.ButCounter=0; }

	// ##### 十字キー
	// 垂直方向
	if( m_XinputState.Buttons[0] && m_XinputState.Buttons[1] )
	{
		if( UpPushCnt  ==1 ){ Virti= 1; }
		if( DownPushCnt==1 ){ Virti=-1; }
	}
	else if( m_XinputState.Buttons[0] ){ Virti= 1; }
	else if( m_XinputState.Buttons[1] ){ Virti=-1; }
	else{ Virti = 0; }

	// 水平方向
	if( m_XinputState.Buttons[2] && m_XinputState.Buttons[3] )
	{
		if( RightPushCnt==1 ){ Horiz= 1; }
		if( LeftPushCnt ==1 ){ Horiz=-1; }
	}
	else if( m_XinputState.Buttons[3] ){ Horiz= 1; }
	else if( m_XinputState.Buttons[2] ){ Horiz=-1; }
	else{ Horiz = 0; }

	// ##### アナログスティック
	static double  MaxOut = iANALOG_STICK_TILT_UP_LIMIT; // input.ThumbLX等の最大値

	// 左
	m_vStickL.x = (double)(m_XinputState.ThumbLX)/MaxOut;
	m_vStickL.y = (double)(m_XinputState.ThumbLY)/MaxOut;

	// 右
	m_vStickR.x = (double)(m_XinputState.ThumbRX)/MaxOut;
	m_vStickR.y = (double)(m_XinputState.ThumbRY)/MaxOut;

	// スティックの傾きが十分に小さい場合は 0 にカットオフする
	RoundingTinyStickTilt();

};

// スティックの傾きが十分に小さい場合は 0 にカットオフする
void VirtualController::RoundingTinyStickTilt()
{
	double lentmp;
	// 左
	lentmp = m_vStickL.len();
	if (lentmp < dANALOG_STICK_TILT_IGNORE_THRESHOLD)
	{
		m_dStickL_len = 0.0;
		m_vStickL.x = 0.0;
		m_vStickL.y = 0.0;
	}
	else m_dStickL_len = lentmp;

	// 右
	lentmp = m_vStickR.len();
	if (lentmp < dANALOG_STICK_TILT_IGNORE_THRESHOLD)
	{
		m_dStickR_len = 0.0;
		m_vStickR.x = 0.0;
		m_vStickR.y = 0.0;
	}
	else m_dStickR_len = lentmp;

};


void VirtualController::Render( Vector2D ControllerShift )
{
	// ################## コントローラーを描画 #######################
	/*
	// コントローラー筐体
	DrawBox2D( 
		(ControllerShift + ControllerBodyTL).toPoint(),
		(ControllerShift + ControllerBodyBR).toPoint(),
		CntBodyColor, TRUE );
		*/

	/*
	// 十字キーを塗りつぶし
	DrawBox2D( 
		(ControllerShift + CrossKeyOutLine[0]).toPoint(), 
		(ControllerShift + CrossKeyOutLine[6]).toPoint(),
		CrossKeyColor, TRUE );
	DrawBox2D( 
		(ControllerShift + CrossKeyOutLine[2]).toPoint(), 
		(ControllerShift + CrossKeyOutLine[8]).toPoint(),
		CrossKeyColor, TRUE );
	*/

	DrawCircle2D( 
		(ControllerShift + CrossKeyCenter).toPoint(),
		(int)CrossKeyAreaRadius,
		CrossKeyColor, TRUE );

	/*
	// 十字キー輪郭
	for( int b=0; b<CrossKeyOutLine.size(); b++ ){
		int e = (b+1)%CrossKeyOutLine.size();
		DrawLine2D( 
			(ControllerShift + CrossKeyOutLine[b]).toPoint(), 
			(ControllerShift + CrossKeyOutLine[e]).toPoint(),
			GetColor( 0,  0,  0 ) );
			//GetColor( 255,255,255 ) );
	}
	*/

	// 十字キー三角形輪郭
	/*
	DrawTriangle2D( 
		(ControllerShift + CrossKeyTriangleUp[0]).toPoint(),
		(ControllerShift + CrossKeyTriangleUp[1]).toPoint(),
		(ControllerShift + CrossKeyTriangleUp[2]).toPoint(),
		GetColor( 255,255,255 ), FALSE );

	DrawTriangle2D( 
		(ControllerShift + CrossKeyTriangleDown[0]).toPoint(),
		(ControllerShift + CrossKeyTriangleDown[1]).toPoint(),
		(ControllerShift + CrossKeyTriangleDown[2]).toPoint(),
		GetColor( 255,255,255 ), FALSE );

	DrawTriangle2D( 
		(ControllerShift + CrossKeyTriangleRight[0]).toPoint(),
		(ControllerShift + CrossKeyTriangleRight[1]).toPoint(),
		(ControllerShift + CrossKeyTriangleRight[2]).toPoint(),
		GetColor( 255,255,255 ), FALSE );

	DrawTriangle2D( 
		(ControllerShift + CrossKeyTriangleLeft[0]).toPoint(),
		(ControllerShift + CrossKeyTriangleLeft[1]).toPoint(),
		(ControllerShift + CrossKeyTriangleLeft[2]).toPoint(),
		GetColor( 255,255,255 ), FALSE );
		*/
	
		// 各種ボタン 
	DrawCircle2D( (ControllerShift + ControllerBotA).toPoint(), (int)ButtonRad, BotAColor, TRUE );
	DrawCircle2D( (ControllerShift + ControllerBotB).toPoint(), (int)ButtonRad, BotBColor, TRUE );
	DrawCircle2D( (ControllerShift + ControllerBotY).toPoint(), (int)ButtonRad, BotYColor, TRUE );
	DrawCircle2D( (ControllerShift + ControllerBotX).toPoint(), (int)ButtonRad, BotXColor, TRUE );


	// ###### 押下されている状態を描画

	// 十字キー
	if( Virti > 0 ) // 上
	{
		/*
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[0] ).toPoint(),
			(ControllerShift + CrossKeyOutLine[10]).toPoint(),
			GetColor( 255,255,255 ), TRUE );
			*/
		DrawTriangle2D( 
			(ControllerShift + CrossKeyTriangleUp[0]).toPoint(),
			(ControllerShift + CrossKeyTriangleUp[1]).toPoint(),
			(ControllerShift + CrossKeyTriangleUp[2]).toPoint(),
			GetColor( 255,255,255 ), TRUE );
	}
	if( Virti < 0 ) // 下
	{
		/*
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[4] ).toPoint(),
			(ControllerShift + CrossKeyOutLine[6] ).toPoint(),
			GetColor( 255,255,255 ), TRUE );
			*/
		DrawTriangle2D( 
			(ControllerShift + CrossKeyTriangleDown[0]).toPoint(),
			(ControllerShift + CrossKeyTriangleDown[1]).toPoint(),
			(ControllerShift + CrossKeyTriangleDown[2]).toPoint(),
			GetColor( 255,255,255 ), TRUE );

	}
	if( Horiz > 0 ) // 右
	{
		/*
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[10]).toPoint(),
			(ControllerShift + CrossKeyOutLine[8] ).toPoint(),
			GetColor( 255,255,255 ), TRUE );
			*/
		DrawTriangle2D( 
			(ControllerShift + CrossKeyTriangleRight[0]).toPoint(),
			(ControllerShift + CrossKeyTriangleRight[1]).toPoint(),
			(ControllerShift + CrossKeyTriangleRight[2]).toPoint(),
			GetColor( 255,255,255 ), TRUE );
	}
	if( Horiz < 0 )  // 左
	{
		/*
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[2] ).toPoint(),
			(ControllerShift + CrossKeyOutLine[4] ).toPoint(),
			GetColor( 255,255,255 ), TRUE );
			*/
		DrawTriangle2D( 
			(ControllerShift + CrossKeyTriangleLeft[0]).toPoint(),
			(ControllerShift + CrossKeyTriangleLeft[1]).toPoint(),
			(ControllerShift + CrossKeyTriangleLeft[2]).toPoint(),
			GetColor( 255,255,255 ), TRUE );
	}

	// A,B,X,Yボタン
	if( ButA.isPushed() )
	{
		DrawCircle2D( (ControllerShift + ControllerBotA).toPoint(), (int)ButtonRad, GetColor( 255,255,255 ), TRUE );
	}
	if( ButB.isPushed() )
	{
		DrawCircle2D( (ControllerShift + ControllerBotB).toPoint(), (int)ButtonRad, GetColor( 255,255,255 ), TRUE );
	}
	if( ButX.isPushed() )
	{
		DrawCircle2D( (ControllerShift + ControllerBotX).toPoint(), (int)ButtonRad, GetColor( 255,255,255 ), TRUE );
	}
	if( ButY.isPushed() )
	{
		DrawCircle2D( (ControllerShift + ControllerBotY).toPoint(), (int)ButtonRad, GetColor( 255,255,255 ), TRUE );
	}

	// ##### アナログスティックの状態を描画
	m_RenderStickL.Render(m_vStickL, ControllerShift);
	m_RenderStickR.Render(m_vStickR, ControllerShift);

};

// 描画補助関数
void VirtualController::RenderStickTrajectory::Render( Vector2D CurStickPos, Vector2D RendPos ) 
{
	// アナログスティックの現在位置を計算
	Vector2D Pos;
	Pos.x = ( m_dRadius*CurStickPos.x ) + m_vCenterPos.x ;
	Pos.y = (-m_dRadius*CurStickPos.y ) + m_vCenterPos.y ;
		
	CurIndex=++CurIndex%TrajectoryList.size();
	TrajectoryList[CurIndex] = Pos;

	// 描画領域の枠を描画
	DrawCircle2D( 
		(RendPos+m_vCenterPos).toPoint(),
		(int)m_dRadius ,
		GetColor( 255, 255, 255 ), FALSE );

	// 軌跡を描画
	// このロジックバグってるな。。。
	// ・始まりが CurIndex からになってない
	// ・閉路を書いている
	for( int i=0; i<TrajectoryList.size()-1; i++ )
	{
		int s = (i)%TrajectoryList.size();
		int e = (i+1)%TrajectoryList.size();
		DrawLine2D(
			(RendPos+TrajectoryList[s]).toPoint(),
			(RendPos+TrajectoryList[e]).toPoint(),
			GetColor( 255, 0, 0 ) );
	}

	// 現在位置を描画
	DrawCircle2D(
		(RendPos+TrajectoryList[CurIndex]).toPoint(),
		3 , GetColor( 255, 255, 255 ), FALSE );
};


// 入力デバイス判定機能（ゲームパッドが接続されていれば、入力デバイスをゲームパッドにセットする）
void VirtualController::CheckAndSetGamePadMode()
{
	// 単純に、関数 GetJoypadNum でパッド接続有無を確認し、
	// 接続されていれば、以降は、DX_INPUT_PAD1 から状態取得を行う。
	// i.e.複数パッド接続を想定しない。
	if( GetJoypadNum()>0 )
	{
		m_eInpuDeviceMode = ID_GamePad;
	}

	return ;
};

void VirtualController::DBG_ShowXinputState()
{
	// 画面に XINPUT_STATE の中身を描画
	int Color = GetColor( 255,255,255 ) ;
	DrawFormatString( 0, 0, Color, "LeftTrigger:%d RightTrigger:%d",
					m_XinputState.LeftTrigger, m_XinputState.RightTrigger ) ;
	DrawFormatString( 0, 16, Color, "ThumbLX:%d ThumbLY:%d",
					m_XinputState.ThumbLX, m_XinputState.ThumbLY ) ;
	DrawFormatString( 0, 32, Color, "ThumbRX:%d ThumbRY:%d",
					m_XinputState.ThumbRX, m_XinputState.ThumbRY ) ;
	DrawString( 0, 64, "Button", Color ) ;
	for( int i = 0 ; i < 16 ; i ++ )
	{
		DrawFormatString( 64 + i % 8 * 64, 64 + i / 8 * 16, Color,
							"%2d:%d", i, m_XinputState.Buttons[ i ] ) ;
	}
};

// ##### 動作検証のための自動操作機能

//自動操作機能のON / OFFを設定するメソッド
void VirtualController::AutoControlOnOff()
{
	// ON/OFF反転
	m_bAutoControlFlg = !m_bAutoControlFlg;

	// OFF→ON のときは初期化処理
	if (m_bAutoControlFlg == true) m_dTimeSinceAutoControlStart = 0;

};

//自動操作をタイムススライスで更新
void VirtualController::UpdateAutoControl(double TimeElaps)
{
	// * TimeSinceAutoControlStart の更新
	m_dTimeSinceAutoControlStart += TimeElaps;

	// * 自動操作シナリオにそってスティック状態（今のところこれだけ）を更新する。
	
	// 初期化
	static int TotalSections = 9;     // 総区間数
	
	/*
	// ## 「切返し接続用ブレーキ」モーションの撮影用
	// 移動するスティック座標配列
	static Vector2D StickPoss[9 + 1] = {
		Vector2D(0,0),
		Vector2D(1,0),
		Vector2D(1,0),
		Vector2D(-cos(0.01),sin(0.01)), // Vector2D(1,0)から180反転させると、RotateTowards3D のバグ？があるため、少しだけずらす。
		Vector2D(-cos(0.01),sin(0.01)),
		Vector2D(1,0),
		Vector2D(1,0),
		Vector2D(-cos(0.01),sin(0.01)), // Vector2D(1,0)から180反転させると、RotateTowards3D のバグ？があるため、少しだけずらす。
		Vector2D(-cos(0.01),sin(0.01)),
		Vector2D(0,0),
	};

	// 時間配分の配列（累積値）
	static double   TimeAlloc[9 + 1] = {
		0.0,
		0.1,
		0.1 + 3.0,
		0.1 + 3.0 + 0.1,
		0.1 + 3.0 + 0.1 + 0.8,
		0.1 + 3.0 + 0.1 + 0.8 + 0.1,
		0.1 + 3.0 + 0.1 + 0.8 + 0.1 + 0.8,
		0.1 + 3.0 + 0.1 + 0.8 + 0.1 + 0.8 + 0.1,
		0.1 + 3.0 + 0.1 + 0.8 + 0.1 + 0.8 + 0.1 + 0.8,
		0.1 + 3.0 + 0.1 + 0.8 + 0.1 + 0.8 + 0.1 + 0.8 + 0.1
	};
	*/

	// ## 「停止接続用ブレーキ」モーションの撮影用
	// 移動するスティック座標配列
	static Vector2D StickPoss[11 + 1] = {
		Vector2D(0,0),
		Vector2D(1,0),
		Vector2D(1,0),
		Vector2D(0,0),
		Vector2D(0,0),
		Vector2D(-1,0),
		Vector2D(-1,0),
		Vector2D(0,0),
		Vector2D(0,0),
		Vector2D(1,0),
		Vector2D(1,0),
		Vector2D(0,0),
	};

	// 時間配分の配列（累積値）
	static double   TimeAlloc[11 + 1] = {
		0.0,
		0.05,
		0.05 + 2.0,
		0.05 + 2.0 + 0.05,
		0.05 + 2.0 + 0.05 + 1.0,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05 + 1.0,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05 + 1.0 + 0.05,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0,
		0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05 + 1.0 + 0.05 + 2.0 + 0.05
	};

	Vector2D StickPos;
	for (int i=0; i<TotalSections; i++)
	{
		if (TimeAlloc[i] <= m_dTimeSinceAutoControlStart && m_dTimeSinceAutoControlStart < TimeAlloc[i + 1])
		{ // i 番目の区間に属していれば、
			double raito = (m_dTimeSinceAutoControlStart - TimeAlloc[i]) / (TimeAlloc[i + 1] - TimeAlloc[i]);
			StickPos = LerpV2D(StickPoss[i], StickPoss[i + 1], raito );

			// スティック状態を更新
			m_vStickL = StickPos;

			// スティックの傾きが十分に小さい場合は 0 にカットオフする
			RoundingTinyStickTilt();

			return;
		}
	}

	// 自動操作再生が完了したため、自動操作フラグをOFFにする
	m_bAutoControlFlg = false;

	return;

}

// 操作記録の開始／終了
// - コントローラステートの更新
// - 終了時は、操作記録配列をファイルにダンプ
void VirtualController::RecodeControlONOFF()
{
	// 再生中は、一度、再生モードを終了させてからでないと、記録モードに移ることはできない。
	if (m_eRecodeReplayState == ID_Replaying) return;

	if (m_eRecodeReplayState == ID_Recoding) // 記録モード中
	{
		// ### 記録モードの終了処理 

		// 記録モードを終了
		m_eRecodeReplayState = ID_DoNothing;

		// 記録キューの内容をファイルにダンプする
		SaveRecodeControlArray();

		// 記録キューを初期化（念の為）
		m_RecodeControlArray.clear();

	}
	else // それ以外
	{
		// ### 記録モードの開始処理 

		// 記録モードを開始
		m_eRecodeReplayState = ID_Recoding;

		// 記録キューを初期化（念の為）
		m_RecodeControlArray.clear();

		// 記録開始からの時間の初期化をしなくてはならない！
		m_dRecodeReplayElapsed = 0;

	}

}

// 記録した操作の再生の開始／終了
// - コントローラステートの更新
// - 開始時に、操作記録配列にファイルからロード。
void VirtualController::ReplayRecodedControlONOFF()
{
	// 録画モード中は、再生モードに入れない
	if (m_eRecodeReplayState == ID_Recoding) return;

	if (m_eRecodeReplayState == ID_Replaying) // 再生モード中
	{
		// ### 再生モードの終了処理 

		// 再生モードを終了
		m_eRecodeReplayState = ID_DoNothing;

		// 記録キューを初期化（念の為）
		m_RecodeControlArray.clear();

	}
	else // それ以外
	{
		// ### 再生モードの開始処理 

		// 再生モードを開始
		m_eRecodeReplayState = ID_Replaying;

		// 記録キューを初期化（念の為）
		m_RecodeControlArray.clear();

		// 操作記録配列をファイルからロード。
		LoadRecodeControlArray();

		// 記録開始からの時間の初期化をしなくてはならない！
		m_dRecodeReplayElapsed = 0;

	}

}

// 記録キュー(m_RecodeControlArray)の内容をファイルにダンプする
void VirtualController::SaveRecodeControlArray()
{
	std::ofstream fs("VirtualController-RecodeControlArray.txt", ios::out | ios::trunc); // 既存の内容を消去し、書き込み。

	// すべての要素をcsv形式でファイル出力する。
	for (int i = 0; i < m_RecodeControlArray.size(); i++)
	{
		fs << m_RecodeControlArray[i].m_dTime << "," 
			<< m_RecodeControlArray[i].m_vStickLoc.x << "," 
			<< m_RecodeControlArray[i].m_vStickLoc.y << std::endl;
	}

}


// 記録キュー(m_RecodeControlArray)をファイルからロード
//   C++メモ STLをできるだけ使ってCSVを読み込んでみる|http://www.tetsuyanbo.net/tetsuyanblog/23821

// http://cplplus.web.fc2.com/Last3.html
// http://www.cellstat.net/readcsv/ .. getline の分割結果をvector配列に格納
// http://marycore.jp/prog/cpp/convert-string-to-number/ .. 文字列(string)を数値(intやfloat)に変換
void VirtualController::LoadRecodeControlArray()
{
	std::ifstream fs("VirtualController-RecodeControlArray.txt");

	// ファイルの中身を一行ずつ読み取る
	string str = "";
	while (getline(fs, str))
	{
		string tmp = "";
		istringstream stream(str);
		vector<string> result;
		// 区切り文字がなくなるまで文字を区切っていく
		while (getline(stream, tmp, ','))
		{
			result.push_back(tmp);
		}
		
		m_RecodeControlArray.push_back(
			RecodeControl(
				stod(result[0]) , Vector2D(stod(result[1]), stod(result[2]))
			)
		);

	}

	fs.close();

}

// 操作記録時のコントローラの更新
void VirtualController::Update_RecodeControl(double TimeElaps)
{
	// 経過時間の更新
	m_dRecodeReplayElapsed += TimeElaps;

	// 現在のコントローラの状態を記録キューに積み上げるだけ。
	m_RecodeControlArray.push_back( RecodeControl(m_dRecodeReplayElapsed, m_vStickL) );
}

// 記録した操作の再生のコントローラの更新
void VirtualController::Update_ReplayRecodedControl(double TimeElaps)
{
	// 経過時間の更新
	m_dRecodeReplayElapsed += TimeElaps;

	// 再生開始してからの経過時間が、記録キューの記録時間からオーバーしてれば終了する
	if (m_RecodeControlArray.back().m_dTime < m_dRecodeReplayElapsed)
	{
		ReplayRecodedControlONOFF();
		return;
	}

	// m_RecodeControlArray から２分法で、補完するための両端のインデックスを調べる。
	int bgn = 0;
	int end = m_RecodeControlArray.size() - 1;
	while ((end - bgn) > 1) // 不等号、間違ってた
	{
		int mid = (bgn + end) / 2; // 整数同士の割算は少数点以下は切捨てられる。
		
		(m_RecodeControlArray[mid].m_dTime > m_dRecodeReplayElapsed ? end = mid : bgn = mid ); // ターゲット時間が記録時間を一致した場合はbgn側に寄せられるように設計
		
	}

	assert( end >= bgn );

	// 経過時間におけるスティック位置を補完処理して計算する
	Vector2D NewStickLoc;
	if (bgn == end) 
	{
		// bgn == end の場合
		NewStickLoc = m_RecodeControlArray[bgn].m_vStickLoc;
	}
	else
	{
		// 線形補間を実施
		double ratio = 
			(m_RecodeControlArray[end].m_dTime - m_dRecodeReplayElapsed) / 
			(m_RecodeControlArray[end].m_dTime - m_RecodeControlArray[bgn].m_dTime);
		NewStickLoc = m_RecodeControlArray[bgn].m_vStickLoc + 
			ratio * (m_RecodeControlArray[end].m_vStickLoc - m_RecodeControlArray[bgn].m_vStickLoc);
	}


	// コントローラ状態を更新
	m_vStickL = NewStickLoc;

	RoundingTinyStickTilt(); // なんとこれがないと動かない。内部で m_dStickL_len の更新をしていて、この整合がとれていないとバグるようだ...



}
string VirtualController::getRecodeReplayState()
{
	string str;

	switch (m_eRecodeReplayState)
	{
	case ID_DoNothing:
		str = "ID_DoNothing";
		break;
	case ID_Recoding:
		str = "ID_Recoding";
		break;
	case ID_Replaying:
		str = "ID_Replaying";
		break;
	}

	return str;
}
;