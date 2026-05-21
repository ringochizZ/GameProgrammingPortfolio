/**
 * @file   GustSkill.cpp
 *
 * @brief  突風スキルに関するソースファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "GustSkill.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
GustSkill::GustSkill(
	ID3D11DeviceContext1* context, 
	ResourceManager* resManager, 
	DirectX::CommonStates* states)
	: Skill{ CONSUMPTION, context, resManager, states, L"gust"}
	, m_gustDir{ DirectX::SimpleMath::Vector3::Zero }
	, m_timer{ 0.0f }
{
	// モデルの読み込み
	SetModel(resManager->GetModel(L"gust"));
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void GustSkill::Update(float elapsedTime)
{
	if (!m_isActive) return;

	m_timer -= elapsedTime;

	for (auto& box : m_hitBoxes) 
	{
		// 当たり判定の移動
		box.position += m_gustDir * GUST_SPEED * elapsedTime;

		// 床の範囲外チェック
		if (std::abs(box.position.x) > FLOOR_SIZE || 
			std::abs(box.position.z) > FLOOR_SIZE)
		{
			m_isActive = false;
			m_hitBoxes.clear();
			return;
		}
	}
}



/**
 * @brief 描画処理
 *
 * @param[in] context コンテキスト
 * @param[in] view    ビュー行列
 * @param[in] proj    プロジェクション行列
 *
 * @return なし
 */
void GustSkill::Render(
	ID3D11DeviceContext1* context,
	DirectX::SimpleMath::Matrix view,
	DirectX::SimpleMath::Matrix proj)
{
	// 基本チェック
	if (!m_isActive || m_pModel == nullptr) return;

	// エフェクトを設定
	m_pModel->UpdateEffects([&](DirectX::IEffect* effect)
		{
			auto basicEffect = dynamic_cast<DirectX::BasicEffect*>(effect);
			if (basicEffect)
			{
				basicEffect->SetEmissiveColor(DirectX::Colors::Navy);
				basicEffect->SetAlpha(0.8f); // 透明度
			}
		});

	// 描画ステートの設定
	context->OMSetBlendState(m_pCommonStates->Additive(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_pCommonStates->DepthRead(), 0);
	context->RSSetState(m_pCommonStates->CullNone());

	// 当たり判定ごとに描画
	for (auto& box : m_hitBoxes)
	{
		// ワールド行列を作成
		DirectX::SimpleMath::Matrix world =
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(box.rotation) *
			DirectX::SimpleMath::Matrix::CreateTranslation(box.position);

		// 描画実行
		m_pModel->Draw(context, *m_pCommonStates, world, view, proj);
	}

	// ステートを元に戻す
	context->OMSetBlendState(m_pCommonStates->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_pCommonStates->DepthDefault(), 0);
	context->RSSetState(m_pCommonStates->CullCounterClockwise());
}



/**
 * @brief スキル発動できたかどうか
 *
 * @param[in] pos 発動位置
 * @param[in] dir 発動方向
 *
 * @return true : 発動できた
 *	       false: 発動できなかった
 */
bool GustSkill::Execute(
	const DirectX::SimpleMath::Vector3& pos, 
	const DirectX::SimpleMath::Vector3& dir)
{
	// 既に発動中なら何もしない
	if (m_isActive) return false;

	m_isActive = true;
	m_timer = LIFE_TIME;
	m_gustDir = dir;

	// 当たり判定を作成
	AttackHitBox box;
	// 当たり判定の大きさを設定（幅、高さ、奥行き）
	box.extents = DirectX::SimpleMath::Vector3(
			GUST_WIDTH  / 2.0f, 
			GUST_HEIGHT / 2.0f, 
			GUST_DEPTH  / 2.0f
	); 
	
	// 発動位置から少し前に出した位置を当たり判定の中心にする
	box.position = pos + dir * HITBOX_OFFSET;
	box.position.y += box.extents.y;
	
	// 飛んでいく方向に合わせて回転を設定
	float angle = std::atan2(dir.x, dir.z);
	box.rotation =DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
			DirectX::SimpleMath::Vector3::Up, 
			angle
	);
	
	// 相手を吹き飛ばすベクトル
	box.pushForce = dir * GUST_POWER;
	box.hit = false;

	// リストに登録
	m_hitBoxes.clear();
	m_hitBoxes.push_back(box);

	// 効果音の再生
	PlaySkillSound();
	return true;
}
