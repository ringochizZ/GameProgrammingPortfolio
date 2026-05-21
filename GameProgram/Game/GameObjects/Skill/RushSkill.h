/**
 * @file   RushSkill.h
 *
 * @brief  突進スキルに関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.04.15
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Skill.h"

// クラスの定義 ===============================================================
/**
 * @brief 突進スキル
 */
class RushSkill : public Skill
{
// 定義の宣言 --------------------------------------------------------------
	static constexpr float CONSUMPTION        =  5.0f;  // 消費量
	static constexpr float DEFAULT_RUSH_SPEED = 30.0f;  // 突進速度
	static constexpr float RUSH_END_TIME      =  0.1f;  // 突進終了時間

// 変数の宣言 --------------------------------------------------------------
private:
	DirectX::SimpleMath::Vector3 m_rushDir; // 方向

	float m_rushTimer;  // 突進時間

// メンバ関数の宣言 --------------------------------------------------------
public:
	RushSkill(
		ID3D11DeviceContext1* context, 
		ResourceManager* resManager, 
		DirectX::CommonStates* states);
	virtual ~RushSkill() = default;

// 操作
public:
	// 更新処理
	void Update(float elapsedTime) override;

	// 描画処理
	void Render(
		ID3D11DeviceContext1* context,
		DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj) override;

	// スキル発動できたかどうか
	bool Execute(
		const DirectX::SimpleMath::Vector3& pos,
		const DirectX::SimpleMath::Vector3& dir) override;

// 取得/設定
public:
	// 速度の取得
	virtual DirectX::SimpleMath::Vector3 GetSelfVelocity() const override
	{
		// 記憶した方向にスピードを返す
		if (m_isActive)
		{
			return m_rushDir * DEFAULT_RUSH_SPEED;
		}

		return DirectX::SimpleMath::Vector3::Zero;
	}

	// 移動制限中かどうか
	bool IsMovementRestricted() const override { return m_isActive; }

};
