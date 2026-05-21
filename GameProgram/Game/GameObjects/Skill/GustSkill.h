/**
 * @file   GustSkill.h
 *
 * @brief  突風スキルに関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.04.24
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Skill.h"
#include "library/DisplayCollision.h"

// クラスの定義 ===============================================================
/**
 * @brief 突風スキル
 */
class GustSkill : public Skill
{
// 定義の宣言 --------------------------------------------------------------
	static constexpr float CONSUMPTION   = 10.0f;  // 消費量
	static constexpr float LIFE_TIME     =  0.5f;  // 存在時間
	static constexpr float GUST_SPEED    =  5.0f;  // 突風の速さ
	static constexpr float GUST_POWER    = 30.0f;  // 吹き飛ばす力
	static constexpr float GUST_WIDTH    =  3.0f;  // 幅
	static constexpr float GUST_HEIGHT   =  2.0f;  // 高さ
	static constexpr float GUST_DEPTH    =  1.0f;  // 奥行き
	static constexpr float FLOOR_SIZE    = 10.0f;  // 床のサイズ
	static constexpr float HITBOX_OFFSET =  1.0f;  // 当たり判定の中心を前に出す距離

// 変数の宣言 --------------------------------------------------------------
private:
	DirectX::SimpleMath::Vector3 m_gustDir; // 方向

	float m_timer;  // タイマー

// メンバ関数の宣言 --------------------------------------------------------
public:
	GustSkill(
		ID3D11DeviceContext1* context, 
		ResourceManager* resManager, 
		DirectX::CommonStates* states);
	virtual ~GustSkill() = default;

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
	// 当たったら消える設定にする
	bool IsDestroyOnHit() const override { return true; }

};
