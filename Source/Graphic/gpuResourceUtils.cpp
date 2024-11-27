#include "gpuResourceUtils.h"
#include <wrl.h>
#include <DirectXTex.h>
#include <filesystem>
HRESULT GpuResourceUtils::loadVertexShader(ID3D11Device*device,const char* filename,ID3D11VertexShader** vertex_shader, D3D11_INPUT_ELEMENT_DESC input_element_desc[],UINT input_element_desc_num, ID3D11InputLayout**input_layout)
{
	std::ifstream fin(filename, std::ios::in | std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	long long size = fin.tellg();
	fin.seekg(0, std::ios::ios_base::beg);

	std::unique_ptr<char[]>data = std::make_unique<char[]>(size);
	fin.read(data.get(), size);
	HRESULT hr = device->CreateVertexShader(data.get(), size, nullptr, vertex_shader);
	hr = device->CreateInputLayout(input_element_desc, input_element_desc_num, data.get(), size, input_layout);
	return hr;
}
HRESULT GpuResourceUtils::loadPixelShader(ID3D11Device* device, const char* filename, ID3D11PixelShader** pixel_shader)
{
	std::ifstream fin(filename, std::ios::in | std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	long long size = fin.tellg();
	fin.seekg(0, std::ios::ios_base::beg);

	std::unique_ptr<char[]>data = std::make_unique<char[]>(size);
	fin.read(data.get(), size);
	HRESULT hr = device->CreatePixelShader(data.get(), size, nullptr, pixel_shader);
	return hr;
}
HRESULT GpuResourceUtils::loadTexture(ID3D11Device* device,const char* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{

	//Get extension
	std::filesystem::path filepath(filename);
	std::string extension = filepath.extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(), tolower);

	//Change to wide character
	std::wstring wfilename = filepath.wstring();
	//Load image

	HRESULT hr{ S_OK };

	DirectX::TexMetadata metaData;
	DirectX::ScratchImage scratchImage;
	if (extension == ".tga")
	{
		hr = DirectX::GetMetadataFromTGAFile(wfilename.c_str(), metaData);
		_ASSERT_EXPR(SUCCEEDED(hr), L"Get meta data failed");
		hr = DirectX::LoadFromTGAFile(wfilename.c_str(), &metaData, scratchImage);
		_ASSERT_EXPR(SUCCEEDED(hr), L"Load TGA file failed");
	}
	else if (extension == ".dds")
	{
		hr = DirectX::GetMetadataFromDDSFile(wfilename.c_str(), DirectX::DDS_FLAGS_NONE, metaData);
		_ASSERT_EXPR(SUCCEEDED(hr), L"Get meta data failed");
		hr = DirectX::LoadFromDDSFile(wfilename.c_str(), DirectX::DDS_FLAGS_NONE, &metaData, scratchImage);
		_ASSERT_EXPR(SUCCEEDED(hr), L"Load DDS file failed");
	}
	else if (extension == ".hdr")
	{
		hr = DirectX::GetMetadataFromHDRFile(wfilename.c_str(), metaData);
		_ASSERT_EXPR(SUCCEEDED(hr), L"Get meta data failed");
		hr = DirectX::LoadFromHDRFile(wfilename.c_str(), &metaData, scratchImage);
		_ASSERT_EXPR(SUCCEEDED(hr), L"Load DDS file failed");
	}
	else
	{
		hr = DirectX::GetMetadataFromWICFile(wfilename.c_str(), DirectX::WIC_FLAGS_NONE, metaData);
		_ASSERT_EXPR(SUCCEEDED(hr), L"Get meta data failed");
		hr = DirectX::LoadFromWICFile(wfilename.c_str(), DirectX::WIC_FLAGS_NONE, &metaData, scratchImage);
		_ASSERT_EXPR(SUCCEEDED(hr), L"Load DDS file failed");

	}
	//Create shader resouce view 
	hr = DirectX::CreateShaderResourceView(device, scratchImage.GetImages(), scratchImage.GetImageCount(), metaData, shader_resource_view);
	_ASSERT_EXPR(SUCCEEDED(hr), L"Create shader resource view failed");
	if (texture2d_desc != nullptr)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource>resource;
		(*shader_resource_view)->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D>texture2d;
		hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), L"Get texture2d resource failed");
		texture2d->GetDesc(texture2d_desc);
	}
	return hr;
}
HRESULT GpuResourceUtils::createDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA data{};
	UINT color = 0xFFFFFFFF;
	data.pSysMem = &color;
	data.SysMemPitch = desc.Width*4;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), L"Create texture2D failed");
	hr = device->CreateShaderResourceView(texture.Get(), nullptr, shader_resource_view);
	_ASSERT_EXPR(SUCCEEDED(hr), L"Create shader resource view failed");
	if (texture2d_desc != nullptr)
	{
		Microsoft::WRL::ComPtr<ID3D11Resource>resource;
		(*shader_resource_view)->GetResource(resource.GetAddressOf());
		Microsoft::WRL::ComPtr<ID3D11Texture2D>texture2d;
		hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), L"Get texture2d resource failed");
		texture2d->GetDesc(texture2d_desc);
	}

	return hr;
}