#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "graphics.h"
#pragma warning( disable : 4996 )
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

class Sprite
{
public:
	Sprite();
	~Sprite() {};
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};
	void render();
	void print();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertex_buffer;
};