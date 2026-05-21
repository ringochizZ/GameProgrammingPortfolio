#include "pch.h"
#include "DisplayCollision.h"

#ifdef _COLLISION_LINE_ON
#include "../ThirdParty/DebugDraw.h"
#endif

using namespace DirectX;

DisplayCollision::DisplayCollision(ID3D11Device* device, ID3D11DeviceContext* context, bool modelActive, bool lineActive, uint32_t collisionMax)
    : m_modelActive(modelActive)
    , m_lineActive(lineActive)
    , m_collisionMax(collisionMax)
{
    m_modelSphere = GeometricPrimitive::CreateSphere(context, 2.0f, 8);
    m_modelBox = GeometricPrimitive::CreateCube(context);

    m_modelEffect = std::make_unique<NormalMapEffect>(device);
    m_modelEffect->SetVertexColorEnabled(false);
    m_modelEffect->SetBiasedVertexNormals(false);
    m_modelEffect->SetInstancingEnabled(true);
    m_modelEffect->SetFogEnabled(false);
    m_modelEffect->SetTexture(nullptr);
    m_modelEffect->DisableSpecular();
    m_modelEffect->EnableDefaultLighting();
    m_modelEffect->SetWorld(SimpleMath::Matrix::Identity);

    m_lineEffect = std::make_unique<BasicEffect>(device);
    m_lineEffect->SetVertexColorEnabled(true);
    m_lineEffect->SetTextureEnabled(false);
    m_lineEffect->SetLightingEnabled(false);
    m_lineEffect->SetWorld(SimpleMath::Matrix::Identity);

    const D3D11_INPUT_ELEMENT_DESC c_InputElements[] =
    {
        { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "InstMatrix",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "InstMatrix",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "InstMatrix",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    DX::ThrowIfFailed(CreateInputLayoutFromEffect(device, m_modelEffect.get(), c_InputElements, std::size(c_InputElements), m_modelInputLayout.ReleaseAndGetAddressOf()));

    auto desc = CD3D11_BUFFER_DESC(static_cast<UINT>(DISPLAY_COLLISION_MAX * sizeof(XMFLOAT3X4)),
        D3D11_BIND_VERTEX_BUFFER,
        D3D11_USAGE_DYNAMIC,
        D3D11_CPU_ACCESS_WRITE);
    DX::ThrowIfFailed(device->CreateBuffer(&desc, nullptr, m_instancedVB.ReleaseAndGetAddressOf()));

#ifdef _COLLISION_LINE_ON
    m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);
    DX::ThrowIfFailed(CreateInputLayoutFromEffect<VertexPositionColor>(device, m_lineEffect.get(), m_lineInputLayout.ReleaseAndGetAddressOf()));
#endif
}

void DisplayCollision::DrawCollision(ID3D11DeviceContext* context, CommonStates* states, const SimpleMath::Matrix& view, const SimpleMath::Matrix& proj, FXMVECTOR baseColor, FXMVECTOR lineColor, float alpha)
{
    SimpleMath::Color color = baseColor;
    color.w = alpha;

    if (m_modelActive)
        DrawCollisionModel(context, states, view, proj, color);

#ifdef _COLLISION_LINE_ON
    SimpleMath::Color c = lineColor;
    if (c.w != 0.0f) color = lineColor;

    if (m_lineActive)
        DrawCollisionLine(context, states, view, proj);
#else
    UNREFERENCED_PARAMETER(lineColor);
#endif

    m_spheres.clear();
    m_boxes.clear();
}

void DisplayCollision::AddBox(const SimpleMath::Vector3& center, const SimpleMath::Vector3& extents, const SimpleMath::Quaternion& rotation, const SimpleMath::Color& lineColor)
{
    m_boxes.push_back({ center, extents, rotation, lineColor });
}

void DisplayCollision::AddSphere(const SimpleMath::Vector3& center, float radius, const SimpleMath::Color& color)
{
    m_spheres.push_back({ center, radius, color });
}

void DisplayCollision::DrawCollisionModel(ID3D11DeviceContext* context, CommonStates* states, const SimpleMath::Matrix& view, const SimpleMath::Matrix& proj, FXMVECTOR color)
{
    assert(static_cast<uint32_t>(m_spheres.size()) <= m_collisionMax);
    assert(static_cast<uint32_t>(m_boxes.size()) <= m_collisionMax);

    // --- Sphere（球）の描画 ---
    m_modelEffect->SetColorAndAlpha(color);
    m_modelEffect->SetView(view);
    m_modelEffect->SetProjection(proj);
    m_modelEffect->Apply(context);

    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        context->Map(m_instancedVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        XMFLOAT3X4* p = static_cast<XMFLOAT3X4*>(mappedResource.pData);
        ZeroMemory(p, sizeof(XMFLOAT3X4) * m_spheres.size());

        for (size_t i = 0; i < m_spheres.size(); ++i)
        {
            p[i]._11 = p[i]._22 = p[i]._33 = m_spheres[i].radius;
            p[i]._14 = m_spheres[i].center.x;
            p[i]._24 = m_spheres[i].center.y;
            p[i]._34 = m_spheres[i].center.z;
        }

        context->Unmap(m_instancedVB.Get(), 0);
    }

    // 半透明描画のためのブレンドステート設定を追加
    context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFF);

    // 深度ステンシルも読み取りモード
    context->OMSetDepthStencilState(states->DepthRead(), 0);

    m_modelSphere->DrawInstanced(m_modelEffect.get(), m_modelInputLayout.Get(), static_cast<uint32_t>(m_spheres.size()), true, false, 0, [=]()
        {
            UINT stride = sizeof(XMFLOAT3X4);
            UINT offset = 0;
            context->OMSetDepthStencilState(states->DepthRead(), 0);
            context->IASetVertexBuffers(1, 1, m_instancedVB.GetAddressOf(), &stride, &offset);
        });
    
    context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFF);

    // --- Box（ボックス）の描画 ---
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        context->Map(m_instancedVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        XMFLOAT3X4* p = static_cast<XMFLOAT3X4*>(mappedResource.pData);
        ZeroMemory(p, sizeof(XMFLOAT3X4) * m_boxes.size());

        for (size_t i = 0; i < m_boxes.size(); ++i)
        {
            SimpleMath::Matrix s = SimpleMath::Matrix::CreateScale(m_boxes[i].extents * 2.0f);
            SimpleMath::Matrix r = SimpleMath::Matrix::CreateFromQuaternion(m_boxes[i].rotate);
            SimpleMath::Matrix t = SimpleMath::Matrix::CreateTranslation(m_boxes[i].center);
            SimpleMath::Matrix m = s * r * t;

            p[i]._11 = m._11; p[i]._12 = m._21; p[i]._13 = m._31; p[i]._14 = m._41;
            p[i]._21 = m._12; p[i]._22 = m._22; p[i]._23 = m._32; p[i]._24 = m._42;
            p[i]._31 = m._13; p[i]._32 = m._23; p[i]._33 = m._33; p[i]._34 = m._43;
        }

        context->Unmap(m_instancedVB.Get(), 0);
    }

    m_modelBox->DrawInstanced(m_modelEffect.get(), m_modelInputLayout.Get(), static_cast<uint32_t>(m_boxes.size()), true, false, 0, [=]()
        {
            UINT stride = sizeof(XMFLOAT3X4);
            UINT offset = 0;
            context->OMSetDepthStencilState(states->DepthRead(), 0);
            context->IASetVertexBuffers(1, 1, m_instancedVB.GetAddressOf(), &stride, &offset);
        });
}

#ifdef _COLLISION_LINE_ON
void DisplayCollision::DrawCollisionLine(ID3D11DeviceContext* context, CommonStates* states, const SimpleMath::Matrix& view, const SimpleMath::Matrix& proj)
{
    context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(states->DepthRead(), 0);
    context->RSSetState(states->CullNone());

    m_lineEffect->SetView(view);
    m_lineEffect->SetProjection(proj);
    m_lineEffect->Apply(context);
    context->IASetInputLayout(m_lineInputLayout.Get());

    m_primitiveBatch->Begin();

    for (const auto& s : m_spheres)
    {
        BoundingSphere bs(s.center, s.radius);
        DX::Draw(m_primitiveBatch.get(), bs, s.lineColor);
    }

    for (const auto& b : m_boxes)
    {
        BoundingOrientedBox obb(b.center, b.extents, b.rotate);
        DX::Draw(m_primitiveBatch.get(), obb, b.lineColor);
    }

    m_primitiveBatch->End();
}
#endif
