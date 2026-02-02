
#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H
#include <unordered_map>
#include <set>
#include <list>
#include <mutex>
#include "page.h"
template <typename T>
/*
is in charge of 4 main things
- Page caching + retrieval
- Eviction
- Flushing/Dirty Page management
- Thread management
- Potentially: Prefetching
*/
class LRUCache{
        public:
            using Key = int;
            using Value = Page<T>;

            explicit LRUCache(size_t cap = BUFFERSIZE)
                : capacity(cap) {}

            bool exists(const Key &k) const {
                std::lock_guard<std::mutex> lg(mtx);
                return map.find(k) != map.end();
            }

            void put(const Key &k, const Value &v) {
                std::lock_guard<std::mutex> lg(mtx);
                auto it = map.find(k);
                if (it != map.end()) {
                    it->second.first = v;
                    order.splice(order.begin(), order, it->second.second);
                    it->second.second = order.begin();
                    return;
                }

                if (map.size() >= capacity) {
                    Key old = order.back();
                    order.pop_back();
                    map.erase(old);
                }
                order.push_front(k);
                map.emplace(k, std::make_pair(v, order.begin()));
            }

            Value* get(const Key &k) {
                std::lock_guard<std::mutex> lg(mtx);
                auto it = map.find(k);
                if (it == map.end()) return nullptr;
                order.splice(order.begin(), order, it->second.second);
                it->second.second = order.begin();
                return &it->second.first;
            }

            void erase(const Key &k) {
                std::lock_guard<std::mutex> lg(mtx);
                auto it = map.find(k);
                if (it == map.end()) return;
                order.erase(it->second.second);
                map.erase(it);
            }

            // Evict least-recently-used entry. Returns key or -1 if empty.
            int evict() {
                std::lock_guard<std::mutex> lg(mtx);
                if (order.empty()) return -1;
                Key old = order.back();
                order.pop_back();
                map.erase(old);
                return old;
            }

            size_t size() const {
                std::lock_guard<std::mutex> lg(mtx);
                return map.size();
            }
        private:
            size_t capacity;
            mutable std::mutex mtx;
            std::list<Key> order; // front = most recent, back = least recent
            std::unordered_map<Key, std::pair<Value, typename std::list<Key>::iterator>> map;
        };
template <typename T>
class BufferPool{
    static constexpr uint64_t BUFFERSIZE = 64;
    public:
        BufferPool();
        ~BufferPool();
        T getObject(uint32_t objId) const;
        void deleteObject(uint32_t objId);
        //gonna have many different ways to do this, 
        //feel like it would be better to deserealize the object then update it and researlize
        void updateObject(uint32_t objId, T replacementObject);
        uint32_t insertObject(T object);
    private:
        LRUCache<T> buffer;
        std::set<int> inUse;
        std::unordered_map<uint32_t, Page<T>> pageTable;
        //Section 1: Page Caching and Retrieval

        /// @brief handles insertion of page into bufferpool
        /// @param pageId Global Id of the page in that file 
        void insertPage(uint32_t pageId);

        Page<T> getPage(uint32_t pageId) const;
        

        //Section 2:Eviction
        
        /// @brief Evict page based on what the LRUCache says
        void evictPage();

        // Section 3:Dirty Page Management

        uint32_t markPageDirty(uint32_t pageId);

        void flushPages();


    };

template <typename T>
BufferPool<T>::BufferPool()
    : buffer() {}

template <typename T>
BufferPool<T>::~BufferPool() {}

template <typename T>
T BufferPool<T>::getObject(uint32_t objId) const {
    return T{};
}

template <typename T>
void BufferPool<T>::deleteObject(uint32_t objId) {
}

template <typename T>
void BufferPool<T>::updateObject(uint32_t objId, T replacementObject) {
}

template <typename T>
uint32_t BufferPool<T>::insertObject(T object) {
}

template <typename T>
void BufferPool<T>::insertPage(uint32_t pageId) {
}

template <typename T>
Page<T> BufferPool<T>::getPage(uint32_t pageId) const {
    return Page<T>();
}

template <typename T>
void BufferPool<T>::evictPage() {
}

template <typename T>
uint32_t BufferPool<T>::markPageDirty(uint32_t pageId) {
    return 0;
}

template <typename T>
void BufferPool<T>::flushPages() {
}

#endif