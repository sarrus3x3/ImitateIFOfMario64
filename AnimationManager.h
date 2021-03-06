#include <queue>

#include <cassert>

#include "DxLib.h"
#include "PlayerCharacterEntity.h"

#include "AnimationPhysics.h"

//class PlayerCharacterEntity;


// #### アニメーション固有情報
// アニメーション自体の属性。
// ex. 連続再生するか？１回しか再生せず、停止するか？、センター位置固定か？とかとか
// ★どこで持つかを書く
struct AnimUniqueInfo
{
	string  m_sAnimName; // アニメーションの名前。デバック等で出力時に使用。

	int   m_CurAttachedMotion; // 現在アタッチしているMotion番号（ 初音ミクxxx.vmd の xxx の番号 ）

	// アニメーション位置調整
	Vector3D m_vPosShift;           // Motionの位置の調整に使用（Motionの位置とEntityの位置を合わせるための補正に使用）

	// センター固定位置 - センター位置を指定。m_vPosShift も有効なことに注意
	bool     m_bCorrectionToCenter;     // ON だとセンター位置を固定する
	bool     m_bCorrectionToCenterButY; // ON だと"Y軸成分を除き"センター位置を固定する。m_bCorrectionToCenter も ON にすること。※ 使用非推奨！
	Vector3D m_vFixCenterPosLocal;  // m_bCorrectionToCenter:ON の場合に固定するセンター位置（モデル位置に対するローカル座標）向きとかは考慮してねーから。

	// アニメーションの繰返し再生に関する
	bool     m_bRepeatAnimation;    // アニメーションを繰り返すか？（ デフォルト ON ）
	
	// アニメーションの再生開始位置を指定する場合は、下のm_fAnimStartTimeを指定

	// オリジナルのモーションからアニメーションを切り出す
	bool     m_bCutPartAnimation;   // オリジナルのモーションからアニメーションを切り出すか（ デフォルト OFF ）。ON の場合は、下の m_fAnimStartTime と m_fAnimEndTime を設定。ON の場合はリピートは許容しない（m_bRepeatAnimation を OFF にすること）。
	float    m_fAnimStartTime;      // （元のモーションに対して）アニメーションの開始時間。Repeat=ON の場合はこの位置から再生開始
	float    m_fAnimEndTime;        // （元のモーションに対して）アニメーションの終了時間。Repeat=ON の場合は設定は無視

	// アニメーションの同期に関する
	float    m_fUniquePlayPitch;    // アニメーション固有の再生ピッチ（最終的な再生ピッチ＝ m_PlayPitch(※) * UniquePlayPitch で計算される。※AnimationManagerクラスのメンバ）	
	float    m_fAnimInterval;       // アニメーション切替時に同期を取るときに使われる。m_MotionTotalTimeとは違うものであることに注意。


	// 標準以外のアニメーションの開始時間（AnimationManager.setAnimExStartTime()メソッドで使用）
	float    m_fExAnimStartTime;

	// 初期化メソッド
	void init()
	{
		m_sAnimName           = "UNDEFINE";
		m_CurAttachedMotion   = -1;
		m_bCorrectionToCenter = false;
		m_bCorrectionToCenterButY = false;
		m_vPosShift           = Vector3D( 0.0, 0.0, 0.0 );
		m_vFixCenterPosLocal  = Vector3D( 0.0, 8.0, 0.0 );
		m_bRepeatAnimation    = true;
		m_bCutPartAnimation   = false;
		m_fAnimStartTime      = 0.0;
		m_fAnimEndTime        = 0.0;
		m_fUniquePlayPitch    = 1.0;
		m_fAnimInterval       = 0.0;
		m_fExAnimStartTime    = 0.0;
	};

};

// #### AnimationManager::setAnim の引数を構造体にしたもの
// アニメーションの予約管理に使用する
struct ArgumentOfSetAnim
{
	PlayerCharacterEntity::AnimationID m_eAnimID; // セットするアニメーションID
	double m_dAnimSwitchTime; // ブレンド時間
	bool   m_bStopPrvAnim;    // 現在のアニメーションの停止要否
	float  m_fStartFrame;     // 任意のモーションの再生開始点

	// コンストラクタ
	ArgumentOfSetAnim( PlayerCharacterEntity::AnimationID AnimID, double AnimSwitchTime, bool StopPrvAnim, float StartFrame )
	{
		m_eAnimID         = AnimID;
		m_dAnimSwitchTime = AnimSwitchTime;
		m_bStopPrvAnim    = StopPrvAnim;
		m_fStartFrame     = StartFrame;
	};

};

// #### アニメーション再生管理構造体
// アニメーション再生状態に従い変化するもの。
// ex. 現在の再生フレーム、再生時間、再生回数等。
struct AnimPlayBackInfo
{
	// 親になるAnimationManagerのポインタ
	// - m_iModelHandle をメソッド内で使用するため。コンストラクタで引数として渡す。
	AnimationManager* m_pAnimationManager;

	// 再生アニメーションのアニメーションID
	PlayerCharacterEntity::AnimationID m_eAnimID;

	// モーションのアタッチインデックス（ MV1AttachAnim のリターン値 ）
	int   m_AttachIndex;

	// アニメーション再生情報
	float m_MotionTotalTime;   // モーションの総再生時間 - アニメーション切出しONの場合は、AnimUniqueInfo.m_fAnimEndTimeの値が設定される。切出しアニメーションの再生時間はm_fAnimLengthに格納する
	float m_CurPlayTime;       // 現在の再生時間
	float m_fBlendRate;        // アニメーションのブレンド率（ 0.0 〜 1.0 の間 ）
	int   m_iPlayCount;        // （setされてからの）アニメーションの再生回数
	bool  m_bPause;            // アニメーション停止（静止）フラグ
	bool  m_bFinished;         // アニメーション終了フラグ アニメーションが総再生時間まで再生された場合など。ループするアニメーションの場合は ON にならない。
	float m_fAnimLength;       // このアニメーションの再生にかかる時間


	// ブレンド制御に使用
	float m_fBlendRemain;      // ブレンドの残り時間。0になった時に完全にこのアニメーションに遷移完了する。ブレンドなしの場合=0
	float m_fAnimSwitchTime;   // アニメーション切替の設定時間

	inline AnimUniqueInfo* getAnimUnqPointer()
	{
		return &(PlayerCharacterEntity::AnimUniqueInfoManager::Instance()->m_pAnimUniqueInfoContainer[m_eAnimID]);
	};

	// コンストラクタ
	AnimPlayBackInfo(AnimationManager* pAnimationManager, PlayerCharacterEntity::AnimationID AnimID, float AnimSwitchTime);

	// デストラクタ
	//~AnimPlayBackInfo();

	// アニメーションのアタッチ処理
	void AttachAni();

	// アニメーションのデタッチ処理
	void DetachAni();

};


// ########################################################################
// ######################## AnimationManager Class ########################
// ########################################################################

class AnimationManager
{
private:
	// ##### MMDモデルのハンドルの原本：
    // AnimationManager のインスタンスを複数生成に対応するため、
	// MV1LoadModel で生成するハンドルはstaticでclassで一つだけ持ち、
	// m_iModelHandle は、m_iModelHandleMaster を MV1DuplicateModel で複製して使う
	static int m_iModelHandleMaster;  

	// #### （現在の）アニメーションに関する情報、属性、設定 等 ####
	// モデル情報
public:
	int   m_iModelHandle;              // MMDモデルのハンドル ※ AnimationManager のインスタンスを複数生成に対応するため、static化（暫定対処）

private:
	int   m_iCenterFrameIndex;         // 「センター」フレームのフレーム番号（ MV1SearchFrame のリターン値 ）

	// 再生ピッチ（再生速度）
	float m_PlayPitch;         // 再生ピッチ（再生速度）

	//  アニメーション再生管理構造体配列
	// - ブレンドされるアニメーションの配列。後ろの要素ほど新しいアニメーションで、最後尾が最新のアニメーションになる。
	// やっぱりやっぱり、先頭が最新になるように。。。
	vector<AnimPlayBackInfo> m_AnimPlayInfoArray;

	// アニメーション予約機能に関する
	queue<ArgumentOfSetAnim> m_qAnimReservationQueue;

	// 姿勢の傾き（旋回動作時に遠心力で体が傾く演出に使用）
	// 正負で傾きが右側か、左側かを示す。
	double m_dBankAngle;

	// #### 補助メソッド ####
	void PlayOneAnim( double TimeElaps, Vector3D Pos, Vector3D Head, AnimPlayBackInfo* pPlayAnim ); // Entity情報を参照させないで直接条件を指定してAnimationを描画する。
	void PlayReservedAnim(); // CurAnimが再生終了したかをチェックし、再生終了していれば予約されたアニメーションを再生設定する。
	void setAnimMain( 
		PlayerCharacterEntity::AnimationID, 
		double AnimSwitchTime=0.0, 
		bool StopPrvAnim =true, 
		bool SyncToPrv   =false,
		float StartFrame = -1.0f
		); 

public:
	// #### コンストラクタ・デストラクタ ####
	AnimationManager();
	
	// #### メソッド ####
	void Play( PlayerCharacterEntity* );   // アニメーションの再生実行
	
	void setPitch( float playpitch ){ m_PlayPitch = playpitch; }; // アニメーションの再生ピッチのセット

	void setBankAngle( double bankangle ){ m_dBankAngle = bankangle; }; // バンク角の設定


	// ----- アニメーションのセット（各Entity Classで定義されるアニメーション固有enumを指定）
	// ブレンド指定する場合は、引数に、ブレンド時間（デフォルト0）、現在のアニメーションの停止要否（デフォルト要）を設定
	// AnimSwitchTime 以降を指定しないとブレンドなしで切替する。
	// SyncToPrv ON にすると、位相を保ってアニメーション切替行う。ただし、Running<->Walkingにしか対応してない
	//【SyncToPrv（モーションの同期的シフト）を使用するに当たっての注意】
	//	* シフトする２つのモーションの AnimUniqueInfo で、正しく「m_fUniquePlayPitch」と「m_fAnimInterval」が設定されている必要がある。
	//	* シフト先のモーションにおける再生開始点は、２つのモーションの開始定義位置を考慮して、２つのモーションの再生時間の比からシフト元モーションの現在再生位置からシフト先モーションにマッピングされる。詳細は実装を見て。
	// StartFrame : デフォルト以外のモーションの開始位置（フレーム）を指定。負値の場合は、デフォルトの再生位置で再生する。
	void setAnim( PlayerCharacterEntity::AnimationID, double AnimSwitchTime=0.0, bool StopPrvAnim=true, bool SyncToPrv=false, float StartFrame=-1.0f ); 

	// ----- アニメーションの再生予約。
	// アニメーション設定情報は AnimReservationQueue にスタックされ、再生中のアニメーションが再生終了したら設定される。
	// ※ 割り込みで新しいアニメーションがsetAnimされた場合は、予約中の設定は破棄される。
	void ReserveAnim( PlayerCharacterEntity::AnimationID, double AnimSwitchTime=0.0, bool StopPrvAnim=true, float StartFrame = -1.0f );

	// ----- アニメーションの再生予約情報の破棄
	// 再生予約されたアニメーションを破棄する = キューを空にする
	// http://qiita.com/D-3/items/9930591bb78df544c066
	void DiscardReservedAnim(){ queue<ArgumentOfSetAnim>().swap(m_qAnimReservationQueue); };

	// ----- m_pCurAnimPlayInfoのAnimationIDを取得
	PlayerCharacterEntity::AnimationID getCurAnimID() { return m_AnimPlayInfoArray[0].m_eAnimID; };

	// ----- m_pCurAnimPlayInfoのアニメーション名を取得
	string getCurAnimName(){ return m_AnimPlayInfoArray[0].getAnimUnqPointer()->m_sAnimName; };

	// ----- m_pCurAnimPlayInfoの再生にかかる時間を取得
	float getCurAnimLength(){ return m_AnimPlayInfoArray[0].m_fAnimLength; };

	// ----- AnimIDで指定されたモーションを、モーションキューから新しい順に検索して構造体ポインタを返す
	// 指定されたモーションが見つからなかった場合はNULLを返す。
	AnimPlayBackInfo* getAnimPlayBackInfoFromAnimID(PlayerCharacterEntity::AnimationID AnimID );



	// #### 補助メソッド ####
	void  PlayMain( double TimeElaps, Vector3D Pos, Vector3D Head );
	float CurPlayTime(){ return m_AnimPlayInfoArray[0].m_CurPlayTime; }
	void  DrawAllow3D( Vector3D cnt, Vector3D heading ); // 矢印を描画
	float getMotionTotalTime(){ return m_AnimPlayInfoArray[0].m_MotionTotalTime; }
	
// ############ 物理演算（髪の毛を揺らすとか）関連 ############
public:
	// 物理演算の種別（列挙型）
	enum PhysicsTypeID
	{
		PHYSICS_NONE     = 0, // 物理演算なし
		PHYSICS_SELFMADE = 1, // 自作の物理演算
		PHYSICS_DXLIB    = 2  // DXライブラリ機能使用
	};

//private:
	// 現在の物理演算の種別
	PhysicsTypeID m_eCurPhysicsType;

	// ボーン表示 / モデル表示（ ExpBoneOfPhysicsPart で使用）
	bool m_bCurBoneExpress; // ON なら ボーン表示

	//### 自作の物理演算（PHYSICS_SELFMADE）で使うインスタンス類
	
	// 右髪用
	StraightMultiConnectedSpringModel *m_pRightHairPhysics;
	MultiJointBoneMotionControl       *m_pRightHairRender;
	int m_iRightHair1FrameIndex;  // "右髪１" フレーム （フレームの表示／非表示で使用）
	
	// 左髪用
	StraightMultiConnectedSpringModel *m_pLeftHairPhysics;
	MultiJointBoneMotionControl       *m_pLeftHairRender;
	int m_iLeftHair1FrameIndex;   // "左髪１" フレーム （フレームの表示／非表示で使用）

public:
	//##### メソッド #####
	//- 物理演算の種別を設定（ DXライブラリの物理演算、オリジナルの物理演算、物理演算なし ）
	void setAnimPhysicsType( PhysicsTypeID id );
	
	//- 物理演算部（髪の毛）の ボーン表示 / モデル表示 の切替（オリジナルの物理演算の場合）
	void ExpBoneOfPhysicsPart( bool BoneExpress ); // true-ボーン表示、false-モデル表示

	bool getCurBoneExpress(){ return m_bCurBoneExpress; };
	PhysicsTypeID getPhysicsType(){ return m_eCurPhysicsType; };

private:
	//###### 補助メソッド ######
	//初期化
	// - コンストラクタ内実行する
	void initAnimPhysics();

	//物理演算の実行、フレームに座標変換行列を設定
	// - AnimationManager::PlayMain内、MV1DrawModel の直前で呼ぶ
	void UpdateAnimPhysics( double TimeElaps );

// ############ デバック用機能 ############
private:

public:
	bool DBG_m_bPauseOn; // オンならAnimationを停止させる

	bool DBG_getPauseState(){ return DBG_m_bPauseOn; }
    Vector3D DBG_RenderCenterFramePos(); // 「センター」フレームの座標位置を描画する、ついでに座標位置を返却する。
	void DBG_setCurPlayTimeOfCurAnim( float time ){ m_AnimPlayInfoArray[0].m_CurPlayTime = time; };
	int  DBG_getModelHandle(){ return m_iModelHandle; };

	int  DGB_m_iHairFrameIndex;    // 髪フレーム
	//MATRIX DGB_m_mHairFrameMatrix;   // 髪フレームのMatirix

	// コンストラクタで初期化後に、モデルを変更したい場合に使用。デバック用の実装
	// AnimManagerが複数インスタンス合った場合の動作は保証しないよん。
	void DBG_RenewModel( int ReneModelHandle ); // 更新したいモデルのハンドルを渡すこと
	
	int DBG_m_iModelHandle_Original; // オリジナルのモデルのハンドルの退避用
	int DBG_m_iModelHandle_Physics;  // 物理演算ありで読み込んだモデルのハンドル
	int DBG_m_iModelHandle_HideHair; // 髪の毛削除を削除したモデルのハンドル

	float DBG_getCurAnimBRate(){ return m_AnimPlayInfoArray[0].m_fBlendRate; };

	int DBG_getAnimPlayInfoArray_Size() { return m_AnimPlayInfoArray[0].m_AttachIndex; };

	float DBG_getCurAnimPlayTime(){ return m_AnimPlayInfoArray[0].m_CurPlayTime; };

	bool DBG_m_bBlendPauseOn; // アニメーションブレンディングの一時停止

	//void DBG_setAnimPlayTime( float playtime ){ m_pCurAnimPlayInfo->m_CurPlayTime = playtime; }; // アニメーションを任意の再生時間に設定



};

 