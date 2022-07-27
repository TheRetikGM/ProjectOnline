#pragma once
#include <cstdint>

namespace Ren::Utils
{
    // Cycling array in a sense that when the maximum capacity is reached, new elements are 
    // inserted from the beginning, thus overriding already existent elements there.
    // Example use case: FpsCounter, Logging
    template<typename T, std::size_t MAX_ENTRIES>
    class CyclingArray
    {
        T mEntries[MAX_ENTRIES];
        uint32_t mStart = 0;
        uint32_t mEnd = 0;
    public:
        inline size_t size() { return mEnd - mStart; }
        void push_back(const T& entry) 
        {
            // We store two numbers identifying begin and end of current array.
            // When maxmimum is reached, all we need to do is to set end to 
            // the begining of array.

            if (mEnd >= MAX_ENTRIES)
                mStart++;

            mEntries[mEnd % MAX_ENTRIES] = entry;
            mEnd++;

            if (mStart >= MAX_ENTRIES)
            {
                mStart = 0;
                mEnd = MAX_ENTRIES;
            }
        }
        inline void clear() { mStart = mEnd = 0; }
        inline T& operator[] (const int& index) { return mEntries[(mStart + index) % MAX_ENTRIES]; }
        inline T& at(const int& index) { return (*this)[index]; }

        class iterator
        {
            uint32_t index = 0;
            CyclingArray* pArr = nullptr;
        public:
            iterator() = default;
            iterator(uint32_t index, CyclingArray* array) : index(index), pArr(array) {}

            bool operator==(const iterator& it) { return index == it.index; }
            bool operator!=(const iterator& it) { return index != it.index; }
            iterator& operator++() { index++; return *this; }
            iterator& operator--() { index--; return *this; }
            T& operator->() { return (*pArr)[index]; }
            T& operator*() { return (*pArr)[index]; }
        };

        iterator begin() { return {0, this}; }
        iterator end() { return {(uint32_t)size(), this}; }
    };
} // namespace Ren::Utils
