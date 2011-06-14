/**
 * @file Time.cpp
 * This file takes control of time.
 * @author Xavier Ho (contact@xavierho.com)
 */
#include "HPTime.h"

#ifndef NULL
    #define NULL 0
#endif

Time::Time()
{
#ifdef _WIN32
    QueryPerformanceFrequency(&frequency);
#endif
    ratio = 1.0;
    stopped = true;
}

Time::~Time()
{
}

void Time::start()
{
#ifdef _WIN32
    QueryPerformanceCounter(&t1);
#else
    gettimeofday(&t1, NULL);
#endif
    stopped = false;
}

void Time::stop()
{
#ifdef _WIN32
    QueryPerformanceCounter(&t2);
#else
    gettimeofday(&t2, NULL);
#endif
    stopped = true;
}

double Time::getMicroseconds()
{
    if (!stopped)
        stop();

#ifdef _WIN32
    double delta = (t2.QuadPart - t1.QuadPart) * 1000000.0 / frequency.QuadPart;
#else
    double delta = (t2.tv_sec - t1.tv_sec) * 1000000.0;
    delta += (t2.tv_usec - t1.tv_usec);
#endif
    return delta;
}

double Time::getMilliseconds()
{
    return getMicroseconds() * 0.001;
}

double Time::getSeconds()
{
    return getMicroseconds() * 0.000001;
}

double Time::getFPS(int framesRendered)
{
    return framesRendered / getSeconds();
}
