//#include <functional>

#include "DxLib.h"

#include "..\\ImitateIFOfMario64\\Vector3D.h"
#include "..\\ImitateIFOfMario64\\Vector2D.h"
#include "..\\ImitateIFOfMario64\\SolidObjects.h"


// 2016/09/15
// スティックの傾きをキャラクタ平面に投影して
// 折角class化したんだから、ソースとヘッダを別ファイルにしようぜ？

// 2016/09/17 14:36
// クラス化完了....


// 扇形
// コンストラクタにパラメータを与えて、扇形の頂点配列を生成するだけ
class SectorFigure2D
{
public:
	// 頂点配列
	Vector2D *m_pVertexes;

	// 頂点数
	int m_iVertexNum;

	// 描画するときの倍率
	static double m_dRenderScale;

	// 描画する時の色
	int m_iRenderColor;

	// 描画する時の回転量
	double m_dRotation;

	// ### コンストラクタ
	// ① 開始角と終了角（ラジアン）を与えられたら、原点中心に扇形を生成する
	SectorFigure2D( double Radius, double StartAng, double EndAng, int DivNum );

	// ② 開始角と終了角をベクトルで与えることもできる
	SectorFigure2D( double Radius, Vector2D StartVec, Vector2D EndVec, int DivNum );

	// デストラクタ
	~SectorFigure2D();

	// 平面キャンバス上に描画する機能を実装する
	void RenderOnCanvas( Vector2D RenderPos );

	// 描画用ポリゴンの生成
	void GeneratePolygons();

private:

	// ポリゴン総数
	int m_iPolygonNum;

	// 描画用のポリゴン集合（ポリゴン三角形の頂点を順番に一つの配列に詰めている）
	Vector2D* m_pPolyVertex;


};

// スクリーン平面クラス
	// 描画オブジェクト
		// 線分（枠）
		// スクリーン上のキャラクタ位置
		// スティックの可動範囲円
	// Render
	// 座標変換行列（ローカル→ワールド）
class ScreenPlane
{
private:
	// #### 描画オブジェクト ####

	// 線分（枠）
	LineSegment m_oFrameT;  // 上枠
	LineSegment m_oFrameB;	// 下枠
	LineSegment m_oFrameL;	// 左枠
	LineSegment m_oFrameR;	// 右枠

	LineSegment m_oAxisX; // x軸
	LineSegment m_oAxisY; // y軸
	
	LineRing    m_oStickRing; // スティックの可動範囲円

	// スクリーン上のキャラクタ位置
	Vector3D    m_vCntStickRing;

public:
	// ワールド座標への変換行列
	MATRIX m_mLocalToWorldMatrix;
	
	// #### メソッド ####

	// コンストラクタ
	ScreenPlane( double Width, double Hight, double Radius );

	// オブジェクトの頂点情報計算
	void setVertex();

	// 描画
	void Render();

	// スクリーン上のキャラクタ位置設定（※ローカル座標）
	void setCntStickRing( Vector3D cnt ){ m_vCntStickRing = cnt; };
};


class StickTiltProjectorDemo
{
public:

	// モデルのステータス
	Vector3D m_vModelCamPos;          // 説明用カメラの位置
	Vector3D m_vModelCamGazePoint;    // 説明用カメラの焦点
	double   m_dModelCamNearClipDist; // 説明用カメラのクリップ距離
	Vector3D m_vPlayerPos;            // （操作する）キャラクタの位置

	Vector3D m_vStickTiltOnScreen; // スティックの傾きの方向（x-z平面上）

	static double SectorRadius;
	static double AngeSize;

	// 垂直方向のスティック傾きの始点
	Vector2D m_vVertiStickTiltDirBgn;

	// 垂直方向のスティック傾きの終点
	Vector2D m_vVertiStickTiltDirEnd;

	// 水平方向のスティック傾きの始点
	Vector2D m_vHorizStickTiltDirBgn;

	// 水平方向のスティック傾きの終点
	Vector2D m_vHorizStickTiltDirEnd;

	// 3Dモデル情報
	int m_iModelCamHandle;

	// スクリーンオブジェクト
	ScreenPlane m_ScPlane;

	// スクリーン上に描画するグリッドの範囲
	double m_dGridRangeOnScreen;

	// コンストラクタ
	StickTiltProjectorDemo(
		Vector3D vModelCamPos,          // 説明用カメラの位置
		Vector3D vModelCamGazePoint,    // 説明用カメラの焦点
		double   dModelCamNearClipDist, // 説明用カメラのクリップ距離
		Vector3D vPlayerPos             // （操作する）キャラクタの位置
		);

	// カメラのビュー行列を取得する
	MATRIX getViewMatrix(){ return mModelCamViewMat; }; // ビュー行列はカメラのローカル座標行列の逆行列である。

	// 計算されたEntityの進行方向を取得する
	// ※ UpdateGrids() の中で計算されることに注意
	Vector3D getEntityMoveDir(){ return (m_vArrowEndEntPln - m_vArrowBgnEntPln).normalize(); };

	// ##### Update 系 関数 #####

	// モデルステータスから座標変換行列を更新
	void UpdateTransMats();

	// モデルステータスから幾何的座標位置を更新
	void UpdateGeoPoss();

	// スクリーン上の円と、それをxz平面上に投影した図形の配列を更新する
	void UpdateScreenCircleProjection();

	// オリジナルのスティックの傾きの扇形 の初期化
	void UpdateSectorOrgStickTiltDir();
	
	// （スティックの傾きから）計算されたキャラクタの進行方向（を図示する扇形）の初期化
	void UpdateSectorCharactrMoveDir();

	// 演出用グリッドの初期化
	void UpdateGrids();


	// ##### Draw 系 関数 #####

	// カメラモデルを描画
	void RenderModelCamera();

	// スクリーンを描画
	void RenderScreen();

	// 各種補助線を描画
	void RenderAuxiliaryLines();

	// ① スクリーン上のスティックの軌跡を描画
	void RenderStickTrackOnScreen();

	// ② ①をxz平面上に投影した図形（楕円）
	void RenderStickTrackProjection();

	// ③ ②を投影平面上で正規化した正円の描画
	void RenderStickTrackProjectionNormalize();

	// #### 変化量の扇型を表示

	// モデル内に描画する３次元図形、ディスプレイ用扇形モデル、の描画
	void RenderSectors();

	// オリジナルのスティックの傾きの変化量を表す扇型を描画
	void RenderOrgStickTiltDirVariation( Vector2D RenderPos );

	// （スティックの傾き）計算されたキャラクタの進行方向の変化量を表す扇型を描画
	void RenderCharactrMoveDirVariation( Vector2D RenderPos );

	// #### 変形のグリッドを表示

	// スクリーン上のグリッドを描画
	void RenderGridOnScreen();

	// Entity平面に投影したグリッドを描画
	void RenderGridGrandPrj();

	// スクリーン上に、スティックの傾き方向に矢印を描画
	//void RenderArrowToStickTiltOnScreen();
	// → RenderGridOnScreen の中で描画する

	// キャラクタ平面上に、スティックの傾き方向に矢印を描画
	//void RenderArrowToStickTiltOnEntityPlane();
	// → RenderGridGrandPrj の中で描画する

private:
	// スティックの軌跡の分割数
	static int m_iStickTrackDivNum;

	// ① スクリーン上のスティックの軌跡
	LineFreeCycle m_StickTrackOnScreen;

	// ② ①をxz平面上に投影した図形（楕円）
	LineFreeCycle m_StickTrackProjection;

	// ③ ②を投影平面上で正規化した正円
	LineFreeCycle m_StickTrackProjectionNormalize;


	// 扇形の分割数
	static int m_iSectorDivNum;


	// ### 扇型

	// ## モデル内に描画する３次元図形、ディスプレイ用扇形モデル

	// 垂直方向の扇形（スクリーン上）
	PlaneConvexFill* m_pVertiSectorOnScreen;

	// 垂直方向の扇形（xz平面に投影）
	PlaneConvexFill* m_pVertiSectorProjection;

	// 水平方向の扇形（スクリーン上）
	PlaneConvexFill* m_pHorizSectorOnScreen;

	// 水平方向の扇形（xz平面に投影）
	PlaneConvexFill* m_pHorizSectorProjection;

	// ## オリジナルのスティックの傾き

	// 垂直方向の扇形
	SectorFigure2D* m_pVertiSectorOrgStickTiltDir;

	// 水平方向の扇形
	SectorFigure2D* m_pHorizSectorOrgStickTiltDir;

	// ## （スティックの傾き）計算されたキャラクタの進行方向

	// 垂直方向の扇形
	SectorFigure2D* m_pVertiSectorCharactrMoveDir;

	// 水平方向の扇形
	SectorFigure2D* m_pHorizSectorCharactrMoveDir;


	// ### グリッド

	// オリジナルのグリッド
	GroundGrid* m_pGridOriginal;

	// スクリーン上のグリッド
	GroundGrid* m_pGridOnScreen;

	// Entity平面に投影したグリッド
	GroundGrid* m_pGridGrandPrj;

	// スクリーン上に描画する、スティックの傾き方向の矢印
	Arrow3D* m_pArrowToStickTiltOnScreen;

	// キャラクタ平面上に描画する、スティックの傾き方向の矢印
	Arrow3D* m_pArrowToStickTiltOnEntityPlane;

	// 矢印の情報（Arrow3Dに一体化させておけばよかった）
	Vector3D m_vArrowBgnScreen, m_vArrowEndScreen;
	Vector3D m_vArrowBgnEntPln, m_vArrowEndEntPln;

	// #### 各種座標変換行列

	// 説明用カメラのローカル→ワールド座標変換行列
	MATRIX mModelCamLocalToWorld;

	// 説明用カメラのローカル→ワールド座標変換行列の逆行列
	MATRIX mModelCamWorldToLocal;

	// スクリーンのローカル→ワールド座標変換行列
	MATRIX mScreenLocalToWorld;

	// スクリーンのローカル→ワールド座標変換行列の逆行列
	MATRIX mScreenWorldToLocal;

	// 説明用カメラの基本変換行列
	MATRIX MatConf;

	// スクリーンローカル座標→キャラクタ平面へスクリーン上方向を保ちながら剛体変換する行列（calcStickTiltPos_RigidTransで使用）
	MATRIX mScreenPosRigidTrans;

	// 説明用カメラのビュー行列（説明用カメラモデルと実際のカメラの方向が違うため、mModelCamWorldToLocalはそのまま使えない）
	MATRIX mModelCamViewMat;


	// #### 各種幾何的位置

	// スクリーン上のキャラクタ位置（スクリーンローカル座標における）
	Vector3D m_vEntiPosAsScreenForScLocal;

	// スクリーン上のキャラクタ位置（ワールド座標における）
	Vector3D m_vEntiPosAsScreenForWorld;

	// スクリーン上の、スクリーンの中心位置（ワールド座標における）
	Vector3D m_vScreenCntPosOnScreen;

	// スクリーンの中心位置をキャラクタ平面に投影した位置
	Vector3D m_vScreenCntPosOnEntityPlane;

	// 関数の器
	//std::function<void(Vector3D,Vector3D&,Vector3D&)> calcStickTiltPosProjection;

	// 関数ポインタ
	void (StickTiltProjectorDemo::*fpCalcStickTiltPosProjection) (
		Vector3D vStickTiltPosForScLocal, // [IN] スクリーンローカル座標におけるスティックの傾き方向位置
		Vector3D &vStickTiltPosForWorld,  // [OUT] ワールド座標におけるスティックの傾き方向位置
		Vector3D &vStickTiltPosProjection // [OUT] xz平面上に投影したスティックの傾き方向位置（ワールド座標における）
		);

	// スティックの傾きの方向（x-z平面上）→ ワールx-z平面上へ投影した座標へ変換する関数を用意する

	// * 射影変換 HomogTrans （ホモグラフィ変換）
	//   旧版。画面スクリーンに投影したスティックの傾き方向をEntityの平面に投影する
	void calcStickTiltPos_HomogTrans(
		Vector3D vStickTiltPosForScLocal, // [IN] スクリーンローカル座標におけるスティックの傾き方向位置
		Vector3D &vStickTiltPosForWorld,  // [OUT] ワールド座標におけるスティックの傾き方向位置
		Vector3D &vStickTiltPosProjection // [OUT] xz平面上に投影したスティックの傾き方向位置（ワールド座標における）
		);

	// * 剛体変換 RigidTrans
	//   改良版。上方向の向きだけを見た目と合せ、角度を保存して変換する。
	void calcStickTiltPos_RigidTrans(
		Vector3D vStickTiltPosForScLocal, // [IN] スクリーンローカル座標におけるスティックの傾き方向位置
		Vector3D &vStickTiltPosForWorld,  // [OUT] ワールド座標におけるスティックの傾き方向位置
		Vector3D &vStickTiltPosProjection // [OUT] xz平面上に投影したスティックの傾き方向位置（ワールド座標における）
		);


};

