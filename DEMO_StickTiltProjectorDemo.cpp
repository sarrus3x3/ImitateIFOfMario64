#include <cassert>

#include "DEMO_StickTiltProjectorDemo.h"

int GlobalLineColor = GetColor( 0, 0, 0 );


// 与えられた位置に、座標軸（ワールド座標）を表示する関数
void DrawCoordi( Vector3D TargetPosVec, double scale )
{
	static const Vector3D vUnitX( 1.0, 0.0, 0.0 );
	static const Vector3D vUnitY( 0.0, 1.0, 0.0 );
	static const Vector3D vUnitZ( 0.0, 0.0, 1.0 );

	// ターゲットposを描画する
	DrawLine3D( TargetPosVec.toVECTOR(), (TargetPosVec+scale*vUnitX).toVECTOR(), GetColor( 255, 0, 0 )); // x軸を赤線で描画
	DrawLine3D( TargetPosVec.toVECTOR(), (TargetPosVec+scale*vUnitY).toVECTOR(), GetColor( 0, 255, 0 )); // y軸を緑線で描画
	DrawLine3D( TargetPosVec.toVECTOR(), (TargetPosVec+scale*vUnitZ).toVECTOR(), GetColor( 0, 0, 255 )); // z軸を青線で描画

};

// 与えられた位置に、（ワールド座標からみ）たローカル座標の基底軸を表示する関数
void DrawCoordi(  MATRIX M, double scale )
{
	Vector3D vUnitX(M.m[0][0], M.m[0][1], M.m[0][2]);
	Vector3D vUnitY(M.m[1][0], M.m[1][1], M.m[1][2]);
	Vector3D vUnitZ(M.m[2][0], M.m[2][1], M.m[2][2]);
	Vector3D vTgtPs(M.m[3][0], M.m[3][1], M.m[3][2]);

	// ターゲットposを描画する
	DrawLine3D( vTgtPs.toVECTOR(), (vTgtPs+scale*vUnitX).toVECTOR(), GetColor( 255, 0, 0 )); // x軸を赤線で描画
	DrawLine3D( vTgtPs.toVECTOR(), (vTgtPs+scale*vUnitY).toVECTOR(), GetColor( 0, 255, 0 )); // y軸を緑線で描画
	DrawLine3D( vTgtPs.toVECTOR(), (vTgtPs+scale*vUnitZ).toVECTOR(), GetColor( 0, 0, 255 )); // z軸を青線で描画
};

// ########################## SectorFigure2D ##########################

// 描画するときの倍率
double SectorFigure2D::m_dRenderScale = 25.0;

// コンストラクタ
// ① 開始角と終了角（ラジアン）を与えられたら、原点中心に扇形を生成する
SectorFigure2D::SectorFigure2D( double Radius, double StartAng, double EndAng, int DivNum ) : 
	m_iRenderColor( 0 ),
	m_dRotation( 0 )
{
	m_iVertexNum = DivNum+2;
	m_pVertexes = new Vector2D[m_iVertexNum];

	assert( EndAng > StartAng );
	assert( DivNum>1 );

	m_pVertexes[0] = Vector2D( 0.0, 0.0 );

	// 分割の間隔
	double IntervalAng = (EndAng - StartAng)/((double)DivNum);

	for( int i=0; i<=DivNum; i++)
	{
		double ang  = StartAng + i*IntervalAng;
		Vector2D vAng( cos(ang), sin(ang) );
		vAng *= Radius;
		m_pVertexes[i+1] = vAng;
	}

};

// コンストラクタ
// ② 開始角と終了角をベクトルで与えることもできる
SectorFigure2D::SectorFigure2D( double Radius, Vector2D StartVec, Vector2D EndVec, int DivNum ) : 
	m_iRenderColor( 0 ),
	m_dRotation( 0 )
{
	double StartAng = atan2( StartVec.y, StartVec.x );
	double EndAng   = atan2( EndVec.y,   EndVec.x );

	m_iVertexNum = DivNum+2;
	m_pVertexes = new Vector2D[m_iVertexNum];

	//assert( EndAng > StartAng );
	// 逆三角関数の”切れ目”に当たってしまった場合の対策をする。
	// → 角の開きが、πより大きくなっている場合は２πシフトする。
	if( !(EndAng > StartAng) )
	{
		EndAng += 2*DX_PI_F;
	}

	assert( DivNum>1 );

	m_pVertexes[0] = Vector2D( 0.0, 0.0 );

	// 分割の間隔
	double IntervalAng = (EndAng - StartAng)/((double)DivNum);

	for( int i=0; i<=DivNum; i++)
	{
		double ang  = StartAng + i*IntervalAng;
		Vector2D vAng( cos(ang), sin(ang) );
		vAng *= Radius;
		m_pVertexes[i+1] = vAng;
	}
}


SectorFigure2D::~SectorFigure2D()
{
	delete m_pVertexes;
	delete m_pPolyVertex;
};


// 描画用ポリゴンの生成
void SectorFigure2D::GeneratePolygons()
{
	// Vertex を計算する
	m_iPolygonNum = m_iVertexNum - 2;

	// Vectexのメモリを確保 Vertex数：DivNum * 4(上面・底辺・側面（２倍）) * 3（１ポリゴンの頂点数）
	int iPolyVectexNum = m_iPolygonNum*3;
	m_pPolyVertex = new Vector2D[iPolyVectexNum];
	
	// 輪郭頂点情報から、ポリゴン情報を生成する。
	// 初めの要素 pVertex2D[0] を中心に放射状に三角形分割する

	int c=0; //カウンタ
	for( int i=2; i<m_iVertexNum; i++ )
	{
		// 三角形
		m_pPolyVertex[ 3*c+0 ] = m_pVertexes[  0];
		m_pPolyVertex[ 3*c+1 ] = m_pVertexes[i-1];
		m_pPolyVertex[ 3*c+2 ] = m_pVertexes[  i];
		c++;
	}

};

// 平面キャンバス上に描画する機能を実装する
void SectorFigure2D::RenderOnCanvas( Vector2D RenderPos )
{
	// 表示時の回転量から回転行列を計算する
	C2DMATRIX RotMat = C2DMGetRot( m_dRotation );

	// 2次元行列クラスを実装しないといけないな.. → ゲームAIの定義を参考にするか

	for( int i=0; i<m_iPolygonNum; i++)
	{
		// Windowに描画するときは画面下向きがy軸性方向になるため、扇形の形を保つためにy成分を反転させる
		Vector2D TmpVec1 = C2DVTransform((m_dRenderScale * m_pPolyVertex[ 3*i+0 ]), RotMat ).reversY();
		Vector2D TmpVec2 = C2DVTransform((m_dRenderScale * m_pPolyVertex[ 3*i+1 ]), RotMat ).reversY();
		Vector2D TmpVec3 = C2DVTransform((m_dRenderScale * m_pPolyVertex[ 3*i+2 ]), RotMat ).reversY();

		Point2D TmpPos1 = (RenderPos + TmpVec1).toPoint();
		Point2D TmpPos2 = (RenderPos + TmpVec2).toPoint();
		Point2D TmpPos3 = (RenderPos + TmpVec3).toPoint();

		DrawTriangle( 
			TmpPos1.x, TmpPos1.y,
			TmpPos2.x, TmpPos2.y,
			TmpPos3.x, TmpPos3.y,
			m_iRenderColor ,
			TRUE );
	}

}

// ########################## StickTiltProjectorDemo ##########################

// スティックの軌跡の分割数
int StickTiltProjectorDemo::m_iStickTrackDivNum = 256;

// 扇形の分割数
int StickTiltProjectorDemo::m_iSectorDivNum = 32;

double StickTiltProjectorDemo::SectorRadius = 3.0;
double StickTiltProjectorDemo::AngeSize = DX_PI_F/12;

Vector2D XPlusDir( 1.0, 0.0 );
Vector2D YPlusDir( 0.0, 1.0 );

// コンストラクタ
StickTiltProjectorDemo::StickTiltProjectorDemo(
		Vector3D vModelCamPos,          // 説明用カメラの位置
		Vector3D vModelCamGazePoint,    // 説明用カメラの焦点
		double   dModelCamNearClipDist, // 説明用カメラのクリップ距離
		Vector3D vPlayerPos             // （操作する）キャラクタの位置
		) : m_vModelCamPos( vModelCamPos ),
			m_vModelCamGazePoint( vModelCamGazePoint ),
			m_dModelCamNearClipDist( dModelCamNearClipDist ),
			m_vPlayerPos( vPlayerPos ),
			m_ScPlane( ScreenPlane( 5.0, 5.0, 3.0 ) ),
			m_StickTrackOnScreen( LineFreeCycle( m_iStickTrackDivNum, GlobalLineColor) ),
			m_StickTrackProjection( LineFreeCycle( m_iStickTrackDivNum, GlobalLineColor) ),
			m_StickTrackProjectionNormalize( LineFreeCycle( m_iStickTrackDivNum, GlobalLineColor) ),
			m_vVertiStickTiltDirBgn( YPlusDir.rot( -AngeSize ) ),
			m_vVertiStickTiltDirEnd( YPlusDir.rot(  AngeSize ) ),
			m_vHorizStickTiltDirBgn( XPlusDir.rot( -AngeSize ) ),
			m_vHorizStickTiltDirEnd( XPlusDir.rot(  AngeSize ) )
{
	// カメラモデルの読み込み
	m_iModelCamHandle = MV1LoadModel( "..\\mmd_model\\Camera\\カメラ.x" );

	// MatConfの設定 :: モデルサイズ・向き等調整
	MatConf = MGetScale( VGet( 10.0, 10.0, 10.0) );
	MatConf = MMult( MatConf, MGetRotX( 0.5*DX_PI_F ) ) ;// 向き調整
	MatConf = MMult( MatConf, MGetRotY( 0.5*DX_PI_F ) ) ;// 向き調整

	// #### 扇形のデータの初期化 ####

	// ## オリジナルのスティックの傾き の初期化
	UpdateSectorOrgStickTiltDir();

	// 各ディスプレイ用扇形モデルの初期化
	m_pVertiSectorOnScreen = new PlaneConvexFill( 
		m_pVertiSectorOrgStickTiltDir->m_pVertexes, 
		m_pVertiSectorOrgStickTiltDir->m_iVertexNum, 
		GetColorF( 0.0f, 1.0f, 0.0f, 0.0f ) );

	m_pVertiSectorProjection = new PlaneConvexFill( 
		m_pVertiSectorOrgStickTiltDir->m_pVertexes, 
		m_pVertiSectorOrgStickTiltDir->m_iVertexNum, 
		GetColorF( 0.0f, 1.0f, 0.0f, 0.0f ) );

	m_pHorizSectorOnScreen = new PlaneConvexFill( 
		m_pHorizSectorOrgStickTiltDir->m_pVertexes, 
		m_pHorizSectorOrgStickTiltDir->m_iVertexNum, 
		GetColorF( 0.0f, 0.0f, 1.0f, 0.0f ) );

	m_pHorizSectorProjection = new PlaneConvexFill( 
		m_pHorizSectorOrgStickTiltDir->m_pVertexes, 
		m_pHorizSectorOrgStickTiltDir->m_iVertexNum, 
		GetColorF( 0.0f, 0.0f, 1.0f, 0.0f ) );

	// モデルステータスから座標変換行列を更新
	UpdateTransMats();

	// モデルステータスから幾何的座標位置を更新
	UpdateGeoPoss();

	// ## （スティックの傾きから）計算されたキャラクタの進行方向（を図示する扇形）を計算する
	UpdateSectorCharactrMoveDir();

}

void StickTiltProjectorDemo::UpdateTransMats()
{
	// スクリーンローカル座標の（ワールド座標における）基底軸を計算する。

	// y'軸 = カメラの方向
	Vector3D Ysc = (m_vModelCamGazePoint-m_vModelCamPos).normalize();

	// x'軸 = カメラの上方向をy軸正方向
	double sgn = -1.0 * (double)( (Ysc.x>0) - (Ysc.x<0) );
	Vector3D Xsc( sgn*Ysc.y, fabs(Ysc.x), 0 );

	// z'軸 = x' × y' → この規則が間違っている？？ DEMO_DashTurnBehavior.h をみる
	//Vector3D Zsc = VCross( Xsc.toVECTOR(), Ysc.toVECTOR() );
	Vector3D Zsc = VCross( Ysc.toVECTOR(), Xsc.toVECTOR() );

	// mModelCamLocalToWorld の計算
	mModelCamLocalToWorld = 
		MGetAxis1( 
			Xsc.toVECTOR(),
			Ysc.toVECTOR(),
			Zsc.toVECTOR(),
			m_vModelCamPos.toVECTOR() );

	// mModelCamWorldToLocal の計算
	mModelCamWorldToLocal = MInverse( mModelCamLocalToWorld );

	// ワールド座標におけるスクリーン座標中心位置の計算
	Vector3D vScreenCntPos = m_dModelCamNearClipDist*Ysc + m_vModelCamPos; 

	// mScreenLocalToWorld の計算
	mScreenLocalToWorld = 
		MGetAxis1( 
			Xsc.toVECTOR(),
			Ysc.toVECTOR(),
			Zsc.toVECTOR(),
			vScreenCntPos.toVECTOR() );

	// mScreenWorldToLocal の計算
	mScreenWorldToLocal = MInverse( mScreenLocalToWorld );

	// 一度、ちゃんと変換されているか、見てみるか。
	DrawCoordi( mModelCamLocalToWorld, 2.0 );
	DrawCoordi( mScreenLocalToWorld, 2.0 );
	// 良さそうだ


};

// モデルステータスから幾何的座標位置を更新
void StickTiltProjectorDemo::UpdateGeoPoss()
{
	// ## スクリーン上のキャラクター位置をもとめる
		
	// スクリーンローカル座標におけるキャラクター位置を求める
	Vector3D vEntiPosForScLocal = VTransform( m_vPlayerPos.toVECTOR(), mScreenWorldToLocal );

	// スクリーンローカル座標におけるカメラ位置を求める
	Vector3D vCamPosForScLocal = VTransform( m_vModelCamPos.toVECTOR(), mScreenWorldToLocal );

	// スクリーンローカル座標におけるカメラ位置-キャラクター位置のxz平面交点を求める
	calcCrossPointWithXZPlane( vCamPosForScLocal, vEntiPosForScLocal, m_vEntiPosAsScreenForScLocal );

	// 上記交点をワールド座標に変換 → これがワールド座標における、スクリーン上（に映る）のキャラクタ位置
	m_vEntiPosAsScreenForWorld = VTransform( m_vEntiPosAsScreenForScLocal.toVECTOR(), mScreenLocalToWorld );

	// ######
	// 悲しいことに、１回のみしか呼ぶことを考えていない。


	// ## スクリーン上のスティックの軌跡と、その投影円を計算

	LineRing StickTrackForScLocal( 3.0, m_iStickTrackDivNum, GlobalLineColor );
	
	for( int i=0; i<m_iStickTrackDivNum; i++)
	{
		calcStickTiltPosProjection( 
			StickTrackForScLocal.editVertexes()[i],
			m_StickTrackOnScreen.editVertexes()[i],
			m_StickTrackProjection.editVertexes()[i]
		);
	}

	// 描画用頂点データの初期化
	m_StickTrackOnScreen.resetVertex();
	m_StickTrackProjection.resetVertex();


	// ## 軌跡を投影したときの方向による差分をプレゼンするための扇形


	for( int i=0; i<m_pVertiSectorOnScreen->getAllVertexNum(); i++)
	{
		calcStickTiltPosProjection( 
			m_pVertiSectorOnScreen->editVertexes()[i],
			m_pVertiSectorOnScreen->editVertexes()[i],
			m_pVertiSectorProjection->editVertexes()[i]
		);

		calcStickTiltPosProjection( 
			m_pHorizSectorOnScreen->editVertexes()[i],
			m_pHorizSectorOnScreen->editVertexes()[i],
			m_pHorizSectorProjection->editVertexes()[i]
		);

	}

	// 描画用頂点データの初期化
	m_pVertiSectorOnScreen->resetVertex();
	m_pVertiSectorProjection->resetVertex();
	m_pHorizSectorOnScreen->resetVertex();
	m_pHorizSectorProjection->resetVertex();


	// ## 投影を正規化した場合の円を描く
	
	// 縁の半径を求めるのが難しい → 力ずくで計算しよう

	Vector3D vPlayerPosProj; // vPlayerPos をxz平面に投影した位置
	Vector3D vTmp;
	calcStickTiltPosProjection( m_vEntiPosAsScreenForScLocal, vTmp, vPlayerPosProj );

	double minimize = 10000; // vPlayerPosProj と 輪郭線 m_StickTrackProjection の最小距離 = m_StickTrackProjectionNormalize の半径とする。

	for( int i=0; i<m_iStickTrackDivNum; i++)
	{
		minimize = min( minimize, (vPlayerPosProj-m_StickTrackProjection.editVertexes()[i]).sqlen() );
	}
	minimize = sqrt(minimize);

	LineRing TmpRing( minimize, m_iStickTrackDivNum, GlobalLineColor );

	// 中心位置をシフトして m_StickTrackProjectionNormalize に設定
	for( int i=0; i<m_iStickTrackDivNum; i++)
	{
		m_StickTrackProjectionNormalize.editVertexes()[i] = TmpRing.editVertexes()[i] + vPlayerPosProj;
	}

	// 描画用頂点データの初期化
	m_StickTrackProjectionNormalize.resetVertex();

}

// スティックの傾きの方向（x-z平面上）→ ワールx-z平面上へ投影した座標へ変換する関数を用意する
void StickTiltProjectorDemo::calcStickTiltPosProjection(
	Vector3D vStickTiltPosForScLocal, // [IN] スクリーンローカル座標におけるスティックの傾き方向位置
	Vector3D &vStickTiltPosForWorld,  // [OUT] ワールド座標におけるスティックの傾き方向位置
	Vector3D &vStickTiltPosProjection // [OUT] xz平面上に投影したスティックの傾き方向位置（ワールド座標における）
	)
{
	// ## キャラクター位置中心にスティックの傾き方向を描画
		
	// スクリーンローカル座標におけるスティックの傾き方向位置を求める
	//Vector3D vStickTiltPosForScLocal = vStickTilt + vEntiPosAsScreenForScLocal;

	// 上記位置をワールド座標に変換 ← スクリーン上スティック傾き方向位置
	vStickTiltPosForWorld = VTransform( vStickTiltPosForScLocal.toVECTOR(), mScreenLocalToWorld );

	// ## スクリーンから見たスティックの傾き方向をキャラクターの地平面に投影

	// カメラ位置-スクリーン上スティック傾き方向位置のxz平面交点を求める
	calcCrossPointWithXZPlane( m_vModelCamPos, vStickTiltPosForWorld, vStickTiltPosProjection );


};

// カメラモデルを描画
void StickTiltProjectorDemo::RenderModelCamera()
{
	// カメラモデルの姿勢調整
	MV1SetMatrix( m_iModelCamHandle, MMult( MatConf, mModelCamLocalToWorld ) );
		
	// カメラの描画
	MV1DrawModel( m_iModelCamHandle );

};

// スクリーンを描画
void StickTiltProjectorDemo::RenderScreen()
{
	// 前処理
	m_ScPlane.setCntStickRing( m_vEntiPosAsScreenForScLocal );
	m_ScPlane.m_mLocalToWorldMatrix = mScreenLocalToWorld;
	m_ScPlane.setVertex();
	// → 本来はUpdateの中でやるべき処理

	// 描画
	m_ScPlane.Render();
	
};

// 各種補助線を描画
void StickTiltProjectorDemo::RenderAuxiliaryLines()
{
	// スクリーン上のキャラクタ位置
	DrawSphere3D( m_vEntiPosAsScreenForWorld.toVECTOR() , 0.3f, 16, GetColor( 255,0,0 )  , GetColor( 0,0,0 ), TRUE ) ;
		
	// スクリーン上のスティック傾き位置の描画
	//DrawSphere3D( vStickTiltPos.toVECTOR()    , 0.3f, 16, GetColor( 0,0,255 )  , GetColor( 0,0,0 ), TRUE ) ;

	// キャラクター地平面へ投影した操舵力方向を描画
	//DrawSphere3D( vSteeringForceDir.toVECTOR(), 0.3f, 16, GetColor( 255,255,0 ), GetColor( 0,0,0 ), TRUE ) ;

	// カメラ位置 - キャラクタ位置 の線分
	DrawLine3D( m_vModelCamPos.toVECTOR(), m_vPlayerPos.toVECTOR(), GetColor( 255,0,0 ) );

	// カメラ位置 - スクリーン上スティック傾き方向位置 の線分
	//DrawLine3D( m_vModelCamPos.toVECTOR(), vSteeringForceDir.toVECTOR(), GetColor( 0,0,255 ) );


	// カメラ視線
	DrawLine3D( m_vModelCamPos.toVECTOR(), m_vModelCamGazePoint.toVECTOR(), GetColor( 0,255,255 ) );


};

// スクリーン上の円を描画
void StickTiltProjectorDemo::RenderStickTrackOnScreen()
{
	m_StickTrackOnScreen.Render();
};

// スクリーン上の円をxz平面上に投影した図形を描画
void StickTiltProjectorDemo::RenderStickTrackProjection()
{
	m_StickTrackProjection.Render();
};

// ③ ②を投影平面上で正規化した正円の描画
void StickTiltProjectorDemo::RenderStickTrackProjectionNormalize()
{
	m_StickTrackProjectionNormalize.Render();
};


// 扇形の描画
void StickTiltProjectorDemo::RenderSectors()
{
	m_pVertiSectorOnScreen->Render();
	m_pVertiSectorProjection->Render();
	m_pHorizSectorOnScreen->Render();
	m_pHorizSectorProjection->Render();
		
};

// オリジナルのスティックの傾きの変化量を表す扇型を描画
void StickTiltProjectorDemo::RenderOrgStickTiltDirVariation( Vector2D RenderPos )
{
	// 扇型の実際に描画される半径
	double RenderRadius = SectorFigure2D::m_dRenderScale * SectorRadius ;

	// 垂直方向の扇形を描画
	m_pVertiSectorOrgStickTiltDir->RenderOnCanvas( RenderPos );

	// 水平方向の扇形を描画
	m_pHorizSectorOrgStickTiltDir->RenderOnCanvas( RenderPos );

	// 枠となる円を描く
	Point2D CircleCnt = RenderPos.toPoint();
	DrawCircle( CircleCnt.x, CircleCnt.y, ((int)RenderRadius)-1, GlobalLineColor, FALSE );

}

// （スティックの傾き）計算されたキャラクタの進行方向の変化量を表す扇型を描画
void StickTiltProjectorDemo::RenderCharactrMoveDirVariation( Vector2D RenderPos )
{
	// 扇型の実際に描画される半径
	double RenderRadius = SectorFigure2D::m_dRenderScale * SectorRadius ;

	// 垂直方向の扇形を描画
	m_pVertiSectorCharactrMoveDir->RenderOnCanvas( RenderPos );

	// 水平方向の扇形を描画
	m_pHorizSectorCharactrMoveDir->RenderOnCanvas( RenderPos );

	// 枠となる円を描く
	Point2D CircleCnt = RenderPos.toPoint();
	DrawCircle( CircleCnt.x, CircleCnt.y, ((int)RenderRadius)-1, GlobalLineColor, FALSE );

	// 垂直方向を上方向に持ってくるためには、さらに機能追加が必要......
	// ここまではやるか..

}

// オリジナルのスティックの傾きの扇形 の初期化
void StickTiltProjectorDemo::UpdateSectorOrgStickTiltDir()
{
	m_pVertiSectorOrgStickTiltDir = new SectorFigure2D( 
		SectorRadius, m_vVertiStickTiltDirBgn, m_vVertiStickTiltDirEnd, m_iSectorDivNum );
	m_pHorizSectorOrgStickTiltDir = new SectorFigure2D( 
		SectorRadius, m_vHorizStickTiltDirBgn, m_vHorizStickTiltDirEnd, m_iSectorDivNum );

	// 色の設定
	m_pVertiSectorOrgStickTiltDir->m_iRenderColor = GetColor( 0, 255, 0 );
	m_pHorizSectorOrgStickTiltDir->m_iRenderColor = GetColor( 0, 0, 255 );

	// 扇形の回転角の設定
	m_pVertiSectorOrgStickTiltDir->m_dRotation = DX_PI_F * (0.5);
	m_pHorizSectorOrgStickTiltDir->m_dRotation = DX_PI_F * (0.5);

	// ポリゴンの生成
	m_pVertiSectorOrgStickTiltDir->GeneratePolygons();
	m_pHorizSectorOrgStickTiltDir->GeneratePolygons();

};

// （スティックの傾きから）計算されたキャラクタの進行方向（を図示する扇形）の初期化
void StickTiltProjectorDemo::UpdateSectorCharactrMoveDir()
{
	Vector3D vTmp;

	Vector3D vVertiCharactrMoveDirBgn;
	Vector3D vVertiCharactrMoveDirEnd;
	calcStickTiltPosProjection( m_vVertiStickTiltDirBgn.toVector3D(), vTmp, vVertiCharactrMoveDirBgn );
	calcStickTiltPosProjection( m_vVertiStickTiltDirEnd.toVector3D(), vTmp, vVertiCharactrMoveDirEnd );
	m_pVertiSectorCharactrMoveDir = new SectorFigure2D( 
		SectorRadius, 
		vVertiCharactrMoveDirBgn.toVector2D(), 
		vVertiCharactrMoveDirEnd.toVector2D(), 
		m_iSectorDivNum );

	Vector3D vHorizCharactrMoveDirBgn;
	Vector3D vHorizCharactrMoveDirEnd;
	calcStickTiltPosProjection( m_vHorizStickTiltDirBgn.toVector3D(), vTmp, vHorizCharactrMoveDirBgn );
	calcStickTiltPosProjection( m_vHorizStickTiltDirEnd.toVector3D(), vTmp, vHorizCharactrMoveDirEnd );
	m_pHorizSectorCharactrMoveDir = new SectorFigure2D( 
		SectorRadius, 
		vHorizCharactrMoveDirBgn.toVector2D(), 
		vHorizCharactrMoveDirEnd.toVector2D(), 
		m_iSectorDivNum );

	// 色の設定
	m_pVertiSectorCharactrMoveDir->m_iRenderColor = GetColor( 0, 255, 0 );
	m_pHorizSectorCharactrMoveDir->m_iRenderColor = GetColor( 0, 0, 255 );

	// 扇形の回転角の設定
	m_pVertiSectorCharactrMoveDir->m_dRotation = DX_PI_F * (1.0+0.5);
	m_pHorizSectorCharactrMoveDir->m_dRotation = DX_PI_F * (1.0+0.5);


	// 描画用ポリゴン情報の初期化
	m_pVertiSectorCharactrMoveDir->GeneratePolygons();
	m_pHorizSectorCharactrMoveDir->GeneratePolygons();

};




// ########################## ScreenPlane ##########################

// コンストラクタ
ScreenPlane::ScreenPlane( double Width, double Hight, double Radius ):
	m_oFrameT( Vector3D( -Width, 0,  Hight), Vector3D(  Width, 0,  Hight ), GlobalLineColor ), 
	m_oFrameB( Vector3D( -Width, 0, -Hight), Vector3D(  Width, 0, -Hight ), GlobalLineColor ), 
	m_oFrameL( Vector3D( -Width, 0, -Hight), Vector3D( -Width, 0,  Hight ), GlobalLineColor ), 
	m_oFrameR( Vector3D(  Width, 0, -Hight), Vector3D(  Width, 0,  Hight ), GlobalLineColor ), 
	m_oAxisX( Vector3D( -Width, 0, 0 ), Vector3D(  Width,  0, 0 ), GlobalLineColor ), 
	m_oAxisY( Vector3D( 0, 0, -Hight ), Vector3D(  0, 0,  Hight ), GlobalLineColor ), 
	m_oStickRing( Radius, 32, GetColor( 255, 0, 0 ) ), 
	m_vCntStickRing( 0.0, 0.0, 0.0 )
{
	m_mLocalToWorldMatrix = MGetIdent();
};

void ScreenPlane::setVertex()
{
	static MATRIX WorkMat;

	// 全描画オブジェクトのVertex情報をリセットする
	m_oFrameT.resetVertex();
	m_oFrameB.resetVertex();
	m_oFrameL.resetVertex();
	m_oFrameR.resetVertex();
	m_oAxisX.resetVertex();
	m_oAxisY.resetVertex();
	m_oStickRing.resetVertex();

	// スティックの範囲円をシフトする
	WorkMat = MGetIdent();
	WorkMat.m[3][0] = m_vCntStickRing.x;
	WorkMat.m[3][1] = m_vCntStickRing.y;
	WorkMat.m[3][2] = m_vCntStickRing.z;
	m_oStickRing.MatTransVertex(WorkMat);

	// m_mLocalToWorldMatrix で座標変換
	m_oFrameT.MatTransVertex(m_mLocalToWorldMatrix);
	m_oFrameB.MatTransVertex(m_mLocalToWorldMatrix);
	m_oFrameL.MatTransVertex(m_mLocalToWorldMatrix);
	m_oFrameR.MatTransVertex(m_mLocalToWorldMatrix);
	m_oAxisX.MatTransVertex(m_mLocalToWorldMatrix);
	m_oAxisY.MatTransVertex(m_mLocalToWorldMatrix);
	m_oStickRing.MatTransVertex(m_mLocalToWorldMatrix);

};

void ScreenPlane::Render()
{
	// 各オブジェクトの描画メソッドを呼ぶだけ
	m_oFrameT.Render();
	m_oFrameB.Render();
	m_oFrameL.Render();
	m_oFrameR.Render();
	m_oAxisX.Render();
	m_oAxisY.Render();

	// 軌跡は別途描画するようにする
	//m_oStickRing.Render();
};

