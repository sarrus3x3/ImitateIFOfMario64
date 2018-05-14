#include <cassert>

#include "AnimationManager.h"
#include "SolidObjects.h"

#define DEBUG_SWITCH_PHYSICS_TYPE

int AnimationManager::m_iModelHandleMaster = -1; // m_iModelHandle の初期化。（静的メンバ変数）

// #### コンストラクタ ####
AnimationManager::AnimationManager() :
	m_PlayPitch(20.0),
	m_dBankAngle( 0.0 ),
	DBG_m_bPauseOn(false),
	DBG_m_bBlendPauseOn( false )

{
	// ################## モデルの読み込み #######################

	if( m_iModelHandleMaster == -1 )
	{
		// 次に読み込むモデルの物理演算モードをリアルタイム物理演算にする
		
		//MV1SetLoadModelUsePhysicsMode( DX_LOADMODEL_PHYSICS_DISABLE ) ;
		MV1SetLoadModelUsePhysicsMode( DX_LOADMODEL_PHYSICS_REALTIME ) ;
		
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

#ifdef DEBUG_SWITCH_PHYSICS_TYPE
	// ######## [DBG] ゲームの途中でモデルを変更する ########

	// オリジナルのモデルのハンドルを退避
	DBG_m_iModelHandle_Original = m_iModelHandle;

	// 髪の毛削除を削除したモデルのハンドル
	DBG_m_iModelHandle_HideHair = MV1LoadModel( "..\\mmd_model\\初音ミク（髪の毛削除）.pmd" ) ;

	// 物理演算ありで読み込んだモデルのハンドル
	MV1SetLoadModelUsePhysicsMode( DX_LOADMODEL_PHYSICS_REALTIME );
	DBG_m_iModelHandle_Physics = MV1LoadModel( "..\\mmd_model\\初音ミク.pmd" ) ;

	// ######## 
#endif

	// 「センター」フレームのフレーム番号を取得
	m_iCenterFrameIndex = MV1SearchFrame( m_iModelHandle, "センター" );
	assert( m_iCenterFrameIndex>=0 && "Fail to MV1SearchFrame." );

	// 「右髪１」フレームのフレーム番号を取得
	DGB_m_iHairFrameIndex = MV1SearchFrame( m_iModelHandle, "右髪１" ); // ★やはりIKフレームは操作できないのか...
	//DGB_m_iHairFrameIndex = MV1SearchFrame( m_iModelHandle, "右ひじ" );
	assert( DGB_m_iHairFrameIndex>=0 && "Fail to MV1SearchFrame." );

#ifndef DEBUG_SWITCH_PHYSICS_TYPE
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
		//MV1SetMaterialAmbColor( m_iModelHandle, i, GetColorF( 0.0f, 0.0f, 0.0f, 0.0f ) ) ;
	
	}
#endif

#ifdef DEBUG_SWITCH_PHYSICS_TYPE
	int ModelHandleList[3] = { 
		DBG_m_iModelHandle_Original, 
		DBG_m_iModelHandle_HideHair, 
		DBG_m_iModelHandle_Physics };

	for( int j=0; j<3; j++ )
	{
		int MaterialNum, i ;
		MaterialNum = MV1GetMaterialNum( ModelHandleList[j] ) ;		// マテリアルの数を取得
		for( i = 0 ; i < MaterialNum ; i ++ )
		{
			// ### 輪郭線を消す
			MV1SetMaterialOutLineDotWidth( ModelHandleList[j], i, 0.0f ) ;
			MV1SetMaterialOutLineWidth( ModelHandleList[j], i, 0.0f ) ;
		}
	}
#endif

	m_AnimPlayInfoArray.insert(m_AnimPlayInfoArray.begin(), AnimPlayBackInfo(this, (PlayerCharacterEntity::AnimationID)-1, 0.0f));
	m_AnimPlayInfoArray[0].AttachAni();

	// アニメーションの物理演算関連の初期化
	initAnimPhysics();

};

// コンストラクタ
AnimPlayBackInfo::AnimPlayBackInfo( AnimationManager* pAnimationManager, PlayerCharacterEntity::AnimationID AnimID, float AnimSwitchTime) :
	m_pAnimationManager(pAnimationManager),
	m_eAnimID(AnimID),
	m_AttachIndex(-1),
	m_MotionTotalTime(0),
	m_CurPlayTime(0),
	m_fBlendRate(1.0f),
	m_iPlayCount(0),
	m_bPause(false),
	m_bFinished(false),
	m_fAnimSwitchTime(AnimSwitchTime),
	m_fBlendRemain(AnimSwitchTime)
{


};

void AnimPlayBackInfo::AttachAni()
{
	// アニメーション固有情報へのポインタを取得
	AnimUniqueInfo* pAnimUnq = getAnimUnqPointer();

	// （MotionIDに従い）m_AttachIndex の設定
	int CurAttachedMotion = pAnimUnq->m_CurAttachedMotion;
	if (CurAttachedMotion >= 0)
	{
		m_AttachIndex = MV1AttachAnim(m_pAnimationManager->m_iModelHandle, CurAttachedMotion, -1, FALSE);
	}
	// m_MotionTotalTime を取得
	if (m_AttachIndex == -1)
	{ // アニメーション未設定
		m_MotionTotalTime = 0.0f;
	}
	else if (!(pAnimUnq->m_bCutPartAnimation))
	{ // モーション切出し OFF
		m_MotionTotalTime = MV1GetAttachAnimTotalTime(m_pAnimationManager->m_iModelHandle, m_AttachIndex);
		m_CurPlayTime = pAnimUnq->m_fAnimStartTime;
		m_fAnimLength = m_MotionTotalTime - pAnimUnq->m_fAnimStartTime;
	}
	else
	{ // モーション切出し ON
	  // PlaySub の仕組みから、これで切出し実現できるはず
		m_CurPlayTime = pAnimUnq->m_fAnimStartTime;
		m_MotionTotalTime = pAnimUnq->m_fAnimEndTime;
		m_fAnimLength = pAnimUnq->m_fAnimEndTime - pAnimUnq->m_fAnimStartTime;
	}
}

void AnimPlayBackInfo::DetachAni()
{
	MV1DetachAnim(m_pAnimationManager->m_iModelHandle, m_AttachIndex); // 古いアニメーションのデタッチ（デタッチしないとAnimationが混じって変なことになる）
}


void AnimationManager::setAnim( PlayerCharacterEntity::AnimationID AnimID, double AnimSwitchTime, bool StopPrvAnim, bool SyncToPrv, float StartFrame )
{
	// 割り込みで新しいアニメーションがsetAnimされた場合は、予約中の設定は破棄される。
	DiscardReservedAnim();

	// setAnimの処理本体を呼ぶ
	setAnimMain( AnimID, AnimSwitchTime, StopPrvAnim, SyncToPrv, StartFrame);
};

// ブレンド指定する場合は、引数に、ブレンド時間（デフォルト0）、現在のアニメーションの停止要否（デフォルト要）を設定
// AnimSwitchTime 以降を指定しないとブレンドなしで切替する。
void AnimationManager::setAnimMain( 
	PlayerCharacterEntity::AnimationID AnimID,
	double AnimSwitchTime, 
	bool StopPrvAnim, 
	bool SyncToPrv,
	float StartFrame )
{
	m_AnimPlayInfoArray[0].m_bPause = StopPrvAnim; // 前のモーションの再生を停止。

	// ブレンド指定有無にかかわらず、先頭に新しいアニメーション構造体を挟んで、引数のパラメータをぶち込む。
	m_AnimPlayInfoArray.insert(m_AnimPlayInfoArray.begin(), AnimPlayBackInfo( this, AnimID, (float)AnimSwitchTime ));

	//m_AnimPlayInfoArray.emplace(m_AnimPlayInfoArray.begin(), this, AnimID, (float)AnimSwitchTime);

	m_AnimPlayInfoArray[0].AttachAni();

	// ################## モーションの設定 #######################

	// ##### アニメーションを「同期」させる #####
	if( SyncToPrv )
	{
		// アニメーションを切替える時、「位相」を引き継ぎたいという場合がある。
		// 例えば、歩き→走りに切り替えた時、足を一番前に突き出している時に切替えたら、
		// 歩きのアニメーションでも同じように足を一番前に突き出した状態にしたい。

		float CurInterval = m_AnimPlayInfoArray[0].getAnimUnqPointer()->m_fAnimInterval;
		float PrvInterval = m_AnimPlayInfoArray[1].getAnimUnqPointer()->m_fAnimInterval;
		float CurAminStart = m_AnimPlayInfoArray[0].getAnimUnqPointer()->m_fAnimStartTime;
		float PrvAminStart = m_AnimPlayInfoArray[1].getAnimUnqPointer()->m_fAnimStartTime;
		
		assert( (CurInterval>0) && (PrvInterval>0) && "同期的シフトするどちらかのモーションのm_fAnimIntervalが未設定." );
		float scale = CurInterval/PrvInterval;
		m_AnimPlayInfoArray[0].m_CurPlayTime = scale * (m_AnimPlayInfoArray[1].m_CurPlayTime - PrvAminStart) + CurAminStart;

	}

	// ##### モーションの開始位置を指定する #####
	if (StartFrame >= 0.0)
	{
		// 開始時刻を StartFrame に変更
		m_AnimPlayInfoArray[0].m_CurPlayTime = StartFrame;
	}


};

void AnimationManager::Play( PlayerCharacterEntity* pEntity )
{
	// ######## アニメーションの再生 ########

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

	// ######## アニメーションのブレンド処理 ########

	// 2018/05/08 多重ブレンド対応
	vector<AnimPlayBackInfo>::iterator it = m_AnimPlayInfoArray.begin();
	double OverBlendRate = 1.0; // 後続モーションに引き継がれるブレンド率

	while (it != m_AnimPlayInfoArray.end())
	{
		it->m_fBlendRemain -= m_PlayPitch * (float)TimeElaps; // ブレンド残り時間を更新

		if (it->m_fBlendRemain > 0)
		{
			// ブレンド率を計算して更新
			float BRate = it->m_fBlendRemain / it->m_fAnimSwitchTime;
			it->m_fBlendRate = (1.0f-BRate) * OverBlendRate; // モーションのブレンド率は、OverBlendRate以上にならない。
			OverBlendRate *= BRate; // ブレンド率の後続のモーションへの引き継ぎ。
		}
		else
		{
			it->m_fBlendRate = OverBlendRate;

			it++;
			// モーションのデタッチ処理（結局、こういう書き方になった...）
			for(vector<AnimPlayBackInfo>::iterator itmp = it; itmp != m_AnimPlayInfoArray.end(); itmp++ )
			{ 
				itmp->DetachAni();
			}
			m_AnimPlayInfoArray.erase(it, m_AnimPlayInfoArray.end()); // 以降のモーション削除
			break; // ループから抜ける
		}
		it++;
	}

	// DBG 
	float time_elaps;
	if( DBG_m_bPauseOn )
	{
		time_elaps = 0.0;
	}
	else
	{
		time_elaps = TimeElaps;
	}

	// ######## アニメーションの再生処理 ########

	// 配列の中身にあるアニメーションを全て再生
	for (int i = 0; i < m_AnimPlayInfoArray.size(); i++)
	{
		PlayOneAnim(time_elaps, Pos, Head, &m_AnimPlayInfoArray[i]); 
	}

	// モデルセンタへのモーション位置の補正ベクトルを、
	// PlayOneAnimで計算されたAnimPlayBackInfo.m_vCorrectionVecから計算
	
	// #### 位置補正をセンターフレームの座標変換行列に反映

	// 位置補正用の座標変換行列を生成
	Vector3D CorrectionVec(0, 0, 0);
	MATRIX TransMac = MGetTranslate(CorrectionVec.toVECTOR());

	// 姿勢傾き（バルク角）の設定
	TransMac = MMult( TransMac, MGetRotZ( (float)m_dBankAngle ) ); // ※ モデルはデフォルトではz軸負方向を向いている

	// モデルはデフォルトでz軸方向に向いているため、x軸方向に向くように回転
	TransMac = MMult( TransMac, MGetRotY( DX_PI * -0.5 ) );

	// Entityの向き設定用の座標変換行列を生成
	
	// 暫定処理。本来は引数にキャラクタのローカル座標情報を渡さなければならない。
	Vector3D Upper( 0, 1, 0 );
	Vector3D Orign( 0, 0, 0 );
	Vector3D Side = Head % Upper;

	// 行列 [ Head, Upper, Side ]
	MATRIX MLoc = MGetAxis1(
		Head.toVECTOR(),
		Upper.toVECTOR(),
		Side.toVECTOR(),
		Orign.toVECTOR()
		);
	
	TransMac = MMult( TransMac, MLoc );

	// Entityの位置設定用の座標変換行列を生成
	TransMac = MMult( TransMac, MGetTranslate(Pos.toVECTOR()) );

	// 座標変換行列をモデルに適用
	MV1SetMatrix( m_iModelHandle, TransMac );

	// m_vHeading のベクトルを描画（補助）
	//DrawAllow3D( Pos, Head );
	
	// アニメーションの物理演算実行
	UpdateAnimPhysics( TimeElaps );

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
	if( !pPlayAnim->m_bPause && !pPlayAnim->m_bFinished ){
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
		if( pPlayAnim->m_CurPlayTime > (pPlayAnim->m_MotionTotalTime) )
		{ 
			pPlayAnim->m_CurPlayTime = (pPlayAnim->m_MotionTotalTime);
			pPlayAnim->m_bFinished = true;
		}
	}

	// ブレンド率を設定
	MV1SetAttachAnimBlendRate( m_iModelHandle, pPlayAnim->m_AttachIndex, pPlayAnim->m_fBlendRate ) ; // ブレンド実行中かにかかわらず、毎回よばれちゃうけど、いかがなものか？（処理性能的に問題ないか？）

    // 再生時間をセットする
	if( pPlayAnim->m_AttachIndex != -1 )
	{
		MV1SetAttachAnimTime( m_iModelHandle, pPlayAnim->m_AttachIndex, pPlayAnim->m_CurPlayTime ) ;
	}

};

void AnimationManager::ReserveAnim( PlayerCharacterEntity::AnimationID AnimID, double AnimSwitchTime, bool StopPrvAnim, float StartFrame )
{
	// キューに積むだけ
	m_qAnimReservationQueue.push( ArgumentOfSetAnim( AnimID, AnimSwitchTime, StopPrvAnim, StartFrame ) );
};

void AnimationManager::PlayReservedAnim()
{
	// 現行アニメーションの再生が完了したか？
	if( (!m_qAnimReservationQueue.empty()) && m_AnimPlayInfoArray[0].m_bFinished )
	{
		// 一番最初に予約されたアニメーションを再生設定
		ArgumentOfSetAnim Arg = m_qAnimReservationQueue.front();
		setAnimMain( Arg.m_eAnimID, Arg.m_dAnimSwitchTime, Arg.m_bStopPrvAnim, false, Arg.m_fStartFrame );
		m_qAnimReservationQueue.pop();
	}
};

AnimPlayBackInfo * AnimationManager::getAnimPlayBackInfoFromAnimID(PlayerCharacterEntity::AnimationID AnimID)
{
	for (int i = 0; i < m_AnimPlayInfoArray.size(); i++)
	{
		if (m_AnimPlayInfoArray[i].m_eAnimID == AnimID)
		{
			return &m_AnimPlayInfoArray[i];
		}
	}

	// 見つからなかったら null を返す。
	return nullptr;
}

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

	// 輪っかを描く
	static PlaneRing RingIns( 
		5.5, 1.25, 32  );
	RingIns.setCenterPos( cnt );
	RingIns.Render(); 

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

void AnimationManager::initAnimPhysics()
{
	// メンバの初期化・設定
	m_eCurPhysicsType = PHYSICS_NONE; // 物理演算なし
	//m_eCurPhysicsType = PHYSICS_DXLIB; // Dxライブラリ
	
	m_bCurBoneExpress = false;        // モデル表示

	std::vector<int> FrameIndexList(8);

	// 各種物理パラメータ
	double M = 0.2;    // 質点の重量（固定）
	double V = 2.0;    // 粘性抵抗（固定）
	double G = 400;     // 重力定数
	double S = 200.0;    // バネ定数（ベース）
	double N = 0.9;      // 自然長算出する上での補正係数

	// 右髪用インスタンス化
	FrameIndexList[0] = MV1SearchFrame( m_iModelHandle, "頭" );
	FrameIndexList[1] = MV1SearchFrame( m_iModelHandle, "右髪１" );
	FrameIndexList[2] = MV1SearchFrame( m_iModelHandle, "右髪２" );
	FrameIndexList[3] = MV1SearchFrame( m_iModelHandle, "右髪３" );
	FrameIndexList[4] = MV1SearchFrame( m_iModelHandle, "右髪４" );
	FrameIndexList[5] = MV1SearchFrame( m_iModelHandle, "右髪５" );
	FrameIndexList[6] = MV1SearchFrame( m_iModelHandle, "右髪６" );
	FrameIndexList[7] = MV1SearchFrame( m_iModelHandle, "右髪７" );

	m_pRightHairPhysics = new StraightMultiConnectedSpringModel( m_iModelHandle, FrameIndexList, 7, M, V, G, S, N );
	m_pRightHairRender  = new MultiJointBoneMotionControl( m_iModelHandle, FrameIndexList, 6 );
	m_iRightHair1FrameIndex = FrameIndexList[1];

	// 左髪用インスタンス化
	FrameIndexList[0] = MV1SearchFrame( m_iModelHandle, "頭" );
	FrameIndexList[1] = MV1SearchFrame( m_iModelHandle, "左髪１" );
	FrameIndexList[2] = MV1SearchFrame( m_iModelHandle, "左髪２" );
	FrameIndexList[3] = MV1SearchFrame( m_iModelHandle, "左髪３" );
	FrameIndexList[4] = MV1SearchFrame( m_iModelHandle, "左髪４" );
	FrameIndexList[5] = MV1SearchFrame( m_iModelHandle, "左髪５" );
	FrameIndexList[6] = MV1SearchFrame( m_iModelHandle, "左髪６" );
	FrameIndexList[7] = MV1SearchFrame( m_iModelHandle, "左髪７" );

	m_pLeftHairPhysics = new StraightMultiConnectedSpringModel( m_iModelHandle, FrameIndexList, 7, M, V, G, S, N );
	m_pLeftHairRender  = new MultiJointBoneMotionControl( m_iModelHandle, FrameIndexList, 6 );
	m_iLeftHair1FrameIndex = FrameIndexList[1];

};

void AnimationManager::UpdateAnimPhysics( double TimeElaps )
{
	if( m_eCurPhysicsType == PHYSICS_SELFMADE )
	{
		// 右髪
		if(m_pRightHairPhysics->Update( TimeElaps ))
		{
			m_pRightHairRender->setBoneAsJointList( m_pRightHairPhysics->m_pPosList );
		}
		if( m_bCurBoneExpress ) m_pRightHairPhysics->DebugRender();

		// 左髪
		if(m_pLeftHairPhysics->Update( TimeElaps ))
		{
			m_pLeftHairRender->setBoneAsJointList( m_pLeftHairPhysics->m_pPosList );
		}
		if( m_bCurBoneExpress ) m_pLeftHairPhysics->DebugRender();
	}
	else if( m_eCurPhysicsType == PHYSICS_DXLIB )
	{
		static const float rate = 10.0;
		MV1PhysicsCalculation( m_iModelHandle, rate * (float)(TimeElaps*1000) );
	}


};

void AnimationManager::ExpBoneOfPhysicsPart( bool BoneExpress )
{
	if( !m_bCurBoneExpress && BoneExpress )
	{ // モデル表示 → ボーン表示
		// 髪のフレームを非表示にする。
		DBG_RenewModel( DBG_m_iModelHandle_HideHair );
	}
	else if( m_bCurBoneExpress && !BoneExpress )
	{ // ボーン表示 → モデル表示
		// 髪のフレームを表示に戻す
		DBG_RenewModel( DBG_m_iModelHandle_Original );
	}
	
	m_bCurBoneExpress = BoneExpress;

};

void AnimationManager::setAnimPhysicsType( PhysicsTypeID id )
{
	// 前の状態のお掃除（一度、物理演算なしの状態に戻す）
	if( m_eCurPhysicsType == PHYSICS_SELFMADE )
	{
		// 座標変換行列をデフォルトに戻す
		m_pRightHairRender->Reset();
		m_pLeftHairRender->Reset();

		// 髪を非表示にしてる場合をもどしたりとか

	}
	else if( m_eCurPhysicsType == PHYSICS_DXLIB )
	{
		DBG_RenewModel( DBG_m_iModelHandle_Original );
	}

	// 今回の状態の設定
	if( id == PHYSICS_SELFMADE )
	{
		// ジョイント位置をフレーム位置に設定する
		m_pRightHairPhysics->setJointPosAsFrame();
		m_pLeftHairPhysics->setJointPosAsFrame();
	}
	else if( id == PHYSICS_DXLIB )
	{
		DBG_RenewModel( DBG_m_iModelHandle_Physics );
	}

	m_eCurPhysicsType = id;
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

void AnimationManager::DBG_RenewModel( int ReneModelHandle )
{
	// 保持している変数で、すげ替える必要があるのは、
	// m_iModelHandle
	// m_pCurAnimPlayInfo->m_AttachIndex
	// m_pPrvAnimPlayInfo->m_AttachIndex

	//AnimPlayBackInfo* AnimInfoList[2]={ m_pCurAnimPlayInfo, m_pPrvAnimPlayInfo };

	// 古いモデルにアタッチされたアニメーションを一度デタッチ
	for (int i = 0; i < m_AnimPlayInfoArray.size(); i++)
	{
		MV1DetachAnim(m_iModelHandle, m_AnimPlayInfoArray[i].m_AttachIndex); // 古いアニメーションのデタッチ（デタッチしないとAnimationが混じって変なことになる）
	}

	// FrameIndex系は大丈夫なはず（ボーンは触ってないので）

	// モデルハンドルを渡されたものに更新
	m_iModelHandle = ReneModelHandle;

	// アニメーションを新しいモデルに再度アタッチ
	// AnimPlayInfo->m_AttachIndex を更新
	for (int i = 0; i < m_AnimPlayInfoArray.size(); i++)
	{
		int CurAttachedMotion = m_AnimPlayInfoArray[i].getAnimUnqPointer()->m_CurAttachedMotion;
		if (CurAttachedMotion >= 0)
		{
			m_AnimPlayInfoArray[i].m_AttachIndex = MV1AttachAnim(m_iModelHandle, CurAttachedMotion, -1, FALSE);
		}
	}

	// 物理演算用のサブファンクションの持ってるモデルハンドルも更新する必要がある
	m_pRightHairPhysics->DBG_RenewModelHandles( m_iModelHandle );
	m_pRightHairRender->DBG_RenewModelHandles( m_iModelHandle );
	m_pLeftHairPhysics->DBG_RenewModelHandles( m_iModelHandle );
	m_pLeftHairRender->DBG_RenewModelHandles( m_iModelHandle );


};
