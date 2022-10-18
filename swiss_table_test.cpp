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

    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1500000; i++) {
        table.insert(i+1);
        // cout << "capacity: " << table.capacity() << ", size: " << table.size() << endl;
    }
    auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    cout << "swiss insert: " << duration.count() << "ms" << endl;

    start = high_resolution_clock::now();
    for (int i = 0; i < 1500000; i++) {
        if (!table.count(i+1))
            cout << "not found: " << (i+1) << endl;
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    cout << "swiss find: " << duration.count() << "ms" << endl;

    unordered_set<int> uset;
    start = high_resolution_clock::now();
    for (int i = 0; i < 1500000; i++) {
        uset.insert(i+1);
        // cout << "capacity: " << uset.capacity() << ", size: " << uset.size() << endl;
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    cout << "unordered insert: " << duration.count() << "ms" << endl;

    start = high_resolution_clock::now();
    for (int i = 0; i < 1500000; i++) {
        if (!uset.count(i+1))
            cout << "not found: " << (i+1) << endl;
    }
    duration = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    cout << "unordered find: " << duration.count() << "ms" << endl;

    // sst::swiss_table_set<string> sset;
    // cout << "capacity: " << sset.capacity() << ", size: " << sset.size() << endl;
    // sset.insert("varad");
    // sset.insert("aishwarya");
    // sset.insert("book");

    // for (const string &s : sset) {
    //     cout << s << endl;
    // }
    // cout << "capacity: " << sset.capacity() << ", size: " << sset.size() << endl;

    return 0;
}
