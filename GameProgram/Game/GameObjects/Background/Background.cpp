/**
 * @file   Background.cpp
 *
 * @brief  背景に関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.05.21
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Background.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief 生成処理
 *
 * @param[in] pDR DeviceResourcesのポインタ
 *
 * @return なし
 */
void Background::Create(DX::DeviceResources* pDR)
{	
	// シェーダーの生成
	BaseRenderer::Create(
        pDR,
		L"Resources/Shaders/BackgroundVS.cso",
		L"Resources/Shaders/BackgroundGS.cso",
		L"Resources/Shaders/BackgroundPS.cso",
        DEFAULT_INPUT_LAYOUT
    );

	// 背景専用のテクスチャ読み込み
	LoadTexture(L"Resources/Textures/Sky.png");
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Background::Render()
{
    auto context = m_pDR->GetD3DDeviceContext();

    // 背景は常に1頂点
    DirectX::VertexPositionColorTexture vertex(
        DirectX::SimpleMath::Vector3::Zero,
        DirectX::SimpleMath::Color(1, 1, 1, 1),
        DirectX::SimpleMath::Vector2::Zero
    );

    // 定数バッファの更新
    CommonConstBuffer cb;
    cb.matWorld = DirectX::SimpleMath::Matrix::Identity;
    cb.matView = DirectX::SimpleMath::Matrix::Identity;
    cb.matProj = DirectX::SimpleMath::Matrix::Identity;
    cb.Diffuse = DirectX::SimpleMath::Vector4::One;
    cb.time = DirectX::SimpleMath::Vector4(m_time, 1, 1, 1);
    context->UpdateSubresource(m_CBuffer.Get(), 0, nullptr, &cb, 0, 0);

    // パイプライン設定
    ID3D11Buffer* cbs[] = { m_CBuffer.Get() };

    // シェーダにコンスタントバッファを渡す
    context->VSSetConstantBuffers(0, 1, cbs);
    context->GSSetConstantBuffers(0, 1, cbs);
    context->PSSetConstantBuffers(0, 1, cbs);

    context->PSSetShaderResources(0, 1, m_textures[0].GetAddressOf());
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);

	// シェーダーの登録
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    context->IASetInputLayout(m_inputLayout.Get());

    m_batch->Begin();
    m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertex, 1);
    m_batch->End();

    // シェーダの登録を解除
    context->VSSetShader(nullptr, nullptr, 0);
    context->GSSetShader(nullptr, nullptr, 0);
    context->PSSetShader(nullptr, nullptr, 0);
}
