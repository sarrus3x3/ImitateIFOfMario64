#include "MyUtilities.h"

// ColorPaletteクラスの静的定数メンバ初期化
const int ColorPalette::Red    = GetColor( 255,   0,   0 );
const int ColorPalette::Blue   = GetColor(   0,   0, 255 );
const int ColorPalette::Green  = GetColor(   0, 255,   0 );
const int ColorPalette::Cyan   = GetColor(   0, 255, 255 );
const int ColorPalette::Yellow = GetColor( 255, 255,   0 );

// 矢印 - 先端が線のタイプ
void DrawArrow2D( Vector2D bgn, Vector2D end, unsigned int Color, int FillFlag, int Thickness )
{
	static const double AllowHeadHight=10.0f; // 矢印の矢の部分の高さ
	static const double AllowHeadWidth= 6.0f; // 矢印の矢の部分の幅（1/2）

	static Vector2D PosL( -AllowHeadHight,  AllowHeadWidth );
	static Vector2D PosR( -AllowHeadHight, -AllowHeadWidth );

	// bgn と end があまりにも小さいと変なことが起きる
	// 矢印描画をあきらめて、ただ線だけを描画する
	if( (end-bgn).sqlen() < 0.01 ){
		DrawLine2D( bgn.toPoint(), end.toPoint(), Color, Thickness );
		return ;
	}

	// 変換後の基底ベクトルを計算
	Vector2D head2D = (end-bgn).normalize();
	Vector2D side2D = head2D.side();

	// 矢印の三角形の位置をローカル座標→ワールド座標に変換 
	Vector2D psl = Vector2DToWorldSpace( PosL, head2D, side2D ) + end;
	Vector2D psr = Vector2DToWorldSpace( PosR, head2D, side2D ) + end;

	// 描画

	if( FillFlag )
	{
		Vector2D newend=end-(AllowHeadHight-1)*head2D; // 線が太くなると棒の部分が矢印の先端からはみ出るので棒の長さを微調整する
		DrawLine2D( bgn.toPoint(), newend.toPoint(), Color, Thickness ); // 本線
		DrawTriangle2D( psl.toPoint(), end.toPoint(), psr.toPoint(), Color, TRUE );
	}
	else
	{
		DrawLine2D( bgn.toPoint(), end.toPoint(), Color, Thickness ); // 本線
		DrawLine2D( psl.toPoint(), end.toPoint(), Color, Thickness ); // 矢印辺１
		DrawLine2D( psr.toPoint(), end.toPoint(), Color, Thickness ); // 矢印辺２
	}

};