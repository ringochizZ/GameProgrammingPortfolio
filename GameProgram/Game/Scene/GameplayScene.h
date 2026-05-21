/**
 * @file   GameplayScene.h
 *
 * @brief  ゲームプレイシーンに関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// 多重インクルードの防止 ====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "../../library/UserResources.h"
#include "../../library/SceneManager.h"

#include "../GameObjects/FloorPrimitive.h"
#include "../GameObjects/Character.h"
#include "../GameObjects/Controller/IControll.h"
#include "../GameObjects/Background/Background.h"
#include "../GameObjects/Skill/Gauge.h"
#include "../Utilities/Particle.h"

// クラスの定義 ===============================================================
/**
 * @brief ゲームプレイシーン
 */
class GameplayScene : public Scene<UserResources>
{
// 定数の宣言 -------------------------------------------------------------
private:
	static constexpr int   RECT_RIGHT        = 1280;	      // 右
	static constexpr int   RECT_BOTTOM       = 720;		      // 下
	static constexpr int   CHARACTER_NUM     = 4;             // キャラクター数
	static constexpr int   COUNTDOWN_TIME    = 3;             // カウントダウンの時間
	static constexpr int   SKILL_NUM         = 3;             // スキル数
	static constexpr int   MAX_COLLISION_NUM = CHARACTER_NUM; // 登録できるコリジョンの最大数
	static constexpr int   FINISH_WAIT_FRAME = 144;			  // 勝敗演出
	static constexpr float START_POS         = 8.0f;          // 初期位置
	static constexpr float BGM_VOLUME        = 0.5f;	      // BGMの音量
	static constexpr float COLLISION_SCALE   = 0.2f;          // コリジョン描画
	
	// 床
	static constexpr float START_FLOOR_SIZE = 10.0f;
	static constexpr float MIN_FLOOR_SIZE   =  1.0f;
	static constexpr float SHRINK_SPEED     = 0.05f;

	// カメラ
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_EYE =
		DirectX::SimpleMath::Vector3(0.0f, 20.0f, 20.0f);
	static constexpr DirectX::SimpleMath::Vector3 CAMERA_UP =
		DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

	// 射影行列
	static constexpr float FOV_Y_DEGREES =  45.0f;
	static constexpr float NEAR_CLIP     =   0.1f;
	static constexpr float FAR_CLIP      = 100.0f;

	// デバッグ表示
	static constexpr float DEBUG_TEXT_POS_X = 10.0f;
	static constexpr float DEBUG_TEXT_POS_Y = 10.0f;

	// テクスチャ
	static constexpr int   ICON_POS_X          = 350;    // アイコンのX座標
	static constexpr int   ICON_POS_Y          = 30;     // アイコンのY座標
	static constexpr int   COUNTDOWN_SIZE_X    = 200;    // カウントダウンの画像サイズX
	static constexpr int   COUNTDOWN_SIZE_Y    = 300;    // カウントダウンの画像サイズY
	static constexpr float TEXTURE_SCALE       = 0.5f;   // スケール
	static constexpr float COUNTDOWN_TEX_SCALE = 1.5f;   // カウントダウンの表示スケール
	static constexpr float ICON_OFFSET         = 120.0f; // アイコン同士の間隔
	static constexpr float READY_TEX           = 1.0f;   // 使えるとき
	static constexpr float NOT_READY_TEX       = 0.5f;   // 使えないとき
	static constexpr float YOU_TEX_POS_X       = 380.0f; // 「YOU」のX座標
	static constexpr float YOU_TEX_POS_Y       = 60.0f;  // 「YOU」のY座標

	// 試合終了
	static constexpr float FINISH_AREA_POS_X =  5.0f; // 終了エリアのX座標
	static constexpr float FINISH_AREA_POS_Z = 15.0f; // 終了エリアのX座標

// データメンバの宣言 -----------------------------------------------
private:
	//  共通ステート
	std::unique_ptr<DirectX::CommonStates> m_states;

	// ビュー行列
	DirectX::SimpleMath::Matrix m_view;

	// 射影行列
	DirectX::SimpleMath::Matrix m_proj;

	bool m_winnerSet;	      // 勝者が決まったか
	int  m_finishTimer;       // 終了演出用のタイマー
	float m_currentFloorSize; // 現在の床のサイズ

	int m_lastCount;		  // 前回のカウント数を保持
	float m_countdownTimer;   // カウントダウン用タイマー
	bool m_countdownFinished; // カウントダウンが終了したかどうか

	// スキルアイコンのテクスチャ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	ID3D11ShaderResourceView* m_skillIcons[SKILL_NUM];
	ID3D11ShaderResourceView* m_countdownTex[COUNTDOWN_TIME];

	std::unique_ptr<FloorPrimitive> m_floor;	//< 床
	std::unique_ptr<Background> m_background;	//< 背景
	std::unique_ptr<Gauge> m_gauge;				//< ゲージ
	std::unique_ptr<Particle> m_particle;		//< パーティクル

	std::vector<std::unique_ptr<Character>> m_characters;	//< キャラクター
	std::vector<std::unique_ptr<IControll>> m_controllers;	//< コントローラー

// メンバ関数の宣言 -------------------------------------------------
public:
	GameplayScene();
	~GameplayScene();

// 操作
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

	// 勝者を決定する関数
	void DetermineWinner();

private:
	// カウントダウン処理
	void HandleCountdown(float elapsedTime);

	// スキルアイコンの描画
	void RenderSkillIcons();

	// カウントダウンの描画
	void RenderCountdown();

	// 試合を終了する処理
	void FinishMatch();

};
