#include "ModelRegistry.h"

#include "Model.h"

void ModelRegistry::add(const Model& model) {
    models.emplace(model.name, model);
}

const Model* ModelRegistry::get(const std::string& name) const {
    auto model = models.find(name);

    if (model == models.end()) return nullptr;

    return &model->second;
}