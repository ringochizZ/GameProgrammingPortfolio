/**
 * @file   RushSkill.cpp
 *
 * @brief  突進スキルに関するソースファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.04.15
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "RushSkill.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
RushSkill::RushSkill(
	ID3D11DeviceContext1* context, 
	ResourceManager* resManager, 
	DirectX::CommonStates* state)
	: Skill{ CONSUMPTION, context, resManager, state, L"rush" }
	, m_rushDir{ DirectX::SimpleMath::Vector3::Zero }
	, m_rushTimer{ 0.0f }
{
}



/**
 * @brief 更新処理
 *
 * @param[in] elapsedTime 経過時間
 *
 * @return なし
 */
void RushSkill::Update(float elapsedTime)
{
	if (m_isActive)
	{
		// 突進中の処理
		m_rushTimer += elapsedTime;

		// 突進時間が一定時間を超えたら終了
		if (m_rushTimer >= RUSH_END_TIME)
		{
			m_isActive = false;
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
void RushSkill::Render(
	ID3D11DeviceContext1* context, 
	DirectX::SimpleMath::Matrix view, 
	DirectX::SimpleMath::Matrix proj)
{
	UNREFERENCED_PARAMETER(context);
	UNREFERENCED_PARAMETER(view);
	UNREFERENCED_PARAMETER(proj);
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
bool RushSkill::Execute(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& dir)
{
	UNREFERENCED_PARAMETER(pos);
	if (m_isActive) return false;

	m_isActive = true;
	m_rushTimer = 0.0f;

	// 方向を保存（dirがほぼ0ならBackwardなどデフォルトの向きを入れる）
	if (dir.LengthSquared() < 0.01f)
	{
		m_rushDir = DirectX::SimpleMath::Vector3::Backward;
	}
	else 
	{
		m_rushDir = dir;
		m_rushDir.Normalize();
	}

	// 効果音の再生
	PlaySkillSound();
	return true;
}
