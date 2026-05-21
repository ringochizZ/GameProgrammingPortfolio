/**
 * @file   PlayerController.cpp
 *
 * @brief  プレイヤーの操作に関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.04.09
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "PlayerController.h"

#include "Game/GameObjects/Character.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief 更新処理
 *
 * @param[in] character   キャラクター
 * @param[in] perception  周囲の状況
 * @param[in] alapsedTime 経過時間
 *
 * @return なし
 */
void PlayerController::Controller(
    Character& character, 
    const PerceptionData& perception,
    float elapsedTime)
{
    UNREFERENCED_PARAMETER(perception);
	UNREFERENCED_PARAMETER(elapsedTime);

    auto kb = DirectX::Keyboard::Get().GetState();
    static DirectX::Keyboard::KeyboardStateTracker tracker;

    // キーの状態を更新
    tracker.Update(kb);

    // 移動指示
    DirectX::SimpleMath::Vector3 moveDir = GetInputDirection();
    if (moveDir.LengthSquared() > 0)
    {
        // 移動入力の追加
        character.AddInputMovement(moveDir, SPEED);
    }

    // スキル使用指示
    if (tracker.pressed.Z) character.RequestSkill(0);
    if (tracker.pressed.X) character.RequestSkill(1);
    if (tracker.pressed.C) character.RequestSkill(2);
}



/**
 * @brief 入力をベクトルに変換する補助関数
 *
 * @param[in] なし
 *
 * @return dir 入力方向のベクトル
 */
DirectX::SimpleMath::Vector3 PlayerController::GetInputDirection()
{
    auto kb = DirectX::Keyboard::Get().GetState();
    DirectX::SimpleMath::Vector3 dir = DirectX::SimpleMath::Vector3::Zero;

    if (kb.Down)  dir.z += 1.0f;
    if (kb.Up)    dir.z -= 1.0f;
    if (kb.Left)  dir.x -= 1.0f;
    if (kb.Right) dir.x += 1.0f;

    if (dir.LengthSquared() > 0) dir.Normalize();
    return dir;
}
