#include <vector>
#include "Vertex.h"

using std::vector;

struct Mesh {
	Mesh(const vector<vertex>& vertices, const vector<uint32_t>& indices, int materialIndex) :
		vertices(vertices), indices(indices), materialIndex(materialIndex) {}

	vector<vertex> vertices;
	vector<uint32_t> indices;
	int materialIndex;
};