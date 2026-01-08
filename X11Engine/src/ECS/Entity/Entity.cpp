#include "Entity.h"

#include <memory>
#include <utility>

#include "Children.h"
#include "IScript.h"
#include "Parent.h"
#include "Scripts.h"

void Entity::addChild(Entity child) {
    Children* children = get<Children>();
    if (children == nullptr) {
        set<Children>({{child}});
    } else {
        children->children.push_back(child);
    }

    child.set<Parent>({*this});
}

void Entity::addScript(std::unique_ptr<IScript>&& script) {
    auto scripts = get<Scripts>();
    if (scripts == nullptr) {
        std::vector<std::unique_ptr<IScript>> scripts_vector;
        scripts_vector.emplace_back(std::move(script));

        set<Scripts>({std::move(scripts_vector)});
    } else {
        scripts->scripts.emplace_back(std::move(script));
    }
}