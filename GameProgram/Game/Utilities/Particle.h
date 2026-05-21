/**
 * @file   Particle.h
 *
 * @brief  パーティクルに関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "ParticleUtility.h"
#include "Library/BaseRenderer.h"

#include <list>

// クラスの定義 ===============================================================
/**
 * @brief パーティクル
 */
class Particle : public BaseRenderer
{
// 定数の宣言 --------------------------------------------------------------
private:
	static constexpr int MAX_PARTICLE_NUM = 5;       // 最大パーティクル数

	static constexpr float GRAVITY = -9.8f;            // 重力加速度
	static constexpr float PARTICLE_LIFE = 1.0f;       // 寿命
	static constexpr float MIN_SPEED = 2.0f;           // 最小の飛び散るスピード
	static constexpr float MAX_SPEED = 6.0f;           // 最大の飛び散るスピード
	static constexpr float MIN_Y = 1.0f;               // 最小の上方向への勢い
	static constexpr float MAX_Y = 4.0f;               // 最大の上方向への勢い

	static constexpr DirectX::SimpleMath::Vector3 START_SIZE =
		DirectX::SimpleMath::Vector3(1.5f, 1.5f, 1.5f);         // 発生時のサイズ
	
	static constexpr DirectX::SimpleMath::Color START_COLOR = 
		DirectX::SimpleMath::Color(1.0f, 0.8f, 0.3f, 1.0f);		// 発生時の色
	static constexpr DirectX::SimpleMath::Color END_COLOR =
		DirectX::SimpleMath::Color(1.0f, 0.2f, 0.0f, 0.0f);		// 終了時の色

// 変数の宣言 --------------------------------------------------------------
private:
	// パーティクル固有のメンバ
	std::list<ParticleUtility> m_particles;

	// カメラの行列
	DirectX::SimpleMath::Matrix m_viewMatrix;
	DirectX::SimpleMath::Matrix m_projMatrix;

	// カメラ座標
	DirectX::SimpleMath::Vector3 m_cameraPosition;
	DirectX::SimpleMath::Vector3 m_cameraTarget;
	DirectX::SimpleMath::Matrix  m_billboard;

// メンバ関数の宣言 --------------------------------------------------------
public:
	Particle();
	~Particle() override = default;

// 操作
public:
	// 生成処理
	void Create(DX::DeviceResources* pDR);

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render() override;

	// パーティクルの発生処理
	void Emit(const DirectX::SimpleMath::Vector3& position);

	// ビルボード処理
	void CreateBillboard(
		const DirectX::SimpleMath::Vector3& target,
		const DirectX::SimpleMath::Vector3& eye,
		const DirectX::SimpleMath::Vector3& up
	);

	// カメラの行列を設定する処理
	void SetCameraMatrices(
		const DirectX::SimpleMath::Matrix& view, 
		const DirectX::SimpleMath::Matrix& proj)
	{
		m_viewMatrix = view;
		m_projMatrix = proj;
	}

};
