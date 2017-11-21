#pragma once

#include "Engine/Core/Time.hpp"

class Interval
{
public:
    //void set_parent_clock(Clock *clock);

    void set_seconds(float seconds) {
        interval_time = seconds;
        target_time = static_cast<float>(GetCurrentTimeSeconds()) + seconds;
    }

    void set_frequency(float hz) { set_seconds(1.0f / hz); }

    bool check()
    {
        float current_time = static_cast<float>(GetCurrentTimeSeconds());
        return (current_time >= target_time);
    }

    bool check_and_decrement()
    {
        if(check()) {
            target_time += interval_time;
            return true;
        } else {
            return false;
        }
    }

    bool check_and_reset()
    {
        if(check()) {
            reset();
            return true;
        } else {
            return false;
        }
    }

    unsigned int decrement_all()
    {
        unsigned int count = 0;
        while(check_and_decrement()) {
            ++count;
        }

        return count;
    }

    void reset()
    {
        target_time = static_cast<float>(GetCurrentTimeSeconds()) + interval_time;
    }

public:
    float interval_time;
    float target_time;

};