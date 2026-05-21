// TornadoPS.hlsl
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
};

float4 main(PS_INPUT input) : SV_Target
{
    // ひとまず動作確認のために不透明な白（または好きな色）を返す
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}