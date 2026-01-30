
#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H
#include <unordered_map>
#include <set>

template <typename T>
class BufferPool{
    static constexpr uint64_t BUFFERSIZE = 64;
    public:
        BufferPool();
        ~BufferPool();
        T getPage() const;
        void insertPage();
        void deletePage();
        uint32_t markPageDirty();

    private:
        std::unordered_map<int,T> buffer;
        std::set<int> inUse;
}
#endif