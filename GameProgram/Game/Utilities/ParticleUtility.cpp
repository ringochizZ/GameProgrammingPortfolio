/**
 * @file   ParticleUtility.cpp
 * 
 * @brief  1粒のパーティクルの挙動に関するソースファイル
 * 
 * @author 小野島樹里
 * 
 * @date   2026.05.20
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ParticleUtility.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 * 
 * @param[in] life        生存時間
 * @param[in] pos         初期座標
 * @param[in] velocity    初期移動速度
 * @param[in] accele      加速度
 * @param[in] startScale  発生時のサイズ
 * @param[in] endScale    消滅時のサイズ
 * @param[in] startColor  発生時の色
 * @param[in] endColor    消滅時の色
 */
ParticleUtility::ParticleUtility(
    float life,
    DirectX::SimpleMath::Vector3 pos,
    DirectX::SimpleMath::Vector3 velocity,
    DirectX::SimpleMath::Vector3 accele,
    DirectX::SimpleMath::Vector3 startScale,
    DirectX::SimpleMath::Vector3 endScale,
    DirectX::SimpleMath::Color startColor,
    DirectX::SimpleMath::Color endColor)
{
    // 生存時間の設定
    m_startLife = life;
    m_life = life;

    // 運動パラメーターの設定
    m_position = pos;
    m_velocity = velocity;
    m_accele = accele;

    // サイズの設定
    m_startScale = startScale;
    m_nowScale = startScale;
    m_endScale = endScale;

    // 色の設定
    m_startColor = startColor;
    m_nowColor = startColor;
    m_endColor = endColor;
}

/**
 * @brief デストラクタ
 */
ParticleUtility::~ParticleUtility()
{
}

/**
 * @brief 更新処理
 * 
 * @param[in] timer ゲーム全体のStepTimer
 * 
 * @return true : 生存している
 *	       false: 生存していない
 */
bool ParticleUtility::Update(float elapsedTime)
{
    // 特定の負の値が設定されている場合は、寿命を減らさずに生存させ続ける
    if (m_life <= ENDLESS)
    {
        return true;
    }

    // 現在の生存比率を算出
    float progress = 1.0f - (m_life / m_startLife);

    // スケール・色の線形補間計算
    m_nowScale = SimpleMath::Vector3::Lerp(m_startScale, m_endScale, progress);
    m_nowColor = SimpleMath::Color::Lerp(m_startColor, m_endColor, progress);

    // 速度の計算
    m_velocity += m_accele * elapsedTime;

    // 座標の計算
    m_position += m_velocity * elapsedTime;

    // 生存時間を減らす
    m_life -= elapsedTime;

    // ライフが0未満になったら削除
    if (m_life < 0.0f)
    {
        return false;
    }

    return true;
}
