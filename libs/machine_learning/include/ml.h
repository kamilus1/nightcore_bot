#ifndef ML_H
#define ML_H
#include "data_structures.h"
#include <stdlib.h>

int ml_linear_regression_data_init(LinearRegressionData *linear_regression_data, double x[], double y[], unsigned int n);

int ml_linear_regression_model_learn(LinearRegressionData *linear_regression_data);

double* ml_linear_regression_predict(LinearRegressionData *linear_regression_data, double inputs[]);


#endif