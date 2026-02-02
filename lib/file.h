#ifndef FILE_H
#define FILE_H

#include "page.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include "bufferPool.h"
template <typename T>
class File {
public:
    static constexpr uint32_t MAX_OBJECTS_PER_PAGE = Page<T>::PAGESIZE / T::SERIALIZED_SIZE;

    explicit File(const std::string& filename);
    ~File();
    uint32_t insertObject(T& object);
    T getObject(uint32_t globalId) const;
    void updateObject(uint32_t globalId, T& object);
    void deleteObject(uint32_t globalId);
    void removeObject(uint32_t globalId);
    void flush();
    uint32_t getObjectCount() const;
    bool exists() const;
    void create();
    void load();

private:
    static constexpr uint16_t getPageId(uint32_t globalId);
    static constexpr uint16_t getSlotId(uint32_t globalId);
    Page<T>& getPage(uint16_t pageId);
    uint16_t findAvailablePage();
    void writePage(uint16_t pageId);
    void readPage(uint16_t pageId);

    std::string filename;
    std::vector<std::unique_ptr<Page<T>>> pages;
    std::fstream fileStream;
    BufferPool<T> bufferPool;
    uint32_t objectCount;
    uint16_t nextPageId;
};


template <typename T>
File<T>::File(const std::string& filename)
    : filename(filename), pages(), fileStream(), bufferPool(), objectCount(0), nextPageId(0) {}

template <typename T>
File<T>::~File() = default;

template <typename T>
uint32_t File<T>::insertObject(T& object) {
    bufferPool.insertObject(object);
    
}

template <typename T>
T File<T>::getObject(uint32_t globalId) const {
    return bufferPool.getObject(globalId);
}

template <typename T>
void File<T>::updateObject(uint32_t globalId, T& object) {
    bufferPool.updateObject(uint32_t,object);
}

template <typename T>
void File<T>::deleteObject(uint32_t globalId) {
    bufferPool.deleteObject(globalId)
}

template <typename T>
void File<T>::removeObject(uint32_t globalId) {
}

template <typename T>
void File<T>::flush() {
}

template <typename T>
uint32_t File<T>::getObjectCount() const {
    return objectCount;
}

template <typename T>
bool File<T>::exists() const {
    std::ifstream f(filename);
    return f.good();
}

template <typename T>
void File<T>::create() {
}

template <typename T>
void File<T>::load() {
}

template <typename T>
constexpr uint16_t File<T>::getPageId(uint32_t globalId) {
    return static_cast<uint16_t>(globalId / MAX_OBJECTS_PER_PAGE);
}

template <typename T>
constexpr uint16_t File<T>::getSlotId(uint32_t globalId) {
    return static_cast<uint16_t>(globalId % MAX_OBJECTS_PER_PAGE);
}

template <typename T>
Page<T>& File<T>::getPage(uint16_t pageId) {
    return *pages.at(pageId);
}

template <typename T>
uint16_t File<T>::findAvailablePage() {
    if (nextPageId < pages.size()) {
        return nextPageId++;
    }
    pages.emplace_back(std::make_unique<Page<T>>(static_cast<uint16_t>(pages.size())));
    return static_cast<uint16_t>(pages.size() - 1);
}

template <typename T>
void File<T>::writePage(uint16_t pageId) {
}

template <typename T>
void File<T>::readPage(uint16_t pageId) {
}

#endif
