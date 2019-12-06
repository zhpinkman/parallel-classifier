#include "defs.hpp"

using namespace std;

vector<string> readFromClassifiers(int numberOfClassifiers)
{
    ifstream fd;
    // int numberOfClassifiersRead = 0;
    vector<string> result(numberOfClassifiers);
    for (int i = 0; i < numberOfClassifiers; i++)
    {
        fd.open(("voterPipe_" + to_string(i)), std::fstream::in);
        string input;
        getline(fd, input);
        result[i] = input;
        fd.close();
    }
    return result;
}

vector<vector<int>> parseResult(vector<string> input)
{
    vector<vector<int>> result;
    for (int i = 0; i < input.size(); i++)
    {
        stringstream ss(input[i]);
        string s;
        vector<int> lineResult;
        while (getline(ss, s, ','))
        {
            lineResult.push_back(atoi(s.c_str()));
        }
        result.push_back(lineResult);
    }
    return result;
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

int find_majority_for_data(vector<int> classes)
{
    map<int, int> counts;
    int majority = -1;
    int majority_index;
    for (int i = 0; i < classes.size(); i++)
    {
        counts[classes[i]]++;
    }
    for (int i = 0; i < counts.size(); i++)
    {
        if (counts[i] > majority)
        {
            majority = counts[i];
            majority_index = i;
        }
    }
    return majority_index;
}

vector<int> voteOnParsedResult(vector<vector<int>> input)
{
    vector<int> result(input[0].size());
    for (int j = 0; j < input[0].size(); j++)
    {
        vector<int> temp;
        for (int i = 0; i < input.size(); i++)
        {
            temp.push_back(input[i][j]);
        }
        result[j] = find_majority_for_data(temp);
    }
    // for (int i = 0; i < result.size(); i++)
    // {
    //     cout << result[i];
    // }
    // cout << endl;
    return result;
}

string generate_string_out_of_vector(vector<int> input)
{
    string output = "";
    for (int i = 0; i < input.size(); i++)
    {
        output += to_string(input[i]) + ',';
    }
    // cout << output << endl;
    return output;
}

void sendResultToParent(string msg)
{
    int fd;
    string pipeName = "voterResultPipe";

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

int main(int argc, char *argv[])
{
    int numberOfClassifiers = atoi(argv[1]);

    vector<string> classes = readFromClassifiers(numberOfClassifiers);
    // cout << classes.size() << endl;
    vector<vector<int>> parsedResult = parseResult(classes);
    vector<int> votedResult = voteOnParsedResult(parsedResult);
    string votedResultString = generate_string_out_of_vector(votedResult);
    sendResultToParent(votedResultString);
    return 0;
}