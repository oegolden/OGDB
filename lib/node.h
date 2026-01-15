// Node object

#ifndef NODE_H

#define NODE_H

#include "relationship.h"
#include "property.h"
#include <cstring>
#include <vector>
#include <cstdint>
#include <array>
class Node
{
    static constexpr inline int LABELSIZE = 64;
public:
    static constexpr std::size_t SERIALIZED_SIZE = LABELSIZE + 8 + 2;
    Node(std::string newlabel);
    /// @brief gets the label for the ndoe
    /// @return returns the label of size LABELSIZE
    Node(std::vector<std::byte> chunk);
    // Accept byte vectors of type uint8_t (used by tests)
    Node(std::vector<uint8_t> chunk);
    std::string getLabel() { return label; }
    /// @brief gets the global offset of the first relationship id
    /// @return returns the global offset of the relationship id of size 16 bits
    uint32_t getFirstRelationshipID() const { return firstRelationshipId; }
    /// @brief gets the global offset of the first property id
    /// @return returns the global offset of the property id of size 16 bits
    uint32_t getFirstPropertyID() const { return firstpropertyId; }
    /// @brief set's wheter or not the node is in use
    /// @param inUseState: whether the node is in use
    void setUseState(bool inUseState);
    /// @brief set's the global offset of the first relationship id
    /// @param
    bool getUseState() const {return inUse; }
    void setFirstRelationship(uint32_t relId);
    /// @brief sets the global offset of the first property id
    void setFirstProperty(uint32_t propId);
    /// @brief takles in a string and renames the label of the node to that
    /// @param  string user input string representing new label name, must be < LABELSIZE
    void setLabel(std::string newLabel);
    /**
     * @brief Serializes the object into a byte array.
     *
     * Converts the object's data into a fixed-size byte array containing:
     * - LABELSIZE bytes for the label data
     * - sizeof(uint32_t) bytes for the first uint32_t field
     * - sizeof(uint32_t) bytes for the second uint32_t field
     *
     * @return std::array<std::byte, (LABELSIZE + sizeof(uint32_t) + sizeof(uint32_t))>
     *         A byte array containing the serialized representation of the object.
     */
    std::array<std::byte, (LABELSIZE + 8 + 2)> serializeObject() const;

private:
    bool inUse;
    std::string label;
    void deserialize(const std::vector<std::byte>& chunk);
    // consider using a union when i want to pointer swivel
    // these are GLOBAL IDs consider when implementing buffer manager
    uint32_t firstRelationshipId;
    uint32_t firstpropertyId;
};

#endif
