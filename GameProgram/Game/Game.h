//
// Game.h
//

#pragma once

#include "ThirdParty/DeviceResources.h"
#include "ThirdParty/StepTimer.h"
#include "ThirdParty/RenderTexture.h"

#include "Library/SceneManager.h"
#include "Library/UserResources.h"
#include "Library/ResourceManager.h"
#include "Library/TransitionMask.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
private:
    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    BOOL m_fullscreen;

    // キーボードステートトラッカー
    DirectX::Keyboard::KeyboardStateTracker m_keyboardTracker;

    std::unique_ptr<DirectX::CommonStates> m_states;

    //トランジションマスクのポインタ
    std::unique_ptr<TransitionMask> m_transitionMask;

    //ノーマルマップ用のテクスチャのポインタ(仮)
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_NormalMap_texture;

    // 画面遷移テクスチャのポインタ
    std::unique_ptr<DX::RenderTexture> m_transitionTexture;

    std::unique_ptr<UserResources> m_userResources;

    // リソース管理
    std::unique_ptr<ResourceManager> m_resourceManager;

    std::unique_ptr<SceneManager<UserResources>> m_sceneManager;

    // オーディオ
    std::unique_ptr<DirectX::AudioEngine> m_audioEngine;

public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize(int& width, int& height) const noexcept;


    // 画面モードを設定する関数（TRUE：フルスクリーン）
    void SetFullscreenState(BOOL value)
    {
        m_fullscreen = value;
        m_deviceResources->GetSwapChain()->SetFullscreenState(m_fullscreen, nullptr);
        if (value) m_deviceResources->CreateWindowSizeDependentResources();
    }


private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

};
