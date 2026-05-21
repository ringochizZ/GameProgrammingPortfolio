/**
 * @file   TransitionMask.cpp
 *
 * @brief  画面切り替え用のマスク表示に関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.04.09
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"

#include "TransitionMask.h"
#include "../ThirdParty/ReadData.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] device
 * @param[in] context
 * @param[in] interval
 */
TransitionMask::TransitionMask(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	float interval
)
	: m_interval{ interval }
	, m_rate{ 0.0f }
	, m_open{ true }
	, m_request{ TransitionMask::CreateMaskRequest::NONE }
{
	// スプライトバッチの作成
	m_spriteBatch=std::make_unique<SpriteBatch>(context);

	// ピクセルシェーダの作成
	std::vector<uint8_t> ps_fade = DX::ReadData(L"Resources/Shaders/FadePS.cso");
	DX::ThrowIfFailed(
		device->CreatePixelShader(ps_fade.data(), ps_fade.size(), nullptr, m_PS_Fade.ReleaseAndGetAddressOf())
	);

	// マスク用テクスチャの作成
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(
			device,
			L"Resources/Textures/FadeMask.dds",
			nullptr,
			m_maskTexture.ReleaseAndGetAddressOf()
		)
	);

	
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ConstantBuffer));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DX::ThrowIfFailed(
		device->CreateBuffer(&bufferDesc,nullptr,m_constantBuffer.ReleaseAndGetAddressOf())
	);

}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime
 *
 * @return なし
 */
void TransitionMask::Update(float elapsedTime)
{
	if (m_open)
	{
		// オープン
		// (減算量をインターバルで割る＝インターバル分の時間にする)
		// 0以下になったら0にする

		m_rate -= elapsedTime / m_interval;
		
		if (m_rate < 0.0f) 
		{
			m_rate = 0.0f;
		}
	}
	else
	{
		// クローズ
		// (加算量をインターバルで割る＝インターバル分の時間にする)
		// 1以上になったら1にする

		m_rate += elapsedTime / m_interval;

		if (m_rate > 1.0f)
		{
			m_rate = 1.0f;
		}
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] context  コンテキスト
 * @param[in] states　 共通ステート
 * @param[in] texture　描画するテクスチャ
 * @param[in] rect　　 描画する矩形
 *
 * @return なし
 */
void TransitionMask::Draw(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	ID3D11ShaderResourceView* texture,
	const RECT& rect
)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	static_cast<ConstantBuffer*>(mappedResource.pData)->rate = m_rate;
	context->Unmap(m_constantBuffer.Get(), 0);

	m_spriteBatch->Begin(SpriteSortMode_Immediate, states->NonPremultiplied(),
		nullptr, nullptr, nullptr, [&]()
		{
			// 定数バッファ
			ID3D11Buffer* cbuf = { m_constantBuffer.Get() };
			context->PSSetConstantBuffers(1, 1, &cbuf);

			// マスク用テクスチャ
			context->PSSetShaderResources(1, 1, m_maskTexture.GetAddressOf());

			// ピクセルシェーダー
			context->PSSetShader(m_PS_Fade.Get(), nullptr, 0);
		}
	);

	// マスクの描画
	m_spriteBatch->Draw(texture, rect);

	m_spriteBatch->End();
}



/**
 * @brief マスクをオープンする処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TransitionMask::Open()
{
	m_open = true;
	m_rate = 1.0f;
}



/**
 * @brief マスクをクローズする処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TransitionMask::Close()
{
	m_open = false;
	m_rate = 0.0f;
}
