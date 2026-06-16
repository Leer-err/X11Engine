#include "MeshBuilder.h"

#include <cstring>
#include <vector>

#include "BufferBuilder.h"
#include "BufferRegistry.h"
#include "Device.h"
#include "GraphicsMesh.h"
#include "Mesh.h"
#include "Meshlet.h"
#include "meshoptimizer.h"

namespace Graphics {

MeshBuilder::MeshBuilder(const void* vertex_data, size_t vertex_data_size,
                         const void* index_data, size_t index_data_size)
    : vertex_data(vertex_data),
      vertex_data_size(vertex_data_size),
      index_data(index_data),
      index_data_size(index_data_size) {}

MeshBuilder::MeshBuilder(const ::Mesh& mesh)
    : vertex_data(mesh.vertices.data()),
      vertex_data_size(mesh.vertices.size() * sizeof(Vertex)),
      index_data(mesh.indices.data()),
      index_data_size(mesh.indices.size() * sizeof(uint32_t)) {}

Graphics::Mesh MeshBuilder::create(Device& device, BufferRegistry& registry,
                                   StagingBuffer& staging_buffer) {
    const size_t max_vertices = 64;
    const size_t max_triangles = 96;
    const float cone_weight = 0.25f;

    auto index_count = index_data_size / sizeof(uint32_t);
    auto vertex_count = vertex_data_size / sizeof(Vertex);
    size_t max_meshlets =
        meshopt_buildMeshletsBound(index_count, max_vertices, max_triangles);
    std::vector<meshopt_Meshlet> meshopt_meshlets(max_meshlets);
    std::vector<uint32_t> meshlet_vertices(index_count);
    std::vector<uint8_t> meshlet_triangles(index_count);

    size_t meshlet_count = meshopt_buildMeshlets(
        meshopt_meshlets.data(), meshlet_vertices.data(),
        meshlet_triangles.data(), (uint32_t*)index_data, index_count,
        (float*)vertex_data, vertex_count, sizeof(Vertex), max_vertices,
        max_triangles, cone_weight);

    const auto& last = meshopt_meshlets[meshlet_count - 1];

    meshlet_vertices.resize(last.vertex_offset + last.vertex_count);
    meshlet_triangles.resize(last.triangle_offset + last.triangle_count * 3);
    meshopt_meshlets.resize(meshlet_count);

    for (const auto& meshlet : meshopt_meshlets) {
        meshopt_optimizeMeshlet(&meshlet_vertices[meshlet.vertex_offset],
                                &meshlet_triangles[meshlet.triangle_offset],
                                meshlet.triangle_count, meshlet.vertex_count);
    }

    std::vector<Meshlet> meshlets;

    for (const auto& meshopt_meshlet : meshopt_meshlets) {
        Meshlet meshlet;
        meshlet.triangle_count = meshopt_meshlet.triangle_count;
        meshlet.triangle_offset = meshopt_meshlet.triangle_offset;
        meshlet.vertex_count = meshopt_meshlet.vertex_count;
        meshlet.vertex_offset = meshopt_meshlet.vertex_offset;

        meshlets.push_back(meshlet);
    }

    auto vertex_buffer = BufferBuilder(vertex_data_size)
                             .isShaderResource()
                             .isCopyDestination()
                             .create(device, registry)
                             .getResult();
    staging_buffer.stageBuffer(vertex_buffer, vertex_data, vertex_data_size);

    size_t meshlet_buffer_size = meshlets.size() * sizeof(Meshlet);
    auto meshlet_buffer = BufferBuilder(meshlet_buffer_size)
                              .isShaderResource()
                              .isCopyDestination()
                              .create(device, registry)
                              .getResult();
    staging_buffer.stageBuffer(meshlet_buffer, meshlets.data(),
                               meshlet_buffer_size);

    size_t meshlet_vertices_buffer_size =
        meshlet_vertices.size() * sizeof(uint32_t);
    auto meshlet_vertices_buffer = BufferBuilder(meshlet_vertices_buffer_size)
                                       .isShaderResource()
                                       .isCopyDestination()
                                       .create(device, registry)
                                       .getResult();
    staging_buffer.stageBuffer(meshlet_vertices_buffer, meshlet_vertices.data(),
                               meshlet_vertices_buffer_size);

    size_t meshlet_indices_buffer_size =
        meshlet_triangles.size() * sizeof(uint8_t);
    auto meshlet_triangles_buffer = BufferBuilder(meshlet_indices_buffer_size)
                                        .isShaderResource()
                                        .isCopyDestination()
                                        .create(device, registry)
                                        .getResult();
    staging_buffer.stageBuffer(meshlet_triangles_buffer,
                               meshlet_triangles.data(),
                               meshlet_indices_buffer_size);

    return Mesh(vertex_buffer, meshlet_buffer, meshlet_vertices_buffer,
                meshlet_triangles_buffer, meshlet_count);
}

}  // namespace Graphics
