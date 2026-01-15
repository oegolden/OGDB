#include "globalStringStorage.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstring>
#include <span>
#include <cstdlib>

//TODO: Rethink this whole thing with deletes in mind, how can we fill the holes in the file?
//keep track of open slots and if the slot has enough size we can put stuff in that
//create a save function that also does clean up tightens strings together updating header offsets
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
        char charsOfSize[8];
        GSS.read(charsOfSize,8);
        long size = std::atoi(charsOfSize);
        char charsOfoffset[8];
        GSS.read(charsOfoffset,8);
        long offset = std::atoi(charsOfoffset);
        //should have hit eof marker so we need to clear
        GSS.clear();
        GSS.seekg(offset, std::ios::beg);
        char headerChars[size];
        GSS.read(headerChars,size);
        //turn headerChars into unordered map
        //first 4 bytes is offset
        //next 4 bytes is length
        for(int i = 0; i < size; i+=8){
            uint32_t offset = headerChars[i] | 
            (headerChars[i+1] << 8) |
            (headerChars[i+2] << 16) | 
            (headerChars[i+3] << 24);
            uint32_t length = headerChars[i + 4] | 
            (headerChars[i+5] << 8) |
            (headerChars[i+6] << 16) | 
            (headerChars[i+7] << 24);
            GlobalStringStorage::stringEntry header = {offset,length};
            headerStore[i/8] = header;
        }
    } else{
        std::ofstream GSS(f);
    }
}

GlobalStringStorage::~GlobalStringStorage()
{
    // currently no dynamic resources require explicit cleanup.
}

void GlobalStringStorage::readInChunk(int headerSlot)
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
    char temp[numBytes];
    GSS.read(temp,readBytes);
    str_store.insert(str_store.end(),temp,temp + numBytes);
    chunkStart = headerSlot;
    GSS.close();
}

std::string GlobalStringStorage::getString(int headerSlot)
{
    //if current chunk contains our string then get it from the chunk
    //else load in the chunk containing that string through CHUNKSIZE
    if (headerSlot < chunkStart || headerSlot > chunkStart + CHUNKSIZE){
        //load in new chunk of strings
        readInChunk(headerSlot);
    }
    //calculate start of string by taking the offset of headerStore[chunkStart] and headerSlot
    int byteOffset = headerStore[headerSlot].offset - headerStore[chunkStart].offset;
    int length = headerStore[headerSlot].length;
    //std::cout << byteOffset << std::endl;
    //std::cout << length << std::endl;
    std::string ret(str_store.data() + byteOffset, length);
    return ret;
}

int GlobalStringStorage::putString(std::string s)
{
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
    if (chunkStart + CHUNKSIZE > last_slot+1){
        str_store.insert(str_store.end(), s.data(), s.data()+s.size());
    }
    GSS.close();
    return last_slot + 1;
}

int GlobalStringStorage::deleteString(int headerSlot){
    newlyOpenSlots.insert(headerStore[headerSlot]);
    headerStore.erase(headerSlot);
    return headerSlot;
}
