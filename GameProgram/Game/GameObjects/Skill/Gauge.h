/**
 * @file   Gauge.h
 *
 * @brief  ゲージに関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "ThirdParty/StepTimer.h"
#include "ThirdParty/DeviceResources.h"
#include "Library/UserInterface.h"

#include <vector>

// クラスの定義 ===============================================================
/**
 * @brief ゲージ
 */
class Gauge
{
// 定数の宣言 --------------------------------------------------------------
	static constexpr float MAX_GAUGE = 100.0f;  // 最大値
	static constexpr float POS_X     = 50.0f;   // X座標
	static constexpr float POS_Y     = 150.0f;  // Y座標

// 変数の宣言 --------------------------------------------------------------
private:
	unsigned int m_menuIndex;
	DX::DeviceResources* m_pDR;

	std::unique_ptr<UserInterface> m_gauge;
	std::unique_ptr<UserInterface> m_frame;
	std::unique_ptr<UserInterface> m_base;

	std::unique_ptr<UserInterface> m_baseWindow;

	int m_windowWidth, m_windowHeight;

// メンバ関数の宣言 --------------------------------------------------------
public:
	Gauge();
	~Gauge();

// 操作
public:
	// 初期化処理
	void Initialize(DX::DeviceResources* pDR, int width, int height);

	// 更新処理
	void Update(float currentGaugeValue);

	// 描画処理
	void Render();

	// 追加処理
	void Add(
		DirectX::SimpleMath::Vector2 position, 
		DirectX::SimpleMath::Vector2 scale, 
		ANCHOR anchor
	);

};
