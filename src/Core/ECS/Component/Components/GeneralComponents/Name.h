#ifndef NAME_H
#define NAME_H

#include <string>

struct Name {
    std::string name;

    std::string serialize() const;
};

#endif  // NAME_H