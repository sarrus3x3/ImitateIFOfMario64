#include <vector>

#include "DxLib.h"

// 基本ライブラリ
#include "MyUtilities.h"
#include "Vector3D.h"
#include "Vector2D.h"

#define DBG_MEASURE_TIME

// アニメーションの物理演算（髪の毛を揺らすとか）関連クラス

// 多関節ボーンのモーション制御クラス
class MultiJointBoneMotionControl
{
public:
	int m_iModelHandle;          // モデルのハンドル

	int m_iJointSize;            // 総関節数
	std::vector<int> m_lFrameIndexList;  // 向きを制御したいボーン（フレーム）の番号リスト（ルート関節から、関節の接続順になっていること）

	MATRIX*   m_pDefaultTransMatList;    // リスト：対象ボーンのデフォルトの座標変換行列（※ただし平行移動成分を0にしたもの）
	Vector3D* m_pDefaultBoneDirList;     // リスト：対象ボーンのデフォルトの向き

	static const VECTOR OrignVec; // 原点ベクトル

public:
	// コンストラクタ
	MultiJointBoneMotionControl( 
		int    ModelHandle,
		std::vector<int> FrameIndexList,
		int    JointSize   // ルート関節はJointSizeに含めない。i.e. 物理演算で設定するJointSizeから-1したものを設定すること。
		);

	void setBoneDirPosMain( 
		Vector3D SpecifyBonePosForWorldCoord,  // ボーンを向けたい方向のベクトル v（ワールド座標）
		Vector3D BoneRootPosForWorldCoord,     // 自ボーンの付け根位置 p （ワールド座標）
		int      TargetFrameIndex,             // 自ボーンのフレーム番号
		int      ParentFrameIndex,             // 親ボーンのフレーム番号
		MATRIX   &DefaultTransMat,             // 対象ボーンの[デフォルト]の座標変換行列（※ただし平行移動成分を0にしたもの）
		Vector3D DefltBoneDirForPrntLocCoord   // 親フレームのローカル座標における[デフォルト]の自ボーンの方向 u 
		);

	// 引数のJointPosList（ワールド座標）に従い、ボーン（フレーム）を設定する
	// JointPosList のサイズは、m_iJointSize。 
	void setBoneAsJointList( Vector3D *pJointPosList ); 

	// 座標変換行列をデフォルトに戻す
	void Reset();

// ############ デバック用機能 ############
	void DBG_RenewModelHandles( int newModelHandle ){ m_iModelHandle=newModelHandle; };

};


// 直線状多重連結ばねモデルの物理演算
class StraightMultiConnectedSpringModel
{
private:
	static const Vector3D m_vVertiDir;  // 鉛直方向ベクトル（static const）

	// モデルハンドル
	int m_iModelHandle;

	// #### パラメータ
	static double m_dTimeElapsedPhys; // 物理演算のタイムステップ。ゲームのタイムステップと独立に設定可能

	double m_dMass;    // 質点の重量（固定）
	double m_dViscous; // 粘性抵抗（固定）
	double m_dGravity; // 重力定数
	double m_dSpring;  // バネ定数（ベース）
	double m_dNaturalFactor; // 自然長算出する上での補正係数

	int m_iJointSize; // 質点数
	std::vector<int>    m_iFrameIndexList;   // フレームindexリスト（まぁ、二重持ちになるけどいいか。大したサイズじゃないし。）
	std::vector<double> m_dSpringList;       // バネ定数のリスト
	std::vector<double> m_dNaturalList;      // バネの自然長のリスト

	// #### 内部用メソッド

	// i番目の質点に働く力を計算（を m で割ったもの）
	Vector3D ForceWorksToMassPoint( int i, Vector3D *pPosList, Vector3D *pVecList );

	void UpdateMain(double time_elapsed);         // 物理演算実施（Δtを引数）
	void UpdateByEuler(double time_elapsed); 	  // オイラー法による数値計算
	void UpdateByRungeKutta(double time_elapsed); // ルンゲクッタ法（４次）による数値計算

public:
	// ### 物理変数
	Vector3D *m_pPosList;  // 質点の位置リスト ※[0]はルート関節であり位置固定（フレーム位置から取得される）
	Vector3D *m_pVelList;  // 質点の速度リスト

	// ### アクセサ
	int    getJointSize(){ return m_iJointSize; };
	void   setTimeElapsedPhys( double time_elapsed ){ m_dTimeElapsedPhys = time_elapsed; };
	double getTimeElapsedPhys( ){ return m_dTimeElapsedPhys; };

public:

	// コンストラクタ
	// フレームリストindexを渡される
	StraightMultiConnectedSpringModel(
		int    ModelHandle,
		std::vector<int> FrameIndexList,
		int    JointSize,
		double Mass,
		double Viscous,
		double Gravity,
		double Spring,
		double NaturalFactor
		);

	// time_elapsedとm_dTimeElapsedPhysから計算して、必要な回数だけUpdateMainを実行する
	// 戻り値：UpdateMain 実行した回数
	int  Update(double time_elapsed); 

	void DebugRender(); // 描画（デバッグ用）

	void setJointPosAsFrame(); // ジョイント位置をデフォルト値に戻す

// ############ デバック用機能 ############
	void DBG_RenewModelHandles( int newModelHandle ){ m_iModelHandle=newModelHandle; };

#ifdef DBG_MEASURE_TIME
	// UpdateMain 1回あたりの平均処理時間（秒）
	double DBG_m_dAverageTimeForUpdate;
	MeasureFPS DBG_m_MeasureFPS;
#endif

};

