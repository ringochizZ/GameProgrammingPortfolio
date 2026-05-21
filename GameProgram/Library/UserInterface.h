/**
 * @file   UserInterface.h
 *
 * @brief  ユーザーインターフェースに関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.02.09
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "ThirdParty/StepTimer.h"
#include "ThirdParty/DeviceResources.h"

#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>

// UIのアンカーポイントの列挙数
enum ANCHOR
{
	TOP_LEFT = 0,
	TOP_CENTER,
	TOP_RIGHT,

	MIDDLE_LEFT,
	MIDDLE_CENTER,
	MIDDLE_RIGHT,

	BOTTOM_LEFT,
	BOTTOM_CENTER,
	BOTTOM_RIGHT
};

// クラスの定義 ===============================================================
/**
 * @brief ユーザーインターフェース
 */
class UserInterface
{
// 列挙型の定義 -----------------------------------------------------
protected:
	/**
	 * @brief データ受け渡し用コンスタントバッファ(送信側)
	 */
	struct ConstBuffer
	{
		DirectX::SimpleMath::Vector2	windowSize;
		float alphaData = 0.0f;
		float dummy = 0.0f;
	};

// 変数の宣言 --------------------------------------------------------------
protected:
	DX::DeviceResources* m_pDR;

	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_CBuffer;

	DX::StepTimer                           m_timer;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	//コモンステート
	std::unique_ptr<DirectX::CommonStates> m_states;
	// テクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	Microsoft::WRL::ComPtr<ID3D11Resource> m_res;

	// 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	// ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	// ジオメトリシェーダ
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;

	int m_windowWidth , m_windowHeight;
	int m_textureWidth, m_textureHeight;

	DirectX::SimpleMath::Vector2 m_scale;
	DirectX::SimpleMath::Vector2 m_baseScale;
	DirectX::SimpleMath::Vector2 m_position;

	ANCHOR m_anchor;

	float m_renderRatio;
	float m_renderRatioOffset;

public:
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

// メンバ関数の宣言 --------------------------------------------------------
public:
	UserInterface();
	~UserInterface();

// 操作
public:
	// テクスチャの読み込み
	void LoadTexture(const wchar_t* path);
	
	// 作成処理
	void Create(DX::DeviceResources* pDR
		, const wchar_t* path
		, DirectX::SimpleMath::Vector2 position
		,DirectX::SimpleMath::Vector2 scale
		,ANCHOR anchor);

	// 描画処理
	void Render();

private:
	// シェーダーの作成処理
	void CreateShader();

// 取得/設定
public:
	// スケールの設定
	void SetScale(DirectX::SimpleMath::Vector2 scale){ m_scale = scale; }
	// スケールの取得
	DirectX::SimpleMath::Vector2 GetScale() { return m_scale; }
	DirectX::SimpleMath::Vector2 GetBaseScale() { return m_baseScale; }

	// 座標の設定
	void SetPosition(DirectX::SimpleMath::Vector2 position){ m_position = position; }
	// 座標の取得
	DirectX::SimpleMath::Vector2 GetPosition() { return m_position; }
	
	// アンカーの設定
	void SetAnchor(ANCHOR anchor){ m_anchor = anchor; }
	// アンカーの取得
	ANCHOR GetAnchor() const { return m_anchor; }
	
	// 描画割合の設定
	void SetRenderRatio(float ratio){ m_renderRatio = ratio; }
	// 描画割合の取得
	float GetRenderRatio() { return m_renderRatio; }
	
	// 描画割合オフセットの設定
	void SetRenderRatioOffset(float offset){ m_renderRatioOffset = offset; }
	// 描画割合オフセットの取得
	float GetRenderRatioOffset() { return m_renderRatioOffset; }

	// ウィンドウサイズの設定
	void SetWindowSize(const int& width, const int& height)
	{
		m_windowWidth = width;
		m_windowHeight = height;
	}

};
