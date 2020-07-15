#ifndef MESH_H
#define MESH_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    bool operator==(const Vertex & Other) const {
        return Position == Other.Position && Normal == Other.Normal && TexCoords == Other.TexCoords;
    }
};

// https://en.cppreference.com/w/cpp/utility/hash
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const & Vertex) const {
            return ((hash<glm::vec3>()(Vertex.Position) ^
                    (hash<glm::vec3>()(Vertex.Normal) << 1)) >> 1) ^
                    (hash<glm::vec2>()(Vertex.TexCoords) << 1);
        }
    };
}

class Mesh {
    public:
        Mesh();
        ~Mesh();

        std::vector<Vertex> VertexBuffer;
        std::vector<uint32_t> IndexBuffer;
};

#endif