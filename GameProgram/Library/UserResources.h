/**
 * @file   UserResources.h
 *
 * @brief  シーンへ渡すユーザーリソースクラス
 *
 * @author 小野島 樹里
 *
 * @date   2026.01.27
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "ThirdParty/StepTimer.h"
#include "ThirdParty/DeviceResources.h"
#include "TransitionMask.h"
#include "ResourceManager.h"

// クラスの定義 ===============================================================
/**
 * @brief 各シーンに共通する内容
 */
class UserResources
{
// 変数の宣言 --------------------------------------------------------------
private:

	// ステップタイマー
	DX::StepTimer* m_timer;

	// デバイスリソース
	DX::DeviceResources* m_deviceResources;

	// キーボードステートトラッカー
	DirectX::Keyboard::KeyboardStateTracker* m_keyboardTracker;

	// マウスステートトラッカー
	DirectX::Mouse::ButtonStateTracker* m_mouseTracker;

	// 共通ステート
	DirectX::CommonStates* m_states;

	// 画面遷移マスク
	TransitionMask* m_transitionMask;

	// オーディオ
	DirectX::AudioEngine* m_audioEngine;

	// リソース管理
	ResourceManager* m_resourcesManager;

// メンバ関数の定義 -------------------------------------------------
public:
	/**
	* @brief コンストラクタ
	*
	* @param[in] なし
	*/
	UserResources()
		: m_timer{ nullptr }
		, m_deviceResources{ nullptr }
		, m_keyboardTracker{ nullptr }
		, m_mouseTracker{ nullptr }
		, m_states{ nullptr }
		, m_transitionMask{ nullptr }
		, m_audioEngine{ nullptr }
		, m_resourcesManager{ nullptr }
	{
	}

	// ------------------------------------------ //
	// ステップタイマー	
	// ------------------------------------------ //

	// ステップタイマーを設定する関数
	void SetStepTimerStates(DX::StepTimer* timer) { m_timer = timer; }

	// ステップタイマーを取得する関数
	DX::StepTimer* GetStepTimer() { return m_timer; }

	// ------------------------------------------ //
	// デバイスリソース	
	// ------------------------------------------ //

	// デバイスリソースを設定する関数
	void SetDeviceResources(DX::DeviceResources* deviceResources) { m_deviceResources = deviceResources; }

	// デバイスリソースを取得する関数
	DX::DeviceResources* GetDeviceResources() { return m_deviceResources; }

	// ------------------------------------------ //
	// キーボードステートトラッカー	
	// ------------------------------------------ //

	// キーボードステートトラッカーを設定する関数
	void SetKeyboardStateTracker(DirectX::Keyboard::KeyboardStateTracker* tracker) { m_keyboardTracker = tracker; }

	// キーボードステートトラッカーを取得する関数
	DirectX::Keyboard::KeyboardStateTracker* GetKeyboardStateTracker() { return m_keyboardTracker; }

	// ------------------------------------------ //
	// マウスステートトラッカー	
	// ------------------------------------------ //

	// マウスステートトラッカーを設定する関数
	void SetMouseStateTracker(DirectX::Mouse::ButtonStateTracker* tracker) { m_mouseTracker = tracker; }

	// マウスステートトラッカーを取得する関数
	DirectX::Mouse::ButtonStateTracker* GetMouseStateTracker() { return m_mouseTracker; }

	// ------------------------------------------ //
	// 共通ステート	
	// ------------------------------------------ //

	// 共通ステートを設定する関数
	void SetCommonStates(DirectX::CommonStates* states) { m_states = states; }

	// 共通ステートを取得する関数
	DirectX::CommonStates* GetCommonStates() { return m_states; }

	// ------------------------------------------ //
	// 画面遷移マスク
	// ------------------------------------------ //

	// 画面遷移マスクを設定する関数
	void SetTransitionMask(TransitionMask* transitionMask) { m_transitionMask = transitionMask; }

	// 画面遷移マスクを取得する関数
	TransitionMask* GetTransitionMask() { return m_transitionMask; }


	// ------------------------------------------ //
	// オーディオ
	// ------------------------------------------ //

	// オーディオを設定する関数
	void SetAudioEngine(DirectX::AudioEngine* audio) { m_audioEngine = audio; }

	// オーディオを取得する関数
	DirectX::AudioEngine* GetAudioEngine() { return m_audioEngine; }


	// ------------------------------------------ //
	// リソース管理
	// ------------------------------------------ //

	// リソース管理を設定する関数
	void SetResourceManager(ResourceManager* manager) { m_resourcesManager = manager; }

	// リソース管理を取得する関数
	ResourceManager* GetResourceManager() { return m_resourcesManager; }

};
