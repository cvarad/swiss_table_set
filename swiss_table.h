#include <immintrin.h>
#include <iostream>

#include "hash.h"

// TODO: 1. Support for initializer list
//       2. Range constructor

namespace sst { // sst = simple swiss table

// Enum ctrl_t taken from absl::raw_hash_set.h
// (https://github.com/abseil/abseil-cpp/blob/master/absl/container/internal/raw_hash_set.h)
enum class ctrl_t : int8_t {
    k_empty = -128,
    k_deleted = -2,
    k_sentinel = -1
};

template <class T>
class iterator { // const iterator only!
private:
    ctrl_t *ctrl = nullptr;
    T *slots = nullptr;

    iterator(ctrl_t *ctrl, T* slots) : ctrl(ctrl), slots(slots) {}

    T operator*() const {
        // skip empty slots
        return *slots;
    }
};

template <class T>
class swiss_table_set {
private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    size_t group_cnt_ = 0;

    ctrl_t *ctrl = nullptr;
    T *slots = nullptr;

public:
    swiss_table_set() {
        capacity_ = 16;
        group_cnt_ = 1;

        ctrl = new ctrl_t[capacity_];
        slots = new T[capacity_];
    }

    ~swiss_table_set() {
    }

private:
    size_t slot_hash(size_t h) { return h >> 7; }
    ctrl_t ctrl_hash(size_t h) { return static_cast<ctrl_t>(h & 0x7f); }

    uint16_t match() {
        auto match = _mm_set1_epi8(96);
        auto ctrl_16 = _mm_loadu_si128((__m128i*) ctrl);
        return _mm_movemask_epi8(_mm_cmpeq_epi8(match, ctrl_16));
    }



public:
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return !size_; }
    
    float load_factor() const {
        return capacity_ ? (float) size_ / capacity_ : 0;
    }

    void resize() {
        if (capacity_) {
            capacity_ *= 2;
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