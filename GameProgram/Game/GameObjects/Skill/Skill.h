/**
 * @file   Skill.h
 *
 * @brief  全スキルの共通インターフェースに関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.04.15
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "library/UserResources.h"
#include "library/ResourceManager.h"

// クラスの定義 ===============================================================
/**
 * @brief スキル
 */
class Skill
{
public:
    enum class HitShape { Box, Sphere };

// 構造体の定義 ------------------------------------------------------------
public:
    /**
     * @brief 当たり判定データ
     */
    struct AttackHitBox
    { 
        HitShape shape = HitShape::Box;
		DirectX::SimpleMath::Vector3    position;  // 中心位置
		DirectX::SimpleMath::Vector3    extents;   // 大きさ
		DirectX::SimpleMath::Quaternion rotation;  // 回転
        DirectX::SimpleMath::Vector3    pushForce; // 相手を吹き飛ばす力
        bool hit = false;                          // 当たったかどうか
    };

// 変数の宣言 --------------------------------------------------------------
protected:
	// 描画用のプリミティブバッチ
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;

    // リソースマネージャー
    ResourceManager* m_pResourceManager;

    // 描画ステート
	DirectX::CommonStates* m_pCommonStates;

    // 効果音
    std::unique_ptr<DirectX::SoundEffect> m_soundEffect;

    float m_consumption;  // 消費量

    bool m_isActive;      // 発動中かどうか

	DirectX::Model* m_pModel;  // モデルのポインタ
    std::vector<AttackHitBox> m_hitBoxes;  // 当たり判定

	std::wstring m_seName; // 効果音の名前

// メンバ関数の宣言 --------------------------------------------------------
public:
    Skill(float consumption,
        ID3D11DeviceContext1* context,
        ResourceManager* resManager,
        DirectX::CommonStates* states,
        const std::wstring& seName = L"")
        : m_consumption{ consumption }
        , m_isActive{ false }
		, m_pResourceManager{ resManager }
        , m_pCommonStates{ states }
		, m_soundEffect{ nullptr }
		, m_seName{ seName }
		, m_pModel{ nullptr }
    {
        m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(context);
    }
    virtual ~Skill() = default;

// 操作
    // 更新処理
    virtual void Update(float elapsedTime) = 0;

    // 描画処理
	virtual void Render(
        ID3D11DeviceContext1* context, 
        DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix proj) = 0;

    // スキル発動できたかどうか
    virtual bool Execute(
        const DirectX::SimpleMath::Vector3& pos,
        const DirectX::SimpleMath::Vector3& dir)
    {
		UNREFERENCED_PARAMETER(pos);
		UNREFERENCED_PARAMETER(dir);

        return false;
    }

    virtual bool Execute(
        const DirectX::SimpleMath::Vector3* pParentPos,
        const DirectX::SimpleMath::Vector3& dir)
    {
		UNREFERENCED_PARAMETER(pParentPos);
		UNREFERENCED_PARAMETER(dir);

        return false;
    }

protected:
    // 再生関数
    void PlaySkillSound()
    {
        // マネージャーから音源を取得して再生
        auto se = m_pResourceManager->GetSound(m_seName);
		se->Play();
    }

// 取得/設定
public:
    // 「自身を動かす力」の取得
    virtual DirectX::SimpleMath::Vector3 GetSelfVelocity() const { return DirectX::SimpleMath::Vector3::Zero; }

    // 「生成された攻撃判定」のリストの取得
    virtual std::vector<AttackHitBox>& GetHitBoxes() { return m_hitBoxes; }

    // 消費量の取得
    float GetConsumption() const { return m_consumption; }

	// 当たったときに消えるかどうか
    virtual bool IsDestroyOnHit() const { return false; }

    // 発動中かどうか
	bool IsActive() const { return m_isActive; }

    // スキルを削除する
    void Deactivate() { m_isActive = false; m_hitBoxes.clear(); }

	// 移動制限があるかどうか
    virtual bool IsMovementRestricted() const { return false; }

    // モデルの設定
    void SetModel(DirectX::Model* model) { m_pModel = model; }

};
