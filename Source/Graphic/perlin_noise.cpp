#include "perlin_noise.h"
#include "gpuResourceUtils.h"
#include "graphics.h"
#include <vector>
#pragma warning( disable : 4996 )
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"
PerlinNoise::PerlinNoise(unsigned int  seed)
{	
	reseed(seed);
}
void PerlinNoise::reseed(unsigned int s) 
{
	this->seed = s;
	for (int i = 0; i < 256; i++)
	{
		p[i] = i;
	}
	std::shuffle(std::begin(p), std::end(p), std::default_random_engine(this->seed));
}
void PerlinNoise::createImage(int height, int width)
{
	this->height = static_cast<float>(height);
	this->width = static_cast<float>(width);

	HRESULT hr = S_OK;
	ID3D11Device* device = Graphics::getInstance().getDevice();
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
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
			vertex.position.x = static_cast<float>(j);
			vertex.position.y = static_cast<float>(i);
			vertex.position.z = 0.0f;
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
	{
		D3D11_BUFFER_DESC desc;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(CBuffer);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		HRESULT hr = device->CreateBuffer(&desc, nullptr, constant_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), L"Create constant buffer failed");
	}

}
void PerlinNoise::setPixel(float x, float y, float r, float g, float b)
{
	ID3D11DeviceContext* device_context = Graphics::getInstance().getDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	HRESULT hr = device_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), L"map failed");
	Vertex* v = static_cast<Vertex*>(mapped_subresource.pData);
	int temp = (int)(y * width + x);
	//v[temp].color = { r,g,b,1.0f };
	device_context->Unmap(vertex_buffer.Get(), 0);
}
void PerlinNoise::render(int octaves, float lacunarity, float gain)
{
	ID3D11DeviceContext* device_context = Graphics::getInstance().getDeviceContext();
	device_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());
	device_context->PSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	device_context->RSGetViewports(&num_viewports, &viewport);
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	// ワールドマトリックスの初期化
	DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();   
	// プロジェクションマトリックスの初期化(射影行列変換)
	DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, screen_width, screen_height, 0.0f, 0.0f, 1.0f);
	CBuffer cbuffer;
	cbuffer.wp= DirectX::XMMatrixTranspose(world * projection);
	cbuffer.octaves = octaves;
	cbuffer.gain = gain;
	cbuffer.lacunarity = lacunarity;
	cbuffer.fx = this->width / (float)this->frequency;
	cbuffer.fy = this->height / (float)this->frequency;
	for (int i = 0; i < 256; i++)
	{
		cbuffer.p[i].value = p[i];
	}
	device_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &cbuffer, 0, 0);



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
	int b = p[(xi + 1) % 255];
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
	int aa = p[(p[xi % 255] + yi) % 255];
	int ab = p[(p[xi % 255] + yi + 1) % 255];
	int ba = p[(p[(xi + 1) % 255] + yi) % 255];
	int bb = p[(p[(xi + 1) % 255] + yi + 1) % 255];

	float average = lerp(v, lerp(u, grad(aa, x, y, 0), grad(ba, x - 1, y, 0)), lerp(u, grad(ab, x, y - 1, 0), grad(bb, x - 1, y - 1, 0)));
	return map(average, -1, 1, 0, 1);
}
float PerlinNoise::accumulatedNoise2D(float x, float y, int octaves , float lacunarity , float gain)
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
void PerlinNoise::print(std::string filename,int octaves,float lacunarity,float gain)
{
	ID3D11DeviceContext* device_context = Graphics::getInstance().getDeviceContext();

	std::vector<unsigned char> vecData((int)this->width * (int)this->height * 4);
	//イメージデータ格納
	for (int j = 0; j < this->height; j++)
	{
		for (int i = 0; i < this->width; i++)
		{
			int iP = (j * this->width + i) * 4;
			float fx = this->width / (float)this->frequency;
			float fy = this->height / (float)this->frequency;
			double color = accumulatedNoise2D(i / fx, j / fy, octaves, lacunarity, gain);
			vecData[iP] = color * 255;
			vecData[iP + 1] = color * 255;
			vecData[iP + 2] = color * 255;
			vecData[iP + 3] = 255;
		}
	}

	int index = 1;
	int length = filename.length();
	std::string temp_filename = filename;
	while (1)
	{
		std::ifstream file_check(temp_filename + ".png");
		if (!file_check.good())
			break;
		temp_filename = filename + "(" + std::to_string(index++) + ")";
	}
	temp_filename += ".png";
	stbi_write_png(temp_filename.c_str(), this->width, this->height, 4, &vecData.front(), 0);
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