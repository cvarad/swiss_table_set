#include <immintrin.h>
#include <iostream>
#include <ctime>
#include <type_traits>
#include <bit>

#include "hash.h"

namespace sst { // sst = simple swiss table

// Enum ctrl_t taken from absl::raw_hash_set.h
// (https://github.com/abseil/abseil-cpp/blob/master/absl/container/internal/raw_hash_set.h)
enum class ctrl_t : int8_t {
    k_empty = -128, // 0b
    k_deleted = -2,
    k_sentinel = -1
};

template <class T>
class iterator { // const iterator only!
public:
    ctrl_t *ctrl = nullptr;
    T *slots = nullptr;

    iterator() : ctrl(nullptr), slots(nullptr) {}
    iterator(ctrl_t *ctrl, T* slots) : ctrl(ctrl), slots(slots) {
        skip_empty_deleted();
    }

    T operator*() const {
        // Maybe throw an exception if accessing invalid memory
        return *slots;
    }

    iterator<T> &operator++() {
        ++ctrl; ++slots;
        skip_empty_deleted();
        return *this;
    }

    iterator<T> operator++(int) {
        auto temp_iter = *this;
        ++*this;
        return temp_iter;
    }

    bool operator==(const iterator<T> &other) const {
        return ctrl == other.ctrl;
    }

    bool operator!=(const iterator<T> &other) const {
        return !(*this == other);
    }

    void skip_empty_deleted() {
        // skip empty or deleted control bytes
        while (*ctrl < ctrl_t::k_sentinel) {
            ++ctrl; ++slots;
        }
        if (*ctrl == ctrl_t::k_sentinel) {
            ctrl = nullptr; slots = nullptr;
        }
    }
};

template <class T>
class swiss_table_set {
private:
    const float max_load_factor = float(7) / 8;

    size_t size_ = 0;
    size_t capacity_ = 0;
    size_t group_cnt_ = 0;

    ctrl_t *ctrl = nullptr;
    T *slots = nullptr;

    uint64_t hash_secret[4];

public:
    swiss_table_set() {
        capacity_ = 16;
        group_cnt_ = 1;

        alloc();
        make_secret(time(nullptr), hash_secret);
    }

    ~swiss_table_set() {
        delete[] ctrl;
        delete[] slots;
    }

private:
    // We use template specializations for hashing.
    // This is the generic declaration of the hash function. This should work
    // for all primitive types.
    size_t hash(const T &key) const {
        return wyhash(&key, sizeof key, 0, hash_secret);
    }

    inline size_t slot_hash(size_t h) { return h >> 7; }
    inline uint8_t ctrl_hash(size_t h) { return h & 0x7f; }

    // matches 16 control bytes to the ctrl_hash of the key
    uint16_t match(ctrl_t *ctrl_, uint8_t ctrl_h) const {
        auto match = _mm_set1_epi8(static_cast<char>(ctrl_h));
        auto ctrl_16 = _mm_loadu_si128((__m128i*) ctrl_);
        return _mm_movemask_epi8(_mm_cmpeq_epi8(match, ctrl_16));
    }

    uint16_t match_empty(ctrl_t *ctrl_) const {
        auto match = _mm_set1_epi8(static_cast<char>(ctrl_t::k_empty));
        auto ctrl_16 = _mm_loadu_si128((__m128i*) ctrl_);
        return _mm_movemask_epi8(_mm_cmpeq_epi8(match, ctrl_16));
    }

    uint16_t match_empty_deleted(ctrl_t *ctrl_) const {
        auto match = _mm_set1_epi8(static_cast<char>(ctrl_t::k_sentinel));
        auto ctrl_16 = _mm_loadu_si128((__m128i*) ctrl_);
        return _mm_movemask_epi8(_mm_cmpgt_epi8(match, ctrl_16));
    }

    void alloc() {
        ctrl = new ctrl_t[capacity_];
        memset(ctrl, (int)ctrl_t::k_empty, capacity_);
        ctrl[capacity_ - 1] = ctrl_t::k_sentinel;
        slots = new T[capacity_];
    }

    void resize() {
        ctrl_t *old_ctrl = ctrl;
        ctrl_t *old_ctrl_head = ctrl;
        T *old_slots = slots;
        T *old_slots_head = slots;

        capacity_ *= 2;
        group_cnt_ *= 2;
        alloc();
        
        // rehash
        for (; *old_ctrl != ctrl_t::k_sentinel; ++old_ctrl, ++old_slots) {
            if (*old_ctrl < ctrl_t::k_sentinel) // if empty or deleted, do nothing
                continue;
            insert_nocheck(*old_slots);
        }

        delete[] old_ctrl_head;
        delete[] old_slots_head;
    }

public:
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return !size_; }
    
    float load_factor() const {
        return capacity_ ? (float) size_ / capacity_ : 0;
    }

    iterator<T> find(const T& key) {
        auto hash_ = hash(key);
        size_t g = slot_hash(hash_) & (group_cnt_ - 1);
        while (true) {
            auto slot_group = slots + g*16;
            auto ctrl_group = ctrl + g*16;
            auto matched = match(ctrl_group, ctrl_hash(hash_));

            uint8_t i;
            while (matched) {
                i = std::countr_zero(matched);
                if (key == *(slot_group + i)) {
                    return {ctrl_group + i, slot_group + i};
                }
                matched &= (matched - 1);
            }

            if (match_empty(ctrl_group)) return end();
            g = (g + 1) & (group_cnt_ - 1);
        }
    }

    size_t count(const T& key) {
        return find(key) != end();
    }

    void insert(const T& val) {
        if (count(val)) return;

        ++size_;
        if (load_factor() > max_load_factor) {
            resize();
        }

        insert_nocheck(val);
    }

    void insert_nocheck(const T& val) {
        auto hash_ = hash(val);
        auto ctrl_h = ctrl_hash(hash_);
        auto slot_h = slot_hash(hash_);
        
        size_t g = slot_h & (group_cnt_ - 1);
        while (true) {
            auto slot_group = slots + g*16;
            auto ctrl_group = ctrl + g*16;
            auto matched = match_empty_deleted(ctrl_group);
            if (matched) {
                uint8_t i = std::countr_zero(matched);
                *(ctrl_group + i) = static_cast<ctrl_t>(ctrl_h);
                *(slot_group + i) = val;
                break;
            }

            g = (g + 1) % group_cnt_;
        }
    }

    void erase(const T& key) {
        iterator<T> it = find(key);
        if (it == end()) return;

        --size_;
        auto ctrl_group = ctrl + (it.ctrl - ctrl) / 16 * 16;
        *it.ctrl = match_empty(ctrl_group) ? ctrl_t::k_empty : ctrl_t::k_deleted;
    }

    void clear () {
        delete[] ctrl;
        delete[] slots;
        capacity_ = 16; group_cnt_ = 1;
        alloc();
    }

    // Iterators
    iterator<T> begin() { return {ctrl, slots}; }
    iterator<T> end() { return {}; }
};

// This is the template specialized hash function, for std::string.
template<>
size_t swiss_table_set<std::string>::hash(const std::string &key) const {
    return wyhash(key.c_str(), key.size(), 0, hash_secret);
}

} // namespace sst