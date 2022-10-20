#include "swiss_table.h"

#include <iostream>
#include <typeinfo>
#include <unordered_set>
#include <chrono>

using namespace std;
using namespace std::chrono;

class CustomType {
private:
    int a, b;
};

int main(int argc, char const *argv[]) {
    sst::swiss_table_set<int> table;

    int N = 1200000;
    auto start = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        table.insert(i*13 + 1);
    }
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    cout << "swiss insert: " << duration.count() << "ms" << endl;

    start = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        if (table.find(i*13 + 1) == table.end())
            cout << "not found: " << (i*13 + 1) << endl;
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    cout << "swiss find: " << duration.count() << "ms" << endl;

    unordered_set<int> uset;
    start = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        uset.insert(i*13 + 1);
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    cout << "unordered insert: " << duration.count() << "ms" << endl;

    start = high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        if (uset.find(i*13 + 1) == uset.end())
            cout << "not found: " << (i*13 + 1) << endl;
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    cout << "unordered find: " << duration.count() << "ms" << endl;

    return 0;
}
