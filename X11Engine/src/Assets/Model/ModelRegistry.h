#ifndef MODEL_REGISTRY_H
#define MODEL_REGISTRY_H

#include <string>
#include <unordered_map>

class Model;

class ModelRegistry {
   public:
    void add(const Model& Model);
    const Model* get(const std::string& name) const;

   private:
    std::unordered_map<std::string, Model> models;
};

#endif  // MODEL_REGISTRY_H