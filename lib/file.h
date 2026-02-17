#ifndef FILE_H
#define FILE_H

#include "page.h"
#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include "bufferPool.h"

template <typename T>
class File {
public:
    static constexpr uint32_t MAX_OBJECTS_PER_PAGE = Page<T>::PAGESIZE / T::SERIALIZED_SIZE;
    static constexpr uint32_t TOTAL_PAGE_SIZE = Page<T>:: PAGESIZE + Page<T>::HEADER_BYTES;

    explicit File(const std::string& filename);
    ~File();
    uint32_t insertObject(T& object);
    T& getObject(uint32_t globalId) const;
    void updateObject(uint32_t globalId, T& object);
    void deleteObject(uint32_t globalId);
    void flush();
    uint32_t getObjectCount() const;
    bool exists() const;

private:
    static constexpr uint16_t getPageId(uint32_t globalId);
    static constexpr uint16_t getSlotId(uint32_t globalId);
    Page<T>& readPage(uint16_t pageId);
    void writePage(uint16_t pageId, Page<T> page);

    std::string filename;
    std::ofstream writeStream;
    BufferPool<T> bufferPool;
    uint32_t objectCount;
    uint16_t nextPageId;
    uint32_t headerSize;
    //first 4 bytes indicate the size of the header
    //read in those bytes and that becomes our header
    std::vector<std::byte> bitmap;

};


template <typename T>
File<T>::File(const std::string& filename)
    : filename(filename), bufferPool(), objectCount(0), nextPageId(0) {
        // open file for reading in binary mode
        fileStream = std::fstream().open(filename, std::ios::in | std::ios::binary);
        if (!fileStream.is_open()) {
            // file doesn't exist or couldn't be opened; nothing to read
            return;
        }

        // read first 4 bytes as header size (uint32_t)
        uint32_t headerSize = 0;
        fileStream.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
        if (fileStream.gcount() != static_cast<std::streamsize>(sizeof(headerSize))) {
            // couldn't read header size; treat as no header
            fileStream.clear();
            fileStream.seekg(0, std::ios::end);
            return;
        }

        if (headerSize == 0) {
            return;
        }

        // read headerSize bytes into bitmap
        bitmap.resize(headerSize);
        fileStream.read(reinterpret_cast<char*>(bitmap.data()), static_cast<std::streamsize>(headerSize));
        std::streamsize actuallyRead = fileStream.gcount();
        if (actuallyRead < static_cast<std::streamsize>(headerSize)) {
            // shrink to actual bytes read
            bitmap.resize(static_cast<size_t>(actuallyRead));
        }   
    }

template <typename T>
File<T>::~File() = default;

template <typename T>
uint32_t File<T>::insertObject(T& object) {
    //check the bitmap for the first open page that exists in buffer pool
    //if no such page exists in the bufferpool, then read a page from one that doesn't
    //get that page from the bufferppol
    //check that page for the first open slot
    auto pagesWithFree = bufferPool.getPagesWithFreeSlots();
    if(pagesWithFree){
        auto curPage bufferPool.getPage(pagesWithFree[0]);
        curPage.insertObject(T);
    } else{
        std::ifstream ifs (filename, std::ifstream::in);
        //read through each byte in our bitmap
        for (for int i = 0; i < bitmap.size(); ++i){
            //if the byte is full of all used pages, then it'll be all 1s
            byte = bitmap[i];
            if (std::to_integer<int>(byte) != 256){
               for (int i = 0, mask = 1; i < 8; i++, mask <<= 1) {
                    if (!(byte & mask)) {
                        ifs.seekg(sizeof(uint32_t)+header.size()+ i*TOTAL_PAGE_SIZE)
                        char* temp;
                        ifs.read(temp, TOTAL_PAGE_SIZE);
                        Page<T> page = Page(temp);
                        bufferPool.insertPage(page);
                        break;
                    }
                } 
            }
        }
        //will have to implement some form of locking for this when dealing with threading.
        //create a new page with the new id and insert that one
        Page<T> tempPage = Page(i);
        tempPage.insertObject(object);
        bufferPool.insertPage(newPage);
    }
    
}

template <typename T>
T& File<T>::getObject(uint32_t globalId) const {
    pageId = getPageId(globalId);
    slotId = getSlotId(globalId);
    Page<T> curPage bufferPool.getObject(pageId);
    if(curPage == nullptr){
        curPage = readPage(pageId);
    }
    return curPage.getObject(slotId);
}

template <typename T>
void File<T>::updateObject(uint32_t globalId, T& object) {
    auto obj = getObject(globalId);
    obj = object;
    bufferPool.markPageDirty(getPageId(globalId));
}

template <typename T>
void File<T>::deleteObject(uint32_t globalId) {
    Page<T> curPage = bufferPool.getPage(getPageId(globalId));
    curPage.deleteObject(getSlotId(globalId));
    // mark this page as free in the bitmap
    uint16_t pageId = getPageId(globalId);
    size_t byteIndex = static_cast<size_t>(pageId) / 8;
    uint8_t bitOffset = static_cast<uint8_t>(pageId % 8);
    if (byteIndex < bitmap.size()) {
        std::byte mask = static_cast<std::byte>(1u << bitOffset);
        bitmap[byteIndex] = bitmap[byteIndex] & static_cast<std::byte>(~mask);
    }
    
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
constexpr uint16_t File<T>::getPageId(uint32_t globalId) {
    return static_cast<uint16_t>(globalId / MAX_OBJECTS_PER_PAGE);
}

template <typename T>
constexpr uint16_t File<T>::getSlotId(uint32_t globalId) {
    return static_cast<uint16_t>(globalId % MAX_OBJECTS_PER_PAGE);
}




template <typename T>
void File<T>::writePage(uint16_t pageId, Page<T> page){

}
//declare filestream seek down to pageId and read in obj of TOTAL_PAGE_SIZE
template <typename T>
Page<T>& File<T>::readPage(uint16_t pageId) {
    std::ifstream ifs (filename, std::ifstream::in)
    ifs.seek(headerSize + 4 + pageId * TOTAL_PAGE_SIZE);
    char* temp = ifs.read(TOTAL_PAGE_SIZE);
    return Page<T>(temp);
}

#endif
