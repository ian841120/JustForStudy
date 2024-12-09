#include "perlinNoise.hlsli"
float4 main(VS_OUT pin) : SV_TARGET
{
    double c = accumulatedNoise2D(pin.position.x/fx , pin.position.y/fy , octaves, lacunarity, gain);
    float4 color = { c, c, c, 1.0f };
    return color;
}