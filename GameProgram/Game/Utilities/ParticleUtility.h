/**
 * @file   ParticleUtility.h
 * 
 * @brief  パーティクルに必要な要素に関するヘッダファイル
 * 
 * @author 小野島樹里
 * 
 * @date   2026.05.20
 */

#pragma once

 // ヘッダファイルの読み込み ===================================================
#include "ThirdParty/StepTimer.h"
#include <SimpleMath.h>

// クラスの定義 ===============================================================
/**
 * @brief 1粒のパーティクルの挙動を管理するクラス
 */
class ParticleUtility
{
// 定数の宣言 --------------------------------------------------------------
private:
	static constexpr float ENDLESS = -100.0f; // 無限の寿命

// メンバ変数の宣言 --------------------------------------------------------
private:
    // 寿命管理
    float m_startLife = 0.0f;
    float m_life = 0.0f;

    // 運動パラメーター
    DirectX::SimpleMath::Vector3 m_position;
    DirectX::SimpleMath::Vector3 m_velocity;
    DirectX::SimpleMath::Vector3 m_accele;

    // サイズ（スケール）
    DirectX::SimpleMath::Vector3 m_startScale;
    DirectX::SimpleMath::Vector3 m_nowScale;
    DirectX::SimpleMath::Vector3 m_endScale;

    // 色
    DirectX::SimpleMath::Color m_startColor;
    DirectX::SimpleMath::Color m_nowColor;
    DirectX::SimpleMath::Color m_endColor;

// メンバ関数の宣言 --------------------------------------------------------
public:
    // コンストラクタ
    ParticleUtility(
        float life,
        DirectX::SimpleMath::Vector3 pos,
        DirectX::SimpleMath::Vector3 velocity,
        DirectX::SimpleMath::Vector3 accele,
        DirectX::SimpleMath::Vector3 startScale,
        DirectX::SimpleMath::Vector3 endScale,
        DirectX::SimpleMath::Color startColor,
        DirectX::SimpleMath::Color endColor);

    // デストラクタ
    ~ParticleUtility();

    // 更新処理
    bool Update(float elapsedTime);

// 取得/設定
public:
	// 座標の取得
    const DirectX::SimpleMath::Vector3& GetPosition() const { return m_position; }

	// サイズの取得
    const DirectX::SimpleMath::Vector3& GetNowScale() const { return m_nowScale; }

	// 色の取得
    const DirectX::SimpleMath::Color& GetColor()    const { return m_nowColor; }
};