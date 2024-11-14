#include "CpuTimer.h"

CpuTimer::CpuTimer()
{
	//Search the computer`s performance counter
	LONGLONG count_per_second;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&count_per_second));
	second_per_count = 1.0 / static_cast<double>(count_per_second);
	//Reset the timer before use the timer
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&current_time));
	base_time = current_time;
	prev_time = current_time;

}
void CpuTimer::Reset()
{

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&current_time));
	base_time = current_time;
	prev_time = current_time;
	stop_time = 0;
	stopped = false;


}
void CpuTimer::Stop()
{
	if (!stopped)
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&stop_time));
		stopped = true;
	}
}
void CpuTimer::Start()
{
	LONGLONG start_time;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&start_time));
	//Accumulate the time elapsed between stop and start 
	//				   |<-------d------->|
	//------*----------*-----------------*-------------->time
	//base_time		stop_time		start_time
	if (stopped)
	{
		pause_time += (start_time - stop_time);
		prev_time = start_time;
		stop_time = 0;
		stopped = false;
	}
}
void CpuTimer::Tick()
{
	if (stopped)
	{
		delta_time = 0.0;
		return;
	}

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&current_time));

	delta_time = (current_time - prev_time) * second_per_count;
	prev_time = current_time;
	if (delta_time < 0.0)
	{
		delta_time = 0.0;
	}

}
float CpuTimer::TotalTime() const
{
	// We might not to count the time when we are stopped.
	// If we subtract the base_time from stop_time,
	// the pause_time between the stop_time and base_time will be ignored.
	// So the correct way is subtract the pause_time from stop_time.
	//				  |<--pause_time-->|						
	//------*---------*----------------*---------*---------*---->timer
	// base_time   stop_time	 start_time	 stop_time	start_time
	if (stopped)
	{
		return static_cast<float>(((stop_time - pause_time) - base_time) * second_per_count);
	}
	// It is probably includes the pause_time between current_time
	// and base_time.
	// So we subtract the pause_time from current_time.
	//				  |<--pause_time-->|						
	//------*---------*----------------*----------*------->timer
	// base_time   stop_time	 start_time	 current_time	

	else
	{
		return static_cast<float>(((current_time - pause_time) - base_time) * second_per_count);
	}
}
float CpuTimer::DeltaTime() const
{
	return static_cast<float>(delta_time);
}
bool CpuTimer::IsStopped() const
{
	return stopped;
}