/**
 * @file   Character.h
 *
 * @brief  キャラクターに関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// 多重インクルードの防止 =====================================================
#pragma once

// 構造体の宣言 ===============================================================
struct PerceptionData;

// ヘッダファイルの読み込み ===================================================
#include <vector>
#include <memory>

#include "library/DisplayCollision.h"
#include "Game/GameObjects/Skill/Skill.h"

// クラスの定義 ===============================================================
/**
 * @brief キャラクター
 */
class Character 
{  
// 列挙型の定義 -------------------------------------------------------------
public:
    /**
     * @brief スキルの種類
     */
    enum class SkillType {
		NONE = -1,  // スキルなし
		RUSH,       // 突進
        GUST,       // 突風
        TORNADO,    // 竜巻
    };

// 定数の宣言 --------------------------------------------------------------
private:
	static constexpr float MAX_GAUGE      = 100.0f;  // 風パワーゲージの最大値
	static constexpr float GRAVITY        =  0.98f;  // 重力加速度
	static constexpr float FRICTION       =  0.85f;  // 摩擦係数
	static constexpr float FLOOR_SIZE     =  10.0f;  // 床のサイズ
    static constexpr float MASS           =   1.0f;  // 重さ
    static constexpr float RADIUS         =   0.5f;  // 半径

// 変数の宣言 --------------------------------------------------------------
private:
    //  共通ステート
    std::unique_ptr<DirectX::CommonStates> m_states;

    DirectX::SimpleMath::Vector3 m_position;         // 座標
    DirectX::SimpleMath::Vector3 m_internalVelocity; // 自走速度
    DirectX::SimpleMath::Vector3 m_externalVelocity; // 外力速度
    DirectX::SimpleMath::Vector3 m_direction;        // 向き

    float m_gauge;    // 風パワーゲージ
	float m_mass;     // 質量
    bool m_isActive;  // 生きているかどうか

    DisplayCollision* m_displayCollision = nullptr;
	std::vector<std::unique_ptr<Skill>> m_skills; // スキルのリスト

	DirectX::Model* m_model; // モデルデータ

    SkillType m_currentSkillType = SkillType::NONE;

// メンバ関数の宣言 -------------------------------------------------
public:
    Character();
    ~Character();

// 操作
public:
    // 更新処理
    void Update(float elapsedTime, float currentFloorSize);

    // 描画処理
    void Render(
        ID3D11DeviceContext1* context,
        DirectX::SimpleMath::Matrix view,
        DirectX::SimpleMath::Matrix proj,
        float currentFloorSize);

    // スキルの描画処理
    void RenderSkills(
        ID3D11DeviceContext1* context,
        DirectX::SimpleMath::Matrix view,
        DirectX::SimpleMath::Matrix proj);

    // リソース作成
    void CreateDeviceDependentResources(ID3D11Device1* device);
   
    // 移動入力の追加
    void AddInputMovement(
        const DirectX::SimpleMath::Vector3& moveVec, 
        float speed);

    // スキル使用のリクエスト
    void RequestSkill(int slotIndex);

    // 外力の追加
    void AddExternalForce(const DirectX::SimpleMath::Vector3& force);

    // 落下の確認
    void CheckFalling(float currentFloorSize);

    // ゲージの操作
    void ConsumeGauge(float amount);

    // スキルの登録
    void AddSkill(std::unique_ptr<Skill> skill);

    // スキルが使用可能かどうか
	bool CanUseSkill(int slotIndex) const;

// 取得/設定
public:
    // 座標の取得
    const DirectX::SimpleMath::Vector3& GetPosition() const { return m_position; }
    // 座標の設定
    void SetPosition(const DirectX::SimpleMath::Vector3& pos) { m_position = pos; }

    // 方向の設定
    void SetDirection(const DirectX::SimpleMath::Vector3& dir)
    {
        DirectX::SimpleMath::Vector3 d = dir;
        d.y = 0; // 上下は向かない
        if (d.LengthSquared() > 0)
        {
            d.Normalize();
            m_direction = d;
        }
    }
    // 方向の取得
    const DirectX::SimpleMath::Vector3& GetDirection() const { return m_direction; }

    // モデルの設定
    void SetModel(DirectX::Model* model) { m_model = model; }

    // 質量の取得
    float GetMass() const { return m_mass; }

    // 風パワーゲージの取得
    float GetWindowPowerGauge() const { return m_gauge; }

	// 生きているかどうか
    bool IsActive() const { return m_isActive; }

    // 完全に画面外かどうか
    bool IsDead() const { return m_position.y < -10.0f; }

    // スキルの中身の取得
    const std::vector<std::unique_ptr<Skill>>& GetSkills() const { return m_skills; }

    // 現在のスキルの種類を取得する
    SkillType GetCurrentSkillType() const { return m_currentSkillType; }

    // スキルの状態を更新する
    void SetCurrentSkillType(SkillType type) { m_currentSkillType = type; }

};
