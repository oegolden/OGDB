// Global String storage manages, getting strings for string properties from text files

#ifndef NODE_H

#define NODE_H

#include "relationship.h"
#include "property.h"
#include <cstring>
#include <vector>
#include <cstdint>
#include <array>
#include <memory>
#include <cstddef>
#include <string>
#include <map>
#include <set>

class GlobalStringStorage
{
    static constexpr int CHUNKSIZE = 64;
public:
    GlobalStringStorage();
    ~GlobalStringStorage();
    std::string getString(int headerSlot);
    int putString(std::string string);
    int deleteString(int headerSlot);
private:
    struct stringEntry{
        uint32_t offset; // Where it starts
        uint32_t length; // How big it is
    };
    //stores the string entries which string properties will index
    std::map<int,stringEntry>headerStore;
    //stores the strings as bytes
    std::vector<char> str_store;
    //name of the file where the string bytes are stored
    std::string filename;
    /* TODO: implemented later 
    std::unique_ptr<std::byte[]> str_bus;
    int open_slots;
    */

    uint32_t chunkStart;
    // Source - https://stackoverflow.com/a
    // Posted by Jarod42
    // Retrieved 2026-01-11, License - CC BY-SA 3.0
    struct LessLength
    {
        bool operator ()(const stringEntry& lhs, const stringEntry& rhs) const
        {
            return lhs.length < rhs.length;
        }
    };

        std::multiset<stringEntry,LessLength> newlyOpenSlots;
        void readInChunk(int headerSlot);
    };

#endif
