#include <string>

// ################## カメラ制御クラス #######################

class CameraWorkManager
{
private:
	CameraWorkManager();

	// コピーコンストラクタ、代入演算子を private に
	CameraWorkManager(const CameraWorkManager&);
	CameraWorkManager& operator=(const CameraWorkManager&);

public:
	// ### カメラモード ###
	// ① RotateCamOnGazePoint : 注視点を中心にカメラが回転
	// ② TrackingMovingTarget : ターゲットを追尾する
	// カメラモード識別enum
	enum CameraModeID
	{
		RotateCamOnGazePoint=0,
		TrackingMovingTarget=1
	};

	static const int m_iCameraModeMax = 2;

private:
	// ### メンバ ###
	CameraModeID m_CurCamMode;

	bool m_bGazingAtTargetPos; // カメラを m_TargetPos の方向をみるかのフラグ

	double m_dTilt; // 視点の傾き 
	double m_dHead; // 視点の方向角
	
	Vector3D m_vFinalCamDist;   // （最終的に設定される）カメラの向く方向ベクトル
	Vector3D m_vFinalTargetPos; // （最終的に設定される）カメラの注視点
	Vector3D m_vFinalCamPos;    // （最終的に設定される）カメラの位置

	Vector3D m_TargetPos;       // 設定可能なカメラの注視点
	double   m_TrgtHight;       // 被写体の高さ（頭をみるように）

	// ① RotateCamOnGazePoint で使用
	double m_dCamDistFrmFP;  // カメラのキャラクターからの距離

	// ② TrackingMovingTarget で使用
	double m_dCamHight;          // カメラの高さ
	double m_dDstCamToTrgtDef;   // カメラと被写体の距離（真上から見た）
	double m_dSqDstCamToTrgtDef; // DstCamToTrgtDef * DstCamToTrgtDef

	// ### メソッド ###
	// Update関数（カメラモード毎に用意）
	void Update_RotateCamOnGazePoint( double timeslice );
	void Update_TrackingMovingTarget( double timeslice );

	// ### 補助メソッド ###
	// ① RotateCamOnGazePoint 用
	void getKeyInput();
	void getMouseWheelInput( double timeslice );


public:
	// シングルトン
	static CameraWorkManager* Instance();

	// カメラ状態を更新
	void Update( double timeslice ); // CamMode追加要

	// DXライブラリのカメラセット関数を実行
	void setCamera();

	// カメラモードを設定
	void setCameraMode( CameraModeID camID ){ m_CurCamMode=camID; };

	// ターゲットを設定
	void setTarget( Vector3D tergetPos ){ m_TargetPos=tergetPos; };

	// 現在のカメラモード名を文字列で返却
	std::string getCurCamModeName(); // CamMode追加要

	// ビュー行列
	MATRIX m_MViewLocal; // Entityのローカル座標でのビュー行列
	MATRIX m_MViewWorld; // ワールド座標でのビュー行列

	// ##### アクセサ類 #####
	Vector3D getCamPos(){ return m_vFinalCamPos; }; // カメラ位置の取得
	double   getCamDistFrmFP(){ return m_dCamDistFrmFP; };

	// カメラ位置を球面座標系で設定 
	// ※ distがうまく動かないので注意！
	//    getMouseWheelInput の中のstatic変数で決まってしまっている！
	//    （m_dCamDistFrmFPはwork変数になっている構造を何とかしないと直らない。）
	void     setCamPosSpherical ( double tilt, double head, double dist )
	{ 
		m_dTilt=tilt;
		m_dHead=head;
		m_dCamDistFrmFP=dist; 
	}; 

	// カメラ位置を直交座標系で設定
	// ※ Updateと併用できないので注意 
	void     setCamFinalPos ( Vector3D Pos ){ m_vFinalCamPos = Pos; }; 


};
