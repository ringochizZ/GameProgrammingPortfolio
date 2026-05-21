/**
 * @file  ButtonManager.cpp
 *
 * @brief  ボタン管理に関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.04.20
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ButtonManager.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
ButtonManager::ButtonManager()
    : m_activeIndex{ 0 }
    , m_selectSound{ nullptr }
    , m_baseY{ 0 }
    , m_minX{ 0 }
    , m_maxX{ 0 }
{
}



/**
 * @brief 初期化処理
 *
 * @param[in] baseY   ボタンのY座標
 * @param[in] minX    ボタンのX座標の最小値
 * @param[in] maxX    ボタンのX座標の最大値
 *
 * @return なし
 */
void ButtonManager::Initialize(float baseY, float minX, float maxX)
{
    m_baseY = baseY;
    m_minX = minX;
    m_maxX = maxX;
}



/**
 * @brief ボタンの登録処理
 *
 * @param[in] button 登録するボタン
 *
 * @return なし
 */
void ButtonManager::AddButton(std::unique_ptr<Button> button)
{
    m_buttons.push_back(std::move(button));
    RealignButtons();
}



/**
 * @brief ボタンを均等に並べる処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ButtonManager::RealignButtons()
{
    if (m_buttons.empty()) return;

    size_t count = m_buttons.size();
    if (count == 1)
    {
		// ボタンが1つだけなら、X座標の中心に配置
		// X座標の中心を計算して、Y座標を固定値(m_baseY)にする
        float centerX = (m_minX + m_maxX) / 2.0f;
        m_buttons[0]->SetPosition(DirectX::SimpleMath::Vector2(centerX, m_baseY));
        return;
    }

    // X座標の間隔を計算
    float step = (m_maxX - m_minX) / (count - 1);
    for (int i = 0; i < count; ++i)
    {
		// X座標を計算
        float x = m_minX + (step * i);

		// X座標を計算して、Y座標を固定値(m_baseY)にする
        m_buttons[i]->SetPosition(DirectX::SimpleMath::Vector2(x, m_baseY));
    }
}



/**
 * @brief 更新処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ButtonManager::Update()
{
    if (m_buttons.empty()) return;

    DirectX::Keyboard::State keystate = DirectX::Keyboard::Get().GetState();
    m_tracker.Update(keystate);

    // 左右キーで選択を移動
    if (m_tracker.pressed.Left) 
    {
        m_activeIndex = (m_activeIndex + 1) % m_buttons.size();
        if (m_selectSound) m_selectSound->Play();
    }
    if (m_tracker.pressed.Right)
    {
        m_activeIndex = (m_activeIndex + m_buttons.size() - 1) % m_buttons.size();
        if (m_selectSound) m_selectSound->Play();
    }

    // スペースキーで決定
    if (m_tracker.pressed.Space)
    {
        m_buttons[m_activeIndex]->OnClick();
    }

    // 各ボタンの状態更新とUpdate呼び出し
    for (int i = 0; i < m_buttons.size(); ++i)
    {
        m_buttons[i]->SetActive(i == m_activeIndex);
        m_buttons[i]->Update();
    }
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ButtonManager::Render()
{
    for (auto& btn : m_buttons)
    {
        btn->Render();
    }
}
