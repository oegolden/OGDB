#include "node.h"
#include <string>
#include <stdexcept>
#include <cstring>
#include <span>
#include <vector>
#include <iostream>

constexpr size_t LABELSIZE = 64;

Node::Node(std::string newlabel) : inUse(true), firstRelationshipId(0), firstpropertyId(0)
{
	setLabel(newlabel);
}

Node::Node(std::vector<std::byte> chunk){
	// Deserialize inUse flag (byte 0)
	inUse = static_cast<bool>(chunk[0]);
	
	// Deserialize firstRelationshipId (bytes 1-4, big-endian)
	firstRelationshipId = (static_cast<uint32_t>(chunk[1]) << 24) |
	                      (static_cast<uint32_t>(chunk[2]) << 16) |
	                      (static_cast<uint32_t>(chunk[3]) << 8) |
	                      static_cast<uint32_t>(chunk[4]);
	
	// Deserialize firstpropertyId (bytes 5-8, big-endian)
	firstpropertyId = (static_cast<uint32_t>(chunk[5]) << 24) |
	                  (static_cast<uint32_t>(chunk[6]) << 16) |
	                  (static_cast<uint32_t>(chunk[7]) << 8) |
	                  static_cast<uint32_t>(chunk[8]);
	
	// Deserialize label (bytes 9 onwards, up to LABELSIZE)
	label = std::string(reinterpret_cast<const char*>(&chunk[9]), LABELSIZE);
	// Trim null terminators and trailing spaces if any
	size_t nullPos = label.find('\0');
	if (nullPos != std::string::npos) {
		label = label.substr(0, nullPos);
	}
}
void Node::setUseState(bool inUseState)
{
	inUse = inUseState;
}

void Node::setFirstRelationship(uint32_t relId)
{
	firstRelationshipId = relId;
}

void Node::setFirstProperty(uint32_t propId)
{
	firstpropertyId = propId;
}

void Node::setLabel(std::string newLabel)
{
	if (newLabel.size() > LABELSIZE || newLabel.size() < 1)
	{
		throw std::length_error("label must have size greater than 0 characters but less than " + std::to_string(LABELSIZE));
	}
	else
	{
		label = newLabel;
	}
}

std::array<std::byte, (LABELSIZE + 8 + 2)> Node::serializeObject() const{
	std::array<std::byte, LABELSIZE + 8 + 2> bitNode;
	if (inUse) {
		bitNode[0] = static_cast<std::byte>(1);
	} else{
		bitNode[0] = static_cast<std::byte>(0);
	}
	bitNode[1] = static_cast<std::byte>((firstRelationshipId >> 24) & 0xFF); // Most significant byte
	bitNode[2] = static_cast<std::byte>((firstRelationshipId >> 16) & 0xFF);
	bitNode[3] = static_cast<std::byte>((firstRelationshipId >> 8) & 0xFF);
	bitNode[4] = static_cast<std::byte>(firstRelationshipId & 0xFF);
	bitNode[5] = static_cast<std::byte>((firstpropertyId >> 24) & 0xFF); // Most significant byte
	bitNode[6] = static_cast<std::byte>((firstpropertyId >> 16) & 0xFF);
	bitNode[7] = static_cast<std::byte>((firstpropertyId >> 8) & 0xFF);
	bitNode[8] = static_cast<std::byte>(firstpropertyId & 0xFF);
	std::byte *dest = &bitNode[9];
	// don't have to cast label since a char array = a byte array kinda
	std::memmove(dest, label.data(), LABELSIZE + 1);
	return bitNode;
}
