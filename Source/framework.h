#pragma once
#include <wrl.h>
#include "cputimer.h"
#include "Graphic\graphics.h"
#include "Graphic\sprite.h"
#include <memory>
#include "Graphic\perlin_noise.h"
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
	std::unique_ptr<Sprite> sprite[4];
	std::unique_ptr<PerlinNoise> perlin_noise;
	char filename[256] = "sample";
	int octaves = 8;
	float lacunarity = 2.0f;
	float gain = 0.5f;
	bool perlin_noise_show = true;


};