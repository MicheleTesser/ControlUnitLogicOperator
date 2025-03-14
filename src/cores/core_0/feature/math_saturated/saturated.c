#include "saturated.h"
#include <stdint.h>

float saturate_float(const float signal, const float upperBound, const float lowerBound)
{
    if (lowerBound >= upperBound){
        return 0;
    }

    uint8_t underbound = signal < lowerBound;
    uint8_t overbound = signal > upperBound;

    return (float)
        ((float)!underbound * (float)!overbound * signal) +
        ((float)!underbound * (float)overbound * lowerBound) +
        ((float)underbound * (float)!overbound * upperBound);   
}
