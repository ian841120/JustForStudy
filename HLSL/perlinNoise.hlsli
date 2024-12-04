struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
cbuffer cbBuffer
{
    uint p[512];
    int octaves;
    float lacunarity;
    float gain;
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
    
    int xi = (int) (floor(x)) & 255;
    int yi = (int) (floor(y)) & 255;
    x -= floor(x);
    y -= floor(y);
    float u = fade(x);
    float v = fade(y);
    int aa = p[p[xi] + yi];
    int ab = p[p[xi] + yi + 1];
    int ba = p[p[xi + 1] + yi];
    int bb = p[p[xi + 1] + yi + 1];

    float average = lerp(v, lerp(u, grad(aa, x, y, 0), grad(ba, x - 1, y, 0)), lerp(u, grad(ab, x, y - 1, 0), grad(bb, x - 1, y - 1, 0)));
    return map(average, -1, 1, 0, 1);
}
