/**
 * @file   TransitionMask.h
 *
 * @brief  画面切り替え用のマスク表示に関するヘッダファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.02.20
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include"SpriteBatch.h"

// クラスの定義 ===============================================================
/**
 * @brief 画面切り替え用のマスク
 */
class TransitionMask
{
// 列挙型の定義 -----------------------------------------------------
public:
	/**
	 * @brief マスクのリクエスト
	 */
	enum class CreateMaskRequest
	{
		NONE,
		COPY
	};

// 変数の宣言 --------------------------------------------------------------
private:

	CreateMaskRequest m_request;

	// 割合(0～1)
	float m_rate;

	// オープン又はクローズするまでの時間
	float m_interval;

	// オープンフラグ（trueの場合オープン）
	bool m_open;

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	struct ConstantBuffer
	{
		float rate;
		float pad[3];
	};

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PS_Fade;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_maskTexture;

// メンバ関数の宣言 -------------------------------------------------
public:

	TransitionMask(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		float interval
	);

// 操作
public:

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Draw(
		ID3D11DeviceContext* context,
		DirectX::CommonStates* states,
		ID3D11ShaderResourceView* texture,
		const RECT& rect
	);

	// オープンする関数
	void Open();

	// クローズする関数
	void Close();

// 取得/設定
public:

	// オープン中かの確認
	bool IsOpen() { return m_open; }

	// クローズ中かの確認
	bool IsClose() { return !m_open; }

	// オープン具合の取得
	float GetOpenRate() { return m_rate; }

	// オープン、クローズまでの時間の設定
	void SetInterval(float interval) { m_interval = interval; }

	// マスクのリクエストの設定
	void SetCreateMaskRequest(CreateMaskRequest request) { m_request = request; }
	// マスクのリクエストの取得
	CreateMaskRequest GetCreateMaskRequest() { return m_request; }

	// 割合の取得
	float GetRate()
	{
		if (m_open) return 1.0f - m_rate;
		return m_rate;
	}

	// 終了したかの確認
	bool IsEnd()
	{
		if (GetRate() == 1.0f) return true;
		return false;
	}

	// クローズしているかの確認
	bool IsClosed() const
	{
		return m_rate >= 1.0f;
	}

	// オープンしているかの確認
	bool IsOpened() const
	{
		return m_rate <= 0.0f;
	}

};
