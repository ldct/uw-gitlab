#ifndef ESTIMATOR_H
#define ESTIMATOR_H

/*
bimodal estimator
*/

#define NUM_SAMPLES 50
#define NUM_OUTLIERS 4

typedef struct _Estimator {
    int samples[NUM_SAMPLES];
    int next_sample;
} Estimator;

void initializeEstimator(Estimator*);

void insertSample(Estimator* estimator, int val);

int produceEstimate(Estimator* estimator);

#endif
