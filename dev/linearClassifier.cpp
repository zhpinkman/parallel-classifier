#include "defs.hpp"

#define NEW_LINE '\n'
#define DATASET "dataset"
#define VALIDATION_DIR 1
#define WEIGHT_VECTOR_DIR 2
#define WEIGHT_VECTOR_FILE_NUMBER 3
#define SEPERATOR "/"
#define COMMA ','
#define ATSIGN '@'
#define CSV_EXT ".csv"
#define EOF_STRING "EOF"
#define WEIGHT_DIR "weight_vectors"
#define CLASSIFIER_PREFIX "classifier_"

using namespace std;

struct Classifier
{
    double betha0;
    double betha1;
    double bias;
};

struct Data
{
    double width;
    double length;
};

typedef vector<Data> Dataset;
typedef vector<int> Labels;
typedef vector<Classifier> Classifiers;

Dataset get_dataset(string dataset_dir);
Data getDatasetLineData(string line);

Classifier getClassifierLineData(string line);

Classifiers get_classifiers(string filename);

double get_score(Data data, Classifier classifier);
vector<int> generate_classes(Dataset dataset, Classifiers classifiers);
int get_most_fitted_class(vector<double> scores);
int get_number_of_class(int _class, vector<int> classes);
string generate_result_string(vector<int> result);
void send_result_to_pipe(string msg, string classifier_index);

void sendToPipe(std::stringstream &sendLine)
{
    std::ofstream pipeStream("voterPipe", std::ofstream::in | std::ofstream::out);
    if (pipeStream.is_open())
    {
        // std::cout << "a " << sendLine.str();
        pipeStream << sendLine.str();
        pipeStream.close();
    }
    else
    {
        std::cerr << "Error in opening fifo from Linear." << std::endl;
    }
}

void sendClassifiedDataToVoter(vector<int> classifiedData)
{
    for (int i = 0; i < classifiedData.size(); i++)
    {
        std::stringstream sendLine;
        sendLine << i << "," << classifiedData[i] << std::endl;
        sendToPipe(sendLine);
    }

    std::stringstream sendLine;
    sendLine << EOF_STRING << "," << EOF_STRING << std::endl;
    sendToPipe(sendLine);
}

int main(int argc, char *const argv[])
{
    string args = argv[1];
    stringstream ss(args);
    string validation_dir;
    string weight_vectors_dir;
    string weight_file_index;
    vector<int> result;
    getline(ss, validation_dir, ATSIGN);
    getline(ss, weight_vectors_dir, ATSIGN);
    getline(ss, weight_file_index, ATSIGN);
    string weight_vector_file_name = weight_vectors_dir + SEPERATOR + CLASSIFIER_PREFIX + weight_file_index + CSV_EXT;
    string dataset_file_name = validation_dir + SEPERATOR + DATASET + CSV_EXT;
    Classifiers classifiers = get_classifiers(weight_vector_file_name);

    Dataset dataset = get_dataset(dataset_file_name);
    // cout << dataset[0].width << " " << dataset[0].length << endl;
    result = generate_classes(dataset, classifiers);
    // sendClassifiedDataToVoter(result);
    string result_string = generate_result_string(result);
    send_result_to_pipe(result_string, weight_file_index);
    return 0;
}

void send_result_to_pipe(string msg, string classifier_index)
{
    int fd;
    string pipeName = "voterPipe_" + classifier_index;

    // cout << pipeName << endl;
    fd = open(pipeName.c_str(), O_WRONLY);
    if (fd < 0)
    {
        cout << "error opening pipe [classifier]" << endl;
    }
    // cout << "2" << endl;
    write(fd, msg.c_str(), msg.length());
    // cout << "3" << endl;
    close(fd);
}

string generate_result_string(vector<int> result)
{
    string output = "";
    for (int i = 0; i < result.size(); i++)
    {
        output += to_string(result[i]) + ',';
    }
    return output;
}

int get_number_of_class(int _class, vector<int> classes)
{
    int count = 0;
    for (int i = 0; i < classes.size(); i++)
    {
        if (_class == classes[i])
        {
            count++;
        }
    }
    return count;
}

int get_most_fitted_class(vector<double> scores)
{
    double most_fitted_score = -FLT_MAX;
    int most_fitted_index;
    for (int i = 0; i < scores.size(); i++)
    {
        if (scores[i] > most_fitted_score)
        {
            most_fitted_score = scores[i];
            most_fitted_index = i;
        }
    }
    return most_fitted_index;
}

vector<int> generate_classes(Dataset dataset, Classifiers classifiers)
{
    vector<int> result;
    for (int i = 0; i < dataset.size(); i++)
    {
        vector<double> class_scores;
        for (int j = 0; j < classifiers.size(); j++)
        {
            class_scores.push_back(get_score(dataset[i], classifiers[j]));
        }
        result.push_back(get_most_fitted_class(class_scores));
    }
    return result;
}

double get_score(Data data, Classifier classifier)
{
    // cout << data.width * classifier.betha1 + data.length * classifier.betha0 + classifier.bias << endl;
    return data.width * classifier.betha1 + data.length * classifier.betha0 + classifier.bias;
}

Classifiers get_classifiers(string filename)
{
    Classifiers classifiers;
    ifstream file;
    file.open(filename);
    string line;
    getline(file, line, NEW_LINE);
    while (getline(file, line, NEW_LINE))
    {
        Classifier lineData;
        classifiers.push_back(getClassifierLineData(line));
    }
    file.close();
    return classifiers;
}

Classifier getClassifierLineData(string line)
{
    stringstream templine(line);
    string data;
    Classifier rowData;
    getline(templine, data, COMMA);
    rowData.betha0 = atof(data.c_str());
    getline(templine, data, COMMA);
    rowData.betha1 = atof(data.c_str());
    getline(templine, data, COMMA);
    rowData.bias = atof(data.c_str());
    return rowData;
}
Dataset get_dataset(string filename)
{
    Dataset dataset;
    ifstream file;
    file.open(filename);
    string line;

    getline(file, line, NEW_LINE);

    while (getline(file, line, NEW_LINE))
    {
        // cout << line << endl;
        Data lineData;
        dataset.push_back(getDatasetLineData(line));
    }
    file.close();
    return dataset;
}

Data getDatasetLineData(string line)
{
    stringstream templine(line);
    string data;
    Data rowData;
    getline(templine, data, COMMA);
    rowData.length = atof(data.c_str());
    getline(templine, data, COMMA);
    rowData.width = atof(data.c_str());
    return rowData;
}
