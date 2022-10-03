#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>       // std::string
#include <vector>       // std::vector
#include <math.h>       // exp
#include <chrono>       // time

using namespace std;

// It works
double sigmoid(double input){
    return 1.0 / (1.0 + exp(-input));
}

void apply_sigmoid(double **array, size_t rows, size_t cols){
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < cols; c++)
            array[r][c] = sigmoid(array[r][c]);
    }
}

void apply_log_odds(double **array, size_t rows, size_t cols){
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < cols; c++)
            array[r][c] = exp(array[r][c]) / (1.0 + exp(array[r][c]));
    }
}

void matrix_multiplication(double **array, size_t rows, size_t cols, double **array2, size_t col2, double **result){
    for(int r = 0; r < rows; r++)
        for (int j = 0; j < col2; j++){
            double sum = 0.0;

            for(int c = 0; c < cols; c++){
                sum += array[r][c] * array2[c][j];
            }
            
            result[r][j] = sum;
        }

}

void transpose(double **array, size_t rows, size_t cols, double **result){
    for(int c = 0; c < cols; c++)
        for(int r = 0; r < rows; r++)
            result[c][r] = array[r][c];
            
}

void calculate_error(int *labels, double **array, size_t rows){
    for(int r = 0; r < rows; r++)
        array[r][0] = labels[r] - array[r][0];
}

void print_matrix(int **array, size_t rows, size_t cols){
    for(int r = 0; r < rows; r++){
        for(int c = 0; c < cols; c++)
            std::cout << array[r][c] << '\t';
        std::cout << std::endl;
    }
}

int main(int argc, char** argv) {
    ifstream inFS; // Input file stream
    string headings;
    string id_in, pclass_in, survived_in, sex_in, age_in;

    const int MAX_LEN = 2000;
    
    vector<string> id(MAX_LEN);

    vector<int> pclass(MAX_LEN);
    vector<int> survived(MAX_LEN);
    vector<int> sex(MAX_LEN);
    vector<double> age(MAX_LEN); // might need to go back and fix this to be double :(

    // Try to open file
    cout << "titanic_project.csv" << endl;

    inFS.open("titanic_project.csv");
    if (!inFS.is_open()) {
        cout << "Could not open file titanic_project.csv." << endl;
        return 1; // 1 indicates error
    }

    // Can now use inFS stream like cin stream
    // titanic_project.csv should contain one string and four integers

    cout << "Reading line 1" << endl;
    getline(inFS, headings);

    // echo heading
    cout << "heading: " << headings << endl;

    int numObservations = 0;
    while (inFS.good()) {

        getline(inFS, id_in, ',');
        getline(inFS, pclass_in, ',');
        getline(inFS, survived_in, ',');
        getline(inFS, sex_in, ',');
        getline(inFS, age_in, '\n');

        id.at(numObservations) = id_in;
        pclass.at(numObservations) = stoi(pclass_in);
        survived.at(numObservations) = stoi(survived_in);
        sex.at(numObservations) = stoi(sex_in);
        age.at(numObservations) = stod(age_in);

        numObservations++;
    }

    id.resize(numObservations);
    pclass.resize(numObservations);
    survived.resize(numObservations);
    sex.resize(numObservations);
    age.resize(numObservations);

    cout << "Closing file titanic_project.csv." << endl;
    inFS.close(); // Done with file, so close it

    cout << "Number of records: " << numObservations << endl;

    /////////////////////////////////////////
    // Logistic Regression

    // Weights
    double **weight;
    weight = new double *[2];
    weight[0] = new double[1];
    weight[1] = new double[1];
    weight[0][0] = 1;
    weight[1][0] = 1;

    // Predictor Sex
    double **data_matrix;
    data_matrix = new double *[800];

    double **test_matrix;
    test_matrix = new double *[246];

    // Target Survived
    int *train_labels;
    train_labels = new int [800];

    int *test_labels;
    test_labels = new int [246];

    // Set Values
    for(int i = 0; i < 1046; i++){
        if(i < 800){
            data_matrix[i] = new double[2];
            data_matrix[i][0] = 1;
            data_matrix[i][1] = sex[i];

            train_labels[i] = survived[i];
        }
        else{
            test_matrix[i - 800] = new double[2];
            test_matrix[i - 800][0] = 1;
            test_matrix[i - 800][1] = sex[i];

            test_labels[i - 800] = survived[i];
        }

    }

    
    // Gradient Descent
    double learning_rate = .001;

    // Time
    chrono::time_point<std::chrono::system_clock> start, end;
    start = chrono::system_clock::now();

    // Training
    for(int i = 0; i < 10000; i++){

    // Memory allocation
    double **result;
    result = (double**)malloc(800*sizeof(double*));
    for (int i = 0; i<800; i++)
        result[i] = (double*)malloc(1*sizeof(double));

    
    double **result2;
    result2 = (double**)malloc(2*sizeof(double*));
    for (int i = 0; i<2; i++)
        result2[i] = (double*)malloc(1*sizeof(double));

    double **result3;
    result3 = (double**)malloc(2*sizeof(double*));
    for (int i = 0; i<2; i++)
        result3[i] = (double*)malloc(800*sizeof(double));



    // Calculation
    matrix_multiplication(data_matrix, 800, 2, weight, 1, result);
    apply_sigmoid(result, 800, 1);
    calculate_error(train_labels, result, 800);

    transpose(data_matrix, 800, 2, result3);
    matrix_multiplication(result3, 2, 800, result, 1, result2);
    weight[0][0] = result2[0][0] * learning_rate + weight[0][0];
    weight[1][0] = result2[1][0] * learning_rate + weight[1][0];


    // Free Memory
    for(int i=0;i<800;i++)
        free(result[i]);
    free(result);

    for(int i=0;i<2;i++)
        free(result2[i]);
    free(result2);

    for(int i=0;i<2;i++)
        free(result3[i]);
    free(result3);

    }
    end = chrono::system_clock::now();
    chrono::duration<double> seconds = end - start;
    cout << "\nTraining Time: " << seconds.count() << "s\n";



    cout << "w1: " << weight[0][0] << endl;
    cout << "w2: " << weight[1][0] << endl;


    // Predict with the Weights we Generated
    double **probabilities;
    probabilities = (double**)malloc(246*sizeof(double*));
    for (int i = 0; i<246; i++)
        probabilities[i] = (double*)malloc(1*sizeof(double));

    matrix_multiplication(test_matrix, 246, 2, weight, 1, probabilities);
    
    apply_log_odds(probabilities, 246, 1);
    
    // Binarize
    for(int i = 0; i < 246; i++)
        if(probabilities[i][0] > .5)
            probabilities[i][0] = 1;
        else
            probabilities[i][0] = 0;

    // Accuracy ~ .7845528
    double sum = 0;
    for(int i = 0; i < 246; i++)
        if(probabilities[i][0] == test_labels[i])
            sum++;
    
    cout << "accuracy: " << sum / 246 << endl;

    // Specificity & Sensitivity
    double TN = 0, FP = 0, TP = 0, FN = 0;;
    for(int i = 0; i<246; i++){
        if(probabilities[i][0] == 0 && test_labels[i] == 0)
            TN++;
        if(probabilities[i][0] == 1 && test_labels[i] == 0)
            FP++;
        if(probabilities[i][0] == 1 && test_labels[i] == 1)
            TP++;
        if(probabilities[i][0] == 0 && test_labels[i] == 1)
            FN++;
    }

    double specificity = (TN / (TN + FP));
    double sensitivity = (TP / (TP + FN));

    cout << "specificity: " << specificity << endl;
    cout << "sensitivity: " << sensitivity << endl;


    // Free Memory
    for(int i=0;i<246;i++)
        free(probabilities[i]);
    free(probabilities);

    /////////////////////////////////////////
    // Naive Bayes

    /////////////////////////////////////////

    cout << "\nProgram Terminated.";

    return 0;
}