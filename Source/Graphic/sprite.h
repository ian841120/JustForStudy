#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "graphics.h"
#include <string>




class Sprite
{
public:
	Sprite(const char* filename = nullptr);
	~Sprite() {};
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
	void render();
	void render(float dx, float dy, float dw, float dh, float r, float g, float b, float a);
	void render(float dx, float dy, float dw, float dh,float angle, float r, float g, float b, float a);
	void render(float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle, float r, float g, float b, float a);
	void print(std::string filename = "sample");
	ID3D11Buffer* getVertexBuffer() { return vertex_buffer.Get(); }
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

	float texture_width;
	float texture_height;

};