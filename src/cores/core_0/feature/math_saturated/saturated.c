#include "saturated.h"
#include <stdint.h>

float saturate_float(const float signal, const float upperBound, const float lowerBound)
{
    if (lowerBound >= upperBound){
        return 0;
    }

    uint8_t underbound = signal < lowerBound;
    uint8_t overbound = signal > upperBound;

    return 
        (!underbound * !overbound * signal) +
        (underbound * !overbound * lowerBound) +
        (!underbound * overbound * upperBound);
}
