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

// ############### 六角形クリスタル アイテム用 ###############
// class HexagonCrystal
// 実体が存在しないため削除


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
	int    m_iGridLinesNum;  // 構成する線分の個数（同じ方向分）

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

	// 頂点を直接編集するためのインターフェス
	Vector3D& editVertexes( int VectorIndex );
	int m_iMaxVectorNum;

	// 描画
	void Render();

};

// ############### 平面図形の基底クラス ###############
class BasePlaneFigure
{
protected:
	VERTEX3D* m_pVertex; // ポリゴン集合を保持
	int m_iPolygonNum;   // ポリゴン数

	Vector3D* m_pRawVertexPos; // VERTEX生成後に中心位置シフトに対応するためオリジナルの頂点位置情報を保持する
	Vector3D* m_pRawVertexNrm; // オリジナルの頂点法線ベクトル情報を保持する ※ オブジェクトの回転時に、法線

	MATERIALPARAM m_Material;         // マテリアルパラメータ
	MATERIALPARAM m_MaterialDefault;  // マテリアルパラメータ（デフォルト）
	
	// Zバッファを使用するかのフラグ
	bool m_bUseZBuffer;

public:
	// コンストラクタ
	BasePlaneFigure( 
		int       PolygonNum,       // ポリゴン（三角形）数
		COLOR_F   EmissivColor,     // オブジェクトの色（自己発光色）
		bool      UseZBuffer        // Zバッファを使用するか？
		);

	// 頂点を編集 <- 法線方向は調整できないので注意。今回はレンダリングを自己発光にするので不都合はないが...
	Vector3D* editVertexes(){ return m_pRawVertexPos; };

	// 全頂点数を取得
	int getAllVertexNum(){ return 3*m_iPolygonNum; };

	// m_pVertex を m_pRawVertexPos で初期化
	void resetVertex();

	// 描画
	void Render();

	// 与えられた行列 Mat で m_pVertex を変換
	void MatTransVertex( const MATRIX &Mat );


};


// ############### 平面輪っか ###############
class PlaneRing : public BasePlaneFigure
{
private:
	Vector3D  m_vCntPos;       // 球の中心位置 

public:
	// コンストラクタ
	PlaneRing( 
		double    Radius,           // 輪の半径（内径）
		double    Width,			// 輪の幅 
		int       DivNum            // 分割数
		);

	// 中心位置の設定（更新）
	void setCenterPos( Vector3D CntPos );

};

// ############### 平面上凸形図形・塗りつぶし ###############

// BasePlaneFigure の継承に書き直す...

class PlaneConvexFill : public BasePlaneFigure
{
public:
	// コンストラクタ
	PlaneConvexFill( 
		Vector2D  *pVertex2D,       // 凸形図形の輪郭頂点の配列
		int       DivNum,           // 分割数（頂点数）
		COLOR_F   EmissivColor      // オブジェクトの色（自己発光色）
		);

};



// ############### 線輪っか ###############
class LineRing
{
private:
	int       m_iVertexNum;    // 頂点数
	VECTOR*   m_pVECTORs;      // 頂点集合を保持
	Vector3D* m_pRawVertexPos; // VERTEX生成後に中心位置シフトに対応するためオリジナルの頂点位置情報を保持する

	unsigned int m_iColor;  // 線の色

public:

	// コンストラクタ
	LineRing( 
		double    Radius,           // 輪の半径
		int       DivNum,           // 分割数
		unsigned int Color          // 線の色
		);

	// 頂点を編集
	Vector3D* editVertexes(){ return m_pRawVertexPos; };

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

// ############### 自由閉路 ###############
// ３次元閉路
class LineFreeCycle
{
private:
	int       m_iVertexNum;    // 頂点数
	VECTOR*   m_pVECTORs;      // 頂点集合を保持
	Vector3D* m_pRawVertexPos; // VERTEX生成後に中心位置シフトに対応するためオリジナルの頂点位置情報を保持する

	unsigned int m_iColor;  // 線の色

public:

	// コンストラクタ
	LineFreeCycle( 
		int       DivNum,           // 分割数
		unsigned int Color          // 線の色
		);

	// 頂点を編集
	Vector3D* editVertexes(){ return m_pRawVertexPos; };

	// m_pVertex を m_pRawVertexPos で初期化
	void resetVertex();

	// 描画
	void Render();

	// 与えられた行列 Mat で m_pVertex を変換
	void MatTransVertex( const MATRIX &Mat );
};

// ############### ３Ｄ矢印 ###############
class Arrow3D
{
private:
	VERTEX3D* m_pVertex; // ポリゴン集合を保持
	int m_iPolygonNum;   // ポリゴン数

	MATERIALPARAM m_Material;         // マテリアルパラメータ
	MATERIALPARAM m_MaterialDefault;  // マテリアルパラメータ（デフォルト）

	// 矢印のパラメータ
	double m_dArwHight; // 矢印の矢の部分の高さ
	double m_dArwWidth; // 矢印の矢の部分の幅
	double m_dBdyWidth; // 矢印の棒の部分の幅

public:

	// コンストラクタ
	Arrow3D( 
		double    ArrowHight, 
		double    ArrowWidth, 
		double    BodyWidth, 
		COLOR_F   EmissivColor
		);

	// 描画
	void Render( Vector3D vBgn, Vector3D vEnd, Vector3D vUpper );

};

// ###############################################
// 基本図形クラス
// * 円柱、円錐、立方体などの簡単な図形。
//   フレームワークを定義した基底クラス。
//   これを継承して各図形クラスを定義するようにする。
// ###############################################
class BasicFig_Base
{
protected:
	//==========メンバ
	int m_iPolygonNum;     // ポリゴン数
	int m_iVectexNum;      // 頂点数

	VERTEX3D* m_pVertex;   // ポリゴン集合を保持
	Vector3D* m_pOrgVertx; // オリジナルの頂点位置情報を保持する
	Vector3D* m_pOrgNorms; // オリジナルの頂点法線ベクトル情報を保持する ※ オブジェクトの回転時に法線方向の追従が必要になるため。

	MATRIX m_MTransMat;    // 座標変換行列（メソッド setMatrix で設定する）


public:
	//==========メソッド

	// コンストラクタ
	// - 図形の特性に合わせて、様々な初期化方法になる。
	//--------------------------
	// * 基底クラスのコンストラクタのパラメータ
	//   - 色
	//   - 頂点数 or ポリゴン数
	// * 基底クラスのコンストラクタ内で行うこと。
	//   - ポインタのメモリ確保。。
	//   - m_pVertex へ色（＋α）の設定。
	//   - 具体的な形状の設定は、基底クラスを継承した図形クラスのコンストラクタ内で行うこと。
	BasicFig_Base( 
		int       PolygonNum,       // ポリゴン（三角形）数
		COLOR_U8  DifColor,         // 頂点ディフューズカラー
		COLOR_U8  SpcColor          // 球の頂点スペキュラカラー
		);

	// 基本変形の実施
	// - オリジナルのvertexに適用される（=図形のデフォルトの姿勢）
	//--------------------------
	// * m_pOrgVertx/m_pOrgNormsに対して変形を実施
	// * m_pVertex を更新（setMatrix を実行）。
	void setDefault( const MATRIX &Mat ) ;

	// 座標変換行列の設定
	// - 描画時に適用される変形
	// - 別の変換を適用すると前の変換はリセットされる。
	//--------------------------
	// * m_pOrgVertx/m_pOrgNorms → m_pVertex へ代入。
	// * m_pVertex に TransMat の座標変換を実施
	void setMatrix( const MATRIX &Mat ) ;

	// 描画
	//--------------------------
	// * 描画
	void Render() ;


};

// 円柱図形クラス（基底基本図形クラスを継承）
class BasicFig_Column : public BasicFig_Base
{
	// 個別に持つメンバはない？

public:
	//==========メソッド

	// コンストラクタ
	// - 図形の具体的な形状の設定を行う
	BasicFig_Column( 
		Vector3D  CenterPos,       // 底面の円形の中心位置
		double    Radius,          // 半径
		double    Hight,           // 円柱の高さ
		int       DivNum,          // 分割数（＝底面の円形の分割数）
		COLOR_U8  DifColor,        // 頂点ディフューズカラー
		COLOR_U8  SpcColor         // 球の頂点スペキュラカラー
		);

};

// 円錐図形クラス（基底基本図形クラスを継承）
class BasicFig_Cone : public BasicFig_Base
{
	// 個別に持つメンバはない？

public:
	//==========メソッド

	// コンストラクタ
	// - 図形の具体的な形状の設定を行う
	BasicFig_Cone( 
		Vector3D  CenterPos,       // 底面の円形の中心位置
		double    Radius,          // 半径
		double    Hight,           // 円錐の高さ
		int       DivNum,          // 分割数（＝底面の円形の分割数）
		COLOR_U8  DifColor,        // 頂点ディフューズカラー
		COLOR_U8  SpcColor         // 球の頂点スペキュラカラー
		);

};

// リッチな座標軸モデル
class CoordinateAxisModel
{
private:
	// メンバ

	// 構成要素となる基本図形
	// * インスタンス化をコンストラクタ内で実行するため、ポインタで持つようにしている。
	BasicFig_Cone   *m_pAxisX_Tip; // Ｘ軸の先端（矢印）部分
	BasicFig_Column *m_pAxisX_Bar; // Ｘ軸の棒部分
	BasicFig_Cone   *m_pAxisY_Tip; // Ｙ軸の先端（矢印）部分
	BasicFig_Column *m_pAxisY_Bar; // Ｙ軸の棒部分
	BasicFig_Cone   *m_pAxisZ_Tip; // Ｚ軸の先端（矢印）部分
	BasicFig_Column *m_pAxisZ_Bar; // Ｚ軸の棒部分

public:

	// コンストラクタ
	//--------------------------
	CoordinateAxisModel(
		double thickness,		// 座標軸の太さ（＝矢印大きさ）
		double axis_x_length,	// Ｘ軸の長さ
		double axis_y_length,	// Ｙ軸の長さ
		double axis_z_length	// Ｚ軸の長さ
		);

	// 座標変換行列の設定
	//--------------------------
	void setMatrix( const MATRIX &Mat ) ;

	// 描画
	//--------------------------
	void Render() ;

};