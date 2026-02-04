#ifndef PAGE_H

#define PAGE_H

#include<cstdint>
#include<array>
#include<queue>
#include<vector>
#include<optional>
#include <cstring>
#include <span>
#include <stdexcept>
#include <algorithm>
#include <iterator> 
#include <cstdint>
#include <iostream>
using namespace std;


/*
TODO: Change page implementation to store objects in memory 
and only use the serialized array when we're trying to serialize and save the page
*/

template <typename T>
class Page{
    public:
        static constexpr int PAGESIZE = 1048576;
        Page();
        Page(uint16_t pageId);
        Page(const char* serializedPage);
        ~Page();
        /// @brief Takes in an object uses it's serialize function and inserts it into page bytea array
        /// @param object object the page is storing: Node, Relationship, Property, TODO: LABEL, DYNAMIC TYPES\
        /// @return the slotId of the inserted node
        uint32_t insertObject(T& object);
        /// @brief get's the object unserialized from the byte chunk representing the object
        /// @param objectId the Id of the object i.e. its place in the file
        /// @return the object stored on the page
        T getPageObject(uint16_t objectId) const;
        /// @brief Delete's the byte chunk that represents the object on that page
        /// @param objectId the Id of the object i.e. its place in the file
        void removeObject(uint16_t objectId);
        /// @brief 
        /// @param objectId 
        void deleteObject(uint16_t objectId);
        /// @brief get's the pageId
        /// @return pageId
        std::unique_ptr<char[]> serializePage() const;
        uint16_t getPageId() const {return pageId;}
        uint16_t getFirstOpenSlot() const;
        uint16_t getOpenSlots() const;
        static constexpr uint16_t objSize = T::SERIALIZED_SIZE;
    private:
        uint16_t pageId;
        // store objects directly; empty optional means free slot
        std::vector<std::optional<T>> item_store;
        std::queue<uint16_t> freeSlots;
};

template <typename T>
Page<T>::Page() : Page(static_cast<uint16_t>(0)) {}

template <typename T>
Page<T>::Page(uint16_t pid) : pageId(pid) {
    size_t maxSlots = PAGESIZE / objSize;
    item_store.resize(maxSlots);
    for (uint16_t i = 0; i < maxSlots; ++i) {
        freeSlots.push(i);
    }
}

template <typename T>
Page<T>::Page(const char* serializedPage){
    // Deserialize format:
    // [uint16_t freeCount][freeCount * uint16_t freeSlotEntries][PAGESIZE bytes of item_store]
    if (!serializedPage) {
        throw std::invalid_argument("serializedPage is null");
    }
    size_t pos = 0;
    uint16_t freeCount = 0;
    std::memcpy(&freeCount, serializedPage + pos, sizeof(uint16_t));
    pos += sizeof(uint16_t);

    size_t maxSlots = PAGESIZE / objSize;
    std::vector<bool> isFree(maxSlots, false);

    // Read free slot indices
    for (uint16_t i = 0; i < freeCount; ++i) {
        uint16_t slot = 0;
        std::memcpy(&slot, serializedPage + pos, sizeof(uint16_t));
        pos += sizeof(uint16_t);
        if (slot < maxSlots) {
            isFree[slot] = true;
            freeSlots.push(slot);
        }
    }

    // Prepare item store and reconstruct objects
    item_store.resize(maxSlots);
    for (size_t i = 0; i < maxSlots; ++i) {
        const char* chunkPtr = serializedPage + pos + i * objSize;
        const std::byte* bptr = reinterpret_cast<const std::byte*>(chunkPtr);
        std::vector<std::byte> chunk(bptr, bptr + objSize);
        if (!isFree[i]) {
            item_store[i] = T(chunk);
        } else {
            item_store[i] = std::nullopt;
        }
    }
}

template <typename T>
uint32_t Page<T>::insertObject(T& object) {
    if (freeSlots.empty()){
        throw std::logic_error("No remaining slots");
    }
    uint32_t slot = freeSlots.front();
    freeSlots.pop();
    item_store[slot] = object;
    return slot;
}

template <typename T>
T Page<T>::getPageObject(uint16_t objectId) const {
    size_t maxSlots = PAGESIZE / objSize;
    if (objectId >= maxSlots) {
        throw std::out_of_range("Id out of page range");
    }
    const auto& opt = item_store[objectId];
    if (!opt.has_value()) {
        throw std::logic_error("No object exists at ObjectId " + std::to_string(objectId));
    }
    return *opt;
}

template <typename T>
void Page<T>::deleteObject(uint16_t objectId){
    size_t maxSlots = PAGESIZE / objSize;
    if (objectId >= maxSlots) return;
    if (item_store[objectId].has_value()) {
        item_store[objectId]->setUseState(false);
    }
}
template <typename T>
void Page<T>::removeObject(uint16_t objectId){
    size_t maxSlots = PAGESIZE / objSize;
    if (objectId >= maxSlots) return;
    if (item_store[objectId].has_value()) {
        item_store[objectId].reset();
        freeSlots.push(objectId);
    }
}
template <typename T>
uint16_t Page<T>::getFirstOpenSlot() const{
    if (freeSlots.empty()) return static_cast<uint16_t>(PAGESIZE / objSize);
    return freeSlots.front();
}

template <typename T>
uint16_t Page<T>::getOpenSlots() const{
    return static_cast<uint16_t>(freeSlots.size());
}

template <typename T>
std::unique_ptr<char[]> Page<T>::serializePage() const {
    std::queue<uint16_t> q = freeSlots;
    //keep track of how many free slots we have to view
    uint16_t freeCount = static_cast<uint16_t>(q.size());
    //header that stores the free slots of the page
    size_t headerSize = sizeof(uint16_t) + static_cast<size_t>(freeCount) * sizeof(uint16_t);
    size_t totalSize = headerSize + PAGESIZE;

    std::unique_ptr<char[]> combined(new char[totalSize]);
    size_t pos = 0;

    // Write freeCount
    std::memcpy(combined.get() + pos, &freeCount, sizeof(uint16_t));
    pos += sizeof(uint16_t);

    // Write free slot entries
    while (!q.empty()) {
        uint16_t item = q.front();
        std::memcpy(combined.get() + pos, &item, sizeof(uint16_t));
        pos += sizeof(uint16_t);
        q.pop();
    }

    // Write item_store bytes (slot-wise)
    size_t maxSlots = PAGESIZE / objSize;
    
    for (size_t i = 0; i < maxSlots; ++i) {
        char* dest = combined.get() + pos + static_cast<size_t>(i) * objSize;
        if (item_store[i].has_value()) {
            auto arr = item_store[i]->serializeObject();
            std::memcpy(dest, arr.data(), objSize);
        } else {
            // zero out free slot
            std::memset(dest, 0, objSize);
        }
    }

    return combined;
}

template <typename T>
Page<T>::~Page() {}

#endif