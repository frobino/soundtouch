#ifndef FIRFILTERUTILS_H_
#define FIRFILTERUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(__GNUC__) && !defined(ANDROID))
    // In GCC, include soundtouch_config.h made by config scritps.
    // Skip this in Android compilation that uses GCC but without configure scripts.
    #include "soundtouch_config.h"
#endif

/*
 * Taken from "STTypes.h"
 */
#ifdef SOUNDTOUCH_INTEGER_SAMPLES
    // 16bit integer sample type
    typedef short SAMPLETYPE;
    // data type for sample accumulation: Use 32bit integer to prevent overflows
    typedef long  LONG_SAMPLETYPE;

#else
    // floating point samples
    typedef float  SAMPLETYPE;
    // data type for sample accumulation: Use double to utilize full precision.
    typedef double LONG_SAMPLETYPE;

#endif  // SOUNDTOUCH_INTEGER_SAMPLES

typedef unsigned int    uint;
typedef unsigned long   ulong;

uint evaluateFilterStereoImpl(SAMPLETYPE *filterCoeffs, uint length, SAMPLETYPE resultDivider, uint resultDivFactor, SAMPLETYPE *dest, const SAMPLETYPE *src, uint numSamples);
uint evaluateFilterMonoImpl(SAMPLETYPE *filterCoeffs, uint length, SAMPLETYPE resultDivider, uint resultDivFactor, SAMPLETYPE *dest, const SAMPLETYPE *src, uint numSamples);
uint evaluateFilterMultiImpl(SAMPLETYPE *filterCoeffs, uint length, SAMPLETYPE resultDivider, uint resultDivFactor, SAMPLETYPE *dest, const SAMPLETYPE *src, uint numSamples, uint numChannels);

#ifdef __cplusplus
}
#endif

#endif /* FIRFILTERUTILS_H_ */
