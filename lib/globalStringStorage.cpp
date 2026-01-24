#include "globalStringStorage.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstring>
#include <span>
#include <cstdlib>

//TODO:: tighten strings together updating header offsets
//TODO:: Make this system crash proof, 
//currently if something fails mid save, you either lose data or have a corrupted file
GlobalStringStorage::GlobalStringStorage(): chunkStart(0)
{
    // TODO: initialize internal buffers and optionally load existing storage
    std::filesystem::path f = "../files/gss.bin";

    if(std::filesystem::exists(f)){
        //last 8 bytes gives the size of the header and the position of the start of the header
        //jump to the start of the header and read from there up till the footer
        //take that chunk and turn it into our header
        std::ifstream GSS(f);
        //first 8 bytes after that represent the size we need to read in
        //second 8 bytes are the offset to the start of the header
        GSS.seekg(-16,std::ios::end);
        unsigned char charsOfSize[8]; 
        GSS.read(reinterpret_cast<char*>(charsOfSize),8);
        uint64_t size = (static_cast<uint64_t>(charsOfSize[0]) << 56) |
                        (static_cast<uint64_t>(charsOfSize[1]) << 48) |
                        (static_cast<uint64_t>(charsOfSize[2]) << 40) |
                        (static_cast<uint64_t>(charsOfSize[3]) << 32) |
                        (static_cast<uint64_t>(charsOfSize[4]) << 24) |
                        (static_cast<uint64_t>(charsOfSize[5]) << 16) |
                        (static_cast<uint64_t>(charsOfSize[6]) << 8) |
                        static_cast<uint64_t>(charsOfSize[7]);
        //std::cout << size;
        unsigned char charsOfoffset[8];
        GSS.read(reinterpret_cast<char*>(charsOfoffset),8);
        uint64_t offset = (static_cast<uint64_t>(charsOfoffset[0]) << 56) |
                          (static_cast<uint64_t>(charsOfoffset[1]) << 48) |
                          (static_cast<uint64_t>(charsOfoffset[2]) << 40) |
                          (static_cast<uint64_t>(charsOfoffset[3]) << 32) |
                          (static_cast<uint64_t>(charsOfoffset[4]) << 24) |
                          (static_cast<uint64_t>(charsOfoffset[5]) << 16) |
                          (static_cast<uint64_t>(charsOfoffset[6]) << 8) |
                          static_cast<uint64_t>(charsOfoffset[7]);
        //std::cout<<offset;
        //should have hit eof marker so we need to clear
        GSS.clear();
        GSS.seekg(offset, std::ios::beg);
        std::vector<char> headerChars(size);
        GSS.read(headerChars.data(),size);
        //turn headerChars into unordered map
        //first 4 bytes is offset
        //next 4 bytes is length
        for(long i = 0; i < size; i+=8){
            uint32_t offset = (static_cast<uint32_t>(static_cast<unsigned char>(headerChars[i])) << 24) | 
            (static_cast<uint32_t>(static_cast<unsigned char>(headerChars[i+1])) << 16) |
            (static_cast<uint32_t>(static_cast<unsigned char>(headerChars[i+2])) << 8) | 
            static_cast<uint32_t>(static_cast<unsigned char>(headerChars[i+3]));
            uint32_t length = (static_cast<uint32_t>(static_cast<unsigned char>(headerChars[i + 4])) << 24) | 
            (static_cast<uint32_t>(static_cast<unsigned char>(headerChars[i+5])) << 16) |
            (static_cast<uint32_t>(static_cast<unsigned char>(headerChars[i+6])) << 8) | 
            static_cast<uint32_t>(static_cast<unsigned char>(headerChars[i+7]));
            GlobalStringStorage::stringEntry header = {offset,length};
            headerStore[i/8] = header;
        }
        //erase last 16 bytes
        std::filesystem::resize_file(f,std::filesystem::file_size(f) - 16);
        //load the first chunk of strings
        if(!headerStore.empty()){
            readInChunk(0);
        }
       //printf(str_store.data());
    } else{
        std::ofstream GSS(f);
    }
}

GlobalStringStorage::~GlobalStringStorage()
{
    saveToDisk();
}

void GlobalStringStorage::readInChunk(uint32_t headerSlot)
{
    //printf("reading in chunk");
    //TODO: change how this is done to accomodate deletes
    //store chunk starting offset in order to tell if the code we currently have is 64 offsets out
    int initialOffset = headerStore[headerSlot].offset;
    int lastSlot = headerStore.rbegin()->first;
    int ending = 0;
    //if current chunk we'retrying to get isn't full
    if( headerSlot - lastSlot < CHUNKSIZE){
        ending = headerStore[lastSlot].offset + headerStore[lastSlot].length;
    } else{
        ending = headerStore[headerSlot + CHUNKSIZE].offset + headerStore[headerSlot + CHUNKSIZE].length;
    }
    int numBytes = ending - initialOffset;
    std::ifstream GSS("../files/gss.bin");
    const std::streamsize readBytes = numBytes;
    GSS.seekg(initialOffset);
    //erase current str_store
    str_store.clear();
    std::vector<char> temp(numBytes);
    GSS.read(temp.data(),readBytes);
    str_store.insert(str_store.end(),temp.begin(),temp.end());
    chunkStart = headerSlot;
    GSS.close();
}

std::string GlobalStringStorage::getString(uint32_t headerSlot)
{
    //if current chunk contains our string then get it from the chunk
    //else load in the chunk containing that string through CHUNKSIZE
    if(headerStore.empty()){
        throw std::out_of_range("No strings have been written");
    }
    if(headerSlot > headerStore.rbegin() -> first){
        throw std::out_of_range("No string exists at this slot");
    }
    if (headerSlot < chunkStart || headerSlot > static_cast<int>(chunkStart + CHUNKSIZE)){
        //load in new chunk of strings
        readInChunk(headerSlot);
    }
    //calculate start of string by taking the offset of headerStore[chunkStart] and headerSlot
    int byteOffset = headerStore[headerSlot].offset - headerStore[chunkStart].offset;
    int length = headerStore[headerSlot].length;
    std::string ret(str_store.data() + byteOffset, length);
    return ret;
}

int GlobalStringStorage::putString(std::string s)
{
    if(s.length() <= 0){
        throw std::length_error("Text must be greater than 0 characters");
    }
    // TODO: append `s` to on-disk storage and update `headerStore`
    uint32_t offset = 0;
    int last_slot = -1;
    //if the header store isn't empty
    if (!headerStore.empty()){
        last_slot = headerStore.rbegin()->first;
        auto &last = headerStore[last_slot];
        offset = last.offset + last.length;
    }
    //initiate ofstream and get the entry
    std::ofstream GSS;
    GSS.open("../files/gss.bin", std::ios::ate | std::ios::binary | std::ios::in | std::ios::out);
    stringEntry entry = {offset, static_cast<uint32_t>(s.size())};
    //find the lower bound in the deleted entries that we can put our string in
    if(!newlyOpenSlots.empty()){
        auto lowerBound = newlyOpenSlots.lower_bound(entry);
        if(lowerBound != newlyOpenSlots.end()){
            entry.offset = lowerBound->offset;
            GSS.seekp(lowerBound->offset);
        } 
    } else{
        headerStore[last_slot + 1] = entry;
    }
    GSS.write(s.data(),s.size());
    //if our current chunk isn't full i.e. last_slot < chunkStart + CHUNKSIZE
    //add to the end of our chunk of strings
    if (static_cast<int>(chunkStart + CHUNKSIZE) > last_slot+1){
        str_store.insert(str_store.end(), s.data(), s.data()+s.size());
    }
    GSS.close();
    return last_slot + 1;
}

int GlobalStringStorage::deleteString(uint32_t headerSlot){
    newlyOpenSlots.insert(headerStore[headerSlot]);
    headerStore.erase(headerSlot);
    return headerSlot;
}

void GlobalStringStorage::saveToDisk(){
    //save header to disk
    std::ofstream GSS;
    GSS.open("../files/gss.bin", std::ios::ate | std::ios::binary | std::ios::in | std::ios::out);
    uint64_t offset = static_cast<uint64_t>(static_cast<std::streamoff>(GSS.tellp()));
    if (offset == -1){
        throw std::logic_error("Something went wrong saving!");
    }
    for(auto& entry: headerStore){
        //cast as byte to ensure masking works the same and that no other opps work
        std::byte headerByteArray[8];
        //writing offset to bytes to write to file
        headerByteArray[0] = static_cast<std::byte>((entry.second.offset >> 24) & 0xFF);
        headerByteArray[1] = static_cast<std::byte>((entry.second.offset >> 16) & 0xFF);
        headerByteArray[2] = static_cast<std::byte>((entry.second.offset >> 8) & 0xFF);
        headerByteArray[3] = static_cast<std::byte>(entry.second.offset & 0xFF);
        //writing length to bytes to write to file
        headerByteArray[4] = static_cast<std::byte>((entry.second.length >> 24) & 0xFF);
        headerByteArray[5] = static_cast<std::byte>((entry.second.length >> 16) & 0xFF);
        headerByteArray[6] = static_cast<std::byte>((entry.second.length >> 8) & 0xFF);
        headerByteArray[7] = static_cast<std::byte>(entry.second.length & 0xFF);
        //write header to file
        GSS.write(reinterpret_cast<const char*>(headerByteArray),8);

    }
    //write down header info: first 8 bytes is size, second 8 is offset 
    uint64_t size = headerStore.size() * 8;
    //std::cout << size << std::endl;
    std::byte headerInfoBytes[16];
    //writing size consistently across endian styles
    headerInfoBytes[0] = static_cast<std::byte>((size >> 56) & 0xFF);
    headerInfoBytes[1] = static_cast<std::byte>((size >> 48) & 0xFF);
    headerInfoBytes[2] = static_cast<std::byte>((size >> 40) & 0xFF);
    headerInfoBytes[3] = static_cast<std::byte>((size >> 32) & 0xFF);
    headerInfoBytes[4] = static_cast<std::byte>((size >> 24) & 0xFF);
    headerInfoBytes[5] = static_cast<std::byte>((size >> 16) & 0xFF);
    headerInfoBytes[6] = static_cast<std::byte>((size >> 8) & 0xFF);
    headerInfoBytes[7] = static_cast<std::byte>(size & 0xFF);
    //writing length
    headerInfoBytes[8] = static_cast<std::byte>((offset >> 56) & 0xFF);
    headerInfoBytes[9] = static_cast<std::byte>((offset >> 48) & 0xFF);
    headerInfoBytes[10] = static_cast<std::byte>((offset >> 40) & 0xFF);
    headerInfoBytes[11] = static_cast<std::byte>((offset >> 32) & 0xFF);
    headerInfoBytes[12] = static_cast<std::byte>((offset >> 24) & 0xFF);
    headerInfoBytes[13] = static_cast<std::byte>((offset >> 16) & 0xFF);
    headerInfoBytes[14] = static_cast<std::byte>((offset >> 8) & 0xFF);
    headerInfoBytes[15] = static_cast<std::byte>(offset & 0xFF);
    GSS.write(reinterpret_cast<const char*>(headerInfoBytes),16);
    GSS.close();
}
