#include "TDStretchUtils.h"

#include "float.h"
#include "math.h"


double calcCrossCorr(int channels, int overlapLength, const float *mixingPos, const float *compare, double *anorm_ptr);
double calcCrossCorrAccumulate(int channels, int overlapLength, const float *mixingPos, const float *compare, double *norm_ptr);


/*
 * TDStretchUtils.c
 *
 *  Created on: Mar 5, 2017
 *      Author: edopao
 */

int seekBestOverlapPositionFullC(int channels, int seekLength, int overlapLength, SAMPLETYPE *pMidBuffer, const SAMPLETYPE *refPos)
{
    int bestOffs;
    double bestCorr;
    int i;
    double norm;

    bestCorr = -FLT_MAX;
    bestOffs = 0;

    // Scans for the best correlation value by testing each possible position
    // over the permitted range.
    bestCorr = calcCrossCorr(channels, overlapLength, refPos, pMidBuffer, &norm);
    bestCorr = (bestCorr + 0.1) * 0.75;

    #pragma omp parallel for
    for (i = 1; i < seekLength; i ++)
    {
        double corr;
        // Calculates correlation value for the mixing position corresponding to 'i'
#ifdef _OPENMP
        // in parallel OpenMP mode, can't use norm accumulator version as parallel executor won't
        // iterate the loop in sequential order
        corr = calcCrossCorr(channels, overlapLength, refPos + channels * i, pMidBuffer, &norm);
#else
        // In non-parallel version call "calcCrossCorrAccumulate" that is otherwise same
        // as "calcCrossCorr", but saves time by reusing & updating previously stored
        // "norm" value
        corr = calcCrossCorrAccumulate(channels, overlapLength, refPos + channels * i, pMidBuffer, &norm);
#endif
        // heuristic rule to slightly favour values close to mid of the range
        double tmp = (double)(2 * i - seekLength) / (double)seekLength;
        corr = ((corr + 0.1) * (1.0 - 0.25 * tmp * tmp));

        // Checks for the highest correlation value
        if (corr > bestCorr)
        {
            // For optimal performance, enter critical section only in case that best value found.
            // in such case repeat 'if' condition as it's possible that parallel execution may have
            // updated the bestCorr value in the mean time
            #pragma omp critical
            if (corr > bestCorr)
            {
                bestCorr = corr;
                bestOffs = i;
            }
        }
    }

#ifdef SOUNDTOUCH_INTEGER_SAMPLES
    adaptNormalizer();
#endif

    return bestOffs;
}


/// Calculate cross-correlation
double calcCrossCorr(int channels, int overlapLength, const float *mixingPos, const float *compare, double *anorm_ptr)
{
	double anorm = *anorm_ptr;
    double corr;
    double norm;
    int i;

    corr = norm = 0;
    // Same routine for stereo and mono. For Stereo, unroll by factor of 2.
    // For mono it's same routine yet unrollsd by factor of 4.
    for (i = 0; i < channels * overlapLength; i += 4)
    {
        corr += mixingPos[i] * compare[i] +
                mixingPos[i + 1] * compare[i + 1];

        norm += mixingPos[i] * mixingPos[i] +
                mixingPos[i + 1] * mixingPos[i + 1];

        // unroll the loop for better CPU efficiency:
        corr += mixingPos[i + 2] * compare[i + 2] +
                mixingPos[i + 3] * compare[i + 3];

        norm += mixingPos[i + 2] * mixingPos[i + 2] +
                mixingPos[i + 3] * mixingPos[i + 3];
    }

    *anorm_ptr = norm;
    return corr / sqrt((norm < 1e-9 ? 1.0 : norm));
}


/// Update cross-correlation by accumulating "norm" coefficient by previously calculated value
double calcCrossCorrAccumulate(int channels, int overlapLength, const float *mixingPos, const float *compare, double *norm_ptr)
{
	double norm = *norm_ptr;
    double corr;
    int i;

    corr = 0;

    // cancel first normalizer tap from previous round
    for (i = 1; i <= channels; i ++)
    {
        norm -= mixingPos[-i] * mixingPos[-i];
    }

    // Same routine for stereo and mono. For Stereo, unroll by factor of 2.
    // For mono it's same routine yet unrollsd by factor of 4.
    for (i = 0; i < channels * overlapLength; i += 4)
    {
        corr += mixingPos[i] * compare[i] +
                mixingPos[i + 1] * compare[i + 1] +
                mixingPos[i + 2] * compare[i + 2] +
                mixingPos[i + 3] * compare[i + 3];
    }

    // update normalizer with last samples of this round
    for (int j = 0; j < channels; j ++)
    {
        i --;
        norm += mixingPos[i] * mixingPos[i];
    }

    *norm_ptr = norm;
    return corr / sqrt((norm < 1e-9 ? 1.0 : norm));
}
