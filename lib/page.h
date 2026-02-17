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
#include <iterator> // For std::forward_iterator_tag
#include <cstddef>  // For std::ptrdiff_t
using namespace std;


/*
TODO: Change page implementation to store objects in memory 
and only use the serialized array when we're trying to serialize and save the page
*/

template <typename T>
class Page{
    public:
        static constexpr int PAGESIZE = 4000;
        static constexpr uint16_t objSize = T::SERIALIZED_SIZE;
        static constexpr size_t MAX_SLOTS = static_cast<size_t>(PAGESIZE) / static_cast<size_t>(objSize);
        static constexpr size_t HEADER_BITS = MAX_SLOTS;
        static constexpr size_t HEADER_BYTES = (HEADER_BITS + 7) / 8;
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
        T getObject(uint16_t objectId) const;
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
        

        using iterator = std::vector<std::optional<T>>::iterator;
        using const_iterator = std::vector<T>::const_iterator;

        iterator begin() {
        return data.begin();
    }

    iterator end() {
        return data.end();
    }

    const_iterator begin() const {
        return data.begin();
    }

    const_iterator end() const {
        return data.end();
    }
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
    size_t maxSlots = MAX_SLOTS;
    item_store.resize(maxSlots);
    for (uint16_t i = 0; i < maxSlots; ++i) {
        freeSlots.push(i);
    }
}

template <typename T>
Page<T>::Page(const char* serializedPage){
    // Deserialize format:
    // [headerBytes bytes: bitset with 1=used,0=free][PAGESIZE bytes of item_store]
    if (!serializedPage) {
        throw std::invalid_argument("serializedPage is null");
    }
    size_t pos = 0;

    size_t maxSlots = MAX_SLOTS;
    size_t headerBytes = HEADER_BYTES;

    // Read header bitset
    std::vector<uint8_t> header(headerBytes);
    std::memcpy(header.data(), serializedPage + pos, headerBytes);
    pos += headerBytes;

    //update freeSlots with the slots that are able to be filled
    std::vector<bool> isFree(maxSlots, false);
    for (size_t i = 0; i < maxSlots; ++i) {
        size_t byteIndex = i / 8;
        size_t bitIndex = i % 8;
        bool used = (header[byteIndex] >> bitIndex) & 0x1;
        if (!used) {
            isFree[i] = true;
            freeSlots.push(static_cast<uint16_t>(i));
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
            item_store[i] = {std::nullopt};
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
T Page<T>::getObject(uint16_t objectId) const {
    size_t maxSlots = MAX_SLOTS;
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
    size_t maxSlots = MAX_SLOTS;
    if (objectId >= maxSlots) return;
    if (item_store[objectId].has_value()) {
        item_store[objectId]->setUseState(false);
        freeSlots.push(objectId);
    }
}

template <typename T>
uint16_t Page<T>::getFirstOpenSlot() const{
    if (freeSlots.empty()) return static_cast<uint16_t>(MAX_SLOTS);
    return freeSlots.front();
}

template <typename T>
uint16_t Page<T>::getOpenSlots() const{
    return static_cast<uint16_t>(freeSlots.size());
}

template <typename T>
std::unique_ptr<char[]> Page<T>::serializePage() const {
    size_t maxSlots = MAX_SLOTS;
    size_t headerBytes = HEADER_BYTES;

    size_t totalSize = headerBytes + PAGESIZE;
    std::unique_ptr<char[]> combined(new char[totalSize]);
    size_t pos = 0;

    // Build header bitset: 1 = used, 0 = free
    std::vector<uint8_t> header(headerBytes, 0);
    for (size_t i = 0; i < maxSlots; ++i) {
        if (item_store[i].has_value()) {
            size_t byteIndex = i / 8;
            size_t bitIndex = i % 8;
            header[byteIndex] |= static_cast<uint8_t>(1u << bitIndex);
        }
    }

    // Write header
    std::memcpy(combined.get() + pos, header.data(), headerBytes);
    pos += headerBytes;

    // Write item_store bytes (slot-wise)
    for (size_t i = 0; i < maxSlots; ++i) {
        char* dest = combined.get() + pos + static_cast<size_t>(i) * objSize;
        if (item_store[i]) {
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