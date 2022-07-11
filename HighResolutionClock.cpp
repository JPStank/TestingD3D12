#include "pch.h"
#include "HighResolutionClock.h"

HighResolutionClock::HighResolutionClock()
	: mDeltaTime(0)
	, mTotalTime(0)
{
	mT0 = std::chrono::high_resolution_clock::now();
}

void HighResolutionClock::Tick()
{
	auto t1 = std::chrono::high_resolution_clock::now();
	mDeltaTime = t1 - mT0;
	mTotalTime += mDeltaTime;
	mT0 = t1;
}

void HighResolutionClock::Reset()
{
	mT0 = std::chrono::high_resolution_clock::now();
	mDeltaTime = std::chrono::high_resolution_clock::duration();
	mTotalTime = std::chrono::high_resolution_clock::duration();
}

double HighResolutionClock::GetDeltaNanoseconds() const
{
	return mDeltaTime.count() * 1.0;
}
double HighResolutionClock::GetDeltaMicroseconds() const
{
	return mDeltaTime.count() * 1e-3;
}

double HighResolutionClock::GetDeltaMilliseconds() const
{
	return mDeltaTime.count() * 1e-6;
}

double HighResolutionClock::GetDeltaSeconds() const
{
	return mDeltaTime.count() * 1e-9;
}

double HighResolutionClock::GetTotalNanoseconds() const
{
	return mTotalTime.count() * 1.0;
}

double HighResolutionClock::GetTotalMicroseconds() const
{
	return mTotalTime.count() * 1e-3;
}

double HighResolutionClock::GetTotalMilliseconds() const
{
	return mTotalTime.count() * 1e-6;
}

double HighResolutionClock::GetTotalSeconds() const
{
	return mTotalTime.count() * 1e-9;
}