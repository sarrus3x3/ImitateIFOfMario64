#include <cassert>

#include "AnimationManager.h"

int AnimationManager::m_iModelHandleMaster = -1; // m_iModelHandle の初期化。（静的メンバ変数）

// #### コンストラクタ ####
AnimationManager::AnimationManager() :
	m_PlayPitch(20.0),
	m_bIsNowBlending( false ),
	m_dBlendElapsed( 0 ),
	m_dAnimSwitchTime( 0 ),
	DBG_m_bPauseOn(false)
{
	// ################## モデルの読み込み #######################

	if( m_iModelHandleMaster == -1 )
	{
		// 次に読み込むモデルの物理演算モードをリアルタイム物理演算にする
		//MV1SetLoadModelUsePhysicsMode( DX_LOADMODEL_PHYSICS_REALTIME ) ;
		MV1SetLoadModelUsePhysicsMode( DX_LOADMODEL_PHYSICS_DISABLE ) ;
		
		// モデルデータの読み込み
		m_iModelHandleMaster = MV1LoadModel( "..\\mmd_model\\初音ミク.pmd" ) ;

		// 初回のインスタンスだけ m_iModelHandle = m_iModelHandleMaster
		m_iModelHandle = m_iModelHandleMaster;

	}
	else
	{
		// ２回め以降の m_iModelHandle は、m_iModelHandleMaster を MV1DuplicateModel で複製して使う
		m_iModelHandle = MV1DuplicateModel( m_iModelHandleMaster );
	}

	// 「センター」フレームのフレーム番号を取得
	m_iCenterFrameIndex = MV1SearchFrame( m_iModelHandle, "センター" );
	assert( m_iCenterFrameIndex>=0 && "Fail to MV1SearchFrame." );

	// 輪郭異常の補正
	int MaterialNum, i ;
	MaterialNum = MV1GetMaterialNum( m_iModelHandle ) ;		// マテリアルの数を取得
	for( i = 0 ; i < MaterialNum ; i ++ )
	{
		// マテリアルの元々の輪郭線の太さを取得
		//DotWidth = MV1GetMaterialOutLineDotWidth( m_iModelHandle, i ) ;

		// マテリアルの輪郭線の太さを10分の１にする
		//MV1SetMaterialOutLineDotWidth( m_iModelHandle, i, DotWidth / 5.0f ) ;

		// ### 輪郭線を消す
		MV1SetMaterialOutLineDotWidth( m_iModelHandle, i, 0.0f ) ;
		MV1SetMaterialOutLineWidth( m_iModelHandle, i, 0.0f ) ;

		// ### マテリアルのアンビエントカラーを無しに（真黒に）
		MV1SetMaterialAmbColor( m_iModelHandle, i, GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ) ;
	
	}

	// モデルの方向をx軸正方向に修正。（デフォルトではz軸方向に向いてるっぽいので。）
	// → 回転値を設定するたびに初期化されるから意味ねー。

	// m_pCurAnimPlayInfo と m_pPrvAnimPlayInfo の初期化
	m_pCurAnimPlayInfo = new AnimPlayBackInfo;
	m_pPrvAnimPlayInfo = new AnimPlayBackInfo;
	m_pCurAnimPlayInfo->init();
	m_pPrvAnimPlayInfo->init();

	m_pCurAnimPlayInfo->m_bRemoved = true;
	m_pPrvAnimPlayInfo->m_bRemoved = true;

};
void AnimationManager::CleanUpAnim( AnimPlayBackInfo* pAnimInfo )
{
	pAnimInfo->m_bRemoved = true;
	MV1DetachAnim( m_iModelHandle, pAnimInfo->m_AttachIndex ); // 古いアニメーションのデタッチ（デタッチしないとAnimationが混じって変なことになる）
};

// 引数のpAnimInfoを指定されたアニメーションで初期化する
void AnimationManager::InitAnimPlayInfoAsAnim( AnimPlayBackInfo* pAnimInfo, PlayerCharacterEntity::AnimationID AnimID )
{
	// 構造体を再利用して、新たに設定されたアニメーションのコンテナを作成する。
	pAnimInfo->init(); // 初期化
	pAnimInfo->m_eAnimID = AnimID; // AnimIDを記憶
	 
	// アニメーション固有情報へのポインタを取得
	AnimUniqueInfo* pAnimUnq = pAnimInfo->getAnimUnqPointer();

	// （MotionIDに従い）m_AttachIndex の設定
	int CurAttachedMotion = pAnimUnq->m_CurAttachedMotion;
	if( CurAttachedMotion>=0 ){
		pAnimInfo->m_AttachIndex = MV1AttachAnim( m_iModelHandle, CurAttachedMotion, -1, FALSE ) ;
	}

	// m_MotionTotalTime を取得
	if( pAnimInfo->m_AttachIndex==-1 )
	{ // アニメーション未設定
		pAnimInfo->m_MotionTotalTime = 0.0f;
	}
	else if( !(pAnimUnq->m_bCutPartAnimation) )
	{ // モーション切出し OFF
		pAnimInfo->m_MotionTotalTime = MV1GetAttachAnimTotalTime( m_iModelHandle, pAnimInfo->m_AttachIndex ) ;	
		pAnimInfo->m_CurPlayTime     = pAnimUnq->m_fAnimStartTime;
	}
	else
	{ // モーション切出し ON
		// PlaySub の仕組みから、これで切出し実現できるはず
		pAnimInfo->m_CurPlayTime     = pAnimUnq->m_fAnimStartTime;
		pAnimInfo->m_MotionTotalTime = pAnimUnq->m_fAnimEndTime;
	}
};

void AnimationManager::setAnim( PlayerCharacterEntity::AnimationID AnimID, double AnimSwitchTime, bool StopPrvAnim, bool SyncToPrv )
{
	// 割り込みで新しいアニメーションがsetAnimされた場合は、予約中の設定は破棄される。
	DiscardReservedAnim();

	// setAnimの処理本体を呼ぶ
	setAnimMain( AnimID, AnimSwitchTime, StopPrvAnim, SyncToPrv );
};

// ブレンド指定する場合は、引数に、ブレンド時間（デフォルト0）、現在のアニメーションの停止要否（デフォルト要）を設定
// AnimSwitchTime 以降を指定しないとブレンドなしで切替する。
void AnimationManager::setAnimMain( 
	PlayerCharacterEntity::AnimationID AnimID,
	double AnimSwitchTime, 
	bool StopPrvAnim, 
	bool SyncToPrv )
{

	// ブレンド中の場合はブレンド処理を解除する
	if( m_bIsNowBlending )
	{ 
		m_pCurAnimPlayInfo->m_fBlendRate = 1.0; // 強制的にアニメーションを切替
		CleanUpAnim( m_pPrvAnimPlayInfo ); // Prvのアニメーションの後処理を行う
		m_bIsNowBlending = false;          // ブレンドは解除された
	}
	
	// ブレンド指定の場合（AnimSwitchTime>0）
	if( AnimSwitchTime > 0 )
	{
		m_bIsNowBlending  = true;
		m_dBlendElapsed   = 0;
		m_dAnimSwitchTime = AnimSwitchTime;

		// フラグが立っていれば前のアニメーション再生を停止（静止）する
		m_pCurAnimPlayInfo->m_bPause = StopPrvAnim;

		// 現在設定されているアニメーションをm_pPrvAnimPlayInfoに移す
		swap( m_pCurAnimPlayInfo, m_pPrvAnimPlayInfo );

	}
	else
	{
		// 現在設定されているアニメーションをm_pPrvAnimPlayInfoに移し、後処理を行う
		swap( m_pCurAnimPlayInfo, m_pPrvAnimPlayInfo );
		CleanUpAnim( m_pPrvAnimPlayInfo );
	}

	// ################## モーションの設定 #######################
	
	// m_pCurAnimPlayInfo をセットされたアニメーションで初期化
	InitAnimPlayInfoAsAnim( m_pCurAnimPlayInfo, AnimID );

	if( SyncToPrv )
	{
		// ##### アニメーションを「同期」させる #####
		// アニメーションを切替える時、「位相」を引き継ぎたいという場合がある。
		// 例えば、歩き→走りに切り替えた時、足を一番前に突き出している時に切替えたら、
		// 歩きのアニメーションでも同じように足を一番前に突き出した状態にしたい。

		float CurInterval = m_pCurAnimPlayInfo->getAnimUnqPointer()->m_fAnimInterval;
		float PrvInterval = m_pPrvAnimPlayInfo->getAnimUnqPointer()->m_fAnimInterval;
		assert( (CurInterval>0) && (PrvInterval>0) && "Unsupport." );
		float scale = CurInterval/PrvInterval;
		m_pCurAnimPlayInfo->m_CurPlayTime = scale * m_pPrvAnimPlayInfo->m_CurPlayTime;

	}

};

void AnimationManager::Play( PlayerCharacterEntity* pEntity )
{
	// ################## アニメーションの再生 #######################

	// 今回の再生タイミングを計算する
	double   telaps    = pEntity->TimeElaps();
	Vector3D Modelpos  = pEntity->Pos();
	Vector3D Modelhead = pEntity->Heading();

	PlayMain( telaps, Modelpos, Modelhead );

}
void AnimationManager::PlayMain( double TimeElaps, Vector3D Pos, Vector3D Head )
{
	// 予約アニメーションの再生可否を確認
	PlayReservedAnim();

	// ################## アニメーションのブレンド処理 #######################

	// アニメーションブレンド中か？
	if( m_bIsNowBlending )
	{
		// m_dBlendElapsed を更新（m_PlayPitch値 は考慮）
		m_dBlendElapsed += m_PlayPitch * (float)TimeElaps;

		// もし、m_dAnimSwitchTime < m_dBlendElapsed の場合は、Curのブレンド値を1にし、ブレンド処理を終了する
		if( m_dAnimSwitchTime < m_dBlendElapsed )
		{
			m_pCurAnimPlayInfo->m_fBlendRate = 1.0;
			CleanUpAnim( m_pPrvAnimPlayInfo ); //Prvのアニメーションの後処理を行う
			m_bIsNowBlending = false;
		}
		else
		{
			// m_dAnimSwitchTime と m_dBlendElapsed から、ブレンド率を計算
			float BRate = (float)(m_dBlendElapsed/m_dAnimSwitchTime);
			m_pCurAnimPlayInfo->m_fBlendRate = BRate;
			m_pPrvAnimPlayInfo->m_fBlendRate = (float)(1.0 - BRate);
		}
	}

	// 再生要否は m_bRemoved の中を見て判断
	if( !m_pCurAnimPlayInfo->m_bRemoved ) PlayOneAnim( TimeElaps, Pos, Head, m_pCurAnimPlayInfo );
	if( !m_pPrvAnimPlayInfo->m_bRemoved ) PlayOneAnim( TimeElaps, Pos, Head, m_pPrvAnimPlayInfo );
	


	// 向きの設定
	// m_vHeading をx-z平面に投影した方向へy軸を中心にモデルを回転させる
	Vector2D head2D = Head.toVector2D();               // headingを2D変換
	double headangle = atan2( head2D.x, head2D.y );    // headingの回転角を取得
	Vector3D RotVec( 0, headangle+DX_PI, 0 );
	MV1SetRotationXYZ( m_iModelHandle, RotVec.toVECTOR() ); // 回転角 + shift 量 モデルを回転させる
	

	// モデルセンタへのモーション位置の補正ベクトルを、
	// PlayOneAnimで計算されたAnimPlayBackInfo.m_vCorrectionVecから計算
	
	// センター位置が固定になるように、モデル（描画）位置を補正するベクトル CorrectionVec を計算する
	
	// Cur側の補正情報取得
	Vector3D CurCorrectVec = m_pCurAnimPlayInfo->m_vCorrectionVec;
	CurCorrectVec += m_pCurAnimPlayInfo->getAnimUnqPointer()->m_vPosShift;
	float    CurBlendRate  = m_pCurAnimPlayInfo->m_fBlendRate;
	
	// Prv側の補正情報取得
	Vector3D PrvCorrectVec = m_pPrvAnimPlayInfo->m_vCorrectionVec;
	PrvCorrectVec += m_pPrvAnimPlayInfo->getAnimUnqPointer()->m_vPosShift;
	float    PrvBlendRate  = m_pPrvAnimPlayInfo->m_fBlendRate;
	if( m_pPrvAnimPlayInfo->m_bRemoved ) PrvBlendRate = 0;
	
	// ブレンド考慮した補正ベクトルを計算
	Vector3D CorrectionVec = ( CurBlendRate*CurCorrectVec + PrvBlendRate*PrvCorrectVec ) / (CurBlendRate+PrvBlendRate);

	// Entityの位置にモデルを配置
	Vector2D PosShiftRot2D = CorrectionVec.toVector2D().rot( -headangle ); // モデルの回転はシフト前の位置を軸に行われるため、シフトベクトルの回転を考慮しないといけない
	Vector3D PosShiftFinally( PosShiftRot2D.x, CorrectionVec.y, PosShiftRot2D.y );
	Vector3D ModPos = Pos + PosShiftFinally; // Motionの表示位置をEntityの位置からシフト
	MV1SetPosition( m_iModelHandle, ModPos.toVECTOR() );
	
	//MV1SetPosition( m_iModelHandle, Pos.toVECTOR() );
	// 差分を考慮して、モデル（描画）位置を設定

	// m_vHeading のベクトルを描画（補助）
	DrawAllow3D( Pos, Head );
	DrawLine3D( Pos.toVECTOR(), (Pos+10*Head).toVECTOR(), GetColor( 255, 0, 0 ) );

	// モデルの描画
    MV1DrawModel( m_iModelHandle ) ;

}

void AnimationManager::PlayOneAnim( double TimeElaps, Vector3D Pos, Vector3D Head, AnimPlayBackInfo* pPlayAnim )
{
	// アニメーション固有情報へのポインタを取得
	AnimUniqueInfo* pAnimUnq = pPlayAnim->getAnimUnqPointer();

	// 今回の再生タイミングを計算する
	float telaps = (float)TimeElaps;

	// curplaytime : 次の再生時刻
	float curplaytime = pPlayAnim->m_CurPlayTime;
	if( !pPlayAnim->m_bPause && !pPlayAnim->m_bFinished && !DBG_m_bPauseOn){
		float UniquePlayPitch = pPlayAnim->getAnimUnqPointer()->m_fUniquePlayPitch;
		curplaytime += m_PlayPitch * UniquePlayPitch * telaps;
	}

	if(pAnimUnq->m_bRepeatAnimation)
	{ // 繰返し再生 ON
		// 再生インターバル内に再マッピングする
		if( pPlayAnim->m_MotionTotalTime > 0 )
		{
			while( curplaytime >= pPlayAnim->m_MotionTotalTime ){ curplaytime -= pPlayAnim->m_MotionTotalTime; }
			pPlayAnim->m_CurPlayTime = curplaytime;
			pPlayAnim->m_iPlayCount++;
		}
		else
		{ // 再生インターバル = 0 のモーション（静止画） 
			pPlayAnim->m_CurPlayTime = 0.0;
		}
	}
	else
	{ // 繰返し再生 OFF
		// m_MotionTotalTime を超えていたら m_MotionTotalTime を設定する
		pPlayAnim->m_CurPlayTime = curplaytime;
		if( pPlayAnim->m_CurPlayTime > pPlayAnim->m_MotionTotalTime )
		{ 
			pPlayAnim->m_CurPlayTime = pPlayAnim->m_MotionTotalTime;
			pPlayAnim->m_bFinished = true;
		}
	}

	// ブレンド率を設定
	if( pAnimUnq->m_bCorrectionToCenter )
	{
		MV1SetAttachAnimBlendRate( m_iModelHandle, pPlayAnim->m_AttachIndex, pPlayAnim->m_fBlendRate ) ; // ブレンド実行中かにかかわらず、毎回よばれちゃうけど、いかがなものか？（処理性能的に問題ないか？）
	}
	else
	{
		MV1SetAttachAnimBlendRate( m_iModelHandle, pPlayAnim->m_AttachIndex, pPlayAnim->m_fBlendRate ) ; // ブレンド実行中かにかかわらず、毎回よばれちゃうけど、いかがなものか？（処理性能的に問題ないか？）
	}

    // 再生時間をセットする
    MV1SetAttachAnimTime( m_iModelHandle, pPlayAnim->m_AttachIndex, pPlayAnim->m_CurPlayTime ) ;

	// センター位置が固定になるように、モデル（描画）位置を補正するベクトル CorrectionVec を計算する
	Vector3D CorrectionVec( 0, 0, 0 );
	if( pAnimUnq->m_bCorrectionToCenter )
	{
		Vector3D DesiredCntPos = pAnimUnq->m_vFixCenterPosLocal;
		Vector3D CurFrmPos( MV1GetAttachAnimFrameLocalPosition( m_iModelHandle, pPlayAnim->m_AttachIndex, m_iCenterFrameIndex ) );
		CorrectionVec = DesiredCntPos - CurFrmPos; // センターフレームの位置と望むセンター位置との差分を計算
	}

	// 計算した CorrectionVec を返却
	pPlayAnim->m_vCorrectionVec = CorrectionVec; 

};

void AnimationManager::ReserveAnim( PlayerCharacterEntity::AnimationID AnimID, double AnimSwitchTime, bool StopPrvAnim )
{
	// キューに積むだけ
	m_qAnimReservationQueue.push( ArgumentOfSetAnim( AnimID, AnimSwitchTime, StopPrvAnim ) );
};

void AnimationManager::PlayReservedAnim()
{
	// 現行アニメーションの再生が完了したか？
	if( (!m_qAnimReservationQueue.empty()) && m_pCurAnimPlayInfo->m_bFinished )
	{
		// 一番最初に予約されたアニメーションを再生設定
		ArgumentOfSetAnim Arg = m_qAnimReservationQueue.front();
		setAnimMain( Arg.m_eAnimID, Arg.m_dAnimSwitchTime, Arg.m_bStopPrvAnim );
		m_qAnimReservationQueue.pop();
	}
};

void AnimationManager::DrawAllow3D( Vector3D cnt, Vector3D heading )
{
	// cnt を中心に heading をx-z平面に投影した方向へ矢印を描画。
	// （　i.e. 矢印は、x-z平面と水平である ）


	static const double AllowLength    = 10.0f;   // 矢印の全長
	static const double AllowHeadHight = 4.0f;    // 矢印の頭の部分の長さ
	static const double AllowHeadWidth = 6.0f;    // 矢印の頭の部分の幅

	static Vector2D PosH( AllowLength, 0 );
	static Vector2D PosL( AllowLength-AllowHeadHight,  0.5*AllowHeadWidth );
	static Vector2D PosR( AllowLength-AllowHeadHight, -0.5*AllowHeadWidth );

	// heading をx-z平面に投影したベクトルを取得
	Vector2D head2D = heading.toVector2D().normalize();
	Vector2D side2D = head2D.side();

	// 矢印の三角形の位置をローカル座標→ワールド座標に変換 
	// Vector2D → Vecto3D へ再変換
	Vector3D posh = Vector2DToWorldSpace( PosH, head2D, side2D ).toVector3D();
	Vector3D posl = Vector2DToWorldSpace( PosL, head2D, side2D ).toVector3D();
	Vector3D posr = Vector2DToWorldSpace( PosR, head2D, side2D ).toVector3D();

	SetUseZBuffer3D( FALSE );
	SetWriteZBuffer3D( FALSE );

	// cntシフト
	// ポリゴンを描画
	DrawTriangle3D( 
		(cnt+posh).toVECTOR(), 
		(cnt+posl).toVECTOR(), 
		(cnt+posr).toVECTOR(), 
		GetColor( 255, 0, 0 ), TRUE ); // 赤色

	SetUseZBuffer3D( TRUE );
	SetWriteZBuffer3D( TRUE );
};

Vector3D AnimationManager::DBG_RenderCenterFramePos()
{
	// フレームの現在のワールド座標を取得
	VECTOR FramePosVEC = MV1GetFramePosition( m_iModelHandle, m_iCenterFrameIndex );

	// フレームの場所に球を描く
	DrawSphere3D( 
		FramePosVEC,
		1.0f, 
		32, 
		GetColor( 255,0,0 ), 
		GetColor( 255, 255, 255 ), 
		TRUE ) ;

	return Vector3D(FramePosVEC);
};

