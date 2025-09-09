#ifndef ML_H
#define ML_H
#include "data_structures.h"
#include <stdlib.h>

int ml_linear_regression_data_init(LinearRegressionData *linear_regression_data, float x[], float y[], unsigned int n);

double* ml_linear_regression_predict(LinearRegressionData *linear_regression_data, double inputs[]);


#endif