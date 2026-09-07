module mesh;

public typedef uint32_t MeshletIndex;

typedef uint32_t VertexIndex;
typedef uint8_t TriangleIndex;

public struct Meshlet{
    uint32_t vertex_offset; 
    uint32_t triangle_offset;

    public uint8_t vertex_count;
    public uint8_t triangle_count;
};

public struct Vertex{
    public float3 position;
    public float2 uv;
};

public struct Mesh<T>{
    T* vertices;
    TriangleIndex* meshlet_triangles;
    VertexIndex* meshlet_vertices;
    public Meshlet* meshlets;

    public T getVertex(in MeshletIndex meshlet_index, in VertexIndex vertex_index){
        Meshlet meshlet = meshlets[meshlet_index];
        VertexIndex meshlet_vertex_index = meshlet.vertex_offset + vertex_index;
        VertexIndex mesh_vertex_index = meshlet_vertices[meshlet_vertex_index];
        return vertices[mesh_vertex_index];
    }

    public uint3 getTriangle(in MeshletIndex meshlet_index, in TriangleIndex triangle_index){
        Meshlet meshlet = meshlets[meshlet_index];
        uint meshlet_triangle_index = meshlet.triangle_offset + triangle_index * 3;

        uint3 triangle;
        triangle.x = meshlet_triangles[meshlet_triangle_index];
        triangle.y = meshlet_triangles[meshlet_triangle_index + 1];
        triangle.z = meshlet_triangles[meshlet_triangle_index + 2];

        return triangle;
    }
};