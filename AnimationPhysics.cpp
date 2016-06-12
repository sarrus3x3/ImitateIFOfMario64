#include "AnimationPhysics.h"

// ################# MultiJointBoneMotionControl #################

// ここでstatic constの初期化
const VECTOR MultiJointBoneMotionControl::OrignVec = { 0, 0, 0 };

// #### コンストラクタ ####
MultiJointBoneMotionControl::MultiJointBoneMotionControl( 
		int    ModelHandle,
		std::vector<int> FrameIndexList,
		int    JointSize
		) :
	m_iModelHandle(ModelHandle),
	m_lFrameIndexList( FrameIndexList ),
	m_iJointSize( JointSize )
{
	// コンテナを用意
	m_pDefaultTransMatList = new MATRIX[m_iJointSize];
	m_pDefaultBoneDirList  = new Vector3D[m_iJointSize];

	for( int i=0; i<m_iJointSize; i++)
	{
		// 対象ボーンのデフォルトの座標変換行列 を保存
		m_pDefaultTransMatList[i] = MV1GetFrameLocalMatrix( m_iModelHandle, m_lFrameIndexList[i+1] );

		// 対象ボーンのデフォルトの向き を計算して保存
		MATRIX ChildLocalMat;
		ChildLocalMat = MV1GetFrameLocalMatrix( m_iModelHandle, m_lFrameIndexList[i+2] ); 
		m_pDefaultBoneDirList[i] = VTransform( OrignVec, ChildLocalMat );

		// 対象ボーンのデフォルトの座標変換行列 の平行移動成分を 0 にする。
		for( int j=0; j<3; j++ ){ m_pDefaultTransMatList[i].m[3][j] = 0.0; }

	}

};

void MultiJointBoneMotionControl::setBoneAsJointList( Vector3D *pJointPosList )
{
	// ルート関節のワールド座標（固定）を取得する。（親フレームindexいらないか？）
	//Vector3D RootJointPosForWorldCoord = MV1GetFramePosition( m_iModelHandle, m_lFrameIndexList[1] ); // ルート関節の番号＝1

	for( int i=0; i<m_iJointSize; i++ )
	{
		setBoneDirPosMain(
			pJointPosList[i+1],
			pJointPosList[i],
			m_lFrameIndexList[i+1],
			m_lFrameIndexList[i],
			m_pDefaultTransMatList[i],
			m_pDefaultBoneDirList[i]
		);
	}

};

void MultiJointBoneMotionControl::setBoneDirPosMain( 
		Vector3D SpecifyBonePosForWorldCoord,  // ボーンを向けたい方向のベクトル v（ワールド座標）
		Vector3D BoneRootPosForWorldCoord,     // 自ボーンの付け根位置 p （ワールド座標）
		int      TargetFrameIndex,             // 自ボーンのフレーム番号
		int      ParentFrameIndex,             // 親ボーンのフレーム番号
		MATRIX   &DefaultTransMat,             // 対象ボーンの[デフォルト]の座標変換行列（※ただし平行移動成分を0にしたもの）
		Vector3D DefltBoneDirForPrntLocCoord   // 親フレームのローカル座標における[デフォルト]の自ボーンの方向 u 
	)
{
	static MATRIX TmpMat; // 作業用行列

	// ① v と p より、フレーム制御ローカル座標におけるボーンを向けたい方向ベクトル v'(=SpecifyDirLoc) を求める
	
	// v および p をフレーム制御ローカル座標に変換
	TmpMat = MV1GetFrameLocalWorldMatrix ( m_iModelHandle, ParentFrameIndex ); // 親ボーンのローカル→ワールト変換行列 取得
	TmpMat = MInverse( TmpMat ) ; // 逆行列を計算
	Vector3D SpecifyBonePosForPrntLocCoord = VTransform( SpecifyBonePosForWorldCoord.toVECTOR(), TmpMat ); 
	Vector3D BoneRootPosForPrntLocCoord    = VTransform( BoneRootPosForWorldCoord.toVECTOR()   , TmpMat ); 

	// ボーンを向けたい方向ベクトル v' を計算
	VECTOR SpecifyDirLoc = 
		(SpecifyBonePosForPrntLocCoord-BoneRootPosForPrntLocCoord).normalize().toVECTOR();

	// ② M×u = v となる、回転行列 R を計算する。
	MATRIX RotMat = MGetRotVec2( DefltBoneDirForPrntLocCoord.toVECTOR(), SpecifyDirLoc );

	// ③ 自フレームの付け根が位置 p になるように、M を p だけ平行移動した M' を生成
	//MATRIX TransMatShiftP = MAdd( 
	//	MGetTranslate( BoneRootPosForPrntLocCoord.toVECTOR() ), DefaultTransMat );
	MATRIX TransMatShiftP = DefaultTransMat;
	TransMatShiftP.m[3][0] = (float)BoneRootPosForPrntLocCoord.x;
	TransMatShiftP.m[3][1] = (float)BoneRootPosForPrntLocCoord.y;
	TransMatShiftP.m[3][2] = (float)BoneRootPosForPrntLocCoord.z;


	// ④ フレームの新しい座標変換行列として、R×M' をセットする。
	TmpMat = MMult( RotMat, TransMatShiftP );
	MV1SetFrameUserLocalMatrix( m_iModelHandle, TargetFrameIndex, TmpMat );

};

void MultiJointBoneMotionControl::Reset()
{
	for( int i=0; i<m_iJointSize; i++)
	{
		// 座標変換行列を元に戻す
		MV1ResetFrameUserLocalMatrix( m_iModelHandle, m_lFrameIndexList[i+1] );
	}
};

// ################# StraightMultiConnectedSpringModel #################


// ここでstatic constの初期化
const Vector3D StraightMultiConnectedSpringModel::m_vVertiDir( 0.0, -1.0, 0.0 );
double StraightMultiConnectedSpringModel::m_dTimeElapsedPhys = 0.01; // 0.0001 くらいにすると安定

// コンストラクタ
//   各種変数を代入
StraightMultiConnectedSpringModel::StraightMultiConnectedSpringModel(
		int    ModelHandle,
		std::vector<int> FrameIndexList,
		int    JointSize,
		double Mass,
		double Viscous,
		double Gravity,
		double Spring,
		double NaturalFactor
		) :
	m_iModelHandle( ModelHandle ),
	m_iFrameIndexList( FrameIndexList ),
	m_iJointSize( JointSize ),
	m_dMass( Mass ),
	m_dViscous( Viscous ),
	m_dGravity( Gravity ),
	m_dSpring( Spring ),
	m_dNaturalFactor( NaturalFactor ),
	m_dSpringList( JointSize ),
	m_dNaturalList( JointSize )
	//m_vPosList( JointSize ),
	//m_vVelList( JointSize, Vector3D(0,0,0) )
{
	// m_pPosList, m_pVelList のメモリ確保
	m_pPosList = new Vector3D[m_iJointSize];
	m_pVelList = new Vector3D[m_iJointSize];

	// m_vPosList の初期化
	for( int i=0; i<m_iJointSize; i++ ){
		// フレーム位置で初期化
		m_pPosList[i] = MV1GetFramePosition( m_iModelHandle, m_iFrameIndexList[i+1] );
	}

	for( int i=0; i<(m_iJointSize-1); i++ ){
		// m_dNaturalList の初期化
		// m_dNaturalFactor で、重力で伸びた時にデフォルトの髪の形状に近づくようにチューンする。
		m_dNaturalList[i] = m_dNaturalFactor * (m_pPosList[i+1]-m_pPosList[i]).len();

		// m_dSpringList の初期化（バネの自然長に比例）
		m_dSpringList[i] = m_dNaturalList[i] * m_dSpring;
	}

};

// 物理演算実施
void StraightMultiConnectedSpringModel::UpdateMain(double time_elapsed)
{
	UpdateByEuler(time_elapsed);
	//UpdateByRungeKutta(time_elapsed);
};

// オイラー法による数値計算
void StraightMultiConnectedSpringModel::UpdateByEuler(double time_elapsed)
{
	// 新Pos,Velのメモリ確保
	static Vector3D *pNewPosList = new Vector3D[m_iJointSize];
	static Vector3D *pNewVelList = new Vector3D[m_iJointSize];

	// r0 の位置を、フレーム位置に更新
	m_pPosList[0]  = MV1GetFramePosition( m_iModelHandle, m_iFrameIndexList[1] );
	pNewPosList[0] = m_pPosList[0];

	// 運動方程式に従い、速度・位置を更新
	for( int i=1; i<m_iJointSize; i++ )
	{
		// 速度を更新
		Vector3D Accel = ForceWorksToMassPoint(i, m_pPosList, m_pVelList ) / m_dMass;

		pNewVelList[i] = time_elapsed * Accel + m_pVelList[i];
		//if( pNewVelList[i].sqlen() > 100.0*100.0 ) pNewVelList[i] = 100 * pNewVelList[i].normalize();

		// 位置を更新
		pNewPosList[i] = time_elapsed * m_pVelList[i] + m_pPosList[i];
		//pNewPosList[i] = time_elapsed * pNewVelList[i] + m_pPosList[i];

	}

	// 新速度・位置を有効化
	swap( m_pPosList, pNewPosList );
	swap( m_pVelList, pNewVelList );
}; 

// ルンゲクッタ法（４次）による数値計算
void StraightMultiConnectedSpringModel::UpdateByRungeKutta(double time_elapsed)
{
	// ルンゲクッタ法勾配 K1-K4 記憶用のメモリ確保
	static Vector3D *pCurK = new Vector3D[m_iJointSize];
	static Vector3D *pCurL = new Vector3D[m_iJointSize];

	// 次ステップの重み付き平均（の途中計算結果）
	static Vector3D *pSumK = new Vector3D[m_iJointSize];
	static Vector3D *pSumL = new Vector3D[m_iJointSize];

	// 新Pos,Velのメモリ確保
	static Vector3D *pNewPosList = new Vector3D[m_iJointSize];
	static Vector3D *pNewVelList = new Vector3D[m_iJointSize];

	// r0 の位置を、フレーム位置に更新
	m_pPosList[0]  = MV1GetFramePosition( m_iModelHandle, m_iFrameIndexList[1] );
	pNewPosList[0] = m_pPosList[0];

	// なぜ位置ずれが起こるのだろう？

	// #### K1の計算
	for( int i=1; i<m_iJointSize; i++ )
	{
		pCurK[i] = ForceWorksToMassPoint(i, m_pPosList, m_pVelList ) / m_dMass;
		pCurL[i] = m_pVelList[i];
		pSumK[i] = pCurK[i]; // 初期化のため加算ではなく上書き
		pSumL[i] = pCurL[i];
	}

	// #### K2の計算

	// Pos, Vec の 0.5 * Dt* K シフトさせた配列を作成
	for( int i=1; i<m_iJointSize; i++ )
	{
		pNewPosList[i] = m_pPosList[i] + 0.5 * time_elapsed * pCurL[i];
		pNewVelList[i] = m_pVelList[i] + 0.5 * time_elapsed * pCurK[i];
	}

	// 暫定Pos,VecからK2を計算
	for( int i=1; i<m_iJointSize; i++ )
	{
		pCurK[i] = ForceWorksToMassPoint(i, pNewPosList, pNewVelList ) / m_dMass;
		pCurL[i] = pNewVelList[i];
		pSumK[i] += 2*pCurK[i];
		pSumL[i] += 2*pCurL[i];
	}

	// #### K3の計算

	// Pos, Vec の 0.5 * Dt* K シフトさせた配列を作成
	for( int i=1; i<m_iJointSize; i++ )
	{
		pNewPosList[i] = m_pPosList[i] + 0.5 * time_elapsed * pCurL[i];
		pNewVelList[i] = m_pVelList[i] + 0.5 * time_elapsed * pCurK[i];
	}

	// 暫定Pos,VecからK2を計算
	for( int i=1; i<m_iJointSize; i++ )
	{
		pCurK[i] = ForceWorksToMassPoint(i, pNewPosList, pNewVelList ) / m_dMass;
		pCurL[i] = pNewVelList[i];
		pSumK[i] += 2*pCurK[i];
		pSumL[i] += 2*pCurL[i];
	}

	// #### K4の計算

	// Pos, Vec の Dt * K シフトさせた配列を作成
	for( int i=1; i<m_iJointSize; i++ )
	{
		pNewPosList[i] = m_pPosList[i] + time_elapsed * pCurL[i];
		pNewVelList[i] = m_pVelList[i] + time_elapsed * pCurK[i];
	}

	// 暫定Pos,VecからK2を計算
	for( int i=1; i<m_iJointSize; i++ )
	{
		pCurK[i] = ForceWorksToMassPoint(i, pNewPosList, pNewVelList ) / m_dMass;
		pCurL[i] = pNewVelList[i];
		pSumK[i] += pCurK[i];
		pSumL[i] += pCurL[i];
	}

	// #### 次のステップの位置・速度を求める
	for( int i=1; i<m_iJointSize; i++ )
	{
		m_pVelList[i] += time_elapsed * (1.0/6.0) * pSumK[i];
		m_pPosList[i] += time_elapsed * (1.0/6.0) * pSumL[i];
	}
};

// i番目の質点に働く力を計算（を m で割ったもの）
Vector3D StraightMultiConnectedSpringModel::ForceWorksToMassPoint
	( int i, Vector3D *pPosList, Vector3D *pVecList )
{
	Vector3D Force(0,0,0);

	// 質点i～質点i+1間のバネから受ける力
	if( i+1<m_iJointSize )
	{
		// バネの伸び
		double Growth = (pPosList[i+1]-pPosList[i]).len() - m_dNaturalList[i]; // バネの伸び
		Force += m_dSpringList[i] * Growth * (pPosList[i+1]-pPosList[i]).normalize();
	}

	// 質点i-1～質点i間のバネから受ける力
	double Growth = (pPosList[i]-pPosList[i-1]).len() - m_dNaturalList[i-1]; // バネの伸び
	Force += -m_dSpringList[i-1] * Growth * (pPosList[i]-pPosList[i-1]).normalize();
		
	// 粘性抵抗
	Force += -m_dViscous * pVecList[i];

	// 質量で割る
	//Force /= m_dMass;

	// 重力
	Force += m_dMass * m_dGravity * m_vVertiDir;

	return Force;
};

// time_elapsedとm_dTimeElapsedPhysから計算して、必要な回数だけUpdateMainを実行する
int StraightMultiConnectedSpringModel::Update(double time_elapsed)
{
	static double fr = 0; // タイムステップの端数

	int counter=0; // UpdateMain 実行回数のカウンタ
	
	fr += time_elapsed;

#ifdef DBG_MEASURE_TIME
	// 時間測定処理
	LONGLONG BeginTime = GetNowHiPerformanceCount();
#endif

	while(1)
	{
		if( fr<m_dTimeElapsedPhys ) break;
		fr -= m_dTimeElapsedPhys;
		UpdateMain(m_dTimeElapsedPhys);
		counter++;
	}

#ifdef DBG_MEASURE_TIME
	// 時間測定処理
	LONGLONG EndTime = GetNowHiPerformanceCount();
	double Average = (double)(EndTime-BeginTime);
	//Average *= 0.000001;
	Average /= counter;
	DBG_m_dAverageTimeForUpdate = DBG_m_MeasureFPS.Update(Average);
#endif

	return counter;
};

// ジョイント位置をデフォルト値に戻す
void StraightMultiConnectedSpringModel::setJointPosAsFrame()
{
	// m_vPosList の初期化
	for( int i=0; i<m_iJointSize; i++ ){
		// フレーム位置で初期化
		m_pPosList[i] = MV1GetFramePosition( m_iModelHandle, m_iFrameIndexList[i+1] );

		// 速度を0に。
		m_pVelList[i] = Vector3D( 0,0,0 );

	}
};

void StraightMultiConnectedSpringModel::DebugRender()
{
	for( int i=0; i+1<m_iJointSize; i++ )
	{
		/*
		// 円錐で表現する
		DrawCone3D( 
			m_vPosList[i+1].toVECTOR(), 
			m_vPosList[i].toVECTOR(), 
			0.5f,
			4,
			GetColor( 255,   0, 0 ),   // 赤色 
			GetColor( 255, 255, 255 ), 
			TRUE ) ;
			*/

		// ジョイントに球を描画し、
		DrawSphere3D( 
			m_pPosList[i].toVECTOR(), 
			1.0f, 
			8, 
			GetColor( 255,   0, 0 ),   // 赤色 
			GetColor( 255, 255, 255 ), 
			TRUE ) ; 

		// ボーン（ジョイント接続）を線を描画
		if( i+1<m_iJointSize )
		{
			DrawLine3D( 
				m_pPosList[i].toVECTOR(), 
				m_pPosList[i+1].toVECTOR(), 
				GetColor( 255, 0, 0 ) ); // 赤色
		}

	}


	

};
