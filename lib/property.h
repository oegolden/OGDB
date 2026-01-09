#ifndef PROPERTY_H
#define PROPERTY_H

#include <string>
#include <array>
template <typename T>
class Property{
public:
    virtual T getValue() const = 0;
    virtual std::string getName() const;
    virtual void setValue() = 0;
    virtual long getNextPropId() const;
    virtual void setNextPropId();
private:
    std::string name;
    long nextPropId;
    long prevPropId;

};

class IntProperty : Property<int>{
    public:
        /// @brief  sets the integer value of the property, no overflow needed
        void setValue() override;
        /// @brief gets the value of the int property namely the integer value
        /// @return returns the integer
        int getValue() const override;
};

class stringProperty : Property<std::string>{
    public:
        /// @brief sets the string value of the string property, allocates to overflow if needed
        void setValue() override;
        /// @brief gets the string value 
        /// @return returns the string, either retrieved from 
        std::string getValue() const override;
};

#endif