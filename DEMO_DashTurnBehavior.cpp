
#include <cassert>

#include "DEMO_DashTurnBehavior.h"

// ######################## FlipbookPlayer Class ########################

// コンストラクタ
FlipbookPlayer::FlipbookPlayer( 
	string GraphFileName, 
	int FrameNum , 
	int XSize, int YSize ):
		m_iMaxFrame( FrameNum )
{
	// ## m_hGraphArray を格納する

	// 暫定配列
	int *pTmpArray = new int[FrameNum];

	// # 画像のサイズを取得

	// 画像のサイズを取得するため一時的にロード
	int hTmpGraph = LoadGraph( GraphFileName.c_str() ); 
	
	// 画像のサイズを取得
	int XBaseGrSize, YBaseGrSize;
	GetGraphSize( hTmpGraph, &XBaseGrSize, &YBaseGrSize );

	// ロードした画像データを開放
	DeleteGraph( hTmpGraph ); // 隠し関数？

	// # XNum, YNum を計算
	int XDivNum, YDivNum;
	XDivNum = XBaseGrSize/XSize;
	YDivNum = YBaseGrSize/YSize;

	// # LoadDivGraph で画像を分割読み込み
	LoadDivGraph( GraphFileName.c_str(), FrameNum, XDivNum, YDivNum, XSize, YSize, pTmpArray );

	// # 配列型をvector<int>型の m_hGraphArray に格納し直す
	m_hGraphArray.assign( &pTmpArray[0], &pTmpArray[FrameNum] );

	// 暫定配列を開放
	delete pTmpArray;

	// （結構面倒くさかったな）

}

// アニメーションの再生実行
void FlipbookPlayer::Play( Vector3D Pos, float Width, int Frame )
{
	// 渡されたFraneが最大フレーム数より大きければ、繰り返しとして解釈する
	int PlayFrame = Frame % m_iMaxFrame;
	
	// DXライブラリの組込み関数で再生実行
	DrawBillboard3D( 
		Pos.toVECTOR(), 
		0.5, 0.5,
		Width,
		0.0,
		m_hGraphArray[PlayFrame],
		TRUE );
}

// ######################## SampleEffectAnimManager Class ########################
// SampleEffectEntityと one - to one でアニメーションを管理を行う

// 静的メンバの定義
FlipbookPlayer* SampleEffectAnimManager::m_pFlipbookPlayerIns = NULL; 
int    SampleEffectAnimManager::m_iMaxFrameNum = 10; // アニメーションの最大フレーム数
string SampleEffectAnimManager::m_sGraphFileName = "promi_star_anime240.png";  // アニメーションの画像データのパス
int    SampleEffectAnimManager::m_iXSize = 240; // アニメーションの１コマのXサイズ
int    SampleEffectAnimManager::m_iYSize = 240; // アニメーションの１コマのYサイズ

// コンストラクタ
SampleEffectAnimManager::SampleEffectAnimManager() :
	m_iCurPlayFrame( 0 ),
	m_bEnd( false ),
	m_bRepeatAnimation( false ),
	m_fAnimWidth( 20.0 )
{
	// 初期化子で全て出来てしまった。
};

// デストラクタ
SampleEffectAnimManager::~SampleEffectAnimManager()
{
	// デストラクタに出番はない
};

// staticメンバの初期化
void SampleEffectAnimManager::Initialize()
{
	// FlipbookPlayer のインスタンス化
	m_pFlipbookPlayerIns = new FlipbookPlayer( m_sGraphFileName, m_iMaxFrameNum, m_iXSize, m_iYSize );
}

// アニメーションの描画
void SampleEffectAnimManager::Render( Vector3D Pos )
{
	// 再生カウンタのインクリ
	m_iCurPlayFrame++;

	if( !(m_iCurPlayFrame<m_iMaxFrameNum) )
	{

		if( m_bRepeatAnimation )
		{
			// ループ再生属性なら、最初に戻す
			m_iCurPlayFrame = 0;
		}
		else
		{
			// そうでなければm_iMaxFrameNumを超えないようにする
			m_iCurPlayFrame = m_iMaxFrameNum-1;
			m_bEnd = true; // 再生終了したら、m_bEndフラグを上げる
		}
	}

	// 再生
	m_pFlipbookPlayerIns->Play( Pos, m_fAnimWidth, m_iCurPlayFrame );
	
}

// アニメーションが再生終了したか？
bool SampleEffectAnimManager::isEnd()
{
	return m_bEnd;
}

// 動作確認できた


// ######################## SampleItemAnimManager Class ########################
// SampleItemEntityと one - to one でアニメーションを管理を行う

// 静的メンバの定義
int SampleItemAnimManager::m_iModelHandleMaster = -1;  // MMDモデルのハンドルの原本
int SampleItemAnimManager::m_iAllModelHandleNum =  0;  // 全モデルハンドル数

// コンストラクタ
SampleItemAnimManager::SampleItemAnimManager() :
	//m_fModelScale( 3.0 ) // 魔法石の場合のスケール
	m_fModelScale( 250.0 ) // コインの場合のスケール
{
	// 総モデル数のカウンタをインクリ
	m_iAllModelHandleNum++ ;

	// コピーモデルハンドルの生成
	
	// 最初のインスタンスのみオリジナルモデルを使用。２つめ以降はコピーを使用
	if( m_iAllModelHandleNum <= 1 )
	{
		m_iModelHandle = m_iModelHandleMaster;
	}
	else
	{
		m_iModelHandle = MV1DuplicateModel( m_iModelHandleMaster );
	}

	// 輪郭線の調整
	int MaterialNum, i ;
	MaterialNum = MV1GetMaterialNum( m_iModelHandle ) ;		// マテリアルの数を取得
	for( i = 0 ; i < MaterialNum ; i ++ )
	{
		// ### 輪郭線を消す
		MV1SetMaterialOutLineDotWidth( m_iModelHandle, i, 0.0f ) ;
		MV1SetMaterialOutLineWidth( m_iModelHandle, i, 0.0f ) ;
	}

}

// デストラクタ
SampleItemAnimManager::~SampleItemAnimManager()
{
	// モデルハンドルの開放
	
	// 一応、オリジナルは消さないようにしておく
	if( m_iModelHandle != m_iModelHandleMaster )
	{
		MV1DeleteModel( m_iModelHandle );
	}

}

// staticメンバの初期化
void SampleItemAnimManager::Initialize()
{
	// MMDモデルのハンドルの原本の生成
	//m_iModelHandleMaster = MV1LoadModel( "..\\mmd_model\\MagicStone\\銀獅式魔法石_ver1.00.pmx" ) ;
	m_iModelHandleMaster = MV1LoadModel( "..\\mmd_model\\coin\\coin_ver1.0_masisi.mv1" ) ;
	assert( m_iModelHandleMaster >= 0 );

	// [メモ]
	// 輪郭線を調整の処理は引き継がれないので、コピーした個々のモデルハンドルに対して実施する必要がある。

}

// アニメーションの描画
void SampleItemAnimManager::Render( Vector3D Pos, Vector3D Head )
{	// 位置補正用の座標変換行列を生成
	MATRIX TransMac;

	// モデルサイズの調整
	TransMac = MGetScale( Vector3D( m_fModelScale, m_fModelScale, m_fModelScale ).toVECTOR() );
	
	// Entityの向き設定用の座標変換行列を生成
	// ↓本当は、head、side、uppperの基底から向き設定行列を計算したい。
	//   基底の順序や、デフォルトのEntityの向きを考慮しないといけないので、とりあえず保留
	Vector2D head2D = Head.toVector2D();               // headingを2D変換
	double headangle = atan2( head2D.x, head2D.y );    // headingの回転角を取得
	TransMac = MMult( TransMac, MGetRotY( headangle+DX_PI ) );

	// Entityの位置設定用の座標変換行列を生成
	MATRIX SiftM = MGetIdent();
	SiftM.m[3][0] = (float)Pos.x;
	SiftM.m[3][1] = (float)Pos.y;
	SiftM.m[3][2] = (float)Pos.z;
	TransMac = MMult( TransMac, SiftM );

	// 座標変換行列をモデルに適用
	MV1SetMatrix( m_iModelHandle, TransMac );

	// モデルの描画
    MV1DrawModel( m_iModelHandle ) ;

};

// ######################## SampleItemEntity Class ########################

// 静的メンバの定義
double   SampleItemEntity::m_dBoundingRadius = 8.0;
double   SampleItemEntity::m_dCharacterHight = 20.0;
double   SampleItemEntity::m_dSqBoundingRadius = SampleItemEntity::m_dBoundingRadius * SampleItemEntity::m_dBoundingRadius;
bool     SampleItemEntity::m_bExpBoundingRadius = false;
double   SampleItemEntity::m_dRotSpeed = DX_PI;
const Vector2D SampleItemEntity::m_vDefaultHeading2D = Vector2D( 1.0, 0 );

// コンストラクタ
SampleItemEntity::SampleItemEntity( SampleGameWorld *world, Vector3D Pos ) : 
		SampleBaseGameEntity( Pos ),
		m_pWorld( world ),
		m_vHeading( Vector3D( 1.0, 0, 0 ) ),
		m_dRotAngle( 0 ),
		m_bGotten( false )
{
	// m_pAnimMgr のインスタンスの生成
	m_pAnimMgr = new SampleItemAnimManager();

};

// デストラクタ 
SampleItemEntity::~SampleItemEntity()
{
	// m_pAnimMgr の後始末
	delete m_pAnimMgr;

};

// 状態の更新
void SampleItemEntity::Update(double time_elapsed) // アイテムが、くるくる回転する
{
	// アイテムを回転させる → 回転速度は、静的メンバで定義する
	m_dRotAngle += m_dRotSpeed * time_elapsed;

	// Headinベクトルをm_dRotAngle分回転させる...
	m_vHeading = m_vDefaultHeading2D.rot( m_dRotAngle ).toVector3D();

	// m_bGotten を参照して、取得済みの場合に、SampleEffectEntity を生成。
	if( m_bGotten )
	{
		// SampleEffectEntity を生成して GameWorld に登録
		m_pWorld->RegisterSampleEffectEntity( new SampleEffectEntity( m_pWorld, m_vPos ) );

		// 自己終了する
		m_bExtinct = true;
	}

};

	// 描画
void SampleItemEntity::Render()
{
	m_pAnimMgr->Render( m_vPos, m_vHeading );
};

// プレーヤーキャラクタとの接触検出→ hasGotten フラグを上げる
bool SampleItemEntity::TouchDetection( Vector3D CharaPos )
{
	// 接触半径の２乗を計算
	double sqlen = m_dBoundingRadius * m_dBoundingRadius;

	Vector3D vDiff = m_vPos-CharaPos;
	
	if( (vDiff.y>0) && (m_dCharacterHight>vDiff.y) 
		&& (vDiff.toVector2D().sqlen()<m_dSqBoundingRadius) )
	{
		// m_bGotten を上げる
		m_bGotten = true;
		
		return true;
	}

	return false;
};


// ######################## SampleEffectEntity Class ########################

// コンストラクタ
SampleEffectEntity::SampleEffectEntity( SampleGameWorld *world, Vector3D Pos ) : 
		SampleBaseGameEntity( Pos ),
		m_pWorld( world )
{
	// m_pAnimMgr のインスタンスの生成
	m_pAnimMgr = new SampleEffectAnimManager();

};

// デストラクタ 
SampleEffectEntity::~SampleEffectEntity()
{
	// m_pAnimMgr の後始末
	delete m_pAnimMgr;

};

// 状態の更新
void SampleEffectEntity::Update(double time_elapsed) // アイテムが、くるくる回転する
{
	// アニメーションの再生が終了したら、自己終了する。
	if( m_pAnimMgr->isEnd() )
	{
		m_bExtinct = true;
	}

};

// 描画
void SampleEffectEntity::Render()
{
	m_pAnimMgr->Render( m_vPos );
};

// ######################## SampleGameWorld Class ########################

// コンストラクタ
SampleGameWorld::SampleGameWorld()
{}

// SampleItemEntity を登録
void SampleGameWorld::RegisterSampleItemEntity( SampleItemEntity* pItem )
{
	// m_pIttemEntityList に登録
	m_pIttemEntityList.push_back( pItem );

	// m_pAllEntityList に登録
	m_pAllEntityList.push_back( pItem );

};

// SampleEffectEntity を登録
void SampleGameWorld::RegisterSampleEffectEntity( SampleEffectEntity* pEffect )
{
	// m_pAllEntityList に登録
	m_pAllEntityList.push_back( pEffect );

};

// 初期化
void SampleGameWorld::Initialize()
{
	SampleEffectAnimManager::Initialize();
	SampleItemAnimManager::Initialize();
};

// 更新
void SampleGameWorld::Update( double time_elapsed, Vector3D CharaPos )
{

	for( list<SampleItemEntity*>::iterator it=m_pIttemEntityList.begin();
		 it!=m_pIttemEntityList.end(); )
	{
		// イテレート中のSTLのlistから要素を安全に削除する方法
		// -> http://marupeke296.com/TIPS_No12_ListElementErase.html
		if((*it)->isExtinct())
		{
			// 削除処理
			it = m_pIttemEntityList.erase( it );
			continue;
		}

		// Entityの更新
		(*it)->TouchDetection( CharaPos );

		// 最後にイテレータをインクリ
		it++; 

	}

	for( list<SampleBaseGameEntity*>::iterator it=m_pAllEntityList.begin(); 
		 it!=m_pAllEntityList.end(); )
	{
		// イテレート中のSTLのlistから要素を安全に削除する方法 
		// -> http://marupeke296.com/TIPS_No12_ListElementErase.html
		if((*it)->isExtinct())
		{
			// 削除処理
			it = m_pAllEntityList.erase( it );
			continue;
		}

		// Entityの更新
		(*it)->Update( time_elapsed );

		// 最後にイテレータをインクリ
		it++; 
	}


};

// 描画
void SampleGameWorld::Render()
{
	list<SampleBaseGameEntity*>::iterator it;
	for( it=m_pAllEntityList.begin(); it!=m_pAllEntityList.end(); it++ )
	{
		(*it)->Render();
	}

};


// アイテム配置定義ビットマップを読み込みフィールドにアイテム配置を行う
void SampleGameWorld::SetItemsToWorld( double Separation , double ItemHight, char *BitMapFileName )
{
	// マップ情報定義ビットマップを読み込む
	int BitMapHandle = LoadSoftImage( BitMapFileName );

	// マップ情報定義ビットマップのサイズを取得（縦横）
	int W, H;
	GetSoftImageSize( BitMapHandle, &W, &H ) ;

	// マップ情報定義ビットマップを解析。
	int  r, g, b, a ;
	for( int i=0; i<W; i++)
	{
		for( int j=0; j<H; j++)
		{
			GetPixelSoftImage( BitMapHandle, j, i, &r, &g, &b, &a ) ;

			// r==0 でなければ、アイテム有りと判断
			if( r==0 )
			{
				Vector3D Pos;
				Pos.y = ItemHight;
				Pos.x = i*Separation + 0.5*Separation;
				Pos.z = j*Separation + 0.5*Separation;

				RegisterSampleItemEntity( new SampleItemEntity( this, Pos ) );
			}
		}
	}

};