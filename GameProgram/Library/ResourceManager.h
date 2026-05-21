/**
 * @file   Resourcemanager.h
 *
 * @brief  リソースの管理に関するヘッダファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.01.31
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include <unordered_map>
#include <string>
#include <wrl/client.h>
#include <memory>

#include <d3d11.h>
#include <Audio.h>
#include <WICTextureLoader.h>

#include "Library/BinaryFile.h"

// クラスの定義 ===============================================================
/**
 * @brief リソース管理
 */
class ResourceManager
{
// 構造体の定義 -----------------------------------------------------
/**
 * @brief シェーダーに渡すデータの構造体
 */
public:
    struct ConstBuffer
    {
        DirectX::SimpleMath::Matrix matWorld;
        DirectX::SimpleMath::Matrix matView;
        DirectX::SimpleMath::Matrix matProj;
        DirectX::SimpleMath::Vector4 Diffuse;
        DirectX::SimpleMath::Vector4 time;
    };

// 変数の宣言 --------------------------------------------------------------
private:
    ID3D11Device* m_device = nullptr;
    DirectX::AudioEngine* m_audio = nullptr;

    std::unique_ptr<DirectX::EffectFactory> m_fxFactory;

	// テクスチャ
    std::unordered_map<
        std::wstring,
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>
    > m_textures;

	// サウンド
    std::unordered_map<
        std::wstring,
        std::unique_ptr<DirectX::SoundEffect>
    > m_sounds;

	// BGM
    std::unordered_map<
        std::wstring,
        std::unique_ptr<DirectX::SoundEffect>
    > m_bgmSources;

    std::unordered_map<
        std::wstring,
        std::unique_ptr<DirectX::SoundEffectInstance>
    > m_bgms;

	// モデル
    std::unordered_map<
        std::wstring,
        std::unique_ptr<DirectX::Model>
    > m_models;

	// 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer>         m_constantBuffer;

// メンバ関数の宣言 -------------------------------------------------
public:
    ResourceManager() = default;
    ~ResourceManager() = default;
    
// 操作
public:
    // 初期化処理
    void Initialize(
        ID3D11Device* device,
        DirectX::AudioEngine* audio
    );

// 取得/設定
public:
    // テクスチャの取得
    ID3D11ShaderResourceView* GetTexture(const std::wstring& name)
    {
        auto it = m_textures.find(name);
        if (it != m_textures.end())
        {
            return it->second.Get();
        }
        return nullptr;
    }

    // サウンドの取得
    DirectX::SoundEffect* GetSound(const std::wstring& name)
    {
        auto it = m_sounds.find(name);
        if (it != m_sounds.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    // BGMの取得
    DirectX::SoundEffectInstance* GetBGM(const std::wstring& name)
    {
        auto it = m_bgms.find(name);
        if (it != m_bgms.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    // モデルの取得
    DirectX::Model* GetModel(const std::wstring& name)
    {
        auto it = m_models.find(name);
        if (it != m_models.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

};
