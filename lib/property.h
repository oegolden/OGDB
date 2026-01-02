#ifndef PROPERTY_H
#define PROPERTY_H

#include <string>
#include <array>
class PropertyBase {
public:
    virtual ~PropertyBase() = default;
    virtual PropertyBase* getNextProperty() const = 0;
    virtual PropertyBase* getPrevProperty() const = 0;
    virtual int getNextPropertyOffset() const = 0;
    virtual int getPrevPropertyOffset() const = 0;
    virtual void setNextProperty(PropertyBase* p) = 0;
    virtual void setPrevProperty(PropertyBase* p) = 0;
    virtual std::string getName() const = 0;
};


class IntProperty(){

};

class StringProperty(){

};

class listProperty(){

};
#endif