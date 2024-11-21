#include "sprite.h"
#include <fstream>
#include <vector>
#pragma warning( disable : 4996 )
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"
Sprite::Sprite()
{
	ID3D11Device* device = Graphics::getInstance().getDevice();

	HRESULT hr = S_OK;
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(Vertex) * 4;
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&buffer_desc, nullptr, vertex_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), "Create vertex buffer failed");
	}
	{
		std::ifstream fin("Shader/spriteVS.cso", std::ios::in | std::ios::binary);

		fin.seekg(0, std::ios_base::end);
		long long size = fin.tellg();
		fin.seekg(0, std::ios::ios_base::beg);

		std::unique_ptr<char[]>data = std::make_unique<char[]>(size);
		fin.read(data.get(), size);
		HRESULT hr = device->CreateVertexShader(data.get(), size, nullptr, vertex_shader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), "Create vertex shader failed");
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
		};
		hr = device->CreateInputLayout(input_element_desc, ARRAYSIZE(input_element_desc), data.get(), size, input_layout.GetAddressOf());
	}
	{
		std::ifstream fin("Shader/spritePS.cso", std::ios::in | std::ios::binary);

		fin.seekg(0, std::ios_base::end);
		long long size = fin.tellg();
		fin.seekg(0, std::ios::ios_base::beg);

		std::unique_ptr<char[]>data = std::make_unique<char[]>(size);
		fin.read(data.get(), size);
		HRESULT hr = device->CreatePixelShader(data.get(), size, nullptr, pixel_shader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), "Create pixel shader failed");
	}
}
void Sprite::render()
{
	ID3D11DeviceContext* device_context = Graphics::getInstance().getDeviceContext();
	DirectX::XMFLOAT2 positions[] = {
		DirectX::XMFLOAT2(-0.5f,0.5f),
		DirectX::XMFLOAT2(0.5f,0.5f),
		DirectX::XMFLOAT2(-0.5f,-0.5f),
		DirectX::XMFLOAT2(0.5f,-0.5f)
	};
	DirectX::XMFLOAT4 colors[] = {
		DirectX::XMFLOAT4(1,0,0,1),
		DirectX::XMFLOAT4(0,1,0,1),
		DirectX::XMFLOAT4(0,0,1,1),
		DirectX::XMFLOAT4(1,1,1,1)
	};
	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	HRESULT hr = device_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), "map failed");
	Vertex* v = static_cast<Vertex*>(mapped_subresource.pData);
	for (int i = 0; i < 4; i++)
	{
		v[i].position.x = positions[i].x;
		v[i].position.y = positions[i].y;
		v[i].position.z = 0.0f;

		v[i].color.x = colors[i].x;
		v[i].color.y = colors[i].y;
		v[i].color.z = colors[i].z;
		v[i].color.w = colors[i].w;
	}
	device_context->Unmap(vertex_buffer.Get(), 0);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	device_context->IASetInputLayout(input_layout.Get());
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	device_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	device_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	device_context->Draw(3,0);
}
void Sprite::print(std::string filename )
{
	ID3D11DeviceContext* device_context = Graphics::getInstance().getDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	HRESULT hr = device_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), "map failed");
	Vertex* v = static_cast<Vertex*>(mapped_subresource.pData);

	device_context->Unmap(vertex_buffer.Get(), 0);

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	device_context->RSGetViewports(&num_viewports, &viewport);
	int iPixelWidth = viewport.Width * 0.5f * (v[1].position.x - v[0].position.x);
	int iPixelHeight = viewport.Height * 0.5f * (v[0].position.y - v[2].position.y);//PNGファイルの高さ（ピクセル単位）
	std::vector<unsigned char> vecData(iPixelWidth * iPixelHeight * 4);
	//イメージデータ格納
	for (int j = 0; j < iPixelHeight; j++)
	{
		DirectX::XMFLOAT4 temp[2];
		temp[0].x = v[0].color.x + j * (v[2].color.x - v[0].color.x) / iPixelHeight;
		temp[0].y = v[0].color.y + j * (v[2].color.y - v[0].color.y) / iPixelHeight;
		temp[0].z = v[0].color.z + j * (v[2].color.z - v[0].color.z) / iPixelHeight;
		temp[0].w = v[0].color.w + j * (v[2].color.w - v[0].color.w) / iPixelHeight;
		temp[1].x = v[1].color.x + j * (v[2].color.x - v[1].color.x) / iPixelHeight;
		temp[1].y = v[1].color.y + j * (v[2].color.y - v[1].color.y) / iPixelHeight;
		temp[1].z = v[1].color.z + j * (v[2].color.z - v[1].color.z) / iPixelHeight;
		temp[1].w = v[1].color.w + j * (v[2].color.w - v[1].color.w) / iPixelHeight;
		for (int i = 0; i < iPixelWidth; i++)
		{
			int iP = (j * iPixelWidth + i) * 4;

			vecData[iP] = (temp[0].x + i * (temp[1].x - temp[0].x) / iPixelWidth) * 255;
			vecData[iP + 1] = (temp[0].y + i * (temp[1].y - temp[0].y) / iPixelWidth) * 255;
			vecData[iP + 2] = (temp[0].z + i * (temp[1].z - temp[0].z) / iPixelWidth) * 255;
			vecData[iP + 3] = (temp[0].w + i * (temp[1].w - temp[0].w) / iPixelWidth) * 255;
		}
	}
	
	int index = 1;
	int length = filename.length();
	std::string temp_filename = filename;
	while (1)
	{
		std::ifstream file_check(temp_filename +".png");
		if (!file_check.good())
			break;
		temp_filename = filename+ "(" + std::to_string(index++) + ")";
	}
	temp_filename += ".png";
	stbi_write_png(temp_filename.c_str(), iPixelWidth, iPixelHeight, 3, &vecData.front(), 0);
}