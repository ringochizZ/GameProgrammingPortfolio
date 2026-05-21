/**
 * @file   CollisionManager.h
 *
 * @brief  当たり判定の管理に関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.20
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include <vector>
#include <memory>

#include "Game/GameObjects/Skill/Skill.h"
#include "Game/GameObjects/Character.h"
#include "Game/Utilities/Particle.h"

// クラスの定義 ===============================================================
/**
 * @brief 当たり判定の管理
 */
class CollisionManager
{
// 定数の宣言 --------------------------------------------------------------
private:
    static constexpr float SIZE = 1.8f;  // キャラクターのサイズ

	static constexpr float TARGET_RADIUS    =  1.0f;  // キャラクターの当たり判定の半径

    static constexpr float PUSH_FORCE       = 30.0f;  // 反発の強さ
    static constexpr float SKILL_PUSH_FORCE =  3.0f;  // スキルの反発の強さ

// メンバ関数の宣言 --------------------------------------------------------
// 操作
public:
    // 更新処理
    static void Update(
        std::vector<std::unique_ptr<Character>>& characters,
        UserResources* userResources,
        Particle* pParticle)
    {
        ProcessSkillCollisions(characters, userResources);
        ProcessCharacterRepulsion(characters, userResources, pParticle);
    }

    // スキルの当たり判定を処理する関数
    static void ProcessSkillCollisions(
        std::vector<std::unique_ptr<Character>>& characters,
        UserResources* userResources)
    {
        for (auto& character : characters)
        {
            for (auto& skill : character->GetSkills())
            {
                if (!skill->IsActive()) continue;

                if (ProcessSkillHit(skill.get(), character.get(), characters, userResources)) continue;
            }
        }
    }

    // スキルの管理をする関数
    static bool ProcessSkillHit(
        Skill* skill, 
        Character* owner, 
        std::vector<std::unique_ptr<Character>>& characters,
        UserResources* userResources)
    {
        for (auto& hitBox : skill->GetHitBoxes())
        {
            if (hitBox.hit) continue;

            for (auto& target : characters)
            {
                // ターゲットが「スキルの持ち主」なら判定をスキップ
                if (target.get() == owner) continue;

                if (!CheckIntersection(hitBox, target.get())) continue;

                ApplyRepulsion(hitBox, target.get());

                // スキルが当たったときに消えるタイプならスキルを終了
                if (skill->IsDestroyOnHit())
                {
                    // 効果音
                    userResources->GetResourceManager()->GetSound(L"hit")->Play();

                    hitBox.hit = true;
                    skill->Deactivate();
                    return true;
                }
            }
        }
        return false;
    }

    // キャラクター同士の反発を処理する関数
    static void ProcessCharacterRepulsion(
        std::vector<std::unique_ptr<Character>>& characters, 
        UserResources* userResources,
        Particle* pParticle)
    {
        for (size_t i = 0; i < characters.size(); ++i)
        {
            for (size_t j = i + 1; j < characters.size(); ++j)
            {
                ApplyCharacterRepulsion(characters[i].get(), characters[j].get(),userResources, pParticle);
            }
        }
    }

    // 反発を加える関数
    static void ApplyCharacterRepulsion(
        Character* a,
        Character* b,
        UserResources* userResources,
        Particle* pParticle)
    {
        using namespace DirectX::SimpleMath;

        Vector3 diff = a->GetPosition() - b->GetPosition();
        diff.y = 0.0f;

        float dist = diff.Length();

        // 重なっていないなら何もしない
        if (dist >= SIZE || dist <= 0.0f) return;

        // 座標を直接操作して引き離す
        diff.Normalize();
        float overlap = SIZE - dist;

        a->SetPosition(a->GetPosition() + diff * (overlap * 0.5f));
        b->SetPosition(b->GetPosition() - diff * (overlap * 0.5f));

        a->AddExternalForce(diff * PUSH_FORCE);
        b->AddExternalForce(-diff * PUSH_FORCE);

		// パーティクルの発生処理
        if (pParticle)
        {
            // AとBのちょうど中間地点の座標を計算
            Vector3 emitPosition = (a->GetPosition() + b->GetPosition()) * 0.5f;

            // 少し地面から浮かせる
            emitPosition.y += 0.5f;

            // パーティクルを発生させる
            pParticle->Emit(emitPosition);
        }

        // 効果音
        userResources->GetResourceManager()->GetSound(L"hit")->Play();
    }

// 取得/設定
private:
    // 数学的な交差判定
    static bool CheckIntersection(const Skill::AttackHitBox& box, Character* target)
    {
        // ターゲットの当たり判定
        DirectX::BoundingSphere targetSphere(target->GetPosition(), 1.0f);

        if (box.shape == Skill::HitShape::Box)
        {
            DirectX::BoundingOrientedBox skillBox(box.position, box.extents, box.rotation);
            return skillBox.Intersects(targetSphere);
        }
        else
        {
            DirectX::BoundingSphere skillSphere(box.position, box.extents.x);
            return skillSphere.Intersects(targetSphere);
        }
    }

    // 反発の力を加える
    static void ApplyRepulsion(const Skill::AttackHitBox& box, Character* target)
    {
        using namespace DirectX::SimpleMath;

        if (box.shape == Skill::HitShape::Box)
        {
            // ボックス型：反発させる
            Vector3 pushDir = box.pushForce;
            float power = SKILL_PUSH_FORCE / target->GetMass();
            target->AddExternalForce(pushDir * power);
        }
        else
        {
            // 球体型：座標を直接ずらす
            Vector3 diff = target->GetPosition() - box.position;
            diff.y = 0.0f;
            float dist = diff.Length();

            // 重なりが全く同じ位置の時のための安全対策
            if (dist <= 0.001f)
            {
                diff = Vector3(1.0f, 0.0f, 0.0f);
                dist = 0.001f;
            }

            diff.Normalize();
            float skillRadius = box.extents.x;

            // めり込んでいる距離を計算
            float overlap = (TARGET_RADIUS + skillRadius) - dist;

            if (overlap > 0.0f)
            {
                // めり込んだ分だけ外側に座標を押し出し、絶対に中に入れないようにする
                target->SetPosition(target->GetPosition() + diff * overlap);
            }
        }
    }
};
