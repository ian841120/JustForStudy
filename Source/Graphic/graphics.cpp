#include "graphics.h"

Graphics* Graphics::instance = nullptr;
Graphics::Graphics(HWND hwnd)
{
	/// <summary>
	/// Initialize the parameter
	/// </summary>

	//Set instance
	instance = this;

	//Get screen size;
	RECT rc;
	GetClientRect(hwnd, &rc);
	UINT screenWidth = rc.right - rc.left;
	UINT screenHeight = rc.bottom - rc.top;
	this->screenHeight = static_cast<float>(screenHeight);
	this->screenWidth = static_cast<float>(screenWidth);

	//Create device ,device context and swap chain
	HRESULT hr{ S_OK };

	UINT createDeviceFlags{ 0 };
	D3D_FEATURE_LEVEL featureLevel{ D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
		&featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);
	_ASSERT_EXPR(SUCCEEDED(hr), L"CREATE SWAPCHAIN FAILED");

	//Create render target view 
	ID3D11Texture2D * backBuffer{};
	hr = swapChain->GetBuffer(0, __uuidof (ID3D11Texture2D), reinterpret_cast<LPVOID*>(&backBuffer));
	_ASSERT_EXPR(SUCCEEDED(hr), L"Get swap chain buffer failed");
	hr = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
	_ASSERT_EXPR(SUCCEEDED(hr), L"Create render target view");
	backBuffer->Release();

	//Create depth stencil view

	ID3D11Texture2D * depthStencilBuffer{};
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = screenWidth;
	texture2dDesc.Height = screenHeight;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2dDesc, NULL, &depthStencilBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), L"Create depth stencil buffer failed");

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = texture2dDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	_ASSERT_EXPR(SUCCEEDED(hr), L"Create depth stencil buffer failed");
	depthStencilBuffer->Release();

	//Set viewport
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	deviceContext->RSSetViewports(1, &viewport);

	imGuiClass = std::make_unique<ImGuiClass>(hwnd, device.Get(), deviceContext.Get());
}
