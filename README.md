## Swiss Table Set
This is my simplified implementation of absl::flat_hash_set.

## Usage
Just copy over [hash.h]() and [swiss_table.h]() to your directory and import [swiss_table.h]() in your code.
<br/>Example usage:
```cpp
#include "swiss_table.h"
#include <iostream>

using namespace std;

int main() {
    sst::swiss_table_set<int> my_set;

    my_set.insert(5);
    my_set.insert(10);
    my_set.erase(10);

    auto it = my_set.find(10);

    if (it == my_set.end())
        cout << "Not found" << endl;

    return 0;
}
```

## Tests
I have created a simple benchmarking test for this project.
<br/>To build and run the tests:
```
make build_test
make run_test
```