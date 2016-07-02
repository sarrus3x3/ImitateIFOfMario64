#include "DxLib.h"

#include "Vector3D.h"
#include "Vector2D.h"

// ############### 円柱を描画するclass ###############
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

// ############### 細かいメッシュの平板 ###############
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

// ############### 直方体を描画するclass、画像貼り付けの実験 ###############
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

// ############### テキスチャを貼り付けられる球 ###############
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

	Vector3D* m_pRawVertexPos; // オリジナルの頂点位置情報を保持する
	Vector3D* m_pRawVertexNrm; // オリジナルの頂点法線ベクトル情報を保持する ※ オブジェクトの回転時に、法線
	Vector3D  m_vCntPos;       // 球の中心位置 
	MATERIALPARAM m_Material;         // マテリアルパラメータ
	MATERIALPARAM m_MaterialDefault;  // マテリアルパラメータ（デフォルト）

	bool m_bOutward; // 外向きかのフラグ

public:
	// オブジェクトタイプ
	enum ObjectTypeID
	{
		OBJECT_SKYDOME,    // スカイドーム（パノラマ球）: エミッシブ光のみ
		OBJECT_NOSPECULAR  // 光沢なし : スペキュラOFF
	};
private:
	ObjectTypeID m_eObjectType; // オブジェクトタイプを保持

public:
	// コンストラクタ
	TextureSphere3D( 
		Vector3D  CntPos,           // 球中心の位置
		double    Radius,           // 球の半径
		bool      Outward,          // 表面の向き  true:外向き、false:内向き
		int       DivNumLongi,      // 経度方向の分割数
		int       DivNumLati,       // 緯度方向の分割数
		int       TexturesHandle,   // テクスチャ画像のハンドル
		ObjectTypeID ObjectType     // オブジェクトタイプ
		);

	// m_pVertex を m_pRawVertexPos で初期化
	void resetVertex();

	// 描画
	void Render();

	// 設定した中心位置で m_pRawVertexPos から m_pVertex を（再）計算
	void setCenterPos( Vector3D CntPos );

	// m_pRawVertexPos をオイラー回転：Y軸回転→Z軸回転→X軸回転 の順に作用
	// ※ この関数を呼んだだけでは、m_pVertex に反映されないので、setCenterPos で更新すること。
	void rotEuler( double angX, double angZ, double angY );

	// 与えられた行列 Mat で m_pVertex を変換
	void MatTransVertex( const MATRIX &Mat );


};

// ############### 地面に方眼紙模様を描画する機能を追加 ###############
class GroundGrid
{
private:
	double m_dGridRange; // グリッド範囲
	int    m_iGridNum;   // グリッド数
	// 原点を中心に、x軸、y軸 : [-m_dGridRange, m_dGridRange] の範囲にグリッドを描画する
	int    m_iColor;     // グリッドの色

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
		int    GridNum,		// グリッド数
		int    Color		// グリッドの色
		);

	// 描画
	void Render();

};

// ############### 平面輪っか ###############
class PlaneRing
{
private:
	VERTEX3D* m_pVertex; // ポリゴン集合を保持
	int m_iPolygonNum;   // ポリゴン数

	Vector3D* m_pRawVertexPos; // VERTEX生成後に中心位置シフトに対応するためオリジナルの頂点位置情報を保持する
	Vector3D  m_vCntPos;       // 球の中心位置 
	MATERIALPARAM m_Material;         // マテリアルパラメータ
	MATERIALPARAM m_MaterialDefault;  // マテリアルパラメータ（デフォルト）

public:
	// コンストラクタ
	PlaneRing( 
		double    Radius,           // 輪の半径（内径）
		double    Width,			// 輪の幅 
		int       DivNum,           // 分割数
		COLOR_U8  DifColor,			// 頂点ディフューズカラー
		COLOR_U8  SpcColor			// 球の頂点スペキュラカラー
		);

	// 描画
	void Render();

	// 中心位置の設定（更新）
	void setCenterPos( Vector3D CntPos );

};

// ############### 線輪っか ###############
class LineRing
{
private:
	int       m_iVertexNum;    // 頂点数
	VECTOR*   m_pVECTORs;      // 頂点集合を保持
	Vector3D* m_pRawVertexPos; // VERTEX生成後に中心位置シフトに対応するためオリジナルの頂点位置情報を保持する
	MATERIALPARAM m_Material;         // マテリアルパラメータ
	MATERIALPARAM m_MaterialDefault;  // マテリアルパラメータ（デフォルト）

	unsigned int m_iColor;  // 線の色

public:

	// コンストラクタ
	LineRing( 
		double    Radius,           // 輪の半径
		int       DivNum,           // 分割数
		unsigned int Color          // 線の色
		);

	// m_pVertex を m_pRawVertexPos で初期化
	void resetVertex();

	// 描画
	void Render();

	// 与えられた行列 Mat で m_pVertex を変換
	void MatTransVertex( const MATRIX &Mat );

};

// ############### 線分 ###############
class LineSegment
{
private:
	int       m_iVertexNum;    // 頂点数
	VECTOR*   m_pVECTORs;      // 頂点集合を保持
	Vector3D* m_pRawVertexPos; // VERTEX生成後に中心位置シフトに対応するためオリジナルの頂点位置情報を保持する
	MATERIALPARAM m_Material;         // マテリアルパラメータ
	MATERIALPARAM m_MaterialDefault;  // マテリアルパラメータ（デフォルト）

	unsigned int m_iColor;  // 線の色

public:

	// コンストラクタ
	LineSegment( 
		Vector3D bgn,  // 始点
		Vector3D end,  // 終点
		unsigned int Color  // 線の色
		);

	// m_pVertex を m_pRawVertexPos で初期化
	void resetVertex();

	// 描画
	void Render();

	// 与えられた行列 Mat で m_pVertex を変換
	void MatTransVertex( const MATRIX &Mat );

};
