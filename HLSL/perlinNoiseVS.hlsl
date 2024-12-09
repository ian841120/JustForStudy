#include "perlinNoise.hlsli"
VS_OUT main( float4 position : POSITION)
{
    VS_OUT vout;
    vout.position = mul(position, wp);
	return vout;
}