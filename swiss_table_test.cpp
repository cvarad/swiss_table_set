#include "swiss_table.h"

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <type_traits>

using namespace std;
using namespace std::chrono;

const char* ANSI_RESET = "\u001B[0m";
const char* GREEN = "\u001B[32m";

template <class T>
std::vector<T> read_data(std::string filename) {
    std::vector<T> data;
    T line;
    
    ifstream data_file(filename);
    while (data_file >> line) {
        data.push_back(line);
    }
    data_file.close();

    return data;
}

void print_time(std::string operation, int64_t duration) {
    printf("%s: %s%ld μs%s\n", operation.c_str(), GREEN, duration, ANSI_RESET);
}

template <class T, template <class U> class S>
void benchmark_set(vector<T> &data) {
    S<T> set;

    // insert
    auto start = high_resolution_clock::now();
    for (T &t : data) {
        set.insert(t);
    }
    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    print_time("Insert", duration.count());

    // find
    start = high_resolution_clock::now();
    for (T &t : data) {
        set.find(t);
    }
    duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    print_time("Find", duration.count());

    // erase
    start = high_resolution_clock::now();
    for (T &t : data) {
        set.erase(t);
    }
    duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    print_time("Erase", duration.count());
}

template <class T>
void test(std::string filename, std::string testname) {
    cout << "\n*** " << testname << " ***" << endl;
    auto words = read_data<T>(filename);

    cout << "Testing: sst::swiss_table_set" << endl;
    benchmark_set<T, sst::swiss_table_set>(words);
    cout << "Testing: std::unordered_set" << endl;
    benchmark_set<T, std::unordered_set>(words);
}

int main(int argc, char const *argv[]) {
    test<std::string>("tests/words.txt", "Words Test"); // Unique words
    test<std::string>("tests/oliver_twist.txt", "Words Test <Oliver Twist>"); // Duplicate words

    test<int16_t>("tests/short_ints_1000.txt", "Short Ints Test <1000>");
    test<int32_t>("tests/ints_1000.txt", "Ints Test <1000>");
    test<int64_t>("tests/long_ints_1000.txt", "Long Ints Test <1000>");
    
    test<int16_t>("tests/short_ints_50000.txt", "Short Ints Test <50000>");
    test<int32_t>("tests/ints_50000.txt", "Ints Test <50000>");
    test<int64_t>("tests/long_ints_50000.txt", "Long Ints Test <50000>");
    
    test<int16_t>("tests/short_ints_1000000.txt", "Short Ints Test <1000000>");
    test<int32_t>("tests/ints_1000000.txt", "Ints Test <1000000>");
    test<int64_t>("tests/long_ints_1000000.txt", "Long Ints Test <1000000>");

    return 0;
}
