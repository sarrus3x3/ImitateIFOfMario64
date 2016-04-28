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


// ################## VirtualController クラスのメンバ・メソッドの定義 #######################

VirtualController::VirtualController() : 
	Virti(0), 
	Horiz(0), 
	UpPushCnt(0), 
	DownPushCnt(0), 
	LeftPushCnt(0), 
	RightPushCnt(0) 
{
	// 内容を設定
	controllerscale = 8.0;

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

	ControllerBodyTL = controllerscale * Vector2D( 0.0, 0.0 );
	ControllerBodyBR = controllerscale * Vector2D( 8.0, 4.0 );

	ControllerBotA	= controllerscale * Vector2D( 6.0, 3.0 );
	ControllerBotB	= controllerscale * Vector2D( 7.0, 2.0 );
	ControllerBotY	= controllerscale * Vector2D( 5.0, 2.0 );
	ControllerBotX	= controllerscale * Vector2D( 6.0, 1.0 );
	
	ButtonRad = controllerscale * 0.5; // ボタンの半径

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
};

void VirtualController::Render( Vector2D ControllerShift )
{
	// ################## コントローラーを描画 #######################
	// コントローラー筐体
	DrawBox2D( 
		(ControllerShift + ControllerBodyTL).toPoint(),
		(ControllerShift + ControllerBodyBR).toPoint(),
		CntBodyColor, TRUE );

	// 十字キーを塗りつぶし
	DrawBox2D( 
		(ControllerShift + CrossKeyOutLine[0]).toPoint(), 
		(ControllerShift + CrossKeyOutLine[6]).toPoint(),
		CrossKeyColor, TRUE );
	DrawBox2D( 
		(ControllerShift + CrossKeyOutLine[2]).toPoint(), 
		(ControllerShift + CrossKeyOutLine[8]).toPoint(),
		CrossKeyColor, TRUE );

	/*
	// 十字キー
	for( int b=0; b<CrossKeyOutLine.size(); b++ ){
		int e = (b+1)%CrossKeyOutLine.size();
		DrawLine2D( 
			(ControllerShift + CrossKeyOutLine[b]).toPoint(), 
			(ControllerShift + CrossKeyOutLine[e]).toPoint(),
			GetColor( 0,  0,  0 ) );
			//GetColor( 255,255,255 ) );
	}
	*/

	// 各種ボタン 
	DrawCircle2D( (ControllerShift + ControllerBotA).toPoint(), (int)ButtonRad, BotAColor, TRUE );
	DrawCircle2D( (ControllerShift + ControllerBotB).toPoint(), (int)ButtonRad, BotBColor, TRUE );
	DrawCircle2D( (ControllerShift + ControllerBotY).toPoint(), (int)ButtonRad, BotYColor, TRUE );
	DrawCircle2D( (ControllerShift + ControllerBotX).toPoint(), (int)ButtonRad, BotXColor, TRUE );


	// 押下されている状態を描画
	if( Virti > 0 )
	{
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[0] ).toPoint(),
			(ControllerShift + CrossKeyOutLine[10]).toPoint(),
			GetColor( 255,255,255 ), TRUE );
	}
	if( Virti < 0 )
	{
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[4] ).toPoint(),
			(ControllerShift + CrossKeyOutLine[6] ).toPoint(),
			GetColor( 255,255,255 ), TRUE );
	}
	if( Horiz > 0 )
	{
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[10]).toPoint(),
			(ControllerShift + CrossKeyOutLine[8] ).toPoint(),
			GetColor( 255,255,255 ), TRUE );
	}
	if( Horiz < 0 )
	{
		DrawBox2D( 
			(ControllerShift + CrossKeyOutLine[2] ).toPoint(),
			(ControllerShift + CrossKeyOutLine[4] ).toPoint(),
			GetColor( 255,255,255 ), TRUE );
	}

	if( ButA.isPushed() )
	{
		DrawCircle2D( (ControllerShift + ControllerBotA).toPoint(), (int)ButtonRad, GetColor( 255,255,255 ), TRUE );
	}


};

