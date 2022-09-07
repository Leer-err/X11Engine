#pragma once

#include <vector>

#include "Graphics/Material.h"

using std::vector;

class MaterialRegistry {
   public:
    static MaterialRegistry* get() {
        static MaterialRegistry instance;
        return &instance;
    }

    Material* AddMaterial(Material&& material) {
        return &m_materials.emplace_back(std::move(material));
    }

   private:
    MaterialRegistry() {}

    MaterialRegistry(const MaterialRegistry&) = delete;
    MaterialRegistry& operator=(const MaterialRegistry&) = delete;
    MaterialRegistry(MaterialRegistry&&) = delete;
    MaterialRegistry& operator=(MaterialRegistry&&) = delete;

    vector<Material> m_materials;
};