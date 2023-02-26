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

    int AddMaterial(Material&& material) {
        int index = m_materials.size();
        m_materials.emplace_back(material);
        return index;
    }

    inline Material* GetMaterial(int index) { return &m_materials[index]; }

   private:
    MaterialRegistry() {}

    MaterialRegistry(const MaterialRegistry&) = delete;
    MaterialRegistry& operator=(const MaterialRegistry&) = delete;
    MaterialRegistry(MaterialRegistry&&) = delete;
    MaterialRegistry& operator=(MaterialRegistry&&) = delete;

    vector<Material> m_materials;
};