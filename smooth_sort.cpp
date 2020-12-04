#include <string>
#include <vector>
#include <fstream>
#include "smoothsort.h"
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;

const char TMP_FILENAME[] = "C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\tmp.txt";
const string RESULT_FILENAME_PREFIX = "C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\result";
const string INPUT_FILENAME_PREFIX = "C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\";

vector<string> get_lines(vector<char> chars) {
    vector<string> lines;
    string row;
    for(char c : chars) {
        if(c != '\n') {
            row += c;
            continue;
        }
        lines.push_back(row);
        row = "";
    }
    if (!row.empty() && row[0] != '\0') {
        lines.push_back(row);
    }
    return lines;
}

void append_sorted(vector<string> lines, string filename) {
    ifstream file;
    ofstream tmp_file;
    file.open(filename, ifstream::in);
    tmp_file.open(TMP_FILENAME, ifstream::out);
    string line;
    getline(file, line);
    int i = 0;
    while (i < lines.size() && file.is_open()) {
        if (lines[i] < line) {
            tmp_file << lines[i] << endl;
            ++i;
            continue;
        }
        tmp_file << line << endl;
        if (!getline(file, line)) {
            break;
        }
    }
    for (; i < lines.size(); ++i) {
        tmp_file << lines[i] << endl;
    }
    if (!file.eof()) {
        tmp_file << line << endl;
        while(getline(file, line)) {
            tmp_file << line << endl;
        }
    }
    if (file.is_open()) {
        file.close();
        if (remove(filename.data()) != 0) {
            perror("Error removing file");
        }
    }
    tmp_file.close();
    /*	Deletes the file if exists */
    if (rename(TMP_FILENAME, filename.data()) != 0)
        perror("Error renaming file");

}

string generate_line(int size_b) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 1);
    std::uniform_int_distribution<int> characters(97, 122);
    std::uniform_int_distribution<int> numbers(48, 57);
    string line;
    for (int i = 0; i < size_b; ++i) {
        int char_or_number = dist(mt);
        if (char_or_number) {
            line += char(characters(mt));
        } else {
            line += char(numbers(mt));
        }
    }
    return line;
}

void generate_test(string filename, int size_mb, int line_length) {
    ofstream file;
    file.open(filename);
    for (int i = 0; i < size_mb*1e6/line_length - 1; ++i) {
        file << generate_line(line_length-1) << endl;
    }
    file << generate_line(line_length-1);
    file.close();
}

vector<string> readBatch(ifstream& is, int num_lines){
    vector<string> lines;
    for (int i = 0; i < num_lines; ++i) {
        string line;
        getline(is, line);
        lines.push_back(line);
    }
    return lines;
}

int main(int argc, char *argv[]) {
//    generate_test("C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\bigfile1000.txt", 1000, 1000);
//    generate_test("C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\bigfile500.txt", 1000, 500);
//    generate_test("C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\bigfile100.txt", 1000, 100);
//    generate_test("C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\mediumfile1000.txt", 100, 1000);
//    generate_test("C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\mediumfile500.txt", 100, 500);
//    generate_test("C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\mediumfile100.txt", 100, 100);
//    generate_test("C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\smallfile1000.txt", 10, 1000);
//    generate_test("C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\smallfile500.txt", 10, 500);
//    generate_test("C:\\Studying\\Sasha\\AlgoFedorus\\smooth-sort\\smallfile100.txt", 10, 100);
//    return 0;
    for (const string& name : vector<string>{"bigfile", "mediumfile", "smallfile"}) {
        int file_size;
        if (name == "bigfile") {
            file_size = 1000;
        } else if (name == "mediumfile") {
            file_size = 100;
        } else {
            file_size = 10;
        }
        cout << "file size: " << file_size << endl;
        for (int row_length : vector<int>{1000, 500, 100}) {
            ifstream bigFile(INPUT_FILENAME_PREFIX + name + to_string(row_length) + ".txt");
            int bufferSize = file_size * int(1e6) / row_length / 4;
            high_resolution_clock::time_point t1 = high_resolution_clock::now();
            vector<duration<double>> inner_time_spans;
            while (bigFile) {
                high_resolution_clock::time_point t1_inner = high_resolution_clock::now();
                vector<string> lines = readBatch(bigFile, bufferSize);
                smoothsort(lines.data(), lines.size());
                append_sorted(lines, RESULT_FILENAME_PREFIX + name + to_string(row_length) + ".txt");
                high_resolution_clock::time_point t2_inner = high_resolution_clock::now();
                auto time_span_inner = duration_cast<duration<double>>(t2_inner - t1_inner);
                inner_time_spans.push_back(time_span_inner);
            }
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            auto time_span = duration_cast<duration<double>>(t2 - t1);
            cout << "Row length: " << row_length << endl;
            for (int i = 0; i < inner_time_spans.size(); ++i) {
                cout << "Chunk " << i << ": it took me " << inner_time_spans[i].count() << " seconds.";
                cout << endl;
            }
            cout << "It took me " << time_span.count() << " seconds.";
            cout << endl << endl;
        }

        cout << endl;
    }
    return 0;
}
