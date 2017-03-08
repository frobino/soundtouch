#include "TDStretchUtils.h"

#include "float.h"
#include "math.h"

typedef struct CorrResult {
    double corr;
    int offs;
} CorrResult_t;

double calcCrossCorr(int channels, int overlapLength, const float *mixingPos, const float *compare, double *anorm_ptr);
double calcCrossCorrAccumulate(int channels, int overlapLength, const float *mixingPos, const float *compare, double *norm_ptr);


int seekBestOverlapPositionFullImpl(int channels, int seekLength, int overlapLength, SAMPLETYPE *pMidBuffer, const SAMPLETYPE *refPos)
{
    int i;
    double norm;
    CorrResult_t bestCorr = { -FLT_MAX, 0 };

    // Scans for the best correlation value by testing each possible position
    // over the permitted range.
    bestCorr.corr = calcCrossCorr(channels, overlapLength, refPos, pMidBuffer, &norm);
    bestCorr.corr = (bestCorr.corr + 0.1) * 0.75;

    #pragma omp declare reduction (maxCorr : CorrResult_t :      \
        omp_out = omp_in.corr > omp_out.corr ? omp_out : omp_in) \
        initializer ( omp_priv = omp_orig )

    #pragma omp parallel for reduction (maxCorr:bestCorr) \
    default(none) firstprivate(norm) private(i) \
    shared(seekLength, channels, overlapLength, refPos, pMidBuffer)
    for (i = 1; i < seekLength; i ++)
    {
        double corr;

        // Calculates correlation value for the mixing position corresponding to 'i'
#ifdef _OPENMP
        // in parallel OpenMP mode, can't use norm accumulator version as parallel executor won't
        // iterate the loop in sequential order
        {
            const float *mixingPos = (const float *)(refPos + channels * i);
            const float *compare = (const float *)pMidBuffer;
            int i;

            corr = norm = 0;
            // Same routine for stereo and mono. For Stereo, unroll by factor of 2.
            // For mono it's same routine yet unrolls by factor of 4.
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

            corr = corr / sqrt((norm < 1e-9 ? 1.0 : norm));
        }
#else
        // In non-parallel version call "calcCrossCorrAccumulate" that is otherwise same
        // as "calcCrossCorr", but saves time by reusing & updating previously stored
        // "norm" value
        corr = calcCrossCorrAccumulate(channels, overlapLength, refPos + channels * i, pMidBuffer, &norm);
#endif

        // heuristic rule to slightly favor values close to mid of the range
        double tmp = (double)(2 * i - seekLength) / (double)seekLength;
        corr = ((corr + 0.1) * (1.0 - 0.25 * tmp * tmp));

        CorrResult_t currentCorr = { corr, i };

        // Checks for the highest correlation value
        if (currentCorr.corr > bestCorr.corr)
        {
            bestCorr.corr = currentCorr.corr;
            bestCorr.offs = currentCorr.offs;
        }
    }

    return bestCorr.offs;
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
    int i, j;

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
    for (j = 0; j < channels; j ++)
    {
        i --;
        norm += mixingPos[i] * mixingPos[i];
    }

    *norm_ptr = norm;
    return corr / sqrt((norm < 1e-9 ? 1.0 : norm));
}
