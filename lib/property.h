#ifndef PROPERTY_H
#define PROPERTY_H

#include <string>
#include <cstdint>
#include <stdexcept>

class Property {
    enum class PropertyType {string, integer};
public:
    static constexpr std::size_t NAMELENGTH = 64;

    Property(const std::string& name, uint16_t prevPropId, PropertyType propType);
    ~Property() = default;

    const std::string& getName() const;
    void setName(const std::string& name);

    int32_t getNextPropId() const;
    void setNextPropId(int32_t id);

    uint16_t getPrevPropId() const;
    void setPrevPropId(uint16_t id);

private:
    std::string name;
    int32_t nextPropId; // -1 indicates no next property
    uint16_t prevPropId;
    PropertyType propType;
};

#endif // PROPERTY_H