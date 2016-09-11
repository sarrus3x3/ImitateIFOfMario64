#include <vector>
#include <list>     

#include "DxLib.h"

// 基本ライブラリ
#include "Vector3D.h"
#include "Vector2D.h"

//=======================================================================//
// 2016/08/28
// 個々のEntityの実装はできたので、
// Entity管理の GameWorld Class に着手する

//=======================================================================//


// ######################## FlipbookPlayer Class ########################
// パラパラ漫画の再生
// 複数Entityからの仕様を想定しているため、再生に関する状態（今の再生フレームなど）は保持しない。

class FlipbookPlayer
{
private:
	int m_iMaxFrame;           // アニメーションの最大フレーム数
	vector<int> m_hGraphArray; // 各フレームの画像データ（ハンドル）を格納する配列

public:
	// #### コンストラクタ・デストラクタ ####
	FlipbookPlayer( string GraphFileName, int FrameNum , int XSize, int YSize );
	
	// #### メソッド ####
	void Play( Vector3D Pos, float Width, int Frame );   // アニメーションの再生実行

};

// ######################## SampleEffectAnimManager Class ########################
// SampleEffectEntityと one - to one でアニメーションを管理を行う

class SampleEffectAnimManager
{
private:
	// #### アニメーション再生に関して
	int  m_iCurPlayFrame; // 現在の再生フレーム数
	bool m_bEnd; // アニメーションの再生が終了した

	// #### 個々のアニメーションの特性に関するもの（アニメーションが複数になるならば、パッケージ化する必要がある。）
	// アニメーション毎にクラスを作るというパッケージ方法も面白いかもしれない。
	// 継承をつかう。（現在の再生フレームなど共通を基底クラスで定義し、固有のアニメーション情報を継承先で定義する）

	// FlipbookPlayerインスタンスへのポインタ
	static FlipbookPlayer* m_pFlipbookPlayerIns; 

	static int   m_iMaxFrameNum;     // アニメーションの最大フレーム数
	static string m_sGraphFileName;  // アニメーションの画像データのパス
	static int   m_iXSize; // アニメーションの１コマのXサイズ
	static int   m_iYSize; // アニメーションの１コマのYサイズ

	bool  m_bRepeatAnimation; // アニメーションの繰返し要否
	float m_fAnimWidth;       // エフェクトの描画サイズ

public:
	// #### コンストラクタ・デストラクタ ####
	SampleEffectAnimManager(); // 特に引数で渡すようなものはない...
	~SampleEffectAnimManager();

	// #### メソッド ####
	static void Initialize(); // 使用する前に一度だけ呼ぶこと。クラスのstaticオブジェクト（FlipbookPlayer）の初期化を行う。
	// * GameWorld の initialize() で一括して呼ぶようにする


	void Render( Vector3D Pos ); // アニメーションの描画

	bool isEnd();  // アニメーションが再生終了したか？

};

// ######################## SampleItemAnimManager Class ########################
// SampleItemEntityと one - to one でアニメーションを管理を行う
// さくっと書いてしまうぞ！

class SampleItemAnimManager
{
private:
	// ##### MMDモデルのハンドルの原本：
    // AnimationManager のインスタンスを複数生成に対応するため、
	// MV1LoadModel で生成するハンドルはstaticでclassで一つだけ持ち、
	// m_iModelHandle は、m_iModelHandleMaster を MV1DuplicateModel で複製して使う
	static int m_iModelHandleMaster;  // MMDモデルのハンドルの原本
	static int m_iAllModelHandleNum;  // 全モデルハンドル数

	// #### （現在の）アニメーションに関する情報、属性、設定 等 ####
	// モデル情報
	int   m_iModelHandle; // MMDモデルのハンドル ※ AnimationManager のインスタンスを複数生成に対応するため、static化（暫定対処）

	float m_fModelScale;  // モデルの倍率

	// 高度な機能は持たせない。

public:
	// #### コンストラクタ・デストラクタ ####
	SampleItemAnimManager(); // 特に引数で渡すようなものはない...
	~SampleItemAnimManager();

	// #### メソッド ####
	static void Initialize(); // 使用する前に一度だけ呼ぶこと。クラスのstaticオブジェクト（FlipbookPlayer）の初期化を行う。
	// * GameWorld の initialize() で一括して呼ぶようにする

	void Render( Vector3D Pos, Vector3D Head ); // アニメーションの描画、向きも設定できるように

};

// ######################## SampleGameWorld Class のプロトタイプ宣言 ########################
class SampleGameWorld;

// ######################## SampleBaseGameEntity Class ########################
// SampleEffectEntity と SampleItemEntity （もとい全てのEntityタイプの）基底クラス（の気分）
// うーん、スカスカｗ。まぁ、単なるテンプレートだからこれでいいのか。

class SampleBaseGameEntity
{
private:
	// うーん、特にはないかな？

protected:
	// Entityが消滅する時にあげるフラグ。EntityManagerが、リストからEntityをUpdateするときに参照してremoveする
	bool m_bExtinct;

	// Entityの位置情報
	Vector3D m_vPos; // 位置情報だけを基底クラスに持たせる意味は？

	// コンストラクタ
	SampleBaseGameEntity( Vector3D Pos ) : m_vPos( Pos ),
		                                   m_bExtinct( false )
	{}

public:

	// デストラクタ
	virtual ~SampleBaseGameEntity(){}

	// 状態の更新
	virtual void Update(double time_elapsed){}; 

	// 描画
	virtual void Render(){};


	// アクセサ類
	Vector3D Pos()const{return m_vPos;}
	bool     isExtinct()const{return m_bExtinct;}


};

// ######################## SampleItemEntity Class ########################
// フィールドにおいてあるアイテム的なもの（コイン、宝石など）のEntity
// - くるくる回転している演出
// - 取れる。
// - SampleBaseGameEntity から継承

class SampleItemEntity : public SampleBaseGameEntity
{
private:
	// GameWorldへのポインタ
	SampleGameWorld *m_pWorld;

	// アニメーションマネージャのインスタンスへのポインタ
	SampleItemAnimManager* m_pAnimMgr; // コンストラクタ内でインスタンス化

	// ローカル座標
	Vector3D m_vHeading;        // Entityの正面向き
	// Vector3D m_vSide;            // Entityの横向き
	// Vector3D m_vUpper;			// Entityの上方向

	// 正面向きベクトルのデフォルト
	static const Vector2D m_vDefaultHeading2D;

	// アイテムの回転速度
	static double m_dRotSpeed;

	// アイテムの回転角（ラジアン）
	double m_dRotAngle;

	// m_vSide    x m_vHeading = m_vUpper
	// m_vHeading x m_vUpper   = m_vSize
	// m_Upper    x m_vSide    = m_vHeading

	// アイテムが取得された時に TouchDetection で上げる → Update でチェックして次の処理を行う
	bool m_bGotten;

	static double m_dBoundingRadius; // キャラクターの接触領域を半径m_dBoundingRadius、高さm_dCharacterHightの円柱としてアイテムとの接触検出する。
	static double m_dCharacterHight; 
	static double m_dSqBoundingRadius; // m_dBoundingRadius の２乗

public:
	// コンストラクタ
	SampleItemEntity( SampleGameWorld *world, Vector3D Pos ); 

	// デストラクタ
	~SampleItemEntity(); // AnimMgrを削除すること★

	// 状態の更新
	void Update(double time_elapsed); // アイテムが、くるくる回転する

	// 描画
	void Render();

	// プレーヤーキャラクタとの接触検出→ Gotten フラグを上げる
	bool TouchDetection( Vector3D CharaPos );

	static bool   m_bExpBoundingRadius; // m_dBoundingRadius を表示する

	// アクセサ類
	Vector3D Heading()const{return m_vHeading; }
	// Vector3D Side()   const{return m_vSide;    }
	// Vector3D Uppder() const{return m_vUpper;   }

};

// ######################## SampleEffectEntity Class ########################
// アイテムを取った後の余韻のエフェクト
// その場でEffectアニメーションを作成して、消滅する。

class SampleEffectEntity : public SampleBaseGameEntity
{
private:
	// GameWorldへのポインタ
	SampleGameWorld *m_pWorld;

	// アニメーションマネージャのインスタンスへのポインタ
	SampleEffectAnimManager* m_pAnimMgr; // コンストラクタ内でインスタンス化

public:
	// コンストラクタ
	SampleEffectEntity( SampleGameWorld *world, Vector3D Pos );

	// デストラクタ
	~SampleEffectEntity();

	// 状態の更新
	void Update(double time_elapsed); // Effectアニメーションの再生が完了したら自己終了する。
	// 描画
	void Render();

	// アクセサ類

};

// 次はSampleGameWorld Classの実装
// 先ずは、アイテムとエフェクトの動きをみるために暫定的な実装にする。


// ######################## SampleGameWorld Class の定義 ########################

// ゲーム上の全Entityの管理を行うクラス

class SampleGameWorld
{
private:
	
	// World に存在するすべてのEntityのリスト。
	list<SampleBaseGameEntity*> m_pAllEntityList;

	// アイテムEntityのみのリスト
	list<SampleItemEntity*> m_pIttemEntityList;

public:
	// コンストラクタ
	SampleGameWorld();

	// 初期化
	static void Initialize(); // DXlibを初期化した後でないとできない初期化処理を実施する。画像データのメモリへのロードとか、モデルのロードとか。

	// 更新
	void Update( double time_elapsed, Vector3D CharaPos );

	// 描画
	void Render();

	// SampleItemEntity を登録
	void RegisterSampleItemEntity( SampleItemEntity* );

	// SampleEffectEntity を登録
	void RegisterSampleEffectEntity( SampleEffectEntity* );

	// アイテム配置定義ビットマップを読み込みフィールドにアイテム配置を行う
	void SetItemsToWorld( double Separation , double ItemHight, char *BitMapFileName );

};







