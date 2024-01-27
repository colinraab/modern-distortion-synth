#ifndef COLIN_CONVERSIONS_H
#define COLIN_CONVERSIONS_H

#include <math.h>

/*
  ==============================================================================

    Conversions.h
    Created: 15 Sep 2022 5:17:16pm
    Author:  Colin Raab

  ==============================================================================
*/

namespace Colin {

typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long int uint64;
typedef long long int int64;

static float linearToDB(const float& sample) {
    return 20*log10f(sample);
}

static float DBtoLinear(const float& sample) {
    return powf(10,sample/20);
}

}

#endif

