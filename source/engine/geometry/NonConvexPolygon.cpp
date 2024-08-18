#include <NonConvexPolygon.hpp>
#include <thirdparty/mapbox/earcut.hpp>


void NonConvexPolygon::Triangulate(std::vector<WaterMeshVertex>& inoutVertices, std::vector<GLuint>& inoutIndices){
    std::vector<GLuint> indices = mapbox::earcut<GLuint>(verticesAndHoles);
    if(!(indices.size() % 3)){
        // add new indices
        GLuint indexOffset = static_cast<GLuint>(inoutVertices.size());
        for(auto&& i : indices){
            inoutIndices.push_back(indexOffset + i);
        }

        // add new vertices
        for(auto&& polyline : verticesAndHoles){
            for(auto&& vertex : polyline){
                inoutVertices.push_back(WaterMeshVertex());
                inoutVertices.back().position[0] = vertex[0];
                inoutVertices.back().position[1] = vertex[1];
            }
        }
    }
}

