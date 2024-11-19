#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "graphics.h"
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
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertex_buffer;
};