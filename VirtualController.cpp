#include "VirtualController.h"

// ################## コントローラー描画のための補助関数 #######################

int DrawLine2D( Point2D bgn, Point2D end, unsigned int Color )
{
	return DrawLine( bgn.x, bgn.y, end.x, end.y, Color );
};

int DrawCircle2D( Point2D cnt, int r, unsigned int Color, int FillFlag = TRUE )
{
	return DrawCircle( cnt.x, cnt.y, r, Color, FillFlag );
};

int DrawBox2D( Point2D TL, Point2D BR, unsigned int Color, int FillFlag = TRUE )
{
	return DrawBox( TL.x, TL.y, BR.x+1, BR.y+1, Color, FillFlag );
};

int DrawTriangle2D( Point2D V1, Point2D V2, Point2D V3, unsigned int Color, int FillFlag = TRUE )
{
	return DrawTriangle( 
		V1.x, V1.y,
		V2.x, V2.y, 
		V3.x, V3.y, 
		Color ,
		FillFlag ) ;
};


// ################## VirtualController クラスのメンバ・メソッドの定義 #######################

VirtualController::VirtualController() : 
	Virti(0), 
	Horiz(0), 
	UpPushCnt(0), 
	DownPushCnt(0), 
	LeftPushCnt(0), 
	RightPushCnt(0),
	m_eInpuDeviceMode( ID_KeyBord )
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

void VirtualController::Update()
{
	switch( m_eInpuDeviceMode )
	{
	case ID_KeyBord:
		UpdateAsKeyBord();
		break;
	case ID_GamePad:
		UpdateAsGamePad();
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

	static const int     MaxOut = 32767; // input.ThumbLX等の最大値

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
	else if( m_XinputState.Buttons[2] ){ Horiz= 1; }
	else if( m_XinputState.Buttons[3] ){ Horiz=-1; }
	else{ Horiz = 0; }

	// ##### アナログスティック
	m_vStickL.x = (double)(m_XinputState.ThumbLX)/(double)MaxOut;
	m_vStickL.y = (double)(m_XinputState.ThumbLY)/(double)MaxOut;

	m_vStickR.x = (double)(m_XinputState.ThumbRX)/(double)MaxOut;
	m_vStickR.y = (double)(m_XinputState.ThumbRY)/(double)MaxOut;


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