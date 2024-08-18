#pragma once


#include <Common.hpp>
#include <WaterMesh.hpp>


class NonConvexPolygon {
    public:
        std::vector<std::vector<std::array<GLfloat,2>>> verticesAndHoles;   // A list of polylines where the first polyline indicates the outer hull vertices and all following polylines indicate hole vertices.

        /**
         * @brief Triangulate this non-convex polygon (@ref verticesAndHoles) and insert the resulting vertices and indices at the end of the given output container.
         * @param[inout] inoutVertices The container of vertices where to store the vertices of this non-convex polygon.
         * @param[inout] inoutIndices The container of indices where to store the indices of the triangulation.
         */
        void Triangulate(std::vector<WaterMeshVertex>& inoutVertices, std::vector<GLuint>& inoutIndices);
};

