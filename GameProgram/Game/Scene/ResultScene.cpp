/**
 * @file   ResultScene.cpp
 *
 * @brief  リザルトシーンに関するソースファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Game/Scene/ResultScene.h"

#include "Game/Scene/GameplayScene.h"
#include "Game/Scene/TitleScene.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
ResultScene::ResultScene()
	: m_characterRot{ 0.0f }
	, m_winPosY{ 0.0f }
	, m_isWinBounceFinished{ false }
	, m_winAnimationTime{ 0.0f }
	, m_prevKeyboard{ 0.0f }
	, m_character{ nullptr }
	, m_enemy{ nullptr }
	, m_crown{ nullptr }
	, m_winSound{ nullptr }
	, m_selectSound{ nullptr }
{
}



/**
 * @brief デストラクタ
 */
ResultScene::~ResultScene()
{
}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ResultScene::Initialize()
{
	//========================================================
	// リソース管理
	//========================================================
	auto res = GetUserResources()->GetResourceManager();

	// サウンドの取得
	m_selectSound = res->GetSound(L"select");
	m_winSound = res->GetSound(L"win");

	// 冠のモデルを取得
	m_crown = res->GetModel(L"crown");

	// 共有データから勝者情報を取得
	std::string winnerStr = GetSceneManager()->GetSharedData("Winner");
	int winnerIdx = std::stoi(winnerStr);

	// モデル名のテーブルを用意
	std::wstring modelKeys[] = { L"1P", L"2P", L"3P", L"4P" };

	// 勝者が存在する場合
	if (winnerIdx >= 0 && winnerIdx < CHRACTER_NUM)
	{
		// モデルの取得
		m_character = res->GetModel(modelKeys[winnerIdx]);
	}
	// 引き分けの場合
	else
	{
		// 1Pを出しておく
		m_character = res->GetModel(L"1P");
	}

	//========================================================

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	// 効果音
	m_winSound->Play();

	// 回転
	m_characterRot = 0.0f;

	// WIN文字の初期座標
	m_winPosY = WIN_BASE_Y;

	// アニメーションの時間
	m_winAnimationTime = 0.0f;

	// アニメーションが終わっていない
	m_isWinBounceFinished = false;

	// シーン開始時にクラッカーを鳴らす
	SpawnParticles();

	// マネージャーの生成
	m_buttonManager = std::make_unique<ButtonManager>();

	// マネージャーの初期設定
	m_buttonManager->Initialize(BASE_Y, MIN_X, MAX_X);
	m_buttonManager->SetSound(m_selectSound);

	D3D11_VIEWPORT windowInfo = GetUserResources()->GetDeviceResources()->GetScreenViewport();
	int w = (int)windowInfo.Width;
	int h = (int)windowInfo.Height;

	// 「おとしあう（スタート）」ボタンの作成
	auto startBtn = std::make_unique<Button>(
		GetUserResources()->GetDeviceResources(), w, h,
		L"Resources/Textures/window.png", L"Resources/Textures/start.png",
		[this]() {
			// 決定音を鳴らす
			auto sound = GetUserResources()->GetResourceManager()->GetSound(L"decision");
			sound->Play();

			// ゲームプレイシーンへ
			ChangeScene<GameplayScene>();
		}
	);
	m_buttonManager->AddButton(std::move(startBtn));

	// 「おわる」ボタンの作成
	auto endBtn = std::make_unique<Button>(
		GetUserResources()->GetDeviceResources(), w, h,
		L"Resources/Textures/window.png", L"Resources/Textures/end.png",
		[this]() {
			// 終了処理
			auto sound = GetUserResources()->GetResourceManager()->GetSound(L"decision");
			sound->Play();
			PostQuitMessage(0);
		}
	);
	m_buttonManager->AddButton(std::move(endBtn));
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void ResultScene::Update(float elapsedTime)
{
	// メニューの更新処理
	m_buttonManager->Update();

	// 回転角度を増加
	m_characterRot += CHARACTER_ROT_SPEED * elapsedTime;

	// WINのアニメーション更新
	UpdateWinAnimation(elapsedTime);

	// パーティクルの更新
	UpdateParticles(elapsedTime);
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ResultScene::Render()
{
	// ビュー行列を設定
	DirectX::SimpleMath::Vector3 eye    = CAMERA_EYE;
	DirectX::SimpleMath::Vector3 target = CAMERA_TARGET; 
	DirectX::SimpleMath::Vector3 up     = CAMERA_UP;

	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(eye, target, up);

	ID3D11DeviceContext1* context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	// 2D 描画開始
	m_spriteBatch->Begin();

	// 背景
	m_spriteBatch->Draw(
		GetUserResources()->GetResourceManager()->GetTexture(L"background"),
		DirectX::SimpleMath::Vector2::Zero,
		nullptr,
		DirectX::Colors::White,
		0.0f,
		DirectX::SimpleMath::Vector2(TEXTURE_SCALE, TEXTURE_SCALE)
	);

	// パーティクルを描画
	for (const auto& p : m_particles)
	{
		m_spriteBatch->Draw(
			m_particleTexture.Get(),
			p.position,
			nullptr,
			p.color,
			p.rotation,
			DirectX::SimpleMath::Vector2(PARTICLE_SIZE / 2, PARTICLE_SIZE / 2),
			p.scale
		);
	}

	// WIN文字の上下バウンドアニメーション
	if (!m_isWinBounceFinished)
	{
		float damping = 1.0f - m_winAnimationTime;
		m_winPosY += std::sinf(2.0f * DirectX::XM_PI * WIN_BOUNCE_FREQ * m_winAnimationTime)
			* WIN_BOUNCE_AMPLITUDE * damping;
	}

	// WINの文字
	m_spriteBatch->Draw(
		m_winTexture.Get(),
		DirectX::SimpleMath::Vector2(WIN_TEX_POS_X, m_winPosY),
		nullptr,
		DirectX::Colors::White,
		0.0f,
		DirectX::SimpleMath::Vector2(TEXTURE_SCALE, TEXTURE_SCALE)
	);

	// 2D 描画終了
	m_spriteBatch->End();

	// メニューの描画処理
	m_buttonManager->Render();

	// 勝ったキャラのモデルを表示
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateRotationY(m_characterRot);
	m_character->Draw(context, *m_states.get(), world, m_view, m_proj);

	// 冠の描画
	DirectX::SimpleMath::Matrix rotZ  = 
		DirectX::SimpleMath::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(CROWN_ROT_SPEED));
	DirectX::SimpleMath::Matrix rotY  = DirectX::SimpleMath::Matrix::CreateRotationY(m_characterRot);
	DirectX::SimpleMath::Matrix trans = 
		DirectX::SimpleMath::Matrix::CreateTranslation(CROWN_POS_X, CROWN_POS_Y, CROWN_POS_Z);
	world = rotY * rotZ * trans;
	m_crown->Draw(context, *m_states.get(), world, m_view, m_proj);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ResultScene::Finalize()
{

}



void ResultScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	
	//  共通ステートの作成
	m_states = std::make_unique<DirectX::CommonStates>(device);
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

	// 画像の読み込み
	// 「WIN」の文字
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device, L"Resources/Textures/Win.png",
			nullptr, m_winTexture.ReleaseAndGetAddressOf()
		)
	);

	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device, L"Resources/Textures/particle.png",
			nullptr, m_particleTexture.ReleaseAndGetAddressOf()
		)
	);
}



void ResultScene::CreateWindowSizeDependentResources()
{
	// 射影行列を作成
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		DirectX::XMConvertToRadians(FOV_Y_DEGREES),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		NEAR_CLIP,
		FAR_CLIP
	);
}



void ResultScene::OnDeviceLost()
{
	Finalize();
}



/**
 * @brief パーティクルの生成処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void ResultScene::SpawnParticles()
{
	m_particles.clear();
	DirectX::XMVECTOR colors[PARTICLE_COLOR_COUNT] = {
		DirectX::Colors::Red, DirectX::Colors::Yellow, DirectX::Colors::Blue,
		DirectX::Colors::Lime, DirectX::Colors::DeepPink, DirectX::Colors::Cyan, DirectX::Colors::Orange };

	for (int i = 0; i < PARTICLE_SPAWN_COUNT; ++i)
	{
		Particle p;
		// 画面の下から発射
		p.position = DirectX::SimpleMath::Vector2((float)(rand() % SCREEN_WIDTH), PARTICLE_SPAWN_START_Y);

		// 初速
		float vx = ((float)rand() / RAND_MAX - 0.5f) * PARTICLE_SPAWN_VX_MULT;
		float vy = PARTICLE_SPAWN_VY_BASE - ((float)rand() / RAND_MAX * PARTICLE_SPAWN_VY_RAND);
		p.velocity = DirectX::SimpleMath::Vector2(vx, vy);

		p.color = colors[rand() % PARTICLE_COLOR_COUNT];
		p.rotation = (float)rand() / RAND_MAX * DirectX::XM_2PI;
		p.rotationSpeed = ((float)rand() / RAND_MAX - 0.5f) * PARTICLE_SPAWN_ROT_SPEED_MULT;
		p.scale = PARTICLE_SPAWN_SCALE_BASE + (float)rand() / RAND_MAX * PARTICLE_SPAWN_SCALE_RAND;
		p.timer = (float)rand() / RAND_MAX * DirectX::XM_2PI;

		m_particles.push_back(p);
	}
}



/**
 * @brief パーティクルの更新処理
 *
 * @param[in] elapsedTime
 *
 * @return なし
 */
void ResultScene::UpdateParticles(float elapsedTime)
{
	for (auto& p : m_particles)
	{
		// 抵抗を少し弱めて遠くまで飛ばす
		p.velocity *= PARTICLE_AIR_RESISTANCE;

		// 重力
		p.velocity.y += PARTICLE_GRAVITY * elapsedTime;

		// 移動
		p.position += p.velocity * elapsedTime;
		p.timer += elapsedTime * PARTICLE_TIMER_SPEED;
		// 速度が遅いときほど横揺れを大きくする
		float swayPower = (std::abs(p.velocity.y) < PARTICLE_SWAY_VELOCITY_Y_THRESH) ? PARTICLE_SWAY_POWER_HIGH : PARTICLE_SWAY_POWER_LOW;
		p.position.x += std::sinf(p.timer) * swayPower;

		p.rotation += p.rotationSpeed * elapsedTime;

		// ループ処理
		if (p.position.y > PARTICLE_LOOP_Y_THRESH)
		{
			p.position.y = PARTICLE_LOOP_RESET_Y;
			p.position.x = (float)(rand() % SCREEN_WIDTH);

			// 落下速度をランダムに設定
			p.velocity.y = PARTICLE_LOOP_VY_BASE + (float)rand() / RAND_MAX * PARTICLE_LOOP_VY_RAND;
			p.velocity.x = ((float)rand() / RAND_MAX - 0.5f) * PARTICLE_LOOP_VX_MULT;
		}
	}
}



/**
 * @brief WINアニメーションの更新処理
 *
 * @param[in] elapsedTime
 *
 * @return なし
 */
void ResultScene::UpdateWinAnimation(float elapsedTime)
{
	m_winPosY = WIN_BASE_Y;
	if (!m_isWinBounceFinished)
	{
		float damping = 1.0f - m_winAnimationTime;
		m_winPosY += std::sinf(2.0f * DirectX::XM_PI * WIN_BOUNCE_FREQ * m_winAnimationTime)
			* WIN_BOUNCE_AMPLITUDE * damping;
	}

	if (!m_isWinBounceFinished)
	{
		m_winAnimationTime += elapsedTime;
		if (m_winAnimationTime > WIN_BOUNCE_DURATION)
		{
			m_winAnimationTime = WIN_BOUNCE_DURATION;
			m_isWinBounceFinished = true;
		}
	}
}
