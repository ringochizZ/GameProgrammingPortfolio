/**
 * @file   ButtonManager.h
 *
 * @brief  ボタン管理に関するヘッダーファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.04.20
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Button.h"
#include "Keyboard.h"

#include <vector>
#include <memory>
#include <Audio.h>

// クラスの定義 ===============================================================
/**
 * @brief ボタン管理
 */
class ButtonManager
{
// 変数の宣言 --------------------------------------------------------------
private:
    std::vector<std::unique_ptr<Button>> m_buttons;
    size_t m_activeIndex;

    DirectX::Keyboard::KeyboardStateTracker m_tracker;
    DirectX::SoundEffect* m_selectSound;

    // 均等配置用の設定値
    float m_baseY;
    float m_minX;
    float m_maxX;

// メンバ関数の宣言 -------------------------------------------------
public:
    ButtonManager();
    ~ButtonManager() = default;

// 操作
public:
    // 初期化処理
    void Initialize(float baseY, float minX, float maxX);

    // ボタンの登録処理
    void AddButton(std::unique_ptr<Button> button);

    // 更新処理
    void Update();

	// 描画処理
    void Render();

private:
	// ボタンを均等に並べる処理
    void RealignButtons();

// 取得/設定
public:
    // サウンドの設定
    void SetSound(DirectX::SoundEffect* sound) { m_selectSound = sound; }

};