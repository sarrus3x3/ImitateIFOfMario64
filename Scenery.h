#include<vector>

#include "SolidObjects.h"

// 木星衛星系のジオラマ
class JupiterSystemDiorama
{
private:
	// #### TextureSphere3D型の惑星オブジェクト

	// 木星オブジェクト
	TextureSphere3D *m_pPrimalyObj;

	// 衛星オブジェクト（配列）
	std::vector<TextureSphere3D> m_cSatelliteObjList;

	// 衛星軌道の線輪オブジェクト（配列）
	std::vector<LineRing> m_cSatelliteOrbitalObjList;

	// #### 属性パラメータ ####

	// 主星半径
	double m_dPrimalyStarRadius;

	// 主星自転周期(rotation period)
	double m_dPrimalyRotationPeriod;

public:
	// 衛星数
	static const int m_iSatelliteNum = 4;

	// 衛星名（列挙型）
	enum SatelliteID
	{
		SATELLITE_IO       = 0,
		SATELLITE_EUROPA   = 1,
		SATELLITE_GANYMEDE = 2,
		SATELLITE_CALLISTO = 3
	};

private:
	// 衛星半径（配列）
	double *m_pSatelliteStarRadius;

	// 衛星軌道半径（配列）
	double *m_pSatelliteOrbitalRadius;

	// 衛星自転周期（単位：日）（配列）
	double *m_pSatelliteRotationPeriod;

	// 衛星公転周期（単位：日）（配列）
	double *m_pSatelliteOrbitalPeriod;

	// 主星自転速度
	double m_dPrimalyRotationSpeed;

	// 衛星自転速度（ラジアン／日）（配列）
	double *m_pSatelliteRotationSpeed;

	// 衛星公転速度（ラジアン／日）（配列）
	double *m_pSatelliteOrbitalSpeed;


	// #### 状態パラメータ ####

	// 主星自転角
	double m_dPrimalyRotateAngle;

	// 衛星自転角（配列）
	std::vector<double> m_dSatelliteRotateAngleList;

	// 衛星公転角（配列）
	std::vector<double> m_dSatelliteOrbitalAngleList;

public:
	// ワールド座標への変換行列
	MATRIX m_mLocalToWorldMatrix;

	// #### メソッド ####

	// コストラクタ
	JupiterSystemDiorama();

	// 衛星位置更新
	void Update( double TimeElapse );

	// オブジェクトの頂点情報位置計算
	//   Render()で描画する前に、
	//   軌道位置などの系状態と変換行列から各オブジェクトの頂点位置を計算する。
	//   Render()と分離しているのは、系状態・変換行列の更新ない時に処理軽減したいため。
	void setVertex();

	// 描画
	void Render();

	// 一回テストしてみたほうがいいでしょう。

};