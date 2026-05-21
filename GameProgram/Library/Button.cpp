/**
 * @file  Button.cpp
 *
 * @brief  ボタンに関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.04.20
 */

 // ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Button.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] pDR           デバイスリソース
 * @param[in] windowWidth   ウィンドウの幅
 * @param[in] windowHeight  ウィンドウの高さ
 * @param[in] basePath      背景画像のパス
 * @param[in] textPath      文字画像のパス
 * @param[in] onClick       決定された時に実行する処理
 */
Button::Button(
    DX::DeviceResources* pDR, 
    int windowWidth, int windowHeight,
    const wchar_t* basePath, const wchar_t* textPath,
    std::function<void()> onClick)
    : m_onClick{ onClick }
    , m_isActive{ false }
{
    // 背景の生成
    m_baseUI = std::make_unique<UserInterface>();
    m_baseUI->Create(
        pDR, basePath, 
        DirectX::SimpleMath::Vector2::Zero, 
        DirectX::SimpleMath::Vector2(NORMAL_SCALE, NORMAL_SCALE), 
        MIDDLE_CENTER
    );
    m_baseUI->SetWindowSize(windowWidth, windowHeight);

    // 文字の生成
    m_textUI = std::make_unique<UserInterface>();
    m_textUI->Create(
        pDR, textPath, 
        DirectX::SimpleMath::Vector2::Zero, 
        DirectX::SimpleMath::Vector2(NORMAL_SCALE, NORMAL_SCALE), 
        MIDDLE_CENTER
    );
    m_textUI->SetWindowSize(windowWidth, windowHeight);
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Button::Update()
{
    // 目標スケールを決める
    DirectX::SimpleMath::Vector2 targetScale = m_baseUI->GetBaseScale();

    if (m_isActive)
    {
        // 選ばれている時はスケールを大きくする
        targetScale += m_baseUI->GetBaseScale() * ACTIVE_SCALE;
    }

    // 算出したスケールを適用
    m_baseUI->SetScale(targetScale);
    m_textUI->SetScale(targetScale);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Button::Render()
{
    // 背景 -> 文字 の順番で描画する
    m_baseUI->Render();
    m_textUI->Render();
}