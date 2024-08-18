#pragma once


#include <Common.hpp>
#include <TextureSubmeshMaterial.hpp>
#include <ShaderTextureMesh.hpp>


#pragma pack(push, 1)
struct TextureMeshVertex {
    GLfloat position[3]; // 3D position vector.
    GLfloat normal[3];   // 3D normal vector.
    GLfloat tangent[3];  // 3D tangent vector.
    GLfloat texCoord[2]; // 2D UV texture coordinates.
};
#pragma pack(pop)


/**
 * @brief Represents a submesh for a texture mesh.
 */
class TextureSubmesh {
    public:
        GLuint vao;                                // The vertex array object.
        GLuint vbo;                                // The vertex buffer object.
        GLuint ebo;                                // The element buffer object.
        std::vector<TextureMeshVertex> vertices;   // List of vertices for this mesh.
        std::vector<GLuint> indices;               // List of indices to vertices that describe triangles of the mesh.
        GLsizei numIndices;                        // The number of indices to draw. This value is set by @ref GenerateGL.
        TextureSubmeshMaterial material;           // The material for this mesh.

        /**
         * @brief Construct a new submesh for a texture mesh.
         */
        TextureSubmesh(){
            vao = 0;
            vbo = 0;
            ebo = 0;
            numIndices = 0;
        }

        /**
         * @brief Generate GL content and free memory.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            if(vertices.empty() || indices.empty()){
                return false;
            }
            if(!material.Generate()){
                return false;
            }
            DEBUG_GLCHECK( glGenVertexArrays(1, &vao); );
            DEBUG_GLCHECK( glGenBuffers(1, &vbo); );
            DEBUG_GLCHECK( glGenBuffers(1, &ebo); );
            DEBUG_GLCHECK( glBindVertexArray(vao); );
                DEBUG_GLCHECK( glBindBuffer(GL_ARRAY_BUFFER, vbo); );
                DEBUG_GLCHECK( glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TextureMeshVertex), &(vertices[0]), GL_STATIC_DRAW); );
                DEBUG_GLCHECK( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); );
                DEBUG_GLCHECK( glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &(indices[0]), GL_STATIC_DRAW); );
                // Position attribute
                DEBUG_GLCHECK( glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextureMeshVertex), (GLvoid*)0); );
                DEBUG_GLCHECK( glEnableVertexAttribArray(0); );
                // Normals
                DEBUG_GLCHECK( glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TextureMeshVertex), (GLvoid*)offsetof(TextureMeshVertex, normal)); );
                DEBUG_GLCHECK( glEnableVertexAttribArray(1); );
                // Tangent
                DEBUG_GLCHECK( glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TextureMeshVertex), (GLvoid*)offsetof(TextureMeshVertex, tangent)); );
                DEBUG_GLCHECK( glEnableVertexAttribArray(2); );
                // TexCoords
                DEBUG_GLCHECK( glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(TextureMeshVertex), (GLvoid*)offsetof(TextureMeshVertex, texCoord)); );
                DEBUG_GLCHECK( glEnableVertexAttribArray(3); );
            DEBUG_GLCHECK( glBindVertexArray(0); );
            numIndices = static_cast<GLsizei>(indices.size());
            vertices.clear();
            indices.clear();
            return true;
        }

        /**
         * @brief Delete GL content.
         */
        void Delete(void){
            material.Delete();
            if(vao){
                DEBUG_GLCHECK( glDeleteVertexArrays(1, &vao); );
                vao = 0;
            }
            if(vbo){
                DEBUG_GLCHECK( glDeleteBuffers(1, &vbo); );
                vbo = 0;
            }
            if(ebo){
                DEBUG_GLCHECK( glDeleteBuffers(1, &ebo); );
                ebo = 0;
            }
            numIndices = 0;
        }

        /**
         * @brief Apply material uniforms and draw this submesh.
         * @param[in] shader The texture mesh shader to be used to set material properties.
         * @param[in] diffuseColorMultiplier Optional diffuse color multiplier.
         * @param[in] specularColorMultiplier Specular color multiplier.
         * @param[in] emissionColorMultiplier Emission color multiplier.
         * @param[in] shininessMultiplier Shininess multiplier.
         */
        void Draw(const ShaderTextureMesh& shader, glm::vec3 diffuseColorMultiplier, glm::vec3 specularColorMultiplier, glm::vec3 emissionColorMultiplier, GLfloat shininessMultiplier){
            material.Apply(shader, diffuseColorMultiplier, specularColorMultiplier, emissionColorMultiplier, shininessMultiplier);
            DrawWithoutMaterial();
        }

        /**
         * @brief Draw the vertices of this submesh without applying material.
         */
        void DrawWithoutMaterial(void){
            DEBUG_GLCHECK( glBindVertexArray(vao); );
            DEBUG_GLCHECK( glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (GLvoid*)0); );
        }

        /**
         * @brief Draw the vertices of this submesh without applying material but with binding the diffuse map.
         */
        void DrawWithoutMaterialButDiffuseMap(void){
            material.BindDiffuseMap();
            DrawWithoutMaterial();
        }
};

