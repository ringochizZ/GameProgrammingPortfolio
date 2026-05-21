/**
 * @file   ResultScene.h
 *
 * @brief  リザルトシーンに関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// 多重インクルードの防止 ====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "Library/SceneManager.h"
#include "Library/UserResources.h"
#include "Library/ButtonManager.h"

// クラスの定義 ===============================================================
/**
 * @brief リザルトシーン
 */
class ResultScene : public Scene<UserResources>
{
// 構造体の宣言 -----------------------------------------------------
private:
	/**
	* @brief クラッカーのパーティクル
	*/
	struct Particle
	{
		DirectX::SimpleMath::Vector2 position;
		DirectX::SimpleMath::Vector2 velocity;
		DirectX::XMVECTOR color = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
		float rotation = 0.0f;
		float rotationSpeed = 0.0f;
		float scale = 1.0f;
		float lifeTime = 0.0f;
		float timer = 0.0f;
	};

// 定数の宣言 --------------------------------------------------------------
private:
	// パーティクル
	static constexpr float PARTICLE_SIZE      = 38.0f;				  // パーティクルの大きさ
	static constexpr int PARTICLE_SPAWN_COUNT = 150;                  // 密度
	static constexpr int PARTICLE_COLOR_COUNT = 7;                    // 色の種類

	static constexpr float PARTICLE_AIR_RESISTANCE         = 0.985f;  // 空気抵抗
	static constexpr float PARTICLE_GRAVITY                = 250.0f;  // 重力
	static constexpr float PARTICLE_TIMER_SPEED            = 4.0f;    // ゆらゆら用のタイマー進行速度
	static constexpr float PARTICLE_SWAY_VELOCITY_Y_THRESH = 200.0f;  // 揺れを大きくするY速度のしきい値
	static constexpr float PARTICLE_SWAY_POWER_HIGH        = 2.0f;    // 速度が遅いときの揺れ幅
	static constexpr float PARTICLE_SWAY_POWER_LOW         = 0.5f;    // 速度が速いときの揺れ幅

	static constexpr float PARTICLE_LOOP_Y_THRESH = 800.0f;           // ループするY座標の下限
	static constexpr float PARTICLE_LOOP_RESET_Y  = -100.0f;          // ループ後のY座標初期位置
	static constexpr float PARTICLE_LOOP_VY_BASE  = 150.0f;           // ループ後のY速度ベース値
	static constexpr float PARTICLE_LOOP_VY_RAND  = 200.0f;           // ループ後のY速度ランダム幅
	static constexpr float PARTICLE_LOOP_VX_MULT  = 100.0f;           // ループ後のX速度ランダム幅

	static constexpr float PARTICLE_SPAWN_START_Y        = 750.0f;    // 発射開始Y座標
	static constexpr float PARTICLE_SPAWN_VX_MULT        = 600.0f;    // 発射X速度のランダム幅
	static constexpr float PARTICLE_SPAWN_VY_BASE        = -1800.0f;  // 発射Y速度ベース値
	static constexpr float PARTICLE_SPAWN_VY_RAND        = 2000.0f;   // 発射Y速度ランダム幅
	static constexpr float PARTICLE_SPAWN_ROT_SPEED_MULT = 20.0f;     // 回転速度のランダム幅
	static constexpr float PARTICLE_SPAWN_SCALE_BASE     = 0.2f;      // スケールベース値
	static constexpr float PARTICLE_SPAWN_SCALE_RAND     = 0.4f;      // スケールランダム幅

	// WINアニメーション
	static constexpr float WIN_BASE_Y           = 80.0f;
	static constexpr float WIN_BOUNCE_AMPLITUDE = 30.0f;
	static constexpr float WIN_BOUNCE_FREQ      = 3.0f;
	static constexpr float WIN_BOUNCE_DURATION  = 1.0f;

	// キャラクター
	static constexpr int CHRACTER_NUM          = 4;
	static constexpr float CHARACTER_ROT_SPEED = DirectX::XM_PI * 0.5f;

	// 画面
	static constexpr int   SCREEN_WIDTH  = 1280;

	// 射影行列
	static constexpr float FOV_Y_DEGREES =  45.0f;
	static constexpr float NEAR_CLIP     =   0.1f;
	static constexpr float FAR_CLIP      = 100.0f;

	// カメラ
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_EYE = 
		DirectX::SimpleMath::Vector3(0.0f, 1.0f, 5.0f);
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_TARGET =
		DirectX::SimpleMath::Vector3(0.0f, 2.0f, 0.0f);
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_UP =
		DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

	// テクスチャ
	static constexpr float TEXTURE_SCALE      = 0.5f;   // 大きさ
	static constexpr float WIN_TEX_POS_X      = 150.f;  // 「WIN」X座標
	static constexpr float BASE_Y =  600.0f;  // Y座標
	static constexpr float MIN_X  =  240.0f;  // 最小X座標
	static constexpr float MAX_X  = 1040.0f;  // 最大X座標

	// 冠
	static constexpr float CROWN_ROT_SPEED = -20.0f;   // 回転速度
	static constexpr float CROWN_POS_X     =   2.5f;   // X座標
	static constexpr float CROWN_POS_Y     =   4.0f;   // Y座標
	static constexpr float CROWN_POS_Z     =  -5.0f;   // Z座標

// データメンバの宣言 -----------------------------------------------
private:
	// クラッカーのパーティクル
	std::vector<Particle> m_particles;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_particleTexture;

	//  共通ステート
	std::unique_ptr<DirectX::CommonStates> m_states;

	// ビュー行列
	DirectX::SimpleMath::Matrix m_view;

	// 射影行列
	DirectX::SimpleMath::Matrix m_proj;

	// 回転
	float m_characterRot;
	
	float m_winPosY; // WIN文字のY座標

	std::unique_ptr<ButtonManager> m_buttonManager;			// メニュー

	// モデル
	DirectX::Model* m_character;
	DirectX::Model* m_enemy;
	DirectX::Model* m_crown;

	DirectX::Keyboard::State m_prevKeyboard;

	// テクスチャ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_winTexture;

	float m_winAnimationTime;   // アニメーション経過時間
	bool m_isWinBounceFinished; // アニメーションが終わったかどうか

	// 音関連
	DirectX::SoundEffect* m_winSound;
	DirectX::SoundEffect* m_selectSound;

// メンバ関数の宣言 -------------------------------------------------
public:
	ResultScene();
	~ResultScene();

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

private:
	// パーティクルの生成処理
	void SpawnParticles();

	// パーティクルの更新処理
	void UpdateParticles(float elapsedTime);

	// WIN文字のアニメーション更新
	void UpdateWinAnimation(float elapsedTime);

};
