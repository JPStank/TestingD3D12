#pragma once

#include <chrono>

class HighResolutionClock
{
public:
	HighResolutionClock();

	void Tick();

	void Reset();

	double GetDeltaNanoseconds() const;
	double GetDeltaMicroseconds() const;
	double GetDeltaMilliseconds() const;
	double GetDeltaSeconds() const;

	double GetTotalNanoseconds() const;
	double GetTotalMicroseconds() const;
	double GetTotalMilliseconds() const;
	double GetTotalSeconds() const;

private:
	std::chrono::high_resolution_clock::time_point mT0;
	std::chrono::high_resolution_clock::duration mDeltaTime;
	std::chrono::high_resolution_clock::duration mTotalTime;
};