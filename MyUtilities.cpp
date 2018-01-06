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
// 与えられた位置に、（ワールド座標からみ）たローカル座標の基底軸を表示する関数
void DrawCoordi(MATRIX M, double scale)
{
	Vector3D vUnitX(M.m[0][0], M.m[0][1], M.m[0][2]);
	Vector3D vUnitY(M.m[1][0], M.m[1][1], M.m[1][2]);
	Vector3D vUnitZ(M.m[2][0], M.m[2][1], M.m[2][2]);
	Vector3D vTgtPs(M.m[3][0], M.m[3][1], M.m[3][2]);

	// ターゲットposを描画する
	DrawLine3D(vTgtPs.toVECTOR(), (vTgtPs + scale*vUnitX).toVECTOR(), ColorPalette::Red  ); // x軸を赤線で描画
	DrawLine3D(vTgtPs.toVECTOR(), (vTgtPs + scale*vUnitY).toVECTOR(), ColorPalette::Green); // y軸を緑線で描画
	DrawLine3D(vTgtPs.toVECTOR(), (vTgtPs + scale*vUnitZ).toVECTOR(), ColorPalette::Blue ); // z軸を青線で描画
};

// ###############################################
// ########## class VisualFootprint
// ###############################################

// 軌跡を記録
void VisualFootprint::Update(Vector3D Pos)
{
	CurIndex = ++CurIndex%TrajectoryList.size();
	TrajectoryList[CurIndex] = Pos;
};

// 軌跡を描画
void VisualFootprint::Render()
{
	// 軌跡を描画
	for (int i = 1; i<TrajectoryList.size(); i++)
	{
		int s = (CurIndex + i) % TrajectoryList.size();
		int e = (CurIndex + i + 1) % TrajectoryList.size();
		DrawLine3D(
			TrajectoryList[s].toVECTOR(),
			TrajectoryList[e].toVECTOR(),
			m_iColor);
	}

};