#include <iostream>
#include <memory>
#include <cstdint>


template<int capacity>
class arena {
private:
    uint8_t* data;
    int offset;

public:
    arena() {
        this->data = new uint8_t[capacity];
        this->offset = 0;
    }

    ~arena() { delete[] this->data; }

    /* Do alligned malloc given the size of the template type
     * `n` not bytes but number of elements of the given type
     */
    template <typename T>
    T* malloc(int n) {
        void*       _tmp_ptr = this->data+this->offset;
        std::size_t _tmp_size = capacity-this->offset;
        /* Thanks to https://lesleylai.info/en/std-align/ */
        void* ptr = std::align(std::alignment_of<T>::value, n*sizeof(T),
                                _tmp_ptr, _tmp_size);
        if (!ptr) {
            throw std::runtime_error("Aligned memory couldn't be allocated");
        }

        /* `std::align` decreases the `_tmp_size` with the number of bytes
         * needed for alignment. So we extract the alignment and add it
         * to the offset
         */
        this->offset += capacity-this->offset-_tmp_size + n*sizeof(T);
        return static_cast<T*>(ptr);
    }

    void* malloc_non_aligned(int bytes) {
        if (this->offset+bytes >= capacity) {
            throw std::out_of_range("Memory size for allocation outside of bounds");
        }

        this->offset += bytes;
        return this->data + this->offset;
    }

    void reset() { this->offset = 0; }

};