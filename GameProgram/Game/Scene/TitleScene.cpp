/**
 * @file   TitleScene.cpp
 *
 * @brief  タイトルシーンに関するソースファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.04.09
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "TitleScene.h"

#include "ThirdParty/DebugDraw.h"
#include "Library/UserResources.h"
#include "Game/Scene/GameplayScene.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
TitleScene::TitleScene()
	: m_enemyRot{ 0.0f }
	, m_playerRot{ 0.0f }
	, m_prevKeyboard{ 0.0f }
	, m_player{ nullptr }
	, m_enemy{ nullptr }
	, m_selectSound{ nullptr }
	, m_isTransitioning{ false }
{
	
}



/**
 * @brief デストラクタ
 */
TitleScene::~TitleScene()
{
}



/**
 * @brief 初期化処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TitleScene::Initialize()
{
	//========================================================
	// リソース管理
	//========================================================
	auto res = GetUserResources()->GetResourceManager();

	// BGMの取得
	auto bgm = res->GetBGM(L"title");
	bgm->SetVolume(BGM_VOLUME);
	bgm->Play(true);

	// サウンドの取得
	m_selectSound = res->GetSound(L"select");

	// モデル取得
	m_player = res->GetModel(L"1P");
	m_enemy  = res->GetModel(L"4P");

	//========================================================

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	// 座標の設定
	m_playerPos = DirectX::SimpleMath::Vector3( MODEL_POS_X, MODEL_POS_Y, 0.0f);
	m_enemyPos  = DirectX::SimpleMath::Vector3(-MODEL_POS_X, MODEL_POS_Y, 0.0f);

	// 回転
	m_playerRot = 0.0f;
	m_enemyRot  = 0.0f;

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
			// 決定された時の処理
			auto sound = GetUserResources()->GetResourceManager()->GetSound(L"decision");
			sound->Play();

			auto transitionMask = GetUserResources()->GetTransitionMask();
			transitionMask->Close();
			m_isTransitioning = true;
			GetUserResources()->GetResourceManager()->GetBGM(L"title")->Stop();
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
 * @param[in] elapsedTime
 *
 * @return なし
 */
void TitleScene::Update(float elapsedTime)
{
	// メニューの更新処理
	m_buttonManager->Update();

	// マスクの状態をチェックしてシーン遷移
	auto transitionMask = GetUserResources()->GetTransitionMask();
	if (m_isTransitioning)
	{
		if (transitionMask->IsClosed())
		{
			ChangeScene<GameplayScene>();
		}
	}

	// キャラクターの回転
	m_playerRot -= DirectX::XM_PI * ROTATION_SPEED * elapsedTime;
	m_enemyRot  += DirectX::XM_PI * ROTATION_SPEED * elapsedTime;
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TitleScene::Render()
{
	// 固定カメラ
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, CAMERA_POS_Z),
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, CAMERA_TARGET_Z),
		DirectX::SimpleMath::Vector3(0.0f, CAMERA_UP_Y, 0.0f)
	);


	// 2D 描画開始
	m_spriteBatch->Begin();

	// 背景の描画
	m_spriteBatch->Draw(
		GetUserResources()->GetResourceManager()->GetTexture(L"background"),
		DirectX::SimpleMath::Vector2::Zero,
		nullptr,
		DirectX::Colors::White,
		0.0f,
		DirectX::SimpleMath::Vector2(TEXTURE_SCALE, TEXTURE_SCALE)
	);

	// 画像の表示場所
	DirectX::SimpleMath::Vector2 pos(TITLE_POS_X, TITLE_POS_Y);

	// タイトルの描画
	m_spriteBatch->Draw(
		m_titleTexture.Get(), 
		pos, 
		nullptr, 
		DirectX::Colors::White,
		0.0f,
		DirectX::SimpleMath::Vector2(TEXTURE_SCALE, TEXTURE_SCALE)
	);

	// 2D 描画終了
	m_spriteBatch->End();

	// メニューの描画処理
	m_buttonManager->Render();
	
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();
	
	// ワールド行列
	DirectX::SimpleMath::Matrix rot   = DirectX::SimpleMath::Matrix::CreateRotationY(m_playerRot);
	DirectX::SimpleMath::Matrix trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_playerPos);
	DirectX::SimpleMath::Matrix m_world = rot * trans;
	// プレイヤーの描画
	m_player->Draw(context, *m_states.get(), m_world, m_view, m_proj);

	// ワールド行列
	rot = DirectX::SimpleMath::Matrix::CreateRotationY(m_enemyRot);
	trans = DirectX::SimpleMath::Matrix::CreateTranslation(m_enemyPos);
	m_world = rot * trans;
	// 敵の描画
	m_enemy->Draw(context, *m_states.get(), m_world, m_view, m_proj);
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void TitleScene::Finalize()
{
}



void TitleScene::CreateDeviceDependentResources()
{
	auto device = GetUserResources()->GetDeviceResources()->GetD3DDevice();
	auto context = GetUserResources()->GetDeviceResources()->GetD3DDeviceContext();

	//  共通ステートの作成
	m_states = std::make_unique<DirectX::CommonStates>(device);
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
	auto blendState = GetUserResources()->GetCommonStates()->NonPremultiplied();
	context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
	auto depthState = GetUserResources()->GetCommonStates()->DepthRead();
	context->OMSetDepthStencilState(depthState, 0);

	// 画像の読み込み
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,L"Resources/Textures/Title.png",
			nullptr,m_titleTexture.ReleaseAndGetAddressOf()
		)
	);

}



void TitleScene::CreateWindowSizeDependentResources()
{
	// 射影行列を作成
	RECT rect = GetUserResources()->GetDeviceResources()->GetOutputSize();
	m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		DirectX::XMConvertToRadians(FOV_Y_DEGREES),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		NEAR_CLIP, FAR_CLIP
	);
}



void TitleScene::OnDeviceLost()
{
	Finalize();
}
