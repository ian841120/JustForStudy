#pragma once
#include <wrl.h>
#include "cputimer.h"
#include "Graphic\graphics.h"
#include <memory>
class Framework
{
public:
	Framework(HWND);
	~Framework() {};
private:
	void update(float elapsed_time);
	void render();
	void calculateFrameRates();
public:
	int run();
	LRESULT CALLBACK handleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	HWND hwnd;
	CpuTimer timer;
	Graphics graphics;
};