#include <vector>
#include "Vertex.h"

using std::vector;

struct Mesh {
	vector<vertex> vertices;
	vector<uint32_t> indices;
	int materialIndex;
};