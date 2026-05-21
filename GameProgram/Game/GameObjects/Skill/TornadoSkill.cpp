/**
 * @file   TornadoSkill.cpp
 *
 * @brief  竜巻スキルに関するソースファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TornadoSkill.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
TornadoSkill::TornadoSkill(
    ID3D11DeviceContext1* context,
    ResourceManager* resManager,
    DirectX::CommonStates* states)
    : Skill{ CONSUMPTION, context, resManager, states, L"tornado"}
	, m_timer{ 0.0f }
	, m_currentRadius{ 0.0f }
	, m_centerPos{ nullptr }
{
	// モデルの読み込み
    SetModel(resManager->GetModel(L"tornado"));
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void TornadoSkill::Update(float elapsedTime)  
{  
   // 発動していない、または親の座標が不明な場合は何もしない  
   if (!m_isActive || m_centerPos == nullptr) return;  

   // 現在の親の座標を確定させる
   DirectX::SimpleMath::Vector3 currentCenter = *m_centerPos;

   m_timer += elapsedTime;  
   m_currentRadius = m_timer * RADIUS_GROWTH_RATE;  

   // 竜巻の終了判定  
   if (m_currentRadius >= MAX_RADIUS)  
   {  
       m_isActive = false;  
       m_timer = 0.0f;  
       m_hitBoxes.clear();   
       return;  
   }  

   // 当たり判定の設定  
   m_hitBoxes.clear();  
   AttackHitBox box;  
   box.shape = HitShape::Sphere;  
   box.position = currentCenter; 
   box.extents = DirectX::SimpleMath::Vector3(m_currentRadius);  
   box.rotation = DirectX::SimpleMath::Quaternion::Identity;  
   box.pushForce = DirectX::SimpleMath::Vector3::Zero; 
   box.hit = false;  

   m_hitBoxes.push_back(box);  
}



/**
 * @brief 描画処理
 *
 * @param[in] context コンテキスト
 * @param[in] view    ビュー行列
 * @param[in] proj    プロジェクション行列
 *
 * @return なし
 */
 /**
  * @brief 描画処理
  */
void TornadoSkill::Render(
    ID3D11DeviceContext1* context,
    DirectX::SimpleMath::Matrix view,
    DirectX::SimpleMath::Matrix proj)
{
    if (!m_isActive || m_pModel == nullptr || m_centerPos == nullptr) return;

    // 行列の計算
    float rotationSpeed = m_timer * ROTATION_SPEED;
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_currentRadius) *
        DirectX::SimpleMath::Matrix::CreateRotationY(rotationSpeed) *
        DirectX::SimpleMath::Matrix::CreateTranslation(*m_centerPos);

    // エフェクト更新
    m_pModel->UpdateEffects([&](DirectX::DX11::IEffect* effect)
        {
            auto basicEffect = dynamic_cast<DirectX::DX11::BasicEffect*>(effect);
            if (basicEffect)
            {
                basicEffect->SetEmissiveColor(DirectX::Colors::AntiqueWhite);
                basicEffect->SetAlpha(0.5f);
            }
        });

    // ステート設定
    context->OMSetBlendState(m_pCommonStates->AlphaBlend(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_pCommonStates->DepthRead(), 0);
    context->RSSetState(m_pCommonStates->CullNone());

    // 描画
    m_pModel->Draw(context, *m_pCommonStates, world, view, proj);

    // ステートを戻す
    context->OMSetBlendState(m_pCommonStates->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_pCommonStates->DepthDefault(), 0);
    context->RSSetState(m_pCommonStates->CullCounterClockwise());
}



/**
 * @brief スキル発動できたかどうか
 *
 * @param[in] pos 発動位置
 * @param[in] dir 発動方向
 *
 * @return true : 発動できた
 *	       false: 発動できなかった
 */
bool TornadoSkill::Execute(
    const DirectX::SimpleMath::Vector3* pParentPos,
    const DirectX::SimpleMath::Vector3& dir)
{
    UNREFERENCED_PARAMETER(dir);

    if(m_isActive) return false;

    m_isActive = true;
    m_centerPos = pParentPos;
    m_timer = 0.0f;
    m_currentRadius = 0.0f;

    // 効果音の再生
    PlaySkillSound();
    return true;
}
