/**
 * @file   EnemyController.cpp
 *
 * @brief  敵の操作に関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.05.21
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "EnemyController.h"

#include "Game/GameObjects/Character.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
EnemyController::EnemyController()
    : m_targetPosition{ DirectX::SimpleMath::Vector3::Zero }
    , m_reachedTarget{ true }
    , m_skillCooldown{ DEFAULT_SKILL_COOLDOWN }
	, m_moveDir{ DirectX::SimpleMath::Vector3::Forward }
	, m_targetUpdateTimer{ 0.0f }
{
    std::random_device rd;
    m_randomEngine.seed(rd());

	// スキル選択の確率分布を設定
    std::vector<float> rate =
    {
        RUSH_SKILL_RATE,
        GUST_SKILL_RATE,
		TORNADO_SKILL_RATE,
		NONE_SKILL_RATE
    };
    m_skillDistribution = std::discrete_distribution<int>(rate.begin(), rate.end());
}



/**
 * @brief 更新処理
 *
 * @param[in] character   キャラクター
 * @param[in] perception  周囲の状況
 * @param[in] alapsedTime 経過時間
 *
 * @return なし
 */
void EnemyController::Controller(
    Character& character,
    const PerceptionData& perception,
    float elapsedTime)
{
    UNREFERENCED_PARAMETER(perception);

    using namespace DirectX::SimpleMath;
    Vector3 currentPos = character.GetPosition();

    // 0.0f ～ 1.0f の値を出す分布
    std::uniform_real_distribution<float> unitDist(0.0f, 1.0f);

    // --- スキル使用の判断 ---
    if (m_skillCooldown <= 0.0f)
    {
        // 離散分布から抽選
        int selectedIndex = m_skillDistribution(m_randomEngine);
        bool usedSkill = false;

        // スキルの使用リクエスト
        switch (static_cast<SkillType>(selectedIndex))
        {
        case SkillType::RUSH:
            character.RequestSkill(0);
            usedSkill = true;
            break;

        case SkillType::GUST:
            character.RequestSkill(1);
            usedSkill = true;
            break;

        case SkillType::TORNADO:
            character.RequestSkill(2);
            usedSkill = true;
            break;

        case SkillType::NONE:
            break;
        }

        if (usedSkill)
        {
            m_skillCooldown = COOLDOWN_TIME;
        }
    }
    else
    {
        m_skillCooldown -= elapsedTime;
    }

    // --- 目的地の更新 ---
    m_targetUpdateTimer -= elapsedTime;
    float distToTarget = (m_targetPosition - currentPos).Length();

    if (m_targetUpdateTimer <= 0.0f || distToTarget < ARRIVAL_RADIUS)
    {
        // 次の目的地を決定
        DecideNextTarget();

        // 次の更新までの待機時間を計算
        m_targetUpdateTimer = TARGET_UPDATE_BASE + unitDist(m_randomEngine);
    }

    // --- 移動方向の計算 ---
    Vector3 toTargetDir = m_targetPosition - currentPos;
    toTargetDir.y = 0; // 高さは考慮しない
    if (toTargetDir.LengthSquared() > 0)
    {
        toTargetDir.Normalize();
    }

	// 移動方向にランダムなゆらぎを加える
    std::uniform_real_distribution<float> noiseDist(-WIGGLE_RANGE, WIGGLE_RANGE);
    Vector3 wiggle(noiseDist(m_randomEngine), 0.0f, noiseDist(m_randomEngine));

    // 目的地への方向とゆらぎを合成
    Vector3 finalDir = toTargetDir + (wiggle * WIGGLE_STRENGTH);
    if (finalDir.LengthSquared() > 0)
    {
        finalDir.Normalize();
    }

    // 現在の移動方向から、目標の移動方向へ滑らかに旋回
    m_moveDir = Vector3::Lerp(m_moveDir, finalDir, TURN_SPEED);
    m_moveDir.Normalize();

    // キャラクターへ移動入力を反映
    character.AddInputMovement(m_moveDir, SPEED);
}



/**
 * @brief 新しい目的地を決める関数
 *
 * @param[in] なし
 *
 * @return なし
 */
void EnemyController::DecideNextTarget()
{
    // 指定された移動範囲から座標を抽選
    std::uniform_real_distribution<float> posDist(AREA_MIN, AREA_MAX);

    m_targetPosition.x = posDist(m_randomEngine);
    m_targetPosition.z = posDist(m_randomEngine);
    m_targetPosition.y = 0.0f;

    m_reachedTarget = false;
}
