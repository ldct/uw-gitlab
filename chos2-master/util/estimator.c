#include <estimator.h>
#include <panic.h>

void initializeEstimator(Estimator* estimator) {
    estimator->next_sample = 0;
}

void insertSample(Estimator* estimator, int val) {
    ASSERT(estimator->next_sample < NUM_SAMPLES, "too many samples inserted");
    estimator->samples[estimator->next_sample++] = val;
}

int produceEstimate(Estimator* estimator) {
    ASSERT(NUM_SAMPLES == estimator->next_sample, "wrong number of samples");

    // sort the array
    for (int i=0; i<NUM_SAMPLES; i++) {
        for (int j=i+1; j<NUM_SAMPLES; j++) {
            if (estimator->samples[i] > estimator->samples[j]) {
                int temp = estimator->samples[i];
                estimator->samples[i] = estimator->samples[j];
                estimator->samples[j] = temp;
            }
        }
    }

    for (int i=0; i<NUM_SAMPLES-(2*NUM_OUTLIERS); i++) {
        estimator->samples[i] = estimator->samples[i+NUM_OUTLIERS];
    }

    int sum = 0;
    for (int i=0; i<NUM_SAMPLES-(2*NUM_OUTLIERS); i++) {
        sum += estimator->samples[i];
    }
    int mean = sum / (NUM_SAMPLES-(2*NUM_OUTLIERS));

    return mean;

}



// unit test

#include <io.h>

void estimatorTest() {
    Estimator estimator;
    initializeEstimator(&estimator);
    for (int i=0; i<NUM_SAMPLES; i++) {
        insertSample(&estimator, 50-i);
    }
    for (int i=0; i<NUM_SAMPLES; i++) {
        logf("sample %d: %d", i, estimator.samples[i]);
    }
    int mean = produceEstimate(&estimator);
    logf("estimatorTest done %d", mean);
}
