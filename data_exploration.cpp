#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm> // for sort()
#include<string.h>
#include<cmath> // for sqrt()

using namespace std;

// function declarations
void print_stats(vector<double>);
double calculateSum(vector<double>);
double calculateMean(vector<double>, double);
double calculateMedian(vector<double>);
double calculateRange(vector<double>);
double covar(vector<double>, vector<double>);
double cor(vector<double>, vector<double>);



int main(int argc, char** argv){

    ifstream inFS;  // input file stream
    string line;
    string rm_in, medv_in;
    const int MAX_LEN = 1000;
    vector<double> rm(MAX_LEN);
    vector<double> medv(MAX_LEN);

    // try to open file
    cout << "Opening file Boston.csv." << endl;

    inFS.open("Boston.csv");

    // output error message if it doesn't open
    if(!inFS.is_open()){
        cout << "Could not open file Boston.csv." << endl;
        return 1; // 1 indicates error
    }

    // can now use inFS stream like cin stream
    // Boston.csv should contain two doubles

    // read first line
    cout << "Reading line 1" << endl;
    getline(inFS, line);

    // echo out the heading
    cout << "Heading: " << line << endl;

    int numObservations = 0;
    while (inFS.good()){

        getline(inFS, rm_in, ',');
        getline(inFS, medv_in, '\n');

        rm.at(numObservations) = stof(rm_in);
        medv.at(numObservations) = stof(medv_in);

        numObservations++;

    }

    rm.resize(numObservations);
    medv.resize(numObservations);

    cout << "New length: " << rm.size() << endl;

    cout << "Closing file Boston.csv" << endl;
    inFS.close(); // done using the file, close it


    cout << "Number of records: " << numObservations << endl;

    cout << endl << endl <<  "----------------------------------" << endl;

    cout << "\nStats for rm" << endl;
    print_stats(rm);



    cout << "\nStats for medv" << endl;
    print_stats(medv);

    cout << "\nCovariance = " << covar(rm, medv) << endl;
    
    cout << "\nCorrelation = " << cor(rm, medv) << endl;

    cout << "\nProgram terminated.";

    return 0;


}

// method that prints out all the stats for rm and medv
void print_stats(vector<double> v){
    double sum = calculateSum(v);
    double mean = calculateMean(v, sum);
    double median = calculateMedian(v);
    double range = calculateRange(v);

    cout << "Sum: " << sum << endl;
    cout << "Mean: " << mean << endl;
    cout << "Median: " << median << endl;
    cout << "Range: " << range << endl;

}

// method to calcuate the sum
double calculateSum(vector<double> v){
        double sum = 0;
        for(int i = 0; i < v.size(); i++){
            sum += v[i];

        }
        return sum;
}

// method to calculate the mean
double calculateMean(vector<double> v, double n){
    return (n/(v.size()));
}

// method to calculate the median
double calculateMedian(vector<double> v){
    // sort vector, then divide vector size in half
    // return v[middle value]
    sort(v.begin(), v.end());
    double mid = v.size() / 2.0;
    return v[mid];

}

// method to calculate the range
double calculateRange(vector<double> v){
    // sort vector, then return difference between
    // last and first elements
    sort(v.begin(), v.end());
    return (v[v.size()-1]) - v[0];

}

// method to calculate the covariance
double covar(vector<double> rm, vector<double> medv){
    // covariance
    // loop from first to last index of vectors
    // vector[i] - mean, for both vectors
    // multiply differences, add to accumulator, reset x and y
    // end loop, divide by size-1, return
    double sum1 = calculateSum(rm);
    double sum2 = calculateSum(medv);
    double rmMEAN = calculateMean(rm, sum1);
    double medvMEAN = calculateMean(medv, sum2);

    double x = 0, y = 0, z = 0, accumulator = 0;
    for(int i = 0; i < rm.size(); i++){
        x = rm[i] - rmMEAN;
        y = medv[i] - medvMEAN;
        z = x * y;
        accumulator += z;

    }
    return accumulator/(rm.size() - 1);


}

// method to calculate correlation
double cor(vector<double> rm, vector<double> medv){

    // get covariance
    // get variance of rm and medv, then sqrt both
    // multiply both quotients and divide into covar
    // return
    double sum1 = calculateSum(rm);
    double sum2 = calculateSum(medv);
    double rmMEAN = calculateMean(rm, sum1);
    double medvMEAN = calculateMean(medv, sum2);

    double x = 0, y = 0, z = 0, accumulator = 0;
    for(int i = 0; i < rm.size(); i++){
        x = rm[i] - rmMEAN;
        x = x*x;
        accumulator += x;
    }
    double sigmaX = accumulator/(rm.size()-1);


    x = 0, y = 0, z = 0, accumulator = 0;
    for(int i = 0; i < medv.size(); i++){
        x = medv[i] - medvMEAN;
        x = x*x;
        accumulator += x;
    }
    double sigmaY = accumulator/(medv.size()-1);

    double covariance = covar(rm, medv);

    return covariance / (sqrt(sigmaX) * sqrt(sigmaY));

}
