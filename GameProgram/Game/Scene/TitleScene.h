/**
 * @file   TitleScene.h
 *
 * @brief  タイトルシーンに関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.04.17
 */

 // 多重インクルードの防止 ====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Library/UserResources.h"
#include "Library/SceneManager.h"
#include "Library/ButtonManager.h"

// クラスの定義 ===============================================================
/**
 * @brief タイトルプレイシーン
 */
class TitleScene : public Scene<UserResources>
{
// 定数の宣言 -------------------------------------------------------------
private:
	static constexpr float BGM_VOLUME        =  0.5f;	// BGMの音量
	static constexpr float ROTATION_SPEED    =  0.5f;	// 回る速さ
	static constexpr float MODEL_POS_X       =  5.5f;	// モデルのX座標
	static constexpr float MODEL_POS_Y       = -3.0f;	// モデルのY座標

	// テクスチャ
	static constexpr float TITLE_POS_X     = 0.0f;    // タイトルX座標
	static constexpr float TITLE_POS_Y	   = 0.0f;    // タイトルY座標
	static constexpr float TEXTURE_SCALE   = 0.5f;    // 大きさ
	static constexpr float BASE_Y          = 600.0f;  // Y座標
	static constexpr float MIN_X           = 440.0f;  // 最小X座標
	static constexpr float MAX_X           = 840.0f;  // 最大X座標

	// カメラの位置
	static constexpr float CAMERA_POS_Z    = 10.0f;
	static constexpr float CAMERA_TARGET_Z =  0.0f;
	static constexpr float CAMERA_UP_Y     =  1.0f;

	// 射影行列
	static constexpr float FOV_Y_DEGREES =  45.0f;
	static constexpr float NEAR_CLIP     =   0.1f;
	static constexpr float FAR_CLIP      = 100.0f;

// データメンバの宣言 -----------------------------------------------
private:
	//  共通ステート
	std::unique_ptr<DirectX::CommonStates> m_states;

	// ビュー行列
	DirectX::SimpleMath::Matrix m_view;

	// 射影行列
	DirectX::SimpleMath::Matrix m_proj;

	// モデル
	DirectX::Model* m_player;
	DirectX::Model* m_enemy;

	// 座標
	DirectX::SimpleMath::Vector3 m_playerPos;
	DirectX::SimpleMath::Vector3 m_enemyPos;

	// 回転
	float m_playerRot;
	float m_enemyRot;

	DirectX::Keyboard::State m_prevKeyboard;

	// テクスチャ関連
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_titleTexture;

	// 効果音
	DirectX::SoundEffect* m_selectSound;

	// メニュー
	std::unique_ptr<ButtonManager> m_buttonManager;

	bool m_isTransitioning; // トランジション中かどうか


// メンバ関数の宣言 -------------------------------------------------------
public:	
	TitleScene();
	~TitleScene();

// 操作
public:
	// 初期化
	void Initialize() override;

	// 更新
	void Update(float elapsedTime) override;

	// 描画
	void Render() override;

	// 終了処理
	void Finalize() override;

	// デバイスに依存するリソースを作成する関数
	void CreateDeviceDependentResources() override;

	// ウインドウサイズに依存するリソースを作成する関数
	void CreateWindowSizeDependentResources() override;

	// デバイスロストした時に呼び出される関数
	void OnDeviceLost() override;

};

