/**
 * @file   Background.h
 *
 * @brief  背景に関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Library/BaseRenderer.h"

// クラスの定義 ===============================================================
/**
 * @brief 背景
 */
class Background : public BaseRenderer
{
// 定数の宣言 --------------------------------------------------------------
private:

// 変数の宣言 --------------------------------------------------------------
private:
	// タイマー
	float m_time;

// メンバ関数の宣言 --------------------------------------------------------
public:
	Background()
		: m_time{ 0.0f }
	{
	}
	~Background() override = default;

// 操作
public:
	// 生成処理
	void Create(DX::DeviceResources* pDR);
	
	// 更新処理
	void Update(float elapsedTime) { m_time += elapsedTime; };

	// 描画処理
	void Render() override;

};
