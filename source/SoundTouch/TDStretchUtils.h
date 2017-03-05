/*
 * TDStretchUtils.h
 *
 *  Created on: Mar 5, 2017
 *      Author: edopao
 */

#ifndef TDSTRETCHUTILS_H_
#define TDSTRETCHUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * FIXME: Porcherie
 */
#define SAMPLETYPE float

int seekBestOverlapPositionFullC(int channels, int seekLength, int overlapLength, SAMPLETYPE *pMidBuffer, const SAMPLETYPE *refPos);

#ifdef __cplusplus
}
#endif

#endif /* TDSTRETCHUTILS_H_ */
