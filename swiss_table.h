#include "hash.h"

// TODO: 1. Support for initializer list
//       2. Range constructor

namespace sst { // sst = simple swiss table

template <class T>
class iterator {
    
};

template <class T>
class swiss_table_set
{
private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    T *table;

public:
    swiss_table_set() {
        table = new T[16]();
    }

    ~swiss_table_set() {
        delete table;
    }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return !size_; }
    
    float load_factor() const {
        return capacity_ ? (float) size_ / capacity_ : 0;
    }

    void resize() {
        if (capacity_) {
            capacity_ *= 2;
        } else {
            capacity_ = 1;
        }
    }

    void insert(const T& val) {
        if (count(val)) return;

    }

    void erase() {

    }

    size_t count(const T& key) {
        return find() != end();
    }

    iterator<T> find(const T& key) {
        return iterator<T>();
    }

    void clear () {

    }

    // Iterators
    iterator<T> begin() { return iterator<T>(); }
    iterator<T> end() { return iterator<T>(); }
};

} // namespace sst