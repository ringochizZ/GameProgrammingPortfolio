/**
 * @file   SituationScanner.h
 *
 * @brief  敵の周辺状況を調べるヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.04.15
 */

 // 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include <vector>
#include "Game/GameObjects/Character.h"

// 構造体の定義 ===============================================================
/**
 * @brief 周辺状況
 */
struct PerceptionData
{
   int nearbyCount  = 0;                         // 射程内にいる人数
   int forwardCount = 0;                         // 前方にいる人数
   float nearestDistance = 0.0f;                 // 一番近いキャラまでの距離
   DirectX::SimpleMath::Vector3 nearestPosition; // 一番近いキャラの位置
};


// 特定のキャラクターの周りを調べて、結果を構造体で返す
static PerceptionData ScanAround(
    Character* self, 
    const std::vector<std::unique_ptr<Character>>& characters)
{
    PerceptionData data;

    for (auto& target : characters)
    {
        if (target.get() == self || !target->IsActive()) continue;

        float dist = DirectX::SimpleMath::Vector3::Distance(self->GetPosition(), target->GetPosition());

        // 一番近いキャラの情報を更新
        if (dist < data.nearestDistance)
        {
            data.nearestDistance = dist;
            data.nearestPosition = target->GetPosition(); // 座標を記録
        }

        // 近くに何人いるかカウント
        if (dist < 5.0f) data.nearbyCount++;
    }
    return data;
}
