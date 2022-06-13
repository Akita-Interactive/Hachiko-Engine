#include "core/hepch.h"
#include "Timer.h"

void Hachiko::Timer::Start()
{
    running = true;
    start_time = SDL_GetTicks();
}

void Hachiko::Timer::Resume()
{
    if (!running)
    {
        running = true;
        start_time = SDL_GetTicks() - (stop_time - start_time);
    }
}

double Hachiko::Timer::Read()
{
    unsigned int now;
    if (running)
    {
        now = SDL_GetTicks();
    }
    else
    {
        now = stop_time;
    }

    current_time = static_cast<double>(now - start_time); // ms
    return current_time;
}

double Hachiko::Timer::Stop()
{
    if (running)
    {
        running = false;
        stop_time = SDL_GetTicks();
        return Read();
    }
    return static_cast<double>(stop_time);
}

void Hachiko::PerformanceTimer::Start()
{
    running = true;
    start_time = SDL_GetPerformanceCounter();
}

void Hachiko::PerformanceTimer::Resume()
{
    if (!running)
    {
        running = true;
        start_time = SDL_GetPerformanceCounter() - (stop_time - start_time);
    }
}

double Hachiko::PerformanceTimer::Read()
{
    static const auto frequency = static_cast<double>(SDL_GetPerformanceFrequency());
    static const auto ms_frequency_factor = 1000.0 / frequency;

    unsigned long long now;
    if (running)
    {
        now = SDL_GetPerformanceCounter();
    }
    else
    {
        now = stop_time;
    }

    current_time = (now - start_time) * ms_frequency_factor;
    return current_time;
}

double Hachiko::PerformanceTimer::Stop()
{
    if (running)
    {
        running = false;
        stop_time = SDL_GetPerformanceCounter();
        return Read();
    }
    return static_cast<double>(stop_time);
}

double Hachiko::GameTimer::delta_time = 0.;
double Hachiko::GameTimer::total_time = 0.;
double Hachiko::GameTimer::prev_tick_time = 0.;
bool Hachiko::GameTimer::running = false;
bool Hachiko::GameTimer::paused = false;
Hachiko::PerformanceTimer Hachiko::GameTimer::timer;

void Hachiko::GameTimer::Start()
{
    total_time = 0.;
    running = true;
    paused = false;
    timer.Start();
    prev_tick_time = timer.Read();
}

double Hachiko::GameTimer::Update()
{
    const double tick_time = timer.Read();
    delta_time = (tick_time - prev_tick_time) / 1000.0;
    prev_tick_time = tick_time;
    if (running)
    {
        total_time = timer.Read();
    }
    return delta_time;
}

void Hachiko::GameTimer::Play()
{
    running = true;
}

void Hachiko::GameTimer::Pause()
{
    paused = true;
    timer.Stop();
}

void Hachiko::GameTimer::Resume()
{
    paused = false;
    timer.Resume();
}

void Hachiko::GameTimer::Stop()
{
    running = false;
    timer.Stop();
}

Hachiko::PerformanceTimer Hachiko::EngineTimer::timer;
double Hachiko::EngineTimer::delta_time = 0.;
double Hachiko::EngineTimer::total_time = 0.;
double Hachiko::EngineTimer::prev_tick_time = 0.;
bool Hachiko::EngineTimer::running = false;

void Hachiko::EngineTimer::Start()
{
    total_time = 0.;
    running = true;
    timer.Start();
    prev_tick_time = timer.Read();
}

double Hachiko::EngineTimer::Update()
{
    const double tick_time = timer.Read();
    delta_time = (tick_time - prev_tick_time) / 1000.0;
    prev_tick_time = tick_time;
    if (running)
    {
        total_time = timer.Read();
    }
    return delta_time;
}
