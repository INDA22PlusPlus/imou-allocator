#include <iostream>
#include <deque>
#include <array>
#include <algorithm>


template<typename T, int chunks = 32>
class fixed_block {
private:
    T* _block = new T[chunks];
    int _size = 0;
    std::deque<T*> _free_ptrs;

    template <typename, int, int> friend class fixed_pool;
public:
    fixed_block() {}
    ~fixed_block() {
        delete[] this->_block;
    }

    T* malloc() {
        /* If there are registred free pointers, return the first of
         * the free pointers and remove it from the queue list
         */
        if (this->_free_ptrs.size()) {
            T* ptr = this->_free_ptrs.front();
            this->_free_ptrs.pop_front();

            return ptr;
        }

        /* Else just return the pointer given the block size
         * and increate the size
         */
        return this->_block + this->_size++;
    }

    void free(T* ptr) {
        /* If given pointer is not inside the allocated memory block
         * throw exception for invalid given pointer */
        if (0 > ptr-this->_block || this->_size <= ptr-this->_block) {
            throw std::invalid_argument("`ptr` not inside memory block");
        }

        auto _ind = std::find(this->_free_ptrs.begin(),
                        this->_free_ptrs.end(),
                        ptr);

        /* Throw exception if trying to free the memory twice */
        if (_ind != this->_free_ptrs.end()) {
            throw std::invalid_argument("`ptr` already is already free");
        }

        /* Else just push the given pointer to the 
         * list of the freed onces
         */
        this->_free_ptrs.push_back(ptr);
    }
};

/* Each chunk represents a place for a data with the given data type */
template<typename T, int chunks, int chunks_per_block = 32>
class fixed_pool {
private:
    std::array<
        fixed_block<T,chunks_per_block>*,
        chunks/chunks_per_block + 1
    > _blocks;

public:
    fixed_pool() {        
        /* Just initialize every block in the pool */
        for (int i = 0; i < this->_blocks.size(); i++) {
            this->_blocks[i] = new fixed_block<T,chunks_per_block>;
        }
    }

    T* malloc() {
        /* Go through every block until a suitable and free is found */
        for (int i = 0; i < this->_blocks.size(); i++) {
            fixed_block<T,chunks_per_block>* block = this->_blocks[i];
            /* Move to the next block if the current one is not empty */
            if (!block->_free_ptrs.size() && block->_size >= chunks_per_block) {
                continue;
            }

            /* Get a memory block allocated by the block */
            return block->malloc();
        }

        /* If no free block is found raise an exception */
        throw std::out_of_range("No free block is found in pool");
    }

    void free(T* ptr) {
        /* Loop though every block to find the one matching to pointer */
        for (int i = 0; i < this->_blocks.size(); i++) {
            fixed_block<T, chunks_per_block>* block = this->_blocks[i];
            
            /* If pointer not in block's memory range - continue */
            if (0 > ptr-block->_block || block->_size <= ptr-block->_block) {
                continue;
            }

            /* Else just free the pointer when the target block is found */
            block->free(ptr);
            return;
        }

        /* If no matching block to the pointer is found, raise an exception */
        throw std::invalid_argument("Pointer not in pool");
    }
};