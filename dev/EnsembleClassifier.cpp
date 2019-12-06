#include "defs.hpp"
#define VALIDATION_DIR 1
#define WEIGHT_VECTOR_DIR 2
#define SEPERATOR "/"
#define NEW_LINE '\n'
#define DATASET "dataset"
#define LABELS "labels"
#define CSV_EXT ".csv"
#define COMMA ','

using namespace std;

struct Data
{
    double width;
    double length;
    double bias = 1;
};

typedef vector<Data> Dataset;
typedef vector<int> Labels;
double calc_accuracy(vector<int> result, Labels labels);

Dataset get_dataset(string dataset_dir);
Data getLineData(string line);
Labels get_labels(string label_dir);
vector<int> parseResult(string input);

vector<string> findWeightFiles(string weightVectorsFolderPath)
{
    vector<string> weightFiles;
    DIR *dir = opendir(weightVectorsFolderPath.c_str());
    if (dir == NULL)
    {
        std::cout << "Directory Not Found!" << std::endl;
        return weightFiles;
    }
    struct dirent *entry;
    while (entry = readdir(dir))
    {
        //subDirectory
        if (entry->d_type == DT_DIR)
        {
            continue;
        }
        //subFile
        if (entry->d_type == DT_REG)
        {
            std::string temp_dir;
            temp_dir = weightVectorsFolderPath + "/" + entry->d_name;
            weightFiles.push_back(temp_dir);
        }
    }
    closedir(dir);
    return weightFiles;
}

int main(int argc, char const *argv[])
{
    vector<string> weightFiles;
    Dataset dataset;
    string validation_dir = argv[1];
    Labels labels = get_labels(validation_dir + SEPERATOR + LABELS + CSV_EXT);
    string weight_vector_dir = argv[2];
    string voter_named_pipe = "voterPipe_";
    string voter_result_pipe = "voterResultPipe";
    weightFiles = findWeightFiles(weight_vector_dir);
    int linearPipe[2];
    int voterPipe[2];

    for (int i = 0; i < weightFiles.size(); i++)
    {
        unlink((voter_named_pipe + to_string(i)).c_str());
        mkfifo((voter_named_pipe + to_string(i)).c_str(), 0666);
    }

    unlink(voter_result_pipe.c_str());
    mkfifo(voter_result_pipe.c_str(), 0666);

    for (int i = 0; i < weightFiles.size(); i++)
    {
        pipe(linearPipe);

        int p = fork();
        if (p == 0)
        {
            close(linearPipe[1]);
            char input[1000];
            read(linearPipe[0], input, 1000);
            close(linearPipe[0]);
            char *args[] = {"./linearClassifier.out", input, NULL};
            execv(args[0], args);
        }
        else if (p > 0)
        {
            close(linearPipe[0]);
            string output = validation_dir + '@' + weight_vector_dir + '@' + to_string(i) + '@';
            write(linearPipe[1], output.c_str(), output.length() + 1);
            close(linearPipe[1]);
            // wait(NULL);
        }
    }

    pipe(voterPipe);
    int p = fork();
    if (p == 0)
    {
        close(voterPipe[1]);
        char input[1000];
        read(voterPipe[0], input, 1000);
        char *args[] = {"./voter.out", input, NULL};
        execv(args[0], args);
        close(voterPipe[0]);
    }
    else if (p > 0)
    {
        close(voterPipe[0]);
        string msg = to_string(weightFiles.size());
        write(voterPipe[1], msg.c_str(), msg.length() + 1);
        close(voterPipe[1]);
    }

    ifstream fd;
    fd.open("voterResultPipe", std::fstream::in);
    string voterResult;
    getline(fd, voterResult);
    // cout << voterResult << endl;
    vector<int> result = parseResult(voterResult);
    // for (int i = 0; i < result.size(); i++)
    // {
    //     cout << result[i];
    // }
    // cout << endl;
    double accuracy = calc_accuracy(result, labels);
    // for (int i = 0; i < result.size(); i++)
    //     cout << result[i];
    // cout << endl;
    // for (int i = 0; i < labels.size(); i++)
    //     cout << labels[i];
    // cout << endl;
    printf("Accuracy: %.2f%%\n", accuracy * 100);

    fd.close();

    unlink(voter_result_pipe.c_str());

    for (int i = 0; i < weightFiles.size(); i++)
    {
        unlink((voter_named_pipe + to_string(i)).c_str());
    }

    // cout << 22 << endl;
    return 0;
}

double calc_accuracy(vector<int> result, Labels labels)
{
    int count = 0;
    // cout << result.size() << endl;
    for (int i = 0; i < result.size(); i++)
    {
        // cout << result[i] << " " << labels[i] << endl;
        if (result[i] == labels[i])
        {
            // cout << "here" << endl;
            count++;
        }
    }
    return double(count) / double(labels.size());
}

vector<int> parseResult(string input)
{
    stringstream ss(input);
    string temp;
    vector<int> result;
    while (getline(ss, temp, COMMA))
    {
        result.push_back(atoi(temp.c_str()));
    }
    return result;
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
        dataset.push_back(getLineData(line));
    }
    file.close();
    return dataset;
}

Data getLineData(string line)
{
    stringstream templine(line);
    string data;
    Data rowData;
    getline(templine, data, COMMA);
    rowData.width = atof(data.c_str());
    getline(templine, data, COMMA);
    rowData.length = atof(data.c_str());
    return rowData;
}

Labels get_labels(string filename)
{
    Labels labels;
    ifstream file;
    file.open(filename);
    string line;

    getline(file, line);

    while (getline(file, line, NEW_LINE))
    {

        labels.push_back(stod(line));
    }
    file.close();
    return labels;
}