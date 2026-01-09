#ifndef PAGE_h

#define PAGE_H

#include<cstdint>
#include<array>
#include<queue>
#include <cstring>
#include <span>
#include <stdexcept>
#include <algorithm>

using namespace std;

template <typename T>
class Page{
    public:
        static constexpr int PAGESIZE = 4096;
        Page(uint16_t pageId);
        /// @brief Takes in an object uses it's serialize function and inserts it into page bytea array
        /// @param object object the page is storing: Node, Relationship, Property, TODO: LABEL, DYNAMIC TYPES
        void insertSerializedObject(T& object);
        /// @brief get's the object unserialized from the byte chunk representing the object
        /// @param objectId the Id of the object i.e. its place in the file
        /// @return the object stored on the page
        T getPageObject(uint16_t objectId) const;
        /// @brief Delete's the byte chunk that represents the object on that page
        /// @param objectId the Id of the object i.e. its place in the file
        void deleteObject(uint16_t objectId);
        /// @brief get's the pageId
        /// @return pageId
        uint16_t getPageId() const {return pageId;}
        uint16_t getFirstpenSlot() const;
        uint16_t getOpenSlots() const;

    private:
        //use a stack to keep track of the closest open 
        std::array<std::byte, PAGESIZE> item_store;
        std::queue<uint16_t> freeSlots;
        uint16_t pageId;
};

// Template implementations must be in the header file
template <typename T>
Page<T>::Page(uint16_t pageId): pageId(pageId) {
    printf("starting");
    for (int i = 0; i < PAGESIZE/sizeof(T); i++){
        freeSlots.push(i);
    }
}

template <typename T>
void Page<T>::insertSerializedObject(T& object) {
    std::array<std::byte, sizeof(T)> chunk = object.serializeObject();
    std::memcpy(item_store.data() + freeSlots.front() * sizeof(T), chunk.data(), chunk.size());
    freeSlots.pop();
}

template <typename T>
T Page<T>::getPageObject(uint16_t objectId) const {
    if(objectId * sizeof(T) > PAGESIZE - sizeof(T)){
        throw std::out_of_range{};
    }
    auto chunk = std::span(this->item_store)
    .subspan(objectId*sizeof(T),sizeof(T));
    return T::unserializeObject(chunk);
}

template <typename T>
void Page<T>::deleteObject(uint16_t objectId){
    std::fill_n(item_store.data(), int(sizeof(T)),static_cast<std::byte>(0));
    freeSlots.push(objectId);
}

template <typename T>
uint16_t Page<T>::getFirstpenSlot() const{
    return freeSlots.front();
}

template <typename T>
uint16_t Page<T>::getOpenSlots() const{
    return freeSlots.size();
}

#endif