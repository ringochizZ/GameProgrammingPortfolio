/**
 * @file   Character.cpp
 *
 * @brief  キャラクターに関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.05.21
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "Character.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
Character::Character()
    : m_position{ DirectX::SimpleMath::Vector3::Zero }
    , m_internalVelocity{ DirectX::SimpleMath::Vector3::Zero }
    , m_externalVelocity{ DirectX::SimpleMath::Vector3::Zero }
    , m_direction{ DirectX::SimpleMath::Vector3::Backward }
    , m_mass{ MASS }
    , m_gauge{ MAX_GAUGE }
    , m_isActive{ true }
	, m_model{ nullptr }
{   
}



/**
 * @brief デストラクタ
 */
Character::~Character()
{
}



/**
 * @brief 更新処理
 *
 * @param[in] alapsedTime      経過時間
 * @param[in] currentFloorSize 現在の床のサイズ
 *
 * @return なし
 */
void Character::Update(float elapsedTime, float currentFloorSize)
{
	// 死亡している場合は落下させる
    if (!m_isActive)
    {
        m_externalVelocity.y -= GRAVITY;
    }

	// ゲージの回復
	if (m_gauge < MAX_GAUGE) m_gauge += 1.0f * elapsedTime;

    // 摩擦
    m_externalVelocity *= FRICTION;
    m_internalVelocity *= FRICTION;

    DirectX::SimpleMath::Vector3 skillVel = DirectX::SimpleMath::Vector3::Zero;

    for (auto& skill : m_skills)
    {
        skill->Update(elapsedTime);
        skillVel += skill->GetSelfVelocity();
		
        if (skill->IsMovementRestricted())
		{			
            // 移動制限中は自走速度をリセット
			m_internalVelocity = DirectX::SimpleMath::Vector3::Zero;
        }
    }

    // 座標を更新
    m_position += (m_internalVelocity + m_externalVelocity + skillVel) * elapsedTime;

	// 落下判定
    CheckFalling(currentFloorSize);
}



/**
 * @brief 描画処理
 *
 * @param[in] context           コンテキスト
 * @param[in] view              ビュー行列
 * @param[in] proj              プロジェ
 * @param[in] world             ワールド行列
 * @param[in] currentFloorSize  現在の床のサイズ
 *
 * @return なし
 */
void Character::Render(
    ID3D11DeviceContext1* context,
    DirectX::SimpleMath::Matrix view,
    DirectX::SimpleMath::Matrix proj,
    float currentFloorSize)
{
    // ワールド行列
    DirectX::SimpleMath::Matrix world = 
        DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XM_PI) *
        DirectX::SimpleMath::Matrix::CreateWorld(
        m_position,
        m_direction,
        DirectX::SimpleMath::Vector3::Up
    );

    // モデルの描画
    m_model->Draw(context, *m_states, world, view, proj);

	// スキルの描画
    for (auto& skill : m_skills)
    {
        skill->Render(context, view, proj);
    }

    // 現在の床のサイズで落下判定
    CheckFalling(currentFloorSize);
}



/**
 * @brief スキルの描画処理
 *
 * @param[in] context           コンテキスト
 * @param[in] view              ビュー行列
 * @param[in] proj              プロジェ
 *
 * @return なし
 */
void Character::RenderSkills(
    ID3D11DeviceContext1* context,
    DirectX::SimpleMath::Matrix view, 
    DirectX::SimpleMath::Matrix proj)
{
    for (auto& skill : m_skills)
    {
        skill->Render(context, view, proj);
    }
}



/**
 * @brief デバイス依存リソースの作成
 *
 * @param[in] device  デバイス
 * @param[in] context コンテキスト
 *
 * @return なし
 */
void Character::CreateDeviceDependentResources(ID3D11Device1* device)
{
    //  共通ステートの作成
    m_states = std::make_unique<DirectX::CommonStates>(device);
}



/**
 * @brief 行動入力の追加
 *
 * @param[in] moveVec 移動ベクトル
 * @param[in] speed   移動速度
 *
 * @return なし
 */
void Character::AddInputMovement(const DirectX::SimpleMath::Vector3& moveVec, float speed)
{
    if (!m_isActive) return;

    if (moveVec.LengthSquared() > 0)
    {
        m_internalVelocity = moveVec * speed;
        
        // 動いた方向を向く
        SetDirection(moveVec);
    }
}



/**
 * @brief スキル使用のリクエスト
 *
 * @param[in] slotIndex   スキルスロットのインデックス
 *
 * @return なし
 */
void Character::RequestSkill(int slotIndex)
{
    if (!m_isActive || slotIndex < 0 || slotIndex >= m_skills.size()) return;

    auto& skill = m_skills[slotIndex];

    // 移動制限中のスキルがあるかチェック
    for (const auto& s : m_skills)
    {
        if (s->IsActive() && s->IsMovementRestricted())
        {
            // 新しいスキルは受け付けない
            return;
        }
    }

    // ゲージの確認
    if (m_gauge >= skill->GetConsumption())
    {
        // スキル実行
        if (skill->Execute(m_position, m_direction) ||
            skill->Execute(&m_position, m_direction))
        {
            ConsumeGauge(skill->GetConsumption());
        }
    }
}



/**
 * @brief 外力の追加
 *
 * @param[in] force 追加する外力
 *
 * @return なし
 */
void Character::AddExternalForce(const DirectX::SimpleMath::Vector3& force)
{
	// 質量に応じて外力を加算
	m_externalVelocity += force / m_mass;
}



/**
 * @brief 落下の確認
 *
 * @param[in] currentFloorSize 現在の床のサイズ
 *
 * @return なし
 */
void Character::CheckFalling(float currentFloorSize)
{
    // 端っこの判定
    if (std::abs(m_position.x) - RADIUS > currentFloorSize || 
        std::abs(m_position.z) - RADIUS > currentFloorSize)
    {
        m_isActive = false;
    }
}



/**
 * @brief ゲージの操作
 *
 * @param[in] amount 消費するゲージの量
 *
 * @return なし
 */
void Character::ConsumeGauge(float amount)
{
    // ゲージの消費
	m_gauge -= amount;
}



/**
 * @brief スキルの登録
 *
 * @param[in] skill 登録するスキル
 *
 * @return なし
 */
void Character::AddSkill(std::unique_ptr<Skill> skill)
{
	// スキルをスロットに追加
    m_skills.push_back(std::move(skill));
}



/**
 * @brief 指定したスキルが使用可能か判定する
 *
 * @param[in] slotIndex   スキルスロットのインデックス
 *
 * @return true : 使用可能
 *	       false: 使用不可能
 */
bool Character::CanUseSkill(int slotIndex) const
{
    // 死んでいたり、インデックスが存在しなければ使えない
    if (!m_isActive || slotIndex < 0 || slotIndex >= m_skills.size()) return false;

    auto& targetSkill = m_skills[slotIndex];

	// スキルがすでに発動している場合は使えない
    if (targetSkill->IsActive())
    {
        return false;
    }

    // 移動制限中のスキルが発動していたら使えない
    for (const auto& s : m_skills)
    {
        if (s->IsActive() && s->IsMovementRestricted())
        {
            return false;
        }
    }

    // ゲージが消費量に満たない場合は使えない
    if (m_gauge < targetSkill->GetConsumption())
    {
        return false;
    }

    // 全ての条件をクリアしていれば使える
    return true;
}
