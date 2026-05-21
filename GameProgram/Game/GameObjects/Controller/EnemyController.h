/**
 * @file   EnemyController.h
 *
 * @brief  敵の操作に関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "IControll.h"

#include <random>

// クラスの定義 ===============================================================
/**
 * @brief 敵コントローラー
 */
class EnemyController : public IControll
{
// 列挙型の定義 -------------------------------------------------------------
public:
    /**
     * @brief スキルの種類
     */
	enum class SkillType 
    {
		RUSH,       // 突進
		GUST,       // 突風
		TORNADO,    // 竜巻
		NONE        // スキルなし
	};

// 定数の宣言 -------------------------------------------------------------
private:
    // 移動関連
    static constexpr float ARRIVAL_RADIUS       =  0.5f;  // 目的地に到着したとみなす距離
    static constexpr float AREA_MIN             = -9.0f;  // 移動範囲の最小
    static constexpr float AREA_MAX             =  9.0f;  // 移動範囲の最大
    static constexpr float TARGET_UPDATE_BASE   =  0.5f;  // 目的地更新最短時間
    static constexpr float SPEED                =  5.0f;  // 移動速度

    // 移動ゆらぎ
    static constexpr float WIGGLE_RANGE    = 0.3f;    // ノイズ生成範囲
    static constexpr float WIGGLE_STRENGTH = 0.2f;    // ノイズ影響度

    // 確率
    static constexpr float RUSH_SKILL_RATE    = 30.0f;  // 突進スキル
    static constexpr float GUST_SKILL_RATE    = 25.0f;  // 突風スキル
    static constexpr float TORNADO_SKILL_RATE = 10.0f;  // 竜巻スキル
    static constexpr float NONE_SKILL_RATE    = 35.0f;  // スキルなし

    static constexpr float COOLDOWN_TIME          = 3.0f;   // スキル使用後のクールダウン
    static constexpr float TURN_SPEED             = 0.2f;   // 旋回の滑らかさ
	static constexpr float DEFAULT_SKILL_COOLDOWN = 2.0f;   // スキルクールダウン時間
    
// 変数の宣言 --------------------------------------------------------------
private:
    DirectX::SimpleMath::Vector3 m_targetPosition; // 目的地
    bool m_reachedTarget;                          // 目的地に到達したか

    float m_skillCooldown; // スキル再使用までの待機時間

    std::mt19937 m_randomEngine; // 乱数生成器

    // スキル抽選用の分布
    std::discrete_distribution<int> m_skillDistribution;

	DirectX::SimpleMath::Vector3 m_moveDir; // 現在の移動方向
    
    float m_targetUpdateTimer;   // ターゲット更新間隔

// メンバ関数の宣言 -------------------------------------------------
public:
	EnemyController();
	~EnemyController() override = default;

// 操作
public:
    // 更新処理
    void Controller(
        Character& character, 
        const PerceptionData& perception,
        float elapsedTime) override;

private:
	// 新しい目的地を決める関数
    void DecideNextTarget();

};
