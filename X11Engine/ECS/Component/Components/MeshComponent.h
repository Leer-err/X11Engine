#pragma once
#include <vector>
#include "Vertex.h"
#include "Component.h"

using std::vector;

struct MeshComponent : public ECS::Component<MeshComponent> {
	MeshComponent(EntityId owner, vector<vertex> vertices = {}, vector<uint32_t> indices = {}) : Component(owner), vertices(vertices), indices(indices) {}
	vector<vertex> vertices;
	vector<uint32_t> indices;
};
