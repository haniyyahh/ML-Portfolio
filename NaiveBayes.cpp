
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>       // std::string
#include <vector>       // std::vector
#include <math.h>       // exp, sqrt
#include <iomanip>
#include <chrono>

using namespace std;

double calc_age_lh(double age, double mean_age, double var_age){
    return 1 / (sqrt(2 * M_PI * var_age)) * exp(-((age-mean_age)*(age-mean_age)) / (2 * var_age));
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
    vector<double> age(MAX_LEN);

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
    // DIVIDING TRAIN AND TEST DATA

    // Predictors: age, pclass, sex
    double data_matrix[800][4];

    double test_matrix[246][4];

    // Target: survived
    double train_labels [800];

    double test_labels[246];

    // Set Values
    for(int i = 0; i < 1046; i++){
        if(i < 800){
            data_matrix[i][0] = 1;
            data_matrix[i][1] = age[i];
            data_matrix[i][2] = pclass[i];
            data_matrix[i][3] = sex[i];

            train_labels[i] = survived[i];
        }
        else{
            test_matrix[i - 800][0] = 1;
            test_matrix[i - 800][1] = age[i];
            test_matrix[i - 800][2] = pclass[i];
            test_matrix[i - 800][3] = sex[i];

            test_labels[i - 800] = survived[i];
        }

    }

    // NAIVE BAYES

    // Time
    chrono::time_point<std::chrono::system_clock> start, end;
    start = chrono::system_clock::now();

    // Calculate priors
    double overallSurvived = 0.0;
    for(int i = 0; i < 800; i++){
        if(train_labels[i] == 1)
            overallSurvived += 1.0;
    }

    double apriori[] = {(800 - overallSurvived) / 800.00, overallSurvived / 800.00};


    // C++: 0.61  0.39
    cout << "\nPrior Probs: " << setprecision(5) << apriori[0] << "  " << apriori[1] << endl;

    double count_survived[] = {(800 - overallSurvived), overallSurvived};
    
    // likelihood for pclass (data_matrix[][2])
    // 2 by 3 matrix, initialize all values as zero
    double lh_pclass[2][3];
    for(int sv = 0; sv < 2; sv++)
        for(int pc = 0; pc < 3; pc++){
            int nrow = 0;
            for(int i = 0; i < 800; i++){
                if(data_matrix[i][2] == pc + 1 && train_labels[i] == sv)
                    nrow++;
            lh_pclass[sv][pc] = nrow / count_survived[sv];
            }
        }
    

    // likelihood for sex
    // 2 by 2 matrix, initialize all values as zero
    double lh_sex[2][2];
    for(int sv = 0; sv < 2; sv++)
        for(int pc = 0; pc < 2; pc++){
            int nrow = 0;
            for(int i = 0; i < 800; i++){
                if(data_matrix[i][3] == pc && train_labels[i] == sv)
                    nrow++;
            lh_sex[sv][pc] = nrow / count_survived[sv];
            }
        }

    cout << endl;

    cout << "likelihood p(survived|pclass)" << endl;
    // likelihood p(survived|pclass)
    for(int r = 0; r < 2; r++){
        for(int c = 0; c < 3; c++)
            cout << lh_pclass[r][c] << '\t';
        cout << endl;
    }

    cout << endl;
    
    cout << "likelihood p(survived|sex)" << endl;
    // likelihood p(survived|sex)
    for(int r = 0; r < 2; r++){
        for(int c = 0; c < 2; c++)
            cout << lh_sex[r][c] << '\t';
        cout << endl;
    }

    // Calculate likelihood of quanitative variables

    // mean
    double age_mean[2];
    double m = 0;
    double m2 = 0;

    for(int sv = 0; sv < 2; sv++){
        for(int i = 0; i < 800; i++)
            if(train_labels[i] == sv){
                age_mean[sv] += data_matrix[i][1];
                if(sv == 0)
                    m++;
                else
                    m2++;
            }
    }

    cout << endl;

    age_mean[0] /= m;
    age_mean[1] /= m2;

    cout << "Age Means: " << age_mean[0] << " " << age_mean[1] << endl;

    // variance
    double age_var[2];
    for(int sv = 0; sv < 2; sv++){
        for(int i = 0; i < 800; i++)
            if(train_labels[i] == sv){
                age_var[sv] += ((age_mean[sv] - data_matrix[i][1])*(age_mean[sv] - data_matrix[i][1]));
            }
    }

    age_var[0] /= (m - 1);
    age_var[1] /= (m2 - 1);

    cout << "Age Variances: " << age_var[0] << " " << age_var[1] << endl;

    cout << endl;


    // Probability density for quantitative data
    // function calc_age_lh(age, mean_age, var_age) above

    // Function for scratch model (calculate raw probabilities given pclass, sex, and age)
    double raw[246];

    for(int i = 0; i < 246; i++){
    int pc = test_matrix[i][2] - 1; 
    int se = test_matrix[i][3]; 
    double ag = test_matrix[i][1];

    double num_s = lh_pclass[1][pc] * lh_sex[1][se] * apriori[1] * 
        calc_age_lh(ag, age_mean[1], age_var[1]);

    double denominator = lh_pclass[1][pc] * lh_sex[1][se] * apriori[1] * 
        calc_age_lh(ag, age_mean[1], age_var[1]) + 
        lh_pclass[0][pc] * lh_sex[0][se] * apriori[0] * 
        calc_age_lh(ag, age_mean[0], age_var[0]);
    
    raw[i] = num_s / denominator;
    }

    end = chrono::system_clock::now();
    chrono::duration<double> seconds = end - start;
    cout << "Training Time: " << seconds.count() << "s\n\n";

    // Predict
    int probabilities[246];

    for(int i = 0; i < 246; i++){
        if(raw[i] > .5)
            probabilities[i] = 1;
        else
            probabilities[i] = 0;
    }
    
    // Accuracy
    double sum = 0;
    for(int i = 0; i < 246; i++)
        if(probabilities[i] == test_labels[i])
            sum++;
    
     cout << "accuracy: " << sum / 246 << endl;

     // Specificity & Sensitivity
    double TN = 0, FP = 0, TP = 0, FN = 0;;
    for(int i = 0; i<246; i++){
        if(probabilities[i] == 0 && test_labels[i] == 0)
            TN++;
        if(probabilities[i] == 1 && test_labels[i] == 0)
            FP++;
        if(probabilities[i] == 1 && test_labels[i] == 1)
            TP++;
        if(probabilities[i] == 0 && test_labels[i] == 1)
            FN++;
    }

    double specificity = (TN / (TN + FP));
    double sensitivity = (TP / (TP + FN));

    cout << "specificity: " << specificity << endl;
    cout << "sensitivity: " << sensitivity << endl;
    
    /////////////////////////////////////////

    cout << "\nProgram Terminated.";

    return 0;
}