
#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H

#include <unordered_map>
#include <set>
#include <list>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <iostream>
#include "page.h"

/*
is in charge of 4 main things
- Page caching + retrieval
- Eviction
- Flushing/Dirty Page management
- Thread management
- Potentially: Prefetching
*/

template <typename T>
class LRUCache {
    public:
        using Key = uint32_t;
        using Value = Page<T>;

        explicit LRUCache(size_t cap = 64)
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
        Value evict() {
            std::lock_guard<std::mutex> lg(mtx);
            if (order.empty()) return -1;
            Key old = order.back();
            auto page = map.find(old);
            order.pop_back();
            map.erase(old);
            return page;
        }

        size_t size() const {
            std::lock_guard<std::mutex> lg(mtx);
            return map.size();
        }

        // Return a vector of keys for pages that have free slots.
        std::vector<Key> getKeysWithFreeSlots() const {
            std::lock_guard<std::mutex> lg(mtx);
            std::vector<Key> out;
            out.reserve(map.size());
            for (const auto &kv : map) {
                const Value &page = kv.second.first;
                if (page.getOpenSlots() > 0) {
                    out.push_back(kv.first);
                }
            }
            return out;
        }

    private:
        size_t capacity;
        mutable std::mutex mtx;
        std::list<Key> order; // front = most recent, back = least recent
        std::unordered_map<Key, std::pair<Value, typename std::list<Key>::iterator>> map;
};

template <typename T>
class BufferPool{
    static constexpr size_t BUFFERSIZE = 64;
public:
    BufferPool();
    ~BufferPool();

    // Section 1: Page Caching and Retrieval

    /// @brief handles insertion of page into bufferpool
    /// @param pageId Global Id of the page in that file
    void insertPage(uint32_t pageId, const Page<T> &page);

    /// @brief returns pointer to the page in the buffer or nullptr if missing
    Page<T>* getPage(uint32_t pageId);

    /// @brief returns a vector with copies of all pages currently cached
    // Return a vector of page IDs that have free slots available
    std::vector<uint32_t> getPagesWithFreeSlots();

    // Section 2: Eviction

    /// @brief Evict page based on what the LRUCache says; returns evicted page id or -1 if none
    Page<T> evictPage();

    // Section 3: Dirty Page Management

    /// @brief mark page dirty; returns true if newly marked, false if already dirty
    bool markPageDirty(uint32_t pageId);

    /// @brief flush dirty pages; returns list of page ids that were flushed
    std::vector<uint32_t> flushPages();

private:
    LRUCache<T> buffer;
    std::set<uint32_t> inUse;
    std::set<uint32_t> isDirty;
};

template <typename T>
BufferPool<T>::BufferPool()
    : buffer(BUFFERSIZE), inUse(), isDirty() {}

template <typename T>
BufferPool<T>::~BufferPool() {}

template <typename T>
void BufferPool<T>::insertPage(uint32_t pageId, const Page<T> &page) {
    buffer.put(pageId, page);
}

template <typename T>
Page<T>* BufferPool<T>::getPage(uint32_t pageId) {
    return buffer.get(pageId);
}

template <typename T>
std::vector<uint32_t> BufferPool<T>::getPagesWithFreeSlots() {
    return buffer.getKeysWithFreeSlots();
}

template <typename T>
Page<T> BufferPool<T>::evictPage() {
    return buffer.evict();
}

template <typename T>
bool BufferPool<T>::markPageDirty(uint32_t pageId) {
    auto it = isDirty.find(pageId);
    if (it != isDirty.end()) {
        return false;
    } else {
        isDirty.insert(pageId);
        return true;
    }
}

template <typename T>
std::vector<uint32_t> BufferPool<T>::flushPages() {
    std::vector<uint32_t> ret(isDirty.begin(), isDirty.end());
    isDirty.clear();
    return ret;
}

#endif