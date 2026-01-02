// Node object

#ifndef NODE_H

#define NODE_H


#include "relationship.h"
#include "property.h"
#include <string>

class Node {
    const int NAMESIZE;
    const int VALUESIZE;

    public:

        struct Label{
            char name[255];
            char value[255];
        };

        Node();
        //starting with singular label per node
        // will probably adopt multiple labels
        Label getLabel() const;
        //Gets the first relationship in the list of them
        Relationship getFirstRelationship() const;
        //gets the first property in the list of them
        PropertyBase* getFirstProperty() const;

        int getRelationshipOffset() const;

        int getPropertyOffset()const;


    private:
        bool inUse;
        int nextRelOffset;
        int nextPropOffset;
        Relationship *nextRelationship;
        PropertyBase *nextProperty;
        Label label;
};

#endif
