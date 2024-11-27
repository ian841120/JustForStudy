#pragma once
#include <d3d11.h>
#include <fstream>
class GpuResourceUtils
{
public:
	static HRESULT loadVertexShader(ID3D11Device* device, const char* filename, ID3D11VertexShader** vertex_shader, D3D11_INPUT_ELEMENT_DESC input_element_desc[], UINT input_element_desc_num, ID3D11InputLayout** input_layout);
	static HRESULT loadPixelShader(ID3D11Device* device, const char* filename, ID3D11PixelShader** pixel_shader);
	static HRESULT loadTexture(ID3D11Device* device, const char* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc = nullptr);
	static HRESULT createDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc = nullptr);
};