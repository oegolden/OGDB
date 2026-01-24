#ifndef RELATIONSHIP_H

#define RELATIONSHIP_H

#include <string>
#include <cstdlib>
#include <cstdint>

class Relationship{
    static constexpr uint8_t TYPESIZE = 32;
    public:
        // Getters
        bool getInUse() const;
        uint32_t getFirstNode() const;
        uint32_t getSecondNode() const;
        std::string getRelationshipType() const;
        uint32_t getFirstPrevRelId() const;
        uint32_t getFirstNextRelId() const;
        uint32_t getSecondPrevRelId() const;
        uint32_t getSecondNextRelId() const;
        uint32_t getNextPropId() const;
        bool getFirstInChainMarker() const;

        // Setters
        void setInUse(bool value);
        void setFirstNode(uint32_t value);
        void setSecondNode(uint32_t value);
        void setRelationshipType(const std::string& value);
        void setFirstPrevRelId(uint32_t value);
        void setFirstNextRelId(uint32_t value);
        void setSecondPrevRelId(uint32_t value);
        void setSecondNextRelId(uint32_t value);
        void setNextPropId(uint32_t value);
        void setFirstInChainMarker(bool value);
                
    private:
        bool inUse;
        uint32_t firstNode;
        uint32_t secondNode;
        std::string relationshipType;
        uint32_t firstPrevRelId;
        uint32_t firstNextRelId;
        uint32_t secondPrevRelId;
        uint32_t secondNextRelId;
        uint32_t nextPropId;
        bool firstInChainMarker;
};
#endif
