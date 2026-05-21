/**
 * @file   SceneManager.h
 *
 * @brief  シーン管理に関するヘッダファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.04.17
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#define ESC_QUIT_ENABLE
#ifdef ESC_QUIT_ENABLE

#include "Keyboard.h"
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <cassert>
#include <unordered_map>
#endif

template <typename T>
class SceneManager;

// クラスの定義 ===============================================================
/**
 * @brief シーンの基底クラス
 */
template <typename T>
class Scene
{
// 変数の宣言 --------------------------------------------------------------
private:
	SceneManager<T>* m_sceneManager;

// メンバ関数の宣言 --------------------------------------------------------
public:
	Scene() : m_sceneManager(nullptr) {}
	virtual ~Scene() = default;

// 操作
public:
	// 初期化処理
	virtual void Initialize() = 0;

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render() = 0;

	// 終了処理
	virtual void Finalize() = 0;

	virtual void CreateDeviceDependentResources() {}
	virtual void CreateWindowSizeDependentResources() {}
	virtual void OnDeviceLost() {}

	void SetSceneManager(SceneManager<T>* sceneManager) { m_sceneManager = sceneManager; }

	template <typename U>
	void ChangeScene();

	template <typename U, typename V>
	void ChangeScene();

// 取得/設定
	T* GetUserResources();
	SceneManager<T>* GetSceneManager() { return m_sceneManager; }
};

// ロード画面の基底クラス
template <typename T>
using LoadingScreen = Scene<T>;

/**
 * @brief シーンの管理
 */
template <typename T>
class SceneManager
{
// エイリアス宣言 ----------------------------------------------------------
public:
	using SharedData = std::unordered_map<std::string, std::string>;

// 変数の宣言 --------------------------------------------------------------
private:
	T* m_userResources;

	std::unique_ptr<Scene<T>> m_scene;
	std::unique_ptr<Scene<T>> m_nextScene;
	std::unique_ptr<LoadingScreen<T>> m_loadingScreen;

	std::thread m_loadingThread;
	std::mutex m_loadingMutex;
	bool m_isLoading;

	// 共有データ
	SharedData m_sharedData;

// メンバ関数の宣言 --------------------------------------------------------
public:
	void DeleteScene();

/**
 * @brief コンストラクタ
 *
 * @param[in] userResources
 */
public:
	SceneManager(T* userResources = nullptr)
		: m_userResources{ userResources }
		, m_scene{}
		, m_nextScene{}
		, m_loadingScreen{}
		, m_isLoading{ false }
		, m_sharedData{}
	{
	}

/**
 * @brief デストラクタ
 */
	virtual ~SceneManager()
	{
		DeleteScene();
		if (m_loadingThread.joinable())
		{
			m_loadingThread.join();
		}
	}

// 操作
public:
	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();
	virtual void OnDeviceLost();

	template <typename U>
	void SetScene();

	template <typename U>
	bool RequestSceneChange();

	template <typename U, typename V>
	bool RequestSceneChange();

	void PrepareNextScene(std::function<std::unique_ptr<Scene<T>>()> sceneFactory);
	bool IsLoading();

// 取得/設定
public:
	void SetUserResources(T* userResources) { m_userResources = userResources; }
	T* GetUserResources() { return m_userResources; }

	// 共有データの取得
	std::string GetSharedData(const std::string& key) const
	{
		if (m_sharedData.count(key) == 0)
		{
			assert(false && "指定されたキーは登録されていません。");
			return std::string{};
		}

		return m_sharedData.at(key);
	}

	// 共有データの設定
	void SetSharedData(const std::string& key, const std::string& value)
	{
		m_sharedData[key] = value;
	}

};

// メンバ関数の定義 ===========================================================
/**
 * @brief シーンの切り替え処理
 *
 * @param[in] なし
 *
 * @return なし
 */
template <typename T>
template <typename U>
void Scene<T>::ChangeScene()
{
	m_sceneManager->RequestSceneChange<U>();
}

template <typename T>
template <typename U, typename V>
void Scene<T>::ChangeScene()
{
	m_sceneManager->RequestSceneChange<U, V>();
}



/**
 * @brief UserResourcesの取得処理
 *
 * @param[in] なし
 *
 * @return なし
 */
template <typename T>
T* Scene<T>::GetUserResources()
{
	assert(m_sceneManager);
	return m_sceneManager->GetUserResources();
}



/**
 * @brief シーンの設定処理
 *
 * @param[in] なし
 *
 * @return なし
 */
template <typename T>
template <typename U>
void SceneManager<T>::SetScene()
{
	assert(m_scene == nullptr);
	RequestSceneChange<U>();
}



/**
 * @brief シーンのリクエスト処理(ロードなし)
 *
 * @param[in] なし
 *
 * @return なし
 */
template <typename T>
template <typename U>
bool SceneManager<T>::RequestSceneChange()
{
	if (!m_nextScene)
	{
		PrepareNextScene([]() { return std::make_unique<U>(); });
		return true;
	}
	return false;
}



/**
 * @brief シーンのリクエスト処理(ロードあり)
 *
 * @param[in] なし
 *
 * @return なし
 */
template <typename T>
template <typename U, typename V>
bool SceneManager<T>::RequestSceneChange()
{
	if (RequestSceneChange<U>())
	{
		m_loadingScreen = std::make_unique<V>();
		m_loadingScreen->SetSceneManager(this);
		m_loadingScreen->Initialize();
	}
	return false;
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime
 *
 * @return なし
 */
template <typename T>
void SceneManager<T>::Update(float elapsedTime)
{
#ifdef ESC_QUIT_ENABLE
	auto kb = DirectX::Keyboard::Get().GetState();
	if (kb.Escape) PostQuitMessage(0);
#endif

	if (!IsLoading() && m_nextScene)
	{
		DeleteScene();
		assert(m_scene == nullptr);
		m_scene = std::move(m_nextScene);

		if (m_loadingScreen)
		{
			m_loadingScreen->Finalize();
			m_loadingScreen.reset();
		}
	}

	if (m_loadingScreen)
	{
		m_loadingScreen->Update(elapsedTime);
		return;
	}

	if (m_scene) m_scene->Update(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
template <typename T>
void SceneManager<T>::Render()
{
	if (m_loadingScreen)
	{
		m_loadingScreen->Render();
		return;
	}
	if (m_scene) m_scene->Render();
}



/**
 * @brief リソース生成（デバイス依存）処理
 *
 * @param[in] なし
 *
 * @return なし
 */
template <typename T>
void SceneManager<T>::CreateDeviceDependentResources()
{
	if (m_scene) m_scene->CreateDeviceDependentResources();
}



/**
 * @brief リソース生成（ウィンドウサイズ依存）処理
 *
 * @param[in] なし
 *
 * @return なし
 */
template <typename T>
void SceneManager<T>::CreateWindowSizeDependentResources()
{
	if (m_scene) m_scene->CreateWindowSizeDependentResources();
}



/**
 * @brief デバイスロスト処理
 *
 * @param[in] なし
 *
 * @return なし
 */
template <typename T>
void SceneManager<T>::OnDeviceLost()
{
	if (m_scene) m_scene->OnDeviceLost();
}



/**
 * @brief シーンの削除処理
 *
 * @param[in] なし
 *
 * @return なし
 */
template <typename T>
void SceneManager<T>::DeleteScene()
{
	if (m_scene)
	{
		m_scene->Finalize();
		m_scene.reset();
	}
}



/**
 * @brief シーンの準備処理
 *
 * @param[in] なし
 *
 * @return なし
 */
template <typename T>
void SceneManager<T>::PrepareNextScene(std::function<std::unique_ptr<Scene<T>>()> sceneFactory)
{
	if (m_loadingThread.joinable())
	{
		m_loadingThread.join();
	}

	m_isLoading = true;
	m_loadingThread = std::thread{
		[=]()
		{
			m_nextScene = sceneFactory();
			m_nextScene->SetSceneManager(this);
			m_nextScene->Initialize();

			std::lock_guard<std::mutex> lock(m_loadingMutex);
			m_isLoading = false;
		}
	};
}



/**
 * @brief ローディング中かどうか
 *
 * @param[in] なし
 *
 * @return true  ローディング中
 * @return false ローディング中ではない
 */
template <typename T>
bool SceneManager<T>::IsLoading()
{
	std::lock_guard<std::mutex> lock(m_loadingMutex);
	return m_isLoading;
}
