/**
 * @file   SkillFactory.h
 *
 * @brief  スキルの生成に関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include <memory>

// クラスの宣言 ===============================================================
class Skill;
class UserResources;

// 列挙型の定義 ===============================================================
/**
 * @brief スキルの種類（ID）
 */
enum class SkillType
{
	RUSH,    // 突進
	GUST,    // 突風
	TORNADO, // 竜巻
};

// 名前空間の定義 =============================================================
namespace SkillFactory  
{  
   std::unique_ptr<Skill> Create(  
       SkillType type,  
       UserResources* userRes);  
}
