/**
 * @file   GameplayScene.cpp
 *
 * @brief  ゲームプレイシーンに関するソースファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "GameplayScene.h"

#include "Game/Scene/ResultScene.h"
#include "Game/Utilities/CollisionManager.h"
#include "Game/GameObjects/Character.h"
#include "Game/GameObjects/Skill/SkillFactory.h"
#include "Game/GameObjects/Controller/PlayerController.h"
#include "Game/GameObjects/Controller/EnemyController.h"

#include <sstream>

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GameplayScene::GameplayScene()
    : m_winnerSet{ false }
    , m_finishTimer{ 0 }
    , m_currentFloorSize{ START_FLOOR_SIZE }
	, m_lastCount{ 0 }
	, m_countdownTimer{ COUNTDOWN_TIME }
	, m_countdownFinished{ false }
    , m_floor{ nullptr }
    , m_background{ nullptr }
	, m_gauge{ nullptr }
	, m_particle{ nullptr }
    , m_skillIcons{ nullptr, nullptr, nullptr } 
	, m_countdownTex{ nullptr, nullptr, nullptr }
{
}



/**
 * @brief デストラクタ
 */
GameplayScene::~GameplayScene()
{
}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void GameplayScene::Initialize()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();

	//========================================================
	// リソース管理
	//========================================================
	auto res = GetUserResources()->GetResourceManager();

	// BGMの取得
	auto bgm = res->GetBGM(L"gameplay");
	bgm->SetVolume(BGM_VOLUME);
	bgm->Play(true);

	// 床の作成
	m_floor = std::make_unique<FloorPrimitive>(device);
	
	// 背景の作成
	m_background = std::make_unique<Background>();
	m_background->Create(GetUserResources()->GetDeviceResources());

	// ゲージの生成と初期化
	m_gauge = std::make_unique<Gauge>();
	m_gauge->Initialize(
		GetUserResources()->GetDeviceResources(),
		RECT_RIGHT,
		RECT_BOTTOM
	);

	// パーティクルの生成と初期化
	m_particle = std::make_unique<Particle>();
	m_particle->Create(GetUserResources()->GetDeviceResources());

	// 初期位置の設定
	SimpleMath::Vector3 startPositions[CHARACTER_NUM] = 
	{
		SimpleMath::Vector3(-START_POS, 0.0f, -START_POS),
		SimpleMath::Vector3( START_POS, 0.0f, -START_POS),
		SimpleMath::Vector3(-START_POS, 0.0f,  START_POS),
		SimpleMath::Vector3( START_POS, 0.0f,  START_POS)
	};

	// スキル構成の設定
	SkillType skillLayout[CHARACTER_NUM][SKILL_NUM] =
	{
		{ SkillType::RUSH, SkillType::GUST, SkillType::TORNADO }, // 1P
		{ SkillType::RUSH, SkillType::GUST, SkillType::TORNADO }, // 2P
		{ SkillType::RUSH, SkillType::GUST, SkillType::TORNADO }, // 3P
		{ SkillType::RUSH, SkillType::GUST, SkillType::TORNADO }  // 4P
	};

	// モデル
	std::wstring modelKeys[CHARACTER_NUM] = { L"1P", L"2P", L"3P", L"4P" };

	for (int i = 0; i < CHARACTER_NUM; i++)
	{
		// キャラクターの作成
		auto character = std::make_unique<Character>();

		// モデルの設定
		auto model = GetUserResources()->GetResourceManager()->GetModel(modelKeys[i]);
		character->SetModel(model);

		character->CreateDeviceDependentResources(device);
		character->SetPosition(startPositions[i]);

		// スキルの設定
		for (int slot = 0; slot < SKILL_NUM; slot++)
		{
			character->AddSkill(SkillFactory::Create(skillLayout[i][slot], GetUserResources()));
		}

		// 配列に追加
		m_characters.push_back(std::move(character));

		// コントローラーの生成
		if (i == 0)
		{
			// 1P（プレイヤー）
			m_controllers.push_back(std::make_unique<PlayerController>());
		}
		else
		{
			// 2P以降（CPU）
			m_controllers.push_back(std::make_unique<EnemyController>());
		}
	}

	// マスクをオープンする
	auto transitionMask = GetUserResources()->GetTransitionMask();
	transitionMask->Open();
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime
 *
 * @return なし
 */
void GameplayScene::Update(float elapsedTime)
{
	// カウントダウンの処理
	if (!m_countdownFinished)
	{
		HandleCountdown(elapsedTime);
		return;
	}

	// 背景の更新処理
	m_background->Update(elapsedTime);

	// パーティクルの更新処理
	m_particle->Update(elapsedTime);

	// 床の縮小
	if (m_currentFloorSize > MIN_FLOOR_SIZE)
	{
		m_currentFloorSize -= SHRINK_SPEED * elapsedTime;
	}

	// コントローラーとキャラクターを結びつける
	for (size_t i = 0; i < m_characters.size(); ++i)
	{
		// 画面外に出ているキャラクターは更新しない
		if (!m_characters[i]->IsDead())
		{
			// 生きているキャラクターのみ
			if (m_characters[i]->IsActive())
			{
				// 周囲の状況をスキャンしてデータを取得
				PerceptionData data = ScanAround(m_characters[i].get(), m_characters);

				// コントローラーの更新
				m_controllers[i]->Controller(*m_characters[i], data, elapsedTime);

				// 当たり判定の更新
				CollisionManager::Update(m_characters, GetUserResources(), m_particle.get());
			}

			// キャラクターの更新処理
			m_characters[i]->Update(elapsedTime, m_currentFloorSize);
		}
	}

	// ゲージの更新
	m_gauge->Update(m_characters[0]->GetWindowPowerGauge());

	// 勝者の決定
	DetermineWinner();

	// プレイヤーが死んでいて、勝敗がまだ決まっていない場合
	if (m_characters[0]->IsDead() && !m_winnerSet &&
		GetUserResources()->GetKeyboardStateTracker()->pressed.Enter)
	{
		// Enterキーが押されたら試合を終了する
		FinishMatch();
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void GameplayScene::Render()
{
	// コンテキストの取得
	ID3D11DeviceContext1* context =
		GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	// ビュー行列
	DirectX::SimpleMath::Vector3 eye = CAMERA_EYE;
	SimpleMath::Vector3 target = SimpleMath::Vector3::Zero;
	SimpleMath::Vector3 up = CAMERA_UP;
	m_view = SimpleMath::Matrix::CreateLookAt(eye, target, up);

	// 背景の描画
	m_background->Render();

	// 床の描画
	m_floor->Render(context, m_view, m_proj, m_currentFloorSize);

	// キャラクターの描画
	for (auto& ch : m_characters)
	{
		// 画面内にいるキャラクターのみ
		if (!ch->IsDead())
		{
			ch->Render(context, m_view, m_proj, m_currentFloorSize);
		}
	}

	// パーティクルの描画
	m_particle->SetCameraMatrices(m_view, m_proj);
	m_particle->CreateBillboard(target, eye, up);
	m_particle->Render();

	// スキルの描画
	for (auto& ch : m_characters)
	{
		// 画面内にいるキャラクターのみ
		if (!ch->IsDead())
		{
			ch->RenderSkills(context, m_view, m_proj);
		}
	}

	// カウントダウンの描画
	if (!m_countdownFinished)
	{
		RenderCountdown();
		return;
	}

	// スキルアイコンの描画
	RenderSkillIcons();

	// ゲージの描画
	m_gauge->Render();
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void GameplayScene::Finalize()
{
}



void GameplayScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	// 共通ステートの作成
	m_states = std::make_unique<CommonStates>(device);

	std::unique_ptr<DirectX::EffectFactory> fx;
	fx = std::make_unique<EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");

	// スプライトバッチの作成
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

	// スキルアイコンの読み込み
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(
			device,
			L"Resources/Textures/RushSkill.png",
			nullptr,
			&m_skillIcons[0]
		)
	);
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(
			device,
			L"Resources/Textures/GustSkill.png",
			nullptr,
			&m_skillIcons[1]
		)
	);
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(
			device,
			L"Resources/Textures/TornadoSkill.png",
			nullptr,
			&m_skillIcons[2]
		)
	);

	// カウントダウンテクスチャの読み込み
	for (int i = 0; i < COUNTDOWN_TIME; ++i)
	{
		std::wstringstream ss;
		ss << L"Resources/Textures/Countdown" << (i + 1) << L".png";
		DX::ThrowIfFailed(
			CreateWICTextureFromFile(
				device,
				ss.str().c_str(),
				nullptr,
				&m_countdownTex[i]
			)
		);
	}
}



void GameplayScene::CreateWindowSizeDependentResources()
{
	// 射影行列を作成
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(FOV_Y_DEGREES),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		NEAR_CLIP,
		FAR_CLIP
	);
}



void GameplayScene::OnDeviceLost()
{
	Finalize();
}



/**
 * @brief　勝者の決定
 *
 * @param[in] なし
 *
 * @return なし
 */
void GameplayScene::DetermineWinner()
{
	// 勝敗判定ロジック
	if (!m_winnerSet)
	{
		int aliveCount = 0;
		int lastAliveIndex = -1;

		for (int i = 0; i < m_characters.size(); ++i)
		{
			if (m_characters[i]->IsActive()) // 生きているかチェック
			{
				aliveCount++;
				lastAliveIndex = i;
			}
		}

		// 生き残りが1人以下になったら
		if (aliveCount <= 1)
		{
			if (aliveCount == 1)
			{
				// 共有データに勝者のインデックスを保存
				GetSceneManager()->SetSharedData("Winner", std::to_string(lastAliveIndex));
			}
			else
			{
				// 引き分け（全滅）の場合は "-1" を保存
				GetSceneManager()->SetSharedData("Winner", "-1");
			}

			m_winnerSet = true;
		}
	}
	else
	{
		// 勝者が決まった後の演出待ち
		m_finishTimer++;
		if (m_finishTimer >= FINISH_WAIT_FRAME)
		{
			// BGMを止める
			GetUserResources()->GetResourceManager()->GetBGM(L"gameplay")->Stop();

			// リザルトシーンへ遷移
			ChangeScene<ResultScene>();
		}
	}
}



/**
 * @brief カウントダウン処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void GameplayScene::HandleCountdown(float elapsedTime)
{
	m_countdownTimer -= elapsedTime;

	// 現在の残り秒数を整数にする
	int currentCount = static_cast<int>(ceil(m_countdownTimer));

	// 数字が切り替わった瞬間
	if (currentCount != m_lastCount)
	{
		if (currentCount >= 1)
		{
			// 3, 2, 1 の音
			GetUserResources()->GetResourceManager()->GetSound(L"countdown")->Play();
		}
		else if (currentCount == 0)
		{
			// 0 の音
			GetUserResources()->GetResourceManager()->GetSound(L"start")->Play();

			// カウントダウン終了
			m_countdownFinished = true;
		}

		// 現在のカウントを「前回のカウント」として保存
		m_lastCount = currentCount;
	}
}



/**
 * @brief スキルアイコンの描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void GameplayScene::RenderSkillIcons()
{
	// 半透明を有効にする
	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, m_states->NonPremultiplied());

	// 色の定義
	DirectX::SimpleMath::Color readyColor(READY_TEX, READY_TEX, READY_TEX, READY_TEX);
	DirectX::SimpleMath::Color notReadyColor(NOT_READY_TEX, NOT_READY_TEX, NOT_READY_TEX, NOT_READY_TEX);

	// 表示位置の設定
	DirectX::SimpleMath::Vector2 pos(ICON_POS_X, ICON_POS_Y);

	for (int i = 0; i < SKILL_NUM; ++i)
	{
		// プレイヤーのスキルが使えるかチェック
		bool canUse = m_characters[0]->CanUseSkill(i);

		// 状態に合わせて色を選択
		DirectX::SimpleMath::Color drawColor = canUse ? readyColor : notReadyColor;

		// 描画
		if (m_skillIcons[i])
		{
			m_spriteBatch->Draw(
				m_skillIcons[i],
				pos + DirectX::SimpleMath::Vector2(ICON_OFFSET * i, 0.0f),
				nullptr,
				drawColor,
				0.0f,
				DirectX::SimpleMath::Vector2::Zero,
				TEXTURE_SCALE
			);
		}
	}

	m_spriteBatch->End();
}



/**
 * @brief カウントダウンの描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void GameplayScene::RenderCountdown()
{
	m_spriteBatch->Begin();

	// 残り秒数を切り上げて整数にする
	int currentCount = static_cast<int>(ceil(m_countdownTimer));

	// 配列の添え字に変換
	int texIndex = currentCount - 1;

	// インデックスが配列の範囲内かチェックして描画
	if (texIndex >= 0 && texIndex < COUNTDOWN_TIME)
	{
		ID3D11ShaderResourceView* textureToDraw = m_countdownTex[texIndex];

		if (textureToDraw)
		{
			RECT outputSize = GetUserResources()->GetDeviceResources()->GetOutputSize();
			SimpleMath::Vector2 screenPos(outputSize.right / 2.0f, outputSize.bottom / 2.0f);

			// 画像サイズを取得
			SimpleMath::Vector2 origin(COUNTDOWN_SIZE_X / 2.0f, COUNTDOWN_SIZE_Y / 2.0f);

			m_spriteBatch->Draw(
				textureToDraw,
				screenPos,
				nullptr,
				Colors::White,
				0.0f,
				origin,
				COUNTDOWN_TEX_SCALE
			);
		}
	}

	// 「YOU」のテクスチャ描画
	auto youTex = GetUserResources()->GetResourceManager()->GetTexture(L"you");
	if (youTex)
	{
		SimpleMath::Vector2 pos(YOU_TEX_POS_X, YOU_TEX_POS_Y);
		m_spriteBatch->Draw(
			youTex,
			pos,
			nullptr,
			Colors::White,
			0.0f,
			SimpleMath::Vector2::Zero,
			1.0f
		);
	}

	m_spriteBatch->End();
}



/**
 * @brief 試合を終了する処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void GameplayScene::FinishMatch()
{
	bool survivorFound = false;
	for (size_t i = 1; i < m_characters.size(); ++i)
	{
		if (m_characters[i]->IsActive())
		{
			if (!survivorFound)
			{
				// 最初に見つけたCPU一人だけを生存者として残す
				survivorFound = true;
			}
			else
			{
				// それ以外の生きているCPUは場外へ飛ばす
				m_characters[i]->SetPosition(SimpleMath::Vector3(FINISH_AREA_POS_X, 0.0f, FINISH_AREA_POS_Z));
			}
		}
	}
}
