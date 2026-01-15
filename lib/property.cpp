#include "property.h"

#include <stdexcept>

Property::Property(const std::string& name, uint16_t prevPropId, PropertyType propType)
	: name(), nextPropId(-1), prevPropId(prevPropId), propType(propType)
{
	if (name.empty() || name.size() > Property::NAMELENGTH) {
		throw std::invalid_argument("Name for property must be 1.." + std::to_string(Property::NAMELENGTH) + " characters");
	}
	this->name = name;
}

const std::string& Property::getName() const {
	return name;
}

void Property::setName(const std::string& newName) {
	if (newName.empty() || newName.size() > Property::NAMELENGTH) {
		throw std::invalid_argument("Name for property must be 1.." + std::to_string(Property::NAMELENGTH) + " characters");
	}
	name = newName;
}

int32_t Property::getNextPropId() const {
	return nextPropId;
}

void Property::setNextPropId(int32_t id) {
	nextPropId = id;
}

uint16_t Property::getPrevPropId() const {
	return prevPropId;
}

void Property::setPrevPropId(uint16_t id) {
	prevPropId = id;
}

