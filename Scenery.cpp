#include "Scenery.h"

// コンストラクタ（パチパチとデータを入力していく）
JupiterSystemDiorama::JupiterSystemDiorama() :
	m_mLocalToWorldMatrix( MGetIdent() ), // マトリクスの初期化
	m_dSatelliteRotateAngleList( m_iSatelliteNum, 0 ),
	m_dSatelliteOrbitalAngleList( m_iSatelliteNum, 0 ),
	m_dPrimalyRotateAngle( 0 )
{
	// ######### 各種変数の初期化処理 #########

	// #### 属性パラメータ ####
	// Desktop\素材\惑星テクスチャ\木星と衛星

	// 主星半径
	m_dPrimalyStarRadius = 8.93650000;

	// 主星自転周期(rotation period)
	m_dPrimalyRotationPeriod = 0.4135;

	// 衛星半径（配列）
	m_pSatelliteStarRadius = new double[m_iSatelliteNum];
	m_pSatelliteStarRadius[SATELLITE_IO      ] = 0.22766667;
	m_pSatelliteStarRadius[SATELLITE_EUROPA  ] = 0.19510000;
	m_pSatelliteStarRadius[SATELLITE_GANYMEDE] = 0.32890000;
	m_pSatelliteStarRadius[SATELLITE_CALLISTO] = 0.30128750;
	/*
	m_pSatelliteStarRadius[SATELLITE_IO      ] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_EUROPA  ] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_GANYMEDE] = 3.0;
	m_pSatelliteStarRadius[SATELLITE_CALLISTO] = 3.0;
	*/

	// 衛星軌道半径（配列）
	m_pSatelliteOrbitalRadius = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalRadius[SATELLITE_IO      ] =  52.71250000;
	m_pSatelliteOrbitalRadius[SATELLITE_EUROPA  ] =  83.87925000;
	m_pSatelliteOrbitalRadius[SATELLITE_GANYMEDE] = 133.80150000;
	m_pSatelliteOrbitalRadius[SATELLITE_CALLISTO] = 235.33862500;

	// 衛星自転周期（配列）
	m_pSatelliteRotationPeriod = new double[m_iSatelliteNum];
	m_pSatelliteRotationPeriod[SATELLITE_IO      ] = 1.769137786;
	m_pSatelliteRotationPeriod[SATELLITE_EUROPA  ] = 3.551181041;
	m_pSatelliteRotationPeriod[SATELLITE_GANYMEDE] = 7.15455296;
	m_pSatelliteRotationPeriod[SATELLITE_CALLISTO] = 16.6890184;

	// 衛星公転周期（配列）（orbital period）
	m_pSatelliteOrbitalPeriod = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalPeriod[SATELLITE_IO      ] = 1.769137786;
	m_pSatelliteOrbitalPeriod[SATELLITE_EUROPA  ] = 3.551181041;
	m_pSatelliteOrbitalPeriod[SATELLITE_GANYMEDE] = 7.15455296;
	m_pSatelliteOrbitalPeriod[SATELLITE_CALLISTO] = 16.6890184;

	// #### TextureSphere3D型の惑星オブジェクト

	// 木星オブジェクト
	m_pPrimalyObj = new TextureSphere3D(
				Vector3D( 0, 0, 0),
				m_dPrimalyStarRadius,
				true,
				32,
				32,
				LoadGraph( "JupiterSystemDiorama\\jupiter.jpg" ),
				TextureSphere3D::OBJECT_NOSPECULAR
			);

	// 衛星オブジェクト（配列）
	//   配列で構築（仮確保）して、vector型のメンバに代入する方法をとる
	//   引数を持つクラスの配列の初期化方法
	//     http://brain.cc.kogakuin.ac.jp/~kanamaru/lecture/C++2/09/09-01.html
	TextureSphere3D Tmp[] = {
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_IO      ],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\0_io_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_EUROPA  ],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\1_europa_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_GANYMEDE],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\2_ganymede_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			),
		TextureSphere3D(
			Vector3D( 0, 0, 0),
			m_pSatelliteStarRadius[SATELLITE_CALLISTO],
			true,
			32,
			32,
			LoadGraph( "JupiterSystemDiorama\\3_callisto_texture.jpg" ),
			TextureSphere3D::OBJECT_NOSPECULAR
			)
	};

	m_cSatelliteObjList.assign( &Tmp[0], &Tmp[m_iSatelliteNum] ); // vector型のメンバに代入

	// 衛星軌道の線輪オブジェクト（配列）
	LineRing Tmp2[] = {
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_IO      ],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_EUROPA  ],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_GANYMEDE],
			32,
			GetColor( 255, 255, 255 )
			),
		LineRing( 
			m_pSatelliteOrbitalRadius[SATELLITE_CALLISTO],
			32,
			GetColor( 255, 255, 255 )
			)
	};

	m_cSatelliteOrbitalObjList.assign( &Tmp2[0], &Tmp2[m_iSatelliteNum] ); // vector型のメンバに代入

	// ### 自転速度・公転速度を計算して格納
	
	// 主星
	m_dPrimalyRotationSpeed = 2*DX_PI_F/(double)m_dPrimalyRotationPeriod;

	// 衛星
	m_pSatelliteRotationSpeed = new double[m_iSatelliteNum];
	m_pSatelliteOrbitalSpeed  = new double[m_iSatelliteNum];
	for( int i=0; i<m_iSatelliteNum; i++ )
	{
		m_pSatelliteRotationSpeed[i] = 2*DX_PI_F/(double)m_pSatelliteRotationPeriod[i];
		m_pSatelliteOrbitalSpeed[i]  = 2*DX_PI_F/(double)m_pSatelliteOrbitalPeriod[i];
	}

};

void JupiterSystemDiorama::setVertex()
{
	static MATRIX WorkMat;

	// #### 自転角、公転角から、ローカル座標における位置を計算

	m_pPrimalyObj->resetVertex();

	// 主星 - 自転
	WorkMat = MGetRotY( (float)m_dPrimalyRotateAngle );

	// 主星 - ワールド座標に変換
	WorkMat = MMult( WorkMat, m_mLocalToWorldMatrix );

	// Vertexに反映
	m_pPrimalyObj->MatTransVertex( WorkMat );


	// 衛星
	for( int i=0; i<m_iSatelliteNum; i++ )
	{ 
		m_cSatelliteObjList[i].resetVertex();

		// 自転
		WorkMat = MGetRotY( (float)m_dSatelliteRotateAngleList[i] );

		// 公転
		Vector2D OrbitalPos2D( 1, 0 );
		OrbitalPos2D = m_pSatelliteOrbitalRadius[i] * OrbitalPos2D.rot(m_dSatelliteOrbitalAngleList[i]);
		//Vector3D OrbitalPos3D = OrbitalPos2D.toVector3D();
		
		WorkMat.m[3][0] = OrbitalPos2D.x;
		WorkMat.m[3][2] = OrbitalPos2D.y;

		// ワールド座標に変換
		WorkMat = MMult( WorkMat, m_mLocalToWorldMatrix ); // 座標変換行列を合成するときは、先に作用させたい行列を MMult の左側に置くこと。

		// Vertexに反映
		m_cSatelliteObjList[i].MatTransVertex( WorkMat );

		// 軌道線
		m_cSatelliteOrbitalObjList[i].resetVertex();
		m_cSatelliteOrbitalObjList[i].MatTransVertex( m_mLocalToWorldMatrix );

	}

};

void JupiterSystemDiorama::Update( double TimeElapse )
{
	// 自転角、公転角を更新
	m_dPrimalyRotateAngle += TimeElapse*m_dPrimalyRotationSpeed;

	for( int i=0; i<m_iSatelliteNum; i++ )
	{
		m_dSatelliteRotateAngleList[i]  += TimeElapse*m_pSatelliteRotationSpeed[i];
		m_dSatelliteOrbitalAngleList[i] -= TimeElapse*m_pSatelliteOrbitalSpeed[i];
	}
};

void JupiterSystemDiorama::Render()
{
	// 各オブジェクトの描画メソッドを呼ぶだけ

	// 主星
	m_pPrimalyObj->Render();

	// 衛星
	for( int i=0; i<m_iSatelliteNum; i++ )
	{ 
		m_cSatelliteObjList[i].Render();

		// 軌道を表示
		m_cSatelliteOrbitalObjList[i].Render();

	}
	
};
