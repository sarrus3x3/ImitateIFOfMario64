#include "DxLib.h"

#include "Vector3D.h"
#include "Vector2D.h"

// 円柱を描画するclass
class Column
{
private:
	VERTEX3D* m_pVertex; // ポリゴン集合を保持
	int m_PolygonNum;    // ポリゴン数

public:
	// コンストラクタ
	Column( 
		Vector3D  CenterPos,       // 底面の円形の中心位置
		double    Radius,          // 半径
		double    Hight,           // 円柱の高さ
		int       DivNum,          // 分割数（＝底面の円形の分割数）
		COLOR_U8  DifColor, // 頂点ディフューズカラー
		COLOR_U8  SpcColor  // 球の頂点スペキュラカラー
		);

	// 描画
	void Render();

};

// 細かいメッシュの平板
class FineMeshedFlatPlate
{
private:
	VERTEX3D* m_pVertex; // ポリゴン集合を保持
	int m_PolygonNum;    // ポリゴン数

public:
	// コンストラクタ
	FineMeshedFlatPlate( 
		Vector2D  BLCorner,        // 平板の（世界平面上の）左下頂点座標
		Vector2D  TRCorner,        // 平板の（世界平面上の）右上頂点座標
		int       DivNum,          // 分割数（縦・横この数で分割されるため、ポリゴン三角形数は DivNum * DivNum * 2 となる）
		COLOR_U8  DifColor,        // 頂点ディフューズカラー
		COLOR_U8  SpcColor         // 球の頂点スペキュラカラー
		);

	// 描画
	void Render();

};

// 直方体を描画するclass、画像貼り付けの実験
class ParallelBox3D
{
private:
	VERTEX3D* m_pVertex; // ポリゴン集合を保持
	int m_PolygonNum;    // ポリゴン数

	// テクスチャ画像のハンドル
	int m_iTexturesHandle;

public:
	// コンストラクタ
	ParallelBox3D( 
		Vector3D  SmallVertex,      // 立方体の頂点で、座標的に一番小さい
		Vector3D  LargeVertex,      // 立方体の頂点で、座標的に一番大きい
		COLOR_U8  DifColor, // 頂点ディフューズカラー
		COLOR_U8  SpcColor  // 球の頂点スペキュラカラー
		);

	// 描画
	void Render();

};


