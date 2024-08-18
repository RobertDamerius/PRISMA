#include <WaterMesh.hpp>


WaterMesh::WaterMesh(){
    vao = vbo = ebo = 0;
    numIndices = 0;
    bufferSizeVertices = 0;
    bufferSizeIndices = 0;
}

void WaterMesh::UpdateMeshData(const std::vector<WaterMeshVertex>& vertices, const std::vector<GLuint>& indices){
    // bind vertex array object
    DEBUG_GLCHECK( glBindVertexArray(vao); );

    // update vertices (use glBufferSubData if enough memory, glBufferData to reallocate memory)
    DEBUG_GLCHECK( glBindBuffer(GL_ARRAY_BUFFER, vbo); );
    if(vertices.size() > bufferSizeVertices){
        DEBUG_GLCHECK( glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(WaterMeshVertex), &(vertices[0]), GL_STATIC_DRAW); );
        bufferSizeVertices = vertices.size();
    }
    else{
        DEBUG_GLCHECK( glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(WaterMeshVertex), &(vertices[0])); );
    }

    // update indices (use glBufferSubData if enough memory, glBufferData to reallocate memory)
    DEBUG_GLCHECK( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); );
    if(indices.size() > bufferSizeIndices){
        DEBUG_GLCHECK( glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &(indices[0]), GL_STATIC_DRAW); );
        bufferSizeIndices = indices.size();
    }
    else{
        DEBUG_GLCHECK( glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), &(indices[0])); );
    }

    DEBUG_GLCHECK( glBindVertexArray(0); );
    numIndices = static_cast<GLsizei>(indices.size());
}

void WaterMesh::ClearMeshData(void){
    std::vector<WaterMeshVertex> emptyVertices;
    std::vector<GLuint> emptyIndices;
    UpdateMeshData(emptyVertices, emptyIndices);
}

void WaterMesh::Generate(void){
    DEBUG_GLCHECK( glGenVertexArrays(1, &vao); );
    DEBUG_GLCHECK( glGenBuffers(1, &vbo); );
    DEBUG_GLCHECK( glGenBuffers(1, &ebo); );
    DEBUG_GLCHECK( glBindVertexArray(vao); );
        DEBUG_GLCHECK( glBindBuffer(GL_ARRAY_BUFFER, vbo); );
        DEBUG_GLCHECK( glBufferData(GL_ARRAY_BUFFER, bufferSizeVertices * sizeof(WaterMeshVertex), nullptr, GL_STATIC_DRAW); );
        DEBUG_GLCHECK( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); );
        DEBUG_GLCHECK( glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSizeIndices * sizeof(GLuint), nullptr, GL_STATIC_DRAW); );
        // position attribute
        DEBUG_GLCHECK( glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(WaterMeshVertex), (GLvoid*)0); );
        DEBUG_GLCHECK( glEnableVertexAttribArray(0); );
    DEBUG_GLCHECK( glBindVertexArray(0); );
    numIndices = 0;
}

void WaterMesh::Delete(void){
    if(vao){
        DEBUG_GLCHECK( glDeleteVertexArrays(1, &(vao)); );
        vao = 0;
    }
    if(vbo){
        DEBUG_GLCHECK( glDeleteBuffers(1, &(vbo)); );
        vbo = 0;
    }
    if(ebo){
        DEBUG_GLCHECK( glDeleteBuffers(1, &(ebo)); );
        ebo = 0;
    }
    numIndices = 0;
}

void WaterMesh::Draw(void){
    DEBUG_GLCHECK( glBindVertexArray(vao); );
    DEBUG_GLCHECK( glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (GLvoid*)0); );
}

