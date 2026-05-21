/**
 * @file   Button.h
 *
 * @brief  ボタンに関するヘッダーファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.04.20
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Library/UserInterface.h"

#include <memory>
#include <functional>
#include <SimpleMath.h>

// クラスの定義 ===============================================================
/**
 * @brief ボタン
 */
class Button
{
// 定数の宣言 --------------------------------------------------------------
private:
	static constexpr float NORMAL_SCALE = 0.5f;  // テクスチャの大きさ
	static constexpr float ACTIVE_SCALE = 0.4f;  // 選択されたときの大きさ

// 変数の宣言 --------------------------------------------------------------
private:
    std::unique_ptr<UserInterface> m_baseUI; // 背景画像
    std::unique_ptr<UserInterface> m_textUI; // 文字画像
    std::function<void()> m_onClick;         // 決定された時に実行する処理

    bool m_isActive; // 選択されているか

// メンバ関数の宣言 -------------------------------------------------
public:
    Button(
        DX::DeviceResources* pDR, 
        int windowWidth, int windowHeight,
        const wchar_t* basePath, const wchar_t* textPath,
        std::function<void()> onClick);
    ~Button() = default;

// 操作
public:
	// 更新処理
    void Update();

	// 描画処理
    void Render();

    // 決定された時の処理
    void OnClick()
    {
        if (m_onClick)
        {
            m_onClick();
        }
    }

// 取得/設定
public:
	// 選択状態の設定
    void SetActive(bool isActive) { m_isActive = isActive; }

	// 座標の設定
    void SetPosition(const DirectX::SimpleMath::Vector2& pos)
    {
        // UIクラスに座標を反映
        m_baseUI->SetPosition(pos);
        m_textUI->SetPosition(pos);
    }

};