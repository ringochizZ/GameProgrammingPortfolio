/**
 * @file   IControll.h
 *
 * @brief  操作に関する基底クラス
 *
 * @author 小野島樹里
 *
 * @date   2026.04.07
 */

// 多重インクルードの防止 =====================================================
#pragma once

// クラスの宣言 ===============================================================
class Character;

// ヘッダファイルの読み込み ===================================================
#include "Game/Utilities/SituationScanner.h"

// クラスの定義 ===============================================================
/**
 * @brief コントロール
 */
class IControll
{
// メンバ関数の宣言 -------------------------------------------------
public:
    virtual ~IControll() = default;

// 操作
public:
    // 更新処理
    virtual void Controller(
        Character& character, 
        const PerceptionData& perception,
        float elapsedTime) = 0;

};
