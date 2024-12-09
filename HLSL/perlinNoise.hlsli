struct VS_OUT
{
    float4 position : SV_POSITION;
};
cbuffer CBuffer : register(b0)
{
    matrix wp;
    int octaves;
    float lacunarity;
    float gain;
    float fx;
    float fy;
    float3 padding;
    int p[256];
    
};
float fade(float t)
{
	//6*t^5-15*t^4+10*t^3
    return 6 * t * t * t * t * t - 15 * t * t * t * t + 10 * t * t * t;
}
float lerp(float t, float a, float b)
{
    return a + t * (b - a);
}
float grad(int hash, float x, float y, float z)
{
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
float map(float val, float ogMin, float ogMax, float newMin, float newMax)
{
    float prop = (val - ogMin) / (ogMax - ogMin);
    return lerp(prop, newMin, newMax);
}
float noise2D(float x, float y)
{
    uint xi = (uint) (floor(x)) & 255;
    uint yi = (uint) (floor(y)) & 255;
    x = frac(x);
    y = frac(y);
    float u = fade(x);
    float v = fade(y);
    //int aa = p[p[xi] + yi];
    //int ab = p[p[xi] + yi + 1];
    //int ba = p[p[xi + 1] + yi];
    //int bb = p[p[xi + 1] + yi + 1];
    int aa = p[(p[xi % 255] + yi) % 255];
    int ab = p[(p[xi % 255] + yi+1) % 255];
    int ba = p[(p[(xi + 1) % 255] + yi) % 255];
    int bb = p[(p[(xi + 1) % 255] + yi + 1) % 255];
    float average = lerp(v, lerp(u, grad(aa, x, y, 0), grad(ba, x - 1, y, 0)), lerp(u, grad(ab, x, y - 1, 0), grad(bb, x - 1, y - 1, 0)));
    return map(average, -1, 1, 0, 1);
}
float accumulatedNoise2D(float x, float y, int octaves, float lacunarity, float gain)
{
    float result = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    for (; octaves > 0; octaves--)
    {
        result += noise2D(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= gain;
        frequency *= lacunarity;
    }
    return result / maxValue;
}