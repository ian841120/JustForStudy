#pragma once
#include <directXmath.h>
#include <d3d11.h>
#include <wrl.h>
#include <algorithm>
#include <random>
#include <string>
class PerlinNoise
{
public:
	PerlinNoise(unsigned int  seed);
	~PerlinNoise() {};
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
	};
	struct P
	{
		int value;
		DirectX::XMFLOAT3 padding;
	};
	struct CBuffer
	{
		DirectX::XMMATRIX wp;
		int octaves;
		float lacunarity;
		float gain;
		float fx;
		float fy;
		DirectX::XMFLOAT3 padding;
		P p[256];
		
	};
	void reseed(unsigned int s);
	void createImage(int height, int width);
	void render(int octaves = 8, float lacunarity = 2.0f, float gain = 0.5f);
	void print(std::string filename = "sample", int octaves = 8, float lacunarity = 2.0f, float gain = 0.5f);
	void setPixel(float x, float y, float r, float g, float b);
	float noise1D(float x);
	float noise2D(float x, float y);
	float accumulatedNoise2D(float x, float y, int octaves = 8, float lacunarity = 2.0f, float gain = 0.5f);
	float noise3D(float z);
	float getHeight()const { return this->height; }
	float getWidth()const { return this->width; }

private:
	int permutation[256] =
	{
		151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		index_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	float fade(float t);
	float lerp(float t, float a, float b);
	float grad(int hash, float x, float y, float z);
	float map(float val, float ogMin, float ogMax, float newMin, float newMax);
	int p[256];
	unsigned int seed;
	UINT index_count;
	float width;
	float height;
	double frequency = 8;
};
