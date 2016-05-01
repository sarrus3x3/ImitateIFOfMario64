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
		int       TexturesHandle,   // テクスチャ画像のハンドル
		COLOR_U8  DifColor, // 頂点ディフューズカラー
		COLOR_U8  SpcColor  // 球の頂点スペキュラカラー
		);

	// 描画
	void Render();

};

// テキスチャを貼り付けられる球
// 【留意事項】
// ・パノラマ球を最背面に描画するため、RenderでZバッファをOFFにしている。
//   描画時はパノラマ球を一番初めに描画すること。（そうしないと、それより前に描画された図形が隠れてしまう！）
class TextureSphere3D
{
private:
	VERTEX3D* m_pVertex; // ポリゴン集合を保持
	int m_iPolygonNum;   // ポリゴン数

	// テクスチャ画像のハンドル
	int m_iTexturesHandle;

	Vector3D* m_pRawVertexPos; // VERTEX生成後に中心位置シフトに対応するためオリジナルの頂点位置情報を保持する
	Vector3D  m_vCntPos;       // 球の中心位置 
	MATERIALPARAM m_Material;         // マテリアルパラメータ
	MATERIALPARAM m_MaterialDefault;  // マテリアルパラメータ（デフォルト）

public:
	// コンストラクタ
	TextureSphere3D( 
		Vector3D  CntPos,           // 球中心の位置
		double    Radius,           // 球の半径
		bool      Outward,          // true:外向き、false:内向き
		int       DivNumLongi,      // 経度方向の分割数
		int       DivNumLati,       // 緯度方向の分割数
		int       TexturesHandle,   // テクスチャ画像のハンドル
		COLOR_U8  DifColor, // 頂点ディフューズカラー
		COLOR_U8  SpcColor  // 球の頂点スペキュラカラー
		);

	// 描画
	void Render();

	// 中心位置の設定（更新）
	void setCenterPos( Vector3D CntPos );

};

// 地面に方眼紙模様を描画する機能を追加
class GroundGrid
{
private:
	double m_dGridRange; // グリッド範囲
	int    m_iGridNum;   // グリッド数
	// 原点を中心に、x軸、y軸 : [-m_dGridRange, m_dGridRange] の範囲にグリッドを描画する

	// 直線の構造体
	struct LINE
	{
		Vector3D to;
		Vector3D from;
	};

	LINE* m_pVertiGrid; // 縦方向gird
	LINE* m_pHorizGrid; // 横方向gird

public:
	// コンストラクタ
	GroundGrid( 
		double GridRange,	// グリッド範囲
		int    GridNum 		// グリッド数
		);

	// 描画
	void Render();

};