#ifndef COLIN_MIX_MATRIX_H
#define COLIN_MIX_MATRIX_H
#include <cmath>

/*
  ==============================================================================

    Mix_Matrix.h
    Created: 2 Nov 2022 10:52:25am
    Author:  Colin Raab

  ==============================================================================
*/

namespace Colin {

static const int numChannels = 8;

struct data {
    float channels[numChannels] = {0};
    
    void fill(float input) {
        for(int i=0; i<numChannels; i++) {
            channels[i] = input;
        }
    }
};

class Mix_Matrix {
protected:
    float coeffs[numChannels] = {0};
    
public:
    Mix_Matrix() {
        coeffs[0] = 1;
        coeffs[1] = 0;
        for (int i = 1; i < (numChannels/2); ++i) {
            double phase = M_PI*i/numChannels;
            coeffs[2*i] = std::cos(phase);
            coeffs[2*i + 1] = std::sin(phase);
        }
    }
    ~Mix_Matrix() = default;
    
    data Householder(data r) {
        data o;
        float factor = -2 / numChannels;
        float sum = 0;
        for(int i=0; i<numChannels; i++)
            sum += r.channels[i];
        sum *= factor;
        for(int i=0; i<numChannels; i++) {
            o.channels[i] = r.channels[i] + sum;
        }
        return o;
    }
    
    data Hadamard(data r) {
        data o;
        float a = 0;
        float b = 0;
        float factor = std::sqrt(1.0/numChannels);
        int hsize = numChannels/2;
        for(int i=0; i<hsize; i++) {
            a = r.channels[i];
            b = r.channels[i+hsize];
            o.channels[i] = (a+b) * factor;
            o.channels[i+hsize] = (a-b) * factor;
        }
        return o;
    }
    
    data stereoToMulti(float l, float r) {
        data o;
        o.channels[0] = l;
        o.channels[1] = r;
        for(int i=2; i<numChannels; i+=2) {
            o.channels[i] = l * coeffs[i] + r * coeffs[i+1];
            o.channels[i+1] = r * coeffs[i] - l * coeffs[i+1];
        }
        return o;
    }
    
    void multiToStereo(data input, float &l, float &r) {
        l = input.channels[0];
        r = input.channels[1];
        for(int i=2; i<numChannels; i+=2) {
            l += input.channels[i] * coeffs[i] - input.channels[i+1] * coeffs[i+1];
            r += input.channels[i+1] * coeffs[i] + input.channels[i] * coeffs[i+1];
        }
        /*
        if(isMono) {
            l *= 2/numChannels;
            r *= 2/numChannels;
        }
        else {
            l *= std::sqrt(2/numChannels);
            r *= std::sqrt(2/numChannels);
        }
        */
    }
    
    void cheapEnergyCrossfade(float x, float &toCoeff, float &fromCoeff) {
        float x2 = 1 - x;
        /// Other powers p can be approximated by: k = -6.0026608 + p*(6.8773512 - 1.5838104*p)
        float A = x*x2, B = A*(1 + (float)1.4186*A);
        float C = (B + x), D = (B + x2);
        toCoeff = C*C;
        fromCoeff = D*D;
    }
};

}


#endif
