#ifndef PAGE_H

#define PAGE_H

#include<cstdint>
#include<array>
#include<queue>
#include <cstring>
#include <span>
#include <stdexcept>
#include <algorithm>
#include <iterator> 

using namespace std;

template <typename T>
class Page{
    public:
        static constexpr int PAGESIZE = 1048576;
        Page(uint16_t pageId);
        ~Page();
        /// @brief Takes in an object uses it's serialize function and inserts it into page bytea array
        /// @param object object the page is storing: Node, Relationship, Property, TODO: LABEL, DYNAMIC TYPES\
        /// @return the slotId of the inserted node
        uint32_t serializeAndInsertObject(T& object);
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
        uint16_t getPageId() const {return pageId;}
        uint16_t getFirstOpenSlot() const;
        uint16_t getOpenSlots() const;
        static constexpr uint16_t objSize = T::SERIALIZED_SIZE;
    private:
        //use a stack to keep track of the closest open 
        std::unique_ptr<std::byte[]> item_store;
        std::queue<uint16_t> freeSlots;
        uint16_t pageId;
};

// Template implementations must be in the header file
template <typename T>
Page<T>::Page(uint16_t pageId): pageId(pageId) {
    item_store = std::make_unique<std::byte[]>(PAGESIZE);
    for (int i = 0; i < PAGESIZE/objSize; i++){
        freeSlots.push(i);
    }
}

template <typename T>
uint32_t Page<T>::serializeAndInsertObject(T& object) {
    if (!freeSlots.size()){
        throw std::logic_error("No remaining slots");
    }
    std::array<std::byte,objSize> chunk = object.serializeObject();
    std::memcpy(item_store.get() + freeSlots.front() * objSize, chunk.data(), chunk.size());
    uint32_t slotId = freeSlots.front();
    freeSlots.pop();
    return slotId;
}

template <typename T>
T Page<T>::getPageObject(uint16_t objectId) const {
    if(objectId * objSize > PAGESIZE - objSize){
        throw std::out_of_range("Id out of page range");
    }
    if (objectId < 0){
        throw std::out_of_range("Only positive Ids");
    } if(item_store[objectId * objSize] == std::byte(-1)){
        throw std::logic_error("No node exists at ObjectId " + std::to_string(objectId));
    }
    std::vector<std::byte> chunk(item_store.get() + objectId*objSize,item_store.get() + ((objectId+1)*objSize));
    return T(chunk);
}

template <typename T>
void Page<T>::deleteObject(uint16_t objectId){
    item_store[objectId] = static_cast<std::byte>(0);
}
template <typename T>
uint16_t Page<T>::getFirstOpenSlot() const{
    return freeSlots.front();
}

template <typename T>
uint16_t Page<T>::getOpenSlots() const{
    return freeSlots.size();
}

template <typename T>
Page<T>::~Page() = default;
#endif