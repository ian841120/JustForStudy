#include "perlin_noise.h"
#include "gpuResourceUtils.h"
#include "graphics.h"
#include <vector>
PerlinNoise::PerlinNoise(unsigned int  seed)
{	
	reseed(seed);
}
void PerlinNoise::reseed(unsigned int s) 
{
	this->seed = s;
	for (unsigned int i = 0; i < 256; i++)
	{
		p[i] = i;
	}
	std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(this->seed));
	for (unsigned int i = 0; i < 256; i++)
	{
		p[256 + i] = p[i];
	}
}
void PerlinNoise::createMesh(int height, int width)
{
	HRESULT hr = S_OK;
	ID3D11Device* device = Graphics::getInstance().getDevice();
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
	};
	hr = GpuResourceUtils::loadVertexShader(device, "Shader/perlinNoiseVS.cso", vertex_shader.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc), input_layout.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), L"Create vertex shader failed");
	hr = GpuResourceUtils::loadPixelShader(device, "Shader/perlinNoisePS.cso", pixel_shader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), L"Create pixel shader failed");
	


	ID3D11DeviceContext* device_context = Graphics::getInstance().getDeviceContext();

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	device_context->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Vertex vertex;
			vertex.position.x = 2.0f * static_cast<float>(j) / screen_width - 1.0f; 
			vertex.position.y = 1.0f - 2.0f * static_cast<float>(i) / screen_height;
			vertex.position.z = 0.0f;
			vertex.color.x = 1.0f;
			vertex.color.y = 1.0f;
			vertex.color.z = 1.0f;
			vertex.color.w = 1.0f;
			vertices.emplace_back(vertex);
		}
	}
	for (int i = 0; i < height - 1; i++)
	{
		for (int j = 0; j < width - 1; j++)
		{
			indices.emplace_back(i * height + j);
			indices.emplace_back(i * height + j + 1);
			indices.emplace_back(i * height + j + width);

			indices.emplace_back(i * height + j + 1);
			indices.emplace_back(i * height + j + width + 1);
			indices.emplace_back(i * height + j + width);
		}
	}
	index_count = static_cast<UINT>(indices.size());

	{
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA subresource_data{};
		subresource_data.pSysMem = vertices.data();
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;
		HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), L"Create vertex buffer failed");
		buffer_desc.ByteWidth = static_cast<UINT>(indices.size() * sizeof(UINT));
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresource_data.pSysMem = indices.data();
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), L"Create index buffer failed");
	}
}
void PerlinNoise::render()
{
	ID3D11DeviceContext* device_context = Graphics::getInstance().getDeviceContext();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	device_context->IASetInputLayout(input_layout.Get());
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	device_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	device_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	device_context->DrawIndexed(index_count, 0, 0);
}
float PerlinNoise::noise1D(float x)
{
	int xi = (int)(std::floorf(x)) & 255;
	x -= std::floorf(x);
	float u = fade(x);
	int a = p[xi];
	int b = p[xi + 1];

	float average = lerp(u, grad(a, x, 0, 0), grad(a, x - 1, 0, 0));

	return map(average, -1, 1, 0, 1);
}
float PerlinNoise::noise2D(float x, float y)
{
	int xi = (int)(std::floorf(x)) & 255;
	int yi = (int)(std::floorf(y)) & 255;
	x -= std::floorf(x);
	y -= std::floorf(y);
	float u = fade(x);
	float v = fade(y);
	int aa = p[p[xi] + yi];
	int ab = p[p[xi] + yi + 1];
	int ba = p[p[xi + 1] + yi];
	int bb = p[p[xi + 1] + yi + 1];

	float average = lerp(v, lerp(u, grad(aa, x, y, 0), grad(ba, x - 1, y, 0)), lerp(u, grad(ab, x, y - 1, 0), grad(bb, x - 1, y - 1, 0)));
	return map(average, -1, 1, 0, 1);
}
inline float PerlinNoise::fade(float t)
{
	//6*t^5-15*t^4+10*t^3
	return 6 * t * t * t * t * t - 15 * t * t * t * t + 10 * t * t * t;
}
inline float PerlinNoise::lerp(float t, float a, float b)
{
	return a + t * (b - a);
}
inline float PerlinNoise::grad(int hash, float x, float y, float z)
{
	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
inline float PerlinNoise::map(float val, float ogMin, float ogMax, float newMin, float newMax)
{
	float prop = (val - ogMin) / (ogMax - ogMin);
	return lerp(prop, newMin, newMax);
}