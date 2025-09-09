#ifndef __DATA_STRUCTURES_H__
#define __DATA_STRUCTURES_H__

#include <stdlib.h>
#include <stdint.h>

typedef struct Queue_{

    uint32_t current_index;
    
}Queue;

typedef struct VectorFloatND_
{
    float *vector; /*vector*/
    uint32_t n; /*n dimensions */
}VectorFloatND;

typedef struct VectorIntND_
{
    int *vector;
    uint32_t n;
}VectorIntND;

typedef struct LinearRegressionData_
{
    VectorFloatND *x; /*x vector*/
    VectorFloatND *y; /*y vector*/
    float slope; /*slope of the line*/
    float intercept; /*intercept of the line*/
    float r_squared; /*r squared value*/
}LinearRegressionData;


typedef struct PolyRegressionData_
{
    VectorFloatND *x; /*x vector*/
    VectorFloatND *y; /*y vector*/
    float *coefficients; /*coefficients of the polynomial*/
    uint32_t degree; /*degree of the polynomial*/
    float r_squared; /*r squared value*/
}PolyRegressionData;

typedef struct LogisticRegressionData_
{
    VectorFloatND *x; /*x vector*/
    VectorFloatND *y; /*y vector*/
    float learning_rate; /*learning rate*/
    uint32_t num_iterations; /*number of iterations*/
    float *weights; /*weights vector*/
    float bias; /*bias term*/
    float cost; /*cost value*/
}LogisticRegressionData;

typedef enum NodeType_ {
    LEAF_NODE,
    INTERNAL_NODE
} NodeType;

typedef struct DecisionTreeNode_ {
    NodeType node_type;
    
    /* For internal nodes */
    uint32_t feature_index;     /* Index of the feature to split on */
    float threshold;            /* Threshold value for the split */
    struct DecisionTreeNode_ *left_child;   /* Left child (feature <= threshold) */
    struct DecisionTreeNode_ *right_child;  /* Right child (feature > threshold) */
    
    /* For leaf nodes */
    float prediction;           /* Predicted value for regression */
    uint32_t class_label;       /* Predicted class for classification */
    float confidence;           /* Confidence/probability of the prediction */
    
    /* Node statistics */
    uint32_t num_samples;       /* Number of training samples that reached this node */
    float impurity;             /* Gini impurity, entropy, or MSE */
    float gain;                 /* Information gain from this split */
} DecisionTreeNode;

typedef enum TreeType_ {
    CLASSIFICATION_TREE,
    REGRESSION_TREE
} TreeType;

typedef enum SplitCriterion_ {
    GINI,           /* For classification */
    ENTROPY,        /* For classification */
    MSE             /* Mean Squared Error for regression */
} SplitCriterion;

typedef struct DecisionTree_ {
    DecisionTreeNode *root;     /* Root node of the tree */
    TreeType tree_type;         /* Classification or regression */
    SplitCriterion criterion;   /* Splitting criterion */
    
    /* Hyperparameters */
    uint32_t max_depth;         /* Maximum depth of the tree */
    uint32_t min_samples_split; /* Minimum samples required to split a node */
    uint32_t min_samples_leaf;  /* Minimum samples required in a leaf node */
    float min_impurity_decrease; /* Minimum impurity decrease to make a split */
    
    /* Training data info */
    uint32_t num_features;      /* Number of features in the dataset */
    uint32_t num_classes;       /* Number of classes (for classification) */
    uint32_t num_training_samples; /* Total number of training samples */
    
    /* Tree statistics */
    uint32_t num_nodes;         /* Total number of nodes in the tree */
    uint32_t actual_depth;      /* Actual depth of the trained tree */
    float training_accuracy;    /* Training accuracy/R² score */
} DecisionTree;

typedef struct RandomForest_ {
    DecisionTree **trees;       /* Array of decision trees */
    uint32_t num_trees;         /* Number of trees in the forest */
    TreeType forest_type;       /* Classification or regression */
    
    /* Bootstrap sampling parameters */
    float bootstrap_ratio;      /* Ratio of samples to use for each tree (0.0-1.0) */
    uint32_t *bootstrap_indices; /* Indices used for bootstrap sampling */
    
    /* Feature selection parameters */
    uint32_t max_features;      /* Maximum features to consider at each split */
    uint32_t *feature_indices;  /* Available feature indices for random selection */
    
    /* Tree hyperparameters (applied to all trees) */
    uint32_t max_depth;         /* Maximum depth for individual trees */
    uint32_t min_samples_split; /* Minimum samples required to split a node */
    uint32_t min_samples_leaf;  /* Minimum samples required in a leaf node */
    float min_impurity_decrease; /* Minimum impurity decrease to make a split */
    
    /* Training data info */
    uint32_t num_features;      /* Total number of features in the dataset */
    uint32_t num_classes;       /* Number of classes (for classification) */
    uint32_t num_training_samples; /* Total number of training samples */
    
    /* Forest performance metrics */
    float oob_score;            /* Out-of-bag score */
    float training_accuracy;    /* Training accuracy/R² score */
    float *feature_importance;  /* Feature importance scores */
    
    /* Voting/Aggregation method */
    uint32_t voting_method;     /* 0: majority vote, 1: weighted vote, 2: average (regression) */
    
    /* Random state for reproducibility */
    uint32_t random_seed;       /* Seed for random number generation */
} RandomForest;

typedef struct BootstrapSample_ {
    uint32_t *sample_indices;   /* Indices of samples in this bootstrap */
    uint32_t sample_size;       /* Size of the bootstrap sample */
    uint32_t *oob_indices;      /* Out-of-bag sample indices */
    uint32_t oob_size;          /* Number of out-of-bag samples */
} BootstrapSample;

typedef struct FeatureSubset_ {
    uint32_t *feature_indices;  /* Selected feature indices for this tree */
    uint32_t num_selected;      /* Number of selected features */
} FeatureSubset;

typedef enum KernelType_ {
    LINEAR_KERNEL,      /* K(x, y) = x^T * y */
    POLYNOMIAL_KERNEL,  /* K(x, y) = (gamma * x^T * y + coef0)^degree */
    RBF_KERNEL,         /* K(x, y) = exp(-gamma * ||x - y||^2) */
    SIGMOID_KERNEL      /* K(x, y) = tanh(gamma * x^T * y + coef0) */
} KernelType;

typedef enum SVMType_ {
    C_SVC,              /* C-Support Vector Classification */
    NU_SVC,             /* nu-Support Vector Classification */
    ONE_CLASS,          /* One-class SVM */
    EPSILON_SVR,        /* epsilon-Support Vector Regression */
    NU_SVR              /* nu-Support Vector Regression */
} SVMType;

typedef struct SupportVector_ {
    VectorFloatND *vector;      /* Support vector feature values */
    float alpha;                /* Lagrange multiplier (dual coefficient) */
    float y_value;              /* Target value (1/-1 for classification, real for regression) */
    uint32_t original_index;    /* Original index in training set */
} SupportVector;

typedef struct SVMKernel_ {
    KernelType kernel_type;     /* Type of kernel function */
    float gamma;                /* Kernel coefficient for RBF, polynomial, sigmoid */
    float coef0;                /* Independent term for polynomial and sigmoid kernels */
    uint32_t degree;            /* Degree for polynomial kernel */
    float **kernel_cache;       /* Precomputed kernel values cache */
    uint32_t cache_size;        /* Size of kernel cache */
} SVMKernel;

typedef struct SVM_ {
    SVMType svm_type;           /* Type of SVM (classification/regression) */
    SVMKernel kernel;           /* Kernel configuration */
    
    /* Hyperparameters */
    float C;                    /* Regularization parameter */
    float nu;                   /* nu parameter for nu-SVM */
    float epsilon;              /* Epsilon for epsilon-SVR */
    float tolerance;            /* Tolerance for stopping criterion */
    uint32_t max_iterations;    /* Maximum number of iterations */
    
    /* Support vectors */
    SupportVector **support_vectors; /* Array of support vectors */
    uint32_t num_support_vectors;     /* Number of support vectors */
    uint32_t *support_vector_counts;  /* Support vectors per class */
    
    /* Decision function parameters */
    float *dual_coefficients;   /* Dual coefficients (alpha_i * y_i) */
    float bias;                 /* Bias term (intercept) */
    
    /* Training data info */
    uint32_t num_features;      /* Number of features */
    uint32_t num_classes;       /* Number of classes (for classification) */
    uint32_t num_training_samples; /* Total training samples */
    
    /* Multi-class strategy (for classification) */
    uint32_t multiclass_strategy; /* 0: one-vs-one, 1: one-vs-rest */
    struct SVM_ **binary_classifiers; /* Binary classifiers for multi-class */
    uint32_t num_binary_classifiers;  /* Number of binary classifiers */
    
    /* Model performance */
    float training_accuracy;    /* Training accuracy/R² score */
    float cross_validation_score; /* Cross-validation score */
    
    /* Optimization details */
    uint32_t iterations_used;   /* Actual iterations used during training */
    float objective_value;      /* Final objective function value */
    
    /* Feature scaling parameters */
    VectorFloatND *feature_means; /* Mean values for feature scaling */
    VectorFloatND *feature_stds;  /* Standard deviations for feature scaling */
    uint8_t is_scaled;            /* Flag indicating if features are scaled */
} SVM;

typedef struct SVMTrainingData_ {
    VectorFloatND **X;          /* Training feature vectors */
    VectorFloatND *y;           /* Training target values */
    uint32_t num_samples;       /* Number of training samples */
    float *sample_weights;      /* Sample weights (optional) */
} SVMTrainingData;

typedef enum DistanceMetric_ {
    EUCLIDEAN_DISTANCE,     /* sqrt(sum((x_i - y_i)^2)) */
    MANHATTAN_DISTANCE,     /* sum(|x_i - y_i|) */
    MINKOWSKI_DISTANCE,     /* (sum(|x_i - y_i|^p))^(1/p) */
    COSINE_DISTANCE,        /* 1 - (x · y) / (||x|| * ||y||) */
    HAMMING_DISTANCE,       /* Number of differing components */
    CHEBYSHEV_DISTANCE      /* max(|x_i - y_i|) */
} DistanceMetric;

typedef enum WeightFunction_ {
    UNIFORM_WEIGHTS,        /* All neighbors have equal weight */
    DISTANCE_WEIGHTS,       /* Weight = 1 / distance */
    GAUSSIAN_WEIGHTS        /* Weight = exp(-distance^2 / (2 * sigma^2)) */
} WeightFunction;

typedef enum KNNAlgorithm_ {
    BRUTE_FORCE,           /* Compute distance to all points */
    KD_TREE,               /* Use k-d tree for nearest neighbor search */
    BALL_TREE,             /* Use ball tree for nearest neighbor search */
    AUTO_ALGORITHM         /* Automatically choose best algorithm */
} KNNAlgorithm;

typedef struct Neighbor_ {
    VectorFloatND *vector;      /* Feature vector of the neighbor */
    float distance;             /* Distance to query point */
    float target_value;         /* Target value (class or regression value) */
    uint32_t class_label;       /* Class label for classification */
    uint32_t original_index;    /* Original index in training set */
    float weight;               /* Weight based on distance */
} Neighbor;

typedef struct KDTreeNode_ {
    VectorFloatND *point;       /* Point stored in this node */
    float target_value;         /* Target value for this point */
    uint32_t split_dimension;   /* Dimension used for splitting */
    float split_value;          /* Value used for splitting */
    struct KDTreeNode_ *left;   /* Left child (values <= split_value) */
    struct KDTreeNode_ *right;  /* Right child (values > split_value) */
    uint8_t is_leaf;           /* Flag indicating if this is a leaf node */
} KDTreeNode;

typedef struct BallTreeNode_ {
    VectorFloatND *centroid;    /* Centroid of the ball */
    float radius;               /* Radius of the ball */
    VectorFloatND **points;     /* Points contained in this ball */
    uint32_t num_points;        /* Number of points in this ball */
    struct BallTreeNode_ *left; /* Left child */
    struct BallTreeNode_ *right; /* Right child */
    uint8_t is_leaf;           /* Flag indicating if this is a leaf node */
} BallTreeNode;

typedef struct KNN_ {
    /* Algorithm configuration */
    uint32_t k;                 /* Number of neighbors to consider */
    DistanceMetric distance_metric; /* Distance metric to use */
    WeightFunction weight_function; /* Weighting function for neighbors */
    KNNAlgorithm algorithm;     /* Algorithm for nearest neighbor search */
    
    /* Distance metric parameters */
    float minkowski_p;          /* p parameter for Minkowski distance */
    float gaussian_sigma;       /* Sigma parameter for Gaussian weights */
    
    /* Training data */
    VectorFloatND **X_train;    /* Training feature vectors */
    VectorFloatND *y_train;     /* Training target values */
    uint32_t *class_labels;     /* Class labels for classification */
    uint32_t num_training_samples; /* Number of training samples */
    uint32_t num_features;      /* Number of features */
    
    /* Classification/Regression mode */
    uint8_t is_classifier;      /* 1 for classification, 0 for regression */
    uint32_t num_classes;       /* Number of classes (for classification) */
    
    /* Tree structures for fast search */
    KDTreeNode *kd_tree_root;   /* Root of k-d tree */
    BallTreeNode *ball_tree_root; /* Root of ball tree */
    
    /* Feature scaling parameters */
    VectorFloatND *feature_means; /* Mean values for feature scaling */
    VectorFloatND *feature_stds;  /* Standard deviations for feature scaling */
    uint8_t is_scaled;            /* Flag indicating if features are scaled */
    
    /* Performance metrics */
    float training_accuracy;    /* Training accuracy/R² score */
    float cross_validation_score; /* Cross-validation score */
    
    /* Search optimization */
    uint32_t leaf_size;         /* Maximum points in leaf nodes (for trees) */
    float distance_upper_bound; /* Upper bound for distance searches */
    
    /* Prediction cache */
    Neighbor **neighbor_cache;  /* Cache for recently found neighbors */
    uint32_t cache_size;        /* Size of neighbor cache */
} KNN;

typedef struct KNNQuery_ {
    VectorFloatND *query_point; /* Point to find neighbors for */
    Neighbor **neighbors;       /* Array of k nearest neighbors */
    float *distances;           /* Distances to k nearest neighbors */
    uint32_t num_neighbors_found; /* Actual number of neighbors found */
    float prediction;           /* Final prediction (class or value) */
    float *class_probabilities; /* Probability for each class (classification) */
} KNNQuery;

typedef enum NaiveBayesType_ {
    GAUSSIAN_NB,            /* Gaussian Naive Bayes (continuous features) */
    MULTINOMIAL_NB,         /* Multinomial Naive Bayes (discrete features) */
    BERNOULLI_NB,           /* Bernoulli Naive Bayes (binary features) */
    CATEGORICAL_NB          /* Categorical Naive Bayes (categorical features) */
} NaiveBayesType;

typedef struct GaussianFeatureStats_ {
    float mean;             /* Mean of the feature for this class */
    float variance;         /* Variance of the feature for this class */
    float std_dev;          /* Standard deviation of the feature for this class */
    uint32_t sample_count;  /* Number of samples used to compute statistics */
} GaussianFeatureStats;

typedef struct MultinomialFeatureStats_ {
    float *feature_counts;  /* Count of each feature value for this class */
    float total_count;      /* Total count of all feature values for this class */
    uint32_t num_features;  /* Number of distinct feature values */
    float alpha;            /* Smoothing parameter (Laplace smoothing) */
} MultinomialFeatureStats;

typedef struct BernoulliFeatureStats_ {
    float probability;      /* P(feature=1|class) */
    uint32_t positive_count; /* Count of feature=1 for this class */
    uint32_t total_count;   /* Total samples for this class */
    float alpha;            /* Smoothing parameter */
} BernoulliFeatureStats;

typedef struct CategoricalFeatureStats_ {
    float *category_counts; /* Count for each category value */
    uint32_t *category_indices; /* Mapping from category to index */
    uint32_t num_categories; /* Number of distinct categories */
    float total_count;      /* Total count for this feature and class */
    float alpha;            /* Smoothing parameter */
} CategoricalFeatureStats;

typedef struct ClassStats_ {
    uint32_t class_label;   /* Class identifier */
    float prior_probability; /* P(class) */
    uint32_t sample_count;  /* Number of training samples for this class */
    
    /* Feature statistics based on Naive Bayes type */
    union {
        GaussianFeatureStats *gaussian_stats;      /* For Gaussian NB */
        MultinomialFeatureStats *multinomial_stats; /* For Multinomial NB */
        BernoulliFeatureStats *bernoulli_stats;    /* For Bernoulli NB */
        CategoricalFeatureStats *categorical_stats; /* For Categorical NB */
    } feature_stats;
    
    float log_prior;        /* Log of prior probability (for numerical stability) */
} ClassStats;

typedef struct NaiveBayes_ {
    NaiveBayesType nb_type; /* Type of Naive Bayes classifier */
    
    /* Class information */
    ClassStats **class_stats; /* Statistics for each class */
    uint32_t num_classes;   /* Number of classes */
    uint32_t *class_labels; /* Array of class labels */
    
    /* Feature information */
    uint32_t num_features;  /* Number of features */
    uint8_t *feature_types; /* Type of each feature (for mixed types) */
    
    /* Training data info */
    uint32_t num_training_samples; /* Total number of training samples */
    
    /* Smoothing parameters */
    float alpha;            /* Global smoothing parameter (Laplace/additive) */
    float var_smoothing;    /* Variance smoothing for Gaussian NB */
    
    /* Model configuration */
    uint8_t fit_prior;      /* Whether to learn class prior probabilities */
    float *class_prior;     /* User-specified class priors (if fit_prior=0) */
    
    /* Feature scaling (for Gaussian NB) */
    VectorFloatND *feature_means; /* Global feature means */
    VectorFloatND *feature_stds;  /* Global feature standard deviations */
    uint8_t is_scaled;            /* Flag indicating if features are scaled */
    
    /* Performance metrics */
    float training_accuracy;    /* Training accuracy */
    float cross_validation_score; /* Cross-validation score */
    
    /* Prediction optimization */
    float **log_likelihood_cache; /* Cache for log likelihoods */
    uint32_t cache_size;          /* Size of likelihood cache */
    
    /* Feature selection support */
    uint32_t *selected_features;  /* Indices of selected features */
    uint32_t num_selected_features; /* Number of selected features */
    uint8_t feature_selection_enabled; /* Flag for feature selection */
} NaiveBayes;

typedef struct NBPrediction_ {
    uint32_t predicted_class;   /* Predicted class label */
    float *class_probabilities; /* Probability for each class */
    float *log_probabilities;   /* Log probabilities for numerical stability */
    float confidence;           /* Confidence of prediction (max probability) */
    float entropy;              /* Entropy of prediction distribution */
} NBPrediction;

typedef struct NBTrainingData_ {
    VectorFloatND **X;          /* Training feature vectors */
    uint32_t *y;                /* Training class labels */
    uint32_t num_samples;       /* Number of training samples */
    float *sample_weights;      /* Sample weights (optional) */
    
    /* For incremental learning */
    uint8_t is_incremental;     /* Flag for incremental training */
    uint32_t batch_size;        /* Batch size for incremental updates */
} NBTrainingData;


#endif