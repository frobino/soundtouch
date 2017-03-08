#include "FIRFilterUtils.h"
#include <assert.h>
#include <stdlib.h>

uint evaluateFilterStereoImpl(SAMPLETYPE *filterCoeffs, uint length, SAMPLETYPE resultDivider, uint resultDivFactor, SAMPLETYPE *dest, const SAMPLETYPE *src, uint numSamples)
{
    int j, end;
#ifdef SOUNDTOUCH_FLOAT_SAMPLES
    // when using floating point samples, use a scaler instead of a divider
    // because division is much slower operation than multiplying.
    double dScaler = 1.0 / (double)resultDivider;
#endif

    assert(length != 0);
    assert(src != NULL);
    assert(dest != NULL);
    assert(filterCoeffs != NULL);

    end = 2 * (numSamples - length);

    #pragma omp parallel for
    for (j = 0; j < end; j += 2)
    {
        const SAMPLETYPE *ptr;
        LONG_SAMPLETYPE suml, sumr;
        uint i;

        suml = sumr = 0;
        ptr = src + j;

        for (i = 0; i < length; i += 4)
        {
            // loop is unrolled by factor of 4 here for efficiency
            suml += ptr[2 * i + 0] * filterCoeffs[i + 0] +
                    ptr[2 * i + 2] * filterCoeffs[i + 1] +
                    ptr[2 * i + 4] * filterCoeffs[i + 2] +
                    ptr[2 * i + 6] * filterCoeffs[i + 3];
            sumr += ptr[2 * i + 1] * filterCoeffs[i + 0] +
                    ptr[2 * i + 3] * filterCoeffs[i + 1] +
                    ptr[2 * i + 5] * filterCoeffs[i + 2] +
                    ptr[2 * i + 7] * filterCoeffs[i + 3];
        }

#ifdef SOUNDTOUCH_INTEGER_SAMPLES
        suml >>= resultDivFactor;
        sumr >>= resultDivFactor;
        // saturate to 16 bit integer limits
        suml = (suml < -32768) ? -32768 : (suml > 32767) ? 32767 : suml;
        // saturate to 16 bit integer limits
        sumr = (sumr < -32768) ? -32768 : (sumr > 32767) ? 32767 : sumr;
#else
        suml *= dScaler;
        sumr *= dScaler;
#endif // SOUNDTOUCH_INTEGER_SAMPLES
        dest[j] = (SAMPLETYPE)suml;
        dest[j + 1] = (SAMPLETYPE)sumr;
    }
    return numSamples - length;
}

uint evaluateFilterMonoImpl(SAMPLETYPE *filterCoeffs, uint length, SAMPLETYPE resultDivider, uint resultDivFactor, SAMPLETYPE *dest, const SAMPLETYPE *src, uint numSamples)
{
    int j, end;
#ifdef SOUNDTOUCH_FLOAT_SAMPLES
    // when using floating point samples, use a scaler instead of a divider
    // because division is much slower operation than multiplying.
    double dScaler = 1.0 / (double)resultDivider;
#endif

    assert(length != 0);

    end = numSamples - length;
    #pragma omp parallel for
    for (j = 0; j < end; j ++)
    {
        const SAMPLETYPE *pSrc = src + j;
        LONG_SAMPLETYPE sum;
        uint i;

        sum = 0;
        for (i = 0; i < length; i += 4)
        {
            // loop is unrolled by factor of 4 here for efficiency
            sum += pSrc[i + 0] * filterCoeffs[i + 0] +
                   pSrc[i + 1] * filterCoeffs[i + 1] +
                   pSrc[i + 2] * filterCoeffs[i + 2] +
                   pSrc[i + 3] * filterCoeffs[i + 3];
        }
#ifdef SOUNDTOUCH_INTEGER_SAMPLES
        sum >>= resultDivFactor;
        // saturate to 16 bit integer limits
        sum = (sum < -32768) ? -32768 : (sum > 32767) ? 32767 : sum;
#else
        sum *= dScaler;
#endif // SOUNDTOUCH_INTEGER_SAMPLES
        dest[j] = (SAMPLETYPE)sum;
    }
    return end;
}

uint evaluateFilterMultiImpl(SAMPLETYPE *filterCoeffs, uint length, SAMPLETYPE resultDivider, uint resultDivFactor, SAMPLETYPE *dest, const SAMPLETYPE *src, uint numSamples, uint numChannels)
{
    int j, end;

#ifdef SOUNDTOUCH_FLOAT_SAMPLES
    // when using floating point samples, use a scaler instead of a divider
    // because division is much slower operation than multiplying.
    double dScaler = 1.0 / (double)resultDivider;
#endif

    assert(length != 0);
    assert(src != NULL);
    assert(dest != NULL);
    assert(filterCoeffs != NULL);
    assert(numChannels < 16);

    end = numChannels * (numSamples - length);

    #pragma omp parallel for
    for (j = 0; j < end; j += numChannels)
    {
        const SAMPLETYPE *ptr;
        LONG_SAMPLETYPE sums[16];
        uint c, i;

        for (c = 0; c < numChannels; c ++)
        {
            sums[c] = 0;
        }

        ptr = src + j;

        for (i = 0; i < length; i ++)
        {
            SAMPLETYPE coef=filterCoeffs[i];
            for (c = 0; c < numChannels; c ++)
            {
                sums[c] += ptr[0] * coef;
                ptr ++;
            }
        }

        for (c = 0; c < numChannels; c ++)
        {
#ifdef SOUNDTOUCH_INTEGER_SAMPLES
            sums[c] >>= resultDivFactor;
#else
            sums[c] *= dScaler;
#endif // SOUNDTOUCH_INTEGER_SAMPLES
            dest[j+c] = (SAMPLETYPE)sums[c];
        }
    }
    return numSamples - length;
}
