/**
 * @file   FloorPrimitive.cpp
 *
 * @brief  床に関するソースファイル
 *
 * @author 小野島 樹里
 *
 * @date   2026.05.21
 */

// ヘッダファイルの読み込み ===================================================
#include "pch.h"
#include "FloorPrimitive.h"

// メンバ関数の定義 ===========================================================
/**
 * @brief コンストラクタ
 *
 * @param[in] なし
 */
FloorPrimitive::FloorPrimitive(ID3D11Device1* device)
	: m_position{ DirectX::SimpleMath::Vector3::Zero }
{   
	//	エフェクトの作成 
	m_BatchEffect = std::make_unique<DirectX::AlphaTestEffect>(device);
	m_BatchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_BatchEffect->SetReferenceAlpha(255);

	//	入力レイアウト生成 
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_BatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(
		DirectX::VertexPositionTexture::InputElements,
		DirectX::VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength, m_InputLayout.GetAddressOf()
	);

	//	共通ステートの作成
	m_States = std::make_unique<DirectX::CommonStates>(device);

	//	テクスチャのロード
	DirectX::CreateWICTextureFromFile(
		device,                         
		L"Resources/Textures/floor.png", 
		nullptr,
		m_Texture.GetAddressOf()
	);

	//	テクスチャデータをBatchEffectに設定する
	m_BatchEffect->SetTexture(m_Texture.Get());
}



/**
 * @brief デストラクタ
 */
FloorPrimitive::~FloorPrimitive()
{
}



/**
 * @brief 描画処理
 *
 * @param[in] context           コンテキスト
 * @param[in] view              ビュー行列
 * @param[in] proj              プロジェ
 * @param[in] world             ワールド行列
 * @param[in] currentFloorSize  現在の床のサイズ
 *
 * @return なし
 */
void FloorPrimitive::Render(
	ID3D11DeviceContext1* context,
	DirectX::SimpleMath::Matrix view, 
	DirectX::SimpleMath::Matrix proj, 
	float currentSize)
{
	//	プリミティブバッチの作成
	m_Batch =
		std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>>(context);

	//	頂点情報（板ポリゴンの頂点） 
	DirectX::VertexPositionTexture vertex[VERTICES_NUM];
	for (int i = 0; i < VERTICES_NUM; i++)
	{
		vertex[i] = VERTICES[i];
	}


	//	テクスチャサンプラーの設定
	ID3D11SamplerState* samplers[1] = { m_States->PointWrap() };
	context->PSSetSamplers(0, 1, samplers);

	//	深度バッファに書き込み参照する 
	context->OMSetDepthStencilState(m_States->DepthDefault(), 0);

	//	裏面は描画しない
	context->RSSetState(m_States->CullCounterClockwise());


	//	不透明のみ描画する設定 
	m_BatchEffect->SetAlphaFunction(D3D11_COMPARISON_NOT_EQUAL);
	m_BatchEffect->SetReferenceAlpha(0);
	m_BatchEffect->SetWorld(DirectX::SimpleMath::Matrix::Identity);
	m_BatchEffect->SetView(view);
	m_BatchEffect->SetProjection(proj);
	m_BatchEffect->Apply(context);
	context->IASetInputLayout(m_InputLayout.Get());

	// 初期サイズに合わせた倍率を計算
	float scale = currentSize / SIZE;
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(scale, TEXTURE_SCALE, scale);

	m_BatchEffect->SetWorld(world);
	m_BatchEffect->SetView(view);
	m_BatchEffect->SetProjection(proj);
	m_BatchEffect->Apply(context);

	//	半透明部分が含まれることを想定した描画 
	m_Batch->Begin();
	m_Batch->DrawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
	m_Batch->End();
}



/**
 * @brief 終了処理
 *
 * @param[in] なし
 *
 * @return なし
 */
void FloorPrimitive::Finalize()
{

}
