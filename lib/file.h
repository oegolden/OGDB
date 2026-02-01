#ifndef FILE_H
#define FILE_H

#include "page.h"
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <fstream>

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
    uint32_t objectCount;
    uint16_t nextPageId;
};

#endif
