#pragma once


#include <Common.hpp>
#include <DynamicMeshProperties.hpp>
#include <AABB.hpp>


#pragma pack(push, 1)
struct DynamicMeshVertex {
    GLfloat position[3];   // 3D position vector.
    GLfloat color[3];      // 3D color vector (R, G, B).
};
#pragma pack(pop)


/**
 * @brief Represents a colored mesh, whose vertices can be updated online.
 */
class DynamicMesh {
    public:
        DynamicMeshProperties properties;

        /**
         * @brief Create a dynamic mesh object.
         */
        DynamicMesh();

        /**
         * @brief Generate GL content (vao, vbo, ebo).
         */
        void Generate(void);

        /**
         * @brief Delete all GL content that was generated by the @ref Generate member function.
         */
        void Delete(void);

        /**
         * @brief Update the mesh data.
         * @param[in] vertices The vertices to be used as mesh data.
         * @param[in] indices The array of zero-based indices to vertices representing triangles.
         * @param[in] aabbOfVertices The axis-aligned bounding box that contains all vertices.
         * @details The mesh has to be generated by @ref Generate before calling this member function!
         */
        void UpdateMeshData(const std::vector<DynamicMeshVertex>& vertices, const std::vector<GLuint>& indices, const AABB aabbOfVertices);

        /**
         * @brief Clear the current mesh data.
         * @details The mesh has to be generated by @ref Generate before calling this member function!
         */
        void ClearMeshData(void);

        /**
         * @brief Check whether the current mesh data is empty.
         * @return True if the current mesh data is empty, false otherwise.
         */
        bool MeshDataIsEmpty(void);

        /**
         * @brief Draw all triangles of the mesh.
         */
        void Draw(void);

        /**
         * @brief Get the axis-aligned bounding box of all vertices of this dynamic mesh transformed according to the @ref properties.
         */
        AABB GetAABBOfTransformedMesh(void);

    private:
        GLuint vao;                   // VAO for mesh data.
        GLuint vbo;                   // VBO for mesh data.
        GLuint ebo;                   // EBO for mesh data.
        GLsizei numIndices;           // The number of indices to be rendered.
        size_t bufferSizeVertices;    // Current buffer size for vertices.
        size_t bufferSizeIndices;     // Current buffer size for indices.
        AABB aabbVertices;            // The axis-aligned bounding box containing all vertices.
};

