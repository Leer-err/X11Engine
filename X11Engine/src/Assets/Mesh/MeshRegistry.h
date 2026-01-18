#pragma once

#include <string>
#include <unordered_map>

#include "Logger.h"
#include "Mesh.h"

class MeshRegistry {
   public:
    MeshRegistry();

    Mesh get(const std::string& name);

   private:
    Mesh load(const std::string& name);

    std::unordered_map<std::string, Mesh> meshes;

    Logger logger;
};