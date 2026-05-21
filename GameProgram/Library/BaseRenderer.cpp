/**
 * @file   BaseRenderer.cpp
 *
 * @brief  描画オブジェクトにおけるシェーダーの一括管理および
 *         共通描画リソースの保持に関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.05.21
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "BaseRenderer.h"

#include "Library/BinaryFile.h"

// インプットレイアウト
const std::vector<D3D11_INPUT_ELEMENT_DESC> BaseRenderer::DEFAULT_INPUT_LAYOUT =
{
    { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,
                                           0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,
        sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, sizeof(DirectX::SimpleMath::Vector3) +
        sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// メンバ関数の定義 ===========================================================
/**
 * @brief 生成処理
 *
 * @param[in] pDR DeviceResourcesのポインタ
 * @param[in] vsPath 頂点シェーダのパス  
 * @param[in] gsPath ジオメトリシェーダのパス
 * @param[in] psPath ピクセルシェーダのパス
 * @param[in] layoutDesc 入力レイアウトの説明
 *
 * @return なし
 */
void BaseRenderer::Create(
    DX::DeviceResources* pDR,
    const wchar_t* vsPath,
    const wchar_t* gsPath,
    const wchar_t* psPath,
    const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc)
{
    m_pDR = pDR;
    auto device = m_pDR->GetD3DDevice();

    // シェーダファイルの読み込みと作成
    // 頂点シェーダー
    if (vsPath) 
    {
        auto VSData = BinaryFile::LoadFile(vsPath);
        device->CreateVertexShader(
            VSData->GetData(), 
            VSData->GetSize(), 
            nullptr, 
            m_vertexShader.ReleaseAndGetAddressOf()
        );

        // インプットレイアウト作成
        device->CreateInputLayout(
            layoutDesc.data(), 
            static_cast<UINT>(layoutDesc.size()),
            VSData->GetData(), 
            VSData->GetSize(), 
            m_inputLayout.ReleaseAndGetAddressOf()
        );
    }

	// ジオメトリシェーダー
    if (gsPath) 
    {
        auto GSData = BinaryFile::LoadFile(gsPath);
        device->CreateGeometryShader(
            GSData->GetData(), 
            GSData->GetSize(), 
            nullptr, 
            m_geometryShader.ReleaseAndGetAddressOf()
        );
    }

	// ピクセルシェーダー
    if (psPath) 
    {
        auto PSData = BinaryFile::LoadFile(psPath);
        device->CreatePixelShader(
            PSData->GetData(), 
            PSData->GetSize(), 
            nullptr, 
            m_pixelShader.ReleaseAndGetAddressOf()
        );
    }

    // 共通コンスタントバッファの作成
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CommonConstBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    device->CreateBuffer(&bd, nullptr, m_CBuffer.ReleaseAndGetAddressOf());

	// コモンステートとプリミティブバッチの生成
	m_states = std::make_unique<DirectX::CommonStates>(device);
	m_batch = std::make_unique<
		DirectX::PrimitiveBatch<
		DirectX::VertexPositionColorTexture>>
		(m_pDR->GetD3DDeviceContext());
}



/**
 * @brief テクスチャの読み込み処理
 *
 * @param[in] path 相対パス
 *
 * @return なし
 */
void BaseRenderer::LoadTexture(const wchar_t* path) 
{
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
    
	// テクスチャを読み込む
    DirectX::CreateWICTextureFromFile(
        m_pDR->GetD3DDevice(), 
        path, nullptr, 
        texture.ReleaseAndGetAddressOf()
    );

	// 読み込んだテクスチャを配列に追加
    m_textures.push_back(texture);
}
