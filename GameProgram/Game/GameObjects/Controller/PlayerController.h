/**
 * @file   PlayerController.h
 *
 * @brief  プレイヤーの操作に関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================
#include "IControll.h"

// クラスの定義 ===============================================================
/**
 * @brief プレイヤーコントローラー
 */
class PlayerController : public IControll
{
// 定数の宣言 --------------------------------------------------------------
private:
    static constexpr float SPEED = 5.0f;  // 移動速度
    
// メンバ関数の宣言 --------------------------------------------------------
public:
	PlayerController() = default;
	~PlayerController() override = default;

// 操作
public:
    // 更新処理
    void Controller(
        Character& character, 
        const PerceptionData& perception,
        float elapsedTime) override;

// 取得/設定
private:
    // 入力をベクトルに変換する補助関数
    DirectX::SimpleMath::Vector3 GetInputDirection();

};
