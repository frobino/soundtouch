#ifndef TDSTRETCHUTILS_H_
#define TDSTRETCHUTILS_H_

#ifdef __cplusplus
extern "C" {
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


int seekBestOverlapPositionFullImpl(int channels, int seekLength, int overlapLength, SAMPLETYPE *pMidBuffer, const SAMPLETYPE *refPos);

#ifdef __cplusplus
}
#endif

#endif /* TDSTRETCHUTILS_H_ */
