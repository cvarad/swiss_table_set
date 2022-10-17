#include "swiss_table.h"

#include <iostream>
using namespace std;

int main(int argc, char const *argv[]) {
    sst::swiss_table_set<int> table;
    cout << table.size() << endl;
    cout << table.capacity() << endl;
    cout << table.empty() << endl;

    cout << ~size_t{} << endl;


    cout << table.match() << endl;
    return 0;
}
