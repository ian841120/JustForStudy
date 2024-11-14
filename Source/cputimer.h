#pragma once
#pragma once
#include <windows.h>
class CpuTimer
{
public:
	CpuTimer();

	float TotalTime()const;
	float DeltaTime()const;
	//Reset the timer 
	void Reset();
	void Start();
	//Stop the timer
	void Stop();
	void Tick();
	bool IsStopped()const;
private:
	double second_per_count = 0;
	double delta_time = -1.0;

	LONGLONG base_time = 0;
	LONGLONG prev_time = 0;
	LONGLONG current_time = 0;
	LONGLONG stop_time = 0;
	LONGLONG pause_time = 0;
	bool stopped = false;
};
// Reference
