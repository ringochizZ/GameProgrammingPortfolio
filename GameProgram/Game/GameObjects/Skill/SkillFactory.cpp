/**
 * @file   SkillFactory.cpp
 *
 * @brief  スキルの生成に関するソースファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.04.03
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "SkillFactory.h"

#include "Game/GameObjects/Skill/Skill.h" 
#include "Game/GameObjects/Skill/RushSkill.h" 
#include "Game/GameObjects/Skill/GustSkill.h"
#include "Game/GameObjects/Skill/TornadoSkill.h"

/**
 * @brief スキルの生成処理
 *
 * @param[in] type     作成したいスキルの種類
 * @param[in] userRes  ユーザーリソース
 * @param[in] display  当たり判定の可視化用ポインタ
 * 
 * @return 生成されたスキルのポインタ
 */
std::unique_ptr<Skill> SkillFactory::Create(
    SkillType type, 
    UserResources* userRes)
{
    auto context = userRes->GetDeviceResources()->GetD3DDeviceContext();
    auto resManager = userRes->GetResourceManager();
    auto states = userRes->GetCommonStates();

    std::unique_ptr<Skill> newSkill = nullptr;

    switch (type)
    {
    case SkillType::RUSH:
        newSkill = std::make_unique<RushSkill>(context, resManager, states);
        break;

    case SkillType::GUST:
        newSkill = std::make_unique<GustSkill>(context, resManager, states);
        break;

	case SkillType::TORNADO:
		newSkill = std::make_unique<TornadoSkill>(context, resManager, states);
		break;

    default:
        return nullptr;
    }

    return newSkill;
}
