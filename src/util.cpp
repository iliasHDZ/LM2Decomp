#include "util.hpp"

float s_secondsPerTick;

float nlGetTickerDifference(u32 a, u32 b) {
    u32 val;
    if (a > b)
        val = a - b;
    else
        val = b - a;

    return (float)val * s_secondsPerTick;
}