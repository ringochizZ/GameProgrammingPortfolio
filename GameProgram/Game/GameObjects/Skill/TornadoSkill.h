/**
 * @file   TornadoSkill.h
 *
 * @brief  竜巻スキルに関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.04.22
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Skill.h"

// クラスの定義 ===============================================================
/**
 * @brief 竜巻スキル
 */
class TornadoSkill : public Skill
{
// 定義の宣言 --------------------------------------------------------------
	static constexpr float CONSUMPTION        = 30.0f;  // 消費量
	static constexpr float MAX_RADIUS         = 3.0f;   // 最大半径
	static constexpr float RADIUS_GROWTH_RATE = 1.5f;   // 半径の成長速度
	static constexpr float ROTATION_SPEED     = 20.0f;  // 回転速度

// 変数の宣言 --------------------------------------------------------------
private:
	float m_timer;            // 経過時間
	float m_currentRadius;    // 現在の竜巻の半径

	const DirectX::SimpleMath::Vector3* m_centerPos; // 竜巻の中心地点

// メンバ関数の宣言 --------------------------------------------------------
public:
	TornadoSkill(
		ID3D11DeviceContext1* context, 
		ResourceManager* resManager,
		DirectX::CommonStates* states);
	virtual ~TornadoSkill() = default;

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
		const DirectX::SimpleMath::Vector3* pParentPos,
		const DirectX::SimpleMath::Vector3& dir) override;

// 取得/設定
public:
	// 移動を制限
	bool IsMovementRestricted() const override { return IsActive(); }

};
