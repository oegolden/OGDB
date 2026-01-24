#include "relationship.h"
#include <algorithm>
#include <string>
#include <stdexcept>
#include <cstdint>
// Getters
bool Relationship::getInUse() const {
    return inUse;
}

uint32_t Relationship::getFirstNode() const {
    return firstNode;
}

uint32_t Relationship::getSecondNode() const {
    return secondNode;
}

std::string Relationship::getRelationshipType() const {
    return relationshipType;
}

uint32_t Relationship::getFirstPrevRelId() const {
    return firstPrevRelId;
}

uint32_t Relationship::getFirstNextRelId() const {
    return firstNextRelId;
}

uint32_t Relationship::getSecondPrevRelId() const {
    return secondPrevRelId;
}

uint32_t Relationship::getSecondNextRelId() const {
    return secondNextRelId;
}

uint32_t Relationship::getNextPropId() const {
    return nextPropId;
}

bool Relationship::getFirstInChainMarker() const {
    return firstInChainMarker;
}

// Setters
void Relationship::setInUse(bool value) {
    inUse = value;
}

void Relationship::setFirstNode(uint32_t value) {
    firstNode = value;
}

void Relationship::setSecondNode(uint32_t value) {
    secondNode = value;
}

void Relationship::setRelationshipType(const std::string& value) {
    // Ensure relationshipType is no more than 4 bytes (4 characters)
    if (value.length() > Relationship::TYPESIZE || value.length() <= 0) {
        throw std::length_error("String must be between 0 and " + std::to_string(TYPESIZE) + " characters ");
    } else {
        relationshipType = value;
    }
}

void Relationship::setFirstPrevRelId(uint32_t value) {
    firstPrevRelId = value;
}

void Relationship::setFirstNextRelId(uint32_t value) {
    firstNextRelId = value;
}

void Relationship::setSecondPrevRelId(uint32_t value) {
    secondPrevRelId = value;
}

void Relationship::setSecondNextRelId(uint32_t value) {
    secondNextRelId = value;
}

void Relationship::setNextPropId(uint32_t value) {
    nextPropId = value;
}

void Relationship::setFirstInChainMarker(bool value) {
    firstInChainMarker = value;
}
