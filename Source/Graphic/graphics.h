#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <memory>
#include "imGuiClass.h"
class Graphics
{
public:
	Graphics(HWND hwnd);
	~Graphics() {};
public:
	static Graphics&		getInstance()					  { return *instance; }
	ID3D11Device*			getDevice()					const { return device.Get(); }
	ID3D11DeviceContext*	getDeviceContext()			const { return deviceContext.Get(); }
	IDXGISwapChain*			getSwapChain()				const { return swapChain.Get(); }
	ID3D11RenderTargetView* getRenderTargetView()		const { return renderTargetView.Get(); }
	ID3D11DepthStencilView* getDepthStencilView()		const { return depthStencilView.Get(); }
	ImGuiClass*				getImGuiClass()				const { return imGuiClass.get(); }
private:
	static Graphics*								instance;
	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;

	std::unique_ptr<ImGuiClass>				imGuiClass;

	float screenWidth;
	float screenHeight;
};