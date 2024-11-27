#include "sprite.hlsli"
Texture2D spriteTexture : register(t0);
SamplerState spriteSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return spriteTexture.Sample(spriteSampler, pin.texcoord) * pin.color;
}