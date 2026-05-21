/**
 * @file   Particle.cpp
 *
 * @brief  パーティクルに関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.05.21
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Particle.h"

#include <random>

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Particle::Particle()
	: m_viewMatrix{ DirectX::SimpleMath::Matrix::Identity }
	, m_projMatrix{ DirectX::SimpleMath::Matrix::Identity }
	, m_cameraPosition{ DirectX::SimpleMath::Vector3::Zero }
	, m_cameraTarget{ DirectX::SimpleMath::Vector3::Zero }
{
}



/**
 * @brief 生成処理
 *
 * @param[in] pDR DeviceResourcesのポインタ
 *
 * @return なし
 */
void Particle::Create(DX::DeviceResources* pDR)
{
	m_pDR = pDR;

	//	シェーダーの作成
	BaseRenderer::Create(
		pDR,
		L"Resources/Shaders/ParticleVS.cso",
		L"Resources/Shaders/ParticleGS.cso",
		L"Resources/Shaders/ParticlePS.cso",
		DEFAULT_INPUT_LAYOUT
	);

	//	画像の読み込み
	LoadTexture(L"Resources/Textures/star.png");
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void Particle::Update(float elapsedTime)
{
	// イテレータを使ってリストを巡回し、寿命が尽きたパーティクルを削除する
	for (auto it = m_particles.begin(); it != m_particles.end(); )
	{
		// falseを返したらリストから除外
		if (!it->Update(elapsedTime))
		{
			it = m_particles.erase(it);
		}
		else
		{
			++it;
		}
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void Particle::Render()
{
	ID3D11DeviceContext1* context = m_pDR->GetD3DDeviceContext();

    // 定数バッファの更新と設定
    CommonConstBuffer cb;
    cb.matWorld = m_billboard.Transpose();
    cb.matView = m_viewMatrix.Transpose();
    cb.matProj = m_projMatrix.Transpose();
    cb.Diffuse  = DirectX::SimpleMath::Vector4::One;

    context->UpdateSubresource(m_CBuffer.Get(), 0, nullptr, &cb, 0, 0);

    ID3D11Buffer* cbs[] = { m_CBuffer.Get() };
    context->VSSetConstantBuffers(0, 1, cbs);
    context->GSSetConstantBuffers(0, 1, cbs);
    context->PSSetConstantBuffers(0, 1, cbs);

    // パイプライン・ステートの設定
    // 加算合成を使う
    context->OMSetBlendState(m_states->Additive(), nullptr, 0xFFFFFFFF);
    // 奥のものは描画するが、パーティクル同士で深度を書き込まない設定
    context->OMSetDepthStencilState(m_states->DepthRead(), 0);
    context->RSSetState(m_states->CullNone());

    context->PSSetShaderResources(0, 1, m_textures[0].GetAddressOf());

	// シェーダの登録
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    context->IASetInputLayout(m_inputLayout.Get());

    // 描画
    m_batch->Begin();

    for (const auto& p : m_particles)
    {
        // 1粒のデータを頂点として作成
        DirectX::VertexPositionColorTexture vertex(
            p.GetPosition(),
            p.GetColor(),
            DirectX::SimpleMath::Vector2(p.GetNowScale().x, 0.0f)
        );

        // 描画
        m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertex, 1);
    }

    m_batch->End();

    // シェーダの登録を解除
    context->VSSetShader(nullptr, nullptr, 0);
    context->GSSetShader(nullptr, nullptr, 0);
    context->PSSetShader(nullptr, nullptr, 0);

    // ブレンドステートなども元に戻しておく
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
}



/**
 * @brief パーティクルの発生処理
 *
 * @param[in] position 発生させる位置
 *
 * @return なし
 */
void Particle::Emit(const DirectX::SimpleMath::Vector3& position)
{
    // 衝突の衝撃で四方八方に散らすための乱数
    std::random_device seed;
    std::default_random_engine engine(seed());
    std::uniform_real_distribution<float> distAngle(0.0f, DirectX::XM_2PI);
    std::uniform_real_distribution<float> distSpeed(MIN_SPEED, MAX_SPEED);  // 飛び散るスピード
    std::uniform_real_distribution<float> distY(MIN_Y, MAX_Y);              // 上方向への勢い

    // 火花を散らす
    for (int i = 0; i < MAX_PARTICLE_NUM; ++i)
    {
        float angle = distAngle(engine);
        float speed = distSpeed(engine);

        // 放射状に広がる速度ベクトルを計算
        DirectX::SimpleMath::Vector3 velocity(
            cosf(angle) * speed,
            distY(engine), // 上方向にも少し跳ね上げる
            sinf(angle) * speed
        );

        // 重力を加えて、放物線を描いて落ちるようにする
        DirectX::SimpleMath::Vector3 acceleration(0.0f, GRAVITY, 0.0f);

        ParticleUtility pU(
            PARTICLE_LIFE,                      // 生存時間
            position,                           // 発生座標
            velocity,                           // 初期速度
            acceleration,                       // 加速度
            START_SIZE,                         // 初期サイズ
            DirectX::SimpleMath::Vector3::Zero, // 最終サイズ
            START_COLOR,                        // 初期色
            END_COLOR                           // 最終色
        );

        m_particles.push_back(pU);
    }
}



/**
 * @brief ビルボード処理
 *
 * @param[in] target 注視点（カメラの注視点）
 * @param[in] eye    視点（カメラの位置）
 * @param[in] up	 上方向
 *
 * @return なし
 */
void Particle::CreateBillboard(
    const DirectX::SimpleMath::Vector3& target, 
    const DirectX::SimpleMath::Vector3& eye, 
    const DirectX::SimpleMath::Vector3& up)
{
    m_cameraPosition = eye;
    m_cameraTarget = target;

    // ビルボード行列を生成
    m_billboard = DirectX::SimpleMath::Matrix::CreateBillboard(
        DirectX::SimpleMath::Vector3::Zero,
        eye - target,
        up
    );

    // シェーダの向きを合わせる180度反転補正
    DirectX::SimpleMath::Matrix rot = DirectX::SimpleMath::Matrix::Identity;
    rot._11 = -1.0f;
    rot._33 = -1.0f;

    m_billboard = rot * m_billboard;
}
