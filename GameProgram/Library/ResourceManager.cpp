/**
 * @file   ResourceManager.cpp
 *
 * @brief  リソース管理に関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.04.17
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "ResourceManager.h"

using namespace DirectX;

// メンバ関数の定義 ===========================================================
/**
 * @brief 初期化処理
 *
 * @param[in] device
 * @param[in] audio
 *
 * @return なし
 */
void ResourceManager::Initialize(
    ID3D11Device* device,
    AudioEngine* audio
)
{
    m_device = device;
    m_audio = audio;

    // ============================
    // テクスチャ読み込み
    // ============================

    // 「おとしあう」の文字画像
    CreateWICTextureFromFile(
        device,
        L"Resources/Textures/start.png",
        nullptr,
        m_textures[L"start"].ReleaseAndGetAddressOf()
    );

    // 「おわる」の文字画像
    CreateWICTextureFromFile(
        device,
        L"Resources/Textures/end.png",
        nullptr,
        m_textures[L"end"].ReleaseAndGetAddressOf()
    );

    // 「あなた」の文字画像
    CreateWICTextureFromFile(
        device,
        L"Resources/Textures/you.png",
        nullptr,
        m_textures[L"you"].ReleaseAndGetAddressOf()
    );

    // 「決定」「選択」の文字画像
    CreateWICTextureFromFile(
        device,
        L"Resources/Textures/OperatingInstructions.png",
        nullptr,
        m_textures[L"operatingInstructions"].ReleaseAndGetAddressOf()
    );

    // 背景の画像
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Background.png",
		nullptr,
		m_textures[L"background"].ReleaseAndGetAddressOf()
	);



    // ============================
    // サウンド読み込み
    // ============================

    // カウントダウン音
    m_sounds[L"countdown"] =
        std::make_unique<SoundEffect>(
            audio,
            L"Resources/Sounds/countdown.wav"
        );

    // ゲームスタート音
    m_sounds[L"start"] =
        std::make_unique<SoundEffect>(
            audio,
            L"Resources/Sounds/start.wav"
        );

    // 当たる音
    m_sounds[L"hit"] =
        std::make_unique<SoundEffect>(
            audio,
            L"Resources/Sounds/hit.wav"
        );

    // 選択音
    m_sounds[L"select"] =
        std::make_unique<SoundEffect>(
            audio,
            L"Resources/Sounds/select.wav"
        );

    // 決定音
    m_sounds[L"decision"] =
        std::make_unique<SoundEffect>(
            audio,
            L"Resources/Sounds/decision.wav"
        );

    // アイテム音
    m_sounds[L"item"] =
        std::make_unique<SoundEffect>(
            audio,
            L"Resources/Sounds/item.wav"
        );

	// 勝利音
	m_sounds[L"win"] =
		std::make_unique<SoundEffect>(
			audio,
			L"Resources/Sounds/win.wav"
		);

    // 突進音
	m_sounds[L"rush"] =
		std::make_unique<SoundEffect>(
			audio,
			L"Resources/Sounds/Rush.wav"
		);

    // 突風音
	m_sounds[L"gust"] =
		std::make_unique<SoundEffect>(
			audio,
			L"Resources/Sounds/Gust.wav"
		);

    // 竜巻音
	m_sounds[L"tornado"] =
		std::make_unique<SoundEffect>(
			audio,
			L"Resources/Sounds/Tornado.wav"
		);


    // ============================
    // BGM読み込み
    // ============================

    // タイトルBGM
    m_bgmSources[L"title"] = 
        std::make_unique<SoundEffect>(
            audio, 
            L"Resources/Sounds/title_bgm.wav"
        );
    m_bgms[L"title"] = m_bgmSources[L"title"]->CreateInstance();

    // ゲームプレイBGM
    m_bgmSources[L"gameplay"] = std::make_unique<SoundEffect>(
        audio,
        L"Resources/Sounds/game_bgm.wav"
        );
    m_bgms[L"gameplay"] = m_bgmSources[L"gameplay"]->CreateInstance();
  

    // ----------------------------
    // EffectFactory（モデル用）
    // ----------------------------
    m_fxFactory = std::make_unique<DirectX::EffectFactory>(device);
    m_fxFactory->SetDirectory(L"Resources/Models");

    // ============================
    // モデル読み込み
    // ============================

    // プレイヤーモデル
    m_models[L"1P"] =
        DirectX::Model::CreateFromSDKMESH(
            device,
            L"Resources/Models/1P.sdkmesh",
            *m_fxFactory
        );

    m_models[L"2P"] =
        DirectX::Model::CreateFromSDKMESH(
            device,
            L"Resources/Models/2P.sdkmesh",
            *m_fxFactory
        );

    m_models[L"3P"] =
        DirectX::Model::CreateFromSDKMESH(
            device,
            L"Resources/Models/3P.sdkmesh",
            *m_fxFactory
        );

    m_models[L"4P"] =
        DirectX::Model::CreateFromSDKMESH(
            device,
            L"Resources/Models/4P.sdkmesh",
            *m_fxFactory
        );

    // 冠モデル
    m_models[L"crown"] =
        DirectX::Model::CreateFromSDKMESH(
            device,
            L"Resources/Models/Crown.sdkmesh",
            *m_fxFactory
        );

	// 突風モデル
	m_models[L"gust"] =
		DirectX::Model::CreateFromSDKMESH(
			device,
			L"Resources/Models/gust.sdkmesh",
			*m_fxFactory
		);

    // 竜巻モデル
	m_models[L"tornado"] =
		DirectX::Model::CreateFromSDKMESH(
			device,
			L"Resources/Models/tornado.sdkmesh",
			*m_fxFactory
		);

}
