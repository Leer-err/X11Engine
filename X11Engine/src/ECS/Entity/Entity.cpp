#include "Entity.h"

#include "Children.h"
#include "Parent.h"

void Entity::addChild(Entity child) {
    Children* children = get<Children>();
    if (children == nullptr) {
        set<Children>({{child}});
    } else {
        children->children.push_back(child);
    }

    child.set<Parent>({*this});
}