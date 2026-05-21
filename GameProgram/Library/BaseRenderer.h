/**
 * @file   BaseRenderer.h
 *
 * @brief  描画オブジェクトにおけるシェーダーの一括管理および
 *         共通描画リソースの保持に関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "ThirdParty/DeviceResources.h"

#include <vector>

// 共通の定数バッファ構造体
struct CommonConstBuffer 
{
    DirectX::SimpleMath::Matrix  matWorld;
    DirectX::SimpleMath::Matrix  matView;
    DirectX::SimpleMath::Matrix  matProj;
    DirectX::SimpleMath::Vector4 Diffuse;
    DirectX::SimpleMath::Vector4 time;
};

// クラスの定義 ===============================================================
/**
 * @brief 描画関連
 */
class BaseRenderer 
{
// 変数の宣言 --------------------------------------------------------------
protected:
    DX::DeviceResources* m_pDR;

    // COMオブジェクト群
    Microsoft::WRL::ComPtr<ID3D11InputLayout>      m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>     m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>   m_geometryShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>      m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer>           m_CBuffer;

    // テクスチャ配列
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textures;

    //	プリミティブバッチ
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
    //	コモンステート
    std::unique_ptr<DirectX::CommonStates> m_states;

    // インプットレイアウト
    static const std::vector<D3D11_INPUT_ELEMENT_DESC> DEFAULT_INPUT_LAYOUT;

// メンバ関数の宣言 --------------------------------------------------------
public:
    BaseRenderer()
        : m_pDR{ nullptr }
    {
    }
    virtual ~BaseRenderer() = default;

// 操作
public:
    // 生成処理
    virtual void Create(
        DX::DeviceResources* pDR,
        const wchar_t* vsPath,
        const wchar_t* gsPath,
        const wchar_t* psPath,
        const std::vector<D3D11_INPUT_ELEMENT_DESC>& layoutDesc);

    // テクスチャ読み込み
    void LoadTexture(const wchar_t* path);

    // 描画処理
    virtual void Render() = 0;

};
