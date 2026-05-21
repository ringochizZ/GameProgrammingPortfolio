/**
 * @file   FloorPrimitive.h
 *
 * @brief  床に関するヘッダファイル
 *
 * @author 小野島樹里
 *
 * @date   2026.05.21
 */

// 多重インクルードの防止 =====================================================
#pragma once

// ヘッダファイルの読み込み ===================================================

// クラスの定義 ===============================================================
/**
 * @brief 床
 */
class FloorPrimitive
{
// 定数の宣言 --------------------------------------------------------------
private:
	static constexpr int VERTICES_NUM = 4; // 頂点数

	static constexpr float POSITION = 10.0f;   // 床の座標
	static constexpr float SIZE     = 10.0f;   // 床のサイズ

	static constexpr float TEXTURE_SCALE = 1.0f;   // テクスチャのスケール

	// 頂点情報（板ポリゴンの頂点）
    const DirectX::VertexPositionTexture VERTICES[VERTICES_NUM] =
    {
        DirectX::VertexPositionTexture(
            DirectX::SimpleMath::Vector3(POSITION, 0.0f, -POSITION),
            DirectX::SimpleMath::Vector2(SIZE / 2.0f, 0.0f)),		  // 0:左上
        DirectX::VertexPositionTexture(
            DirectX::SimpleMath::Vector3(POSITION, 0.0f,  POSITION),
            DirectX::SimpleMath::Vector2(SIZE / 2.0f, SIZE / 2.0f)),  // 1:右上
        DirectX::VertexPositionTexture(
            DirectX::SimpleMath::Vector3(-POSITION, 0.0f,  POSITION),
            DirectX::SimpleMath::Vector2(0.0f, SIZE / 2.0f)),		  // 3:右下
        DirectX::VertexPositionTexture(
            DirectX::SimpleMath::Vector3(-POSITION, 0.0f, -POSITION),
            DirectX::SimpleMath::Vector2(0.0f, 0.0f)),				  // 2:左下
    };

// 変数の宣言 --------------------------------------------------------------
private:
	// 座標
    DirectX::SimpleMath::Vector3 m_position;

    //  入力レイアウト 
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
    
    //  共通ステートオブジェクトへのポインタ
    std::unique_ptr<DirectX::CommonStates> m_States;
    
    //  テクスチャハンドル 
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Texture;
    
    //  エフェクト 
    std::unique_ptr<DirectX::AlphaTestEffect> m_BatchEffect;
    
    //  プリミティブバッチ 
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_Batch;

// メンバ関数の宣言 -------------------------------------------------
public:
    FloorPrimitive(ID3D11Device1* device);
    ~FloorPrimitive();

// 操作
public:
	// 描画処理
    void Render(
        ID3D11DeviceContext1* context, 
        DirectX::SimpleMath::Matrix view, 
        DirectX::SimpleMath::Matrix proj,
        float currentSize);
    
    // 終了処理
    void Finalize();
    
};
