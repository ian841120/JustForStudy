#include <sstream>
#include "Framework.h"

// Vertical Sync setting   1=60fps , 0=infinity
static const int syncInterval = 1;
Framework::Framework(HWND hwnd) :hwnd(hwnd),graphics(hwnd)
{
	perlin_noise = std::make_unique<PerlinNoise>(1532512342);
	perlin_noise->createImage(700, 700);
	double frequency = 8;
	float image_height = perlin_noise->getHeight();
	float image_width = perlin_noise->getWidth();
	double fx = image_width / frequency;
	double fy = image_height / frequency;
	for (int y = 0; y < image_height; y++)
	{
		for (int x = 0; x < image_width; x++)
		{
			double p = perlin_noise->accumulatedNoise2D(x / fx, y / fy);
			perlin_noise->setPixel(x, y, p, p, p);
		}
	}
}
void Framework::update(float elapsedTime)
{

	graphics.getImGuiClass()->newFrame();
}
void Framework::render()
{
	FLOAT color[]{ 0.5f,0.5f,0.5f, 0.2f };
	ID3D11DeviceContext* dc = graphics.getDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.getRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.getDepthStencilView();
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
	graphics.getImGuiClass()->render();

	//Sprite Render
	{
		perlin_noise->render();
	}

	graphics.getSwapChain()->Present(syncInterval, 0);
}
void Framework::calculateFrameRates()
{
	static int frames = 0;
	static float time_elapsed = 0;
	frames++;
	if ((timer.TotalTime() - time_elapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frames);
		float mspf = 1000.0f / fps;
		std::ostringstream outs;
		outs.precision(6);
		outs << " FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms) ";
		SetWindowTextA(hwnd, outs.str().c_str());
		frames = 0;
		time_elapsed += 1.0f;
	}
}
int Framework::run()
{
	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		//Window message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.Tick();
			calculateFrameRates();
			float elapsed_time = syncInterval == 0
				? timer.DeltaTime()
				: syncInterval / 60.0f
				;
			update(elapsed_time);
			render();
		}
	}
	return static_cast<int>(msg.wParam);
}
LRESULT CALLBACK Framework::handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (graphics.getImGuiClass()->handleMessage(hwnd, msg, wParam, lParam))
	{
		return true;
	}
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		}
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.Start();
		break;
	case WM_MOUSEWHEEL:
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}