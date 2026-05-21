/**
 * @file   Gauge.cpp
 *
 * @brief  ゲージに関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.05.21
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Gauge.h"

#include "Library/BinaryFile.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Gauge::Gauge()
    : m_menuIndex{ 0 }
    ,m_windowHeight{ 0 }
    ,m_windowWidth{ 0 }
    ,m_pDR{ nullptr }
    ,m_gauge{ nullptr }
    ,m_frame{ nullptr }
{
}



/**
 * @brief デストラクタ
 */
Gauge::~Gauge()
{
}



/**
 * @brief 初期化処理
 *
 * @param[in] pDR    DeviceResourcesのポインタ
 * @param[in] width  幅
 * @param[in] height 高さ
 *
 * @return なし
 */
void Gauge::Initialize(DX::DeviceResources* pDR,int width,int height)
{
	m_pDR = pDR;
    m_windowWidth = width;
    m_windowHeight = height;

    Add(
        SimpleMath::Vector2(POS_X, POS_Y),
        SimpleMath::Vector2::One, 
        ANCHOR::MIDDLE_LEFT);
}



/**
 * @brief 更新処理
 *
 * @param[in] currentGaugeValue 現在のゲージ値
 *
 * @return なし
 */
void Gauge::Update(float currentGaugeValue) 
{
    // 割合を計算
    float ratio = currentGaugeValue / MAX_GAUGE;

    // ゲージの描画割合を更新
    m_gauge->SetRenderRatio(ratio);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Gauge::Render()
{
    m_base->Render();
    m_gauge->Render();
    m_frame->Render();
}



/**
 * @brief 追加処理
 *
 * @param[in] position 座標
 * @param[in] scale    スケール
 * @param[in] anchor
 *
 * @return なし
 */
void Gauge::Add( 
    DirectX::SimpleMath::Vector2 position, 
    DirectX::SimpleMath::Vector2 scale, 
    ANCHOR anchor)
{
    // ベース
    m_base = std::make_unique<UserInterface>();
    m_base->Create(
        m_pDR
        , L"Resources/Textures/frame_base.png"
        , position
        , scale
        , anchor
    );
    m_base->SetWindowSize(m_windowWidth, m_windowHeight);

    // ゲージ
    m_gauge = std::make_unique<UserInterface>();
    m_gauge->Create(
        m_pDR
        , L"Resources/Textures/frame_back.png"
        , position
        , scale
        , anchor
    );
    m_gauge->SetWindowSize(m_windowWidth, m_windowHeight);

    // フレーム
    m_frame = std::make_unique<UserInterface>();
    m_frame->Create(
        m_pDR
        , L"Resources/Textures/frame.png"
        , position
        , scale
        , anchor
    );
    m_frame->SetWindowSize(m_windowWidth, m_windowHeight);
}
