#pragma once


#include <Common.hpp>
#include <MeshBase.hpp>
#include <TextureSubmesh.hpp>
#include <AABB.hpp>
#include <ShaderTextureMesh.hpp>


/**
 * @brief Represents a mesh that contains textures.
 */
class TextureMesh: public MeshBase {
    public:
        /**
         * @brief Generate all GL content for this mesh and free memory.
         * @return True if success, false otherwise. If this mesh has already been generated, false is returned.
         */
        bool Generate(void);

        /**
         * @brief Delete all GL content for this mesh.
         */
        void Delete(void);

        /**
         * @brief Draw this mesh by applying the correct material uniforms for each submesh.
         * @param[in] shader The texture mesh shader to be used to set material properties.
         * @param[in] diffuseColorMultiplier Diffuse color multiplier.
         * @param[in] specularColorMultiplier Specular color multiplier.
         * @param[in] emissionColorMultiplier Emission color multiplier.
         * @param[in] shininessMultiplier Shininess multiplier.
         */
        void Draw(const ShaderTextureMesh& shader, glm::vec3 diffuseColorMultiplier, glm::vec3 specularColorMultiplier, glm::vec3 emissionColorMultiplier, GLfloat shininessMultiplier);

        /**
         * @brief Draw the vertices of all submeshes without applying material.
         */
        void DrawWithoutMaterial(void);

        /**
         * @brief Draw the vertices of all submeshes without applying material but with binding the diffuse map.
         */
        void DrawWithoutMaterialButDiffuseMap(void);

        /**
         * @brief Get the axis-aligned bounding box for all mesh vertices.
         */
        AABB GetAABBOfVertices(void);

        /**
         * @brief Read mesh data from an obj wavefront file.
         * @param[in] filename Filename of the obj wavefront file.
         * @return True if success, false otherwise.
         */
        bool ReadFromFile(std::string filename);

        /**
         * @brief Check whether at least one diffuse map of this mesh contains at least one alpha value of less than 0xFF.
         * @return True if any alpha value of the diffuse map if less than 0xFF, false otherwise.
         */
        bool IsTransparent(void);

    private:
        std::vector<TextureSubmesh> submeshes;   // Container of submeshes.
        AABB aabbVertices;                       // The axis-aligned bounding box containing all vertices of all @ref submeshes.

        /**
         * @brief Read the material library file.
         * @param[out] materials Output where to append the materials to.
         * @param[in] filename The filename of the mtl file to read.
         * @return True if success, false otherwise.
         */
        bool ReadMaterialLibrary(std::vector<TextureSubmeshMaterial>& materials, std::string filename);

        /**
         * @brief Read the next line from the file.
         * @param[in] file The open file from which to read.
         * @return The line exluding <CR> and <LF>.
         */
        std::string ReadNextLine(std::ifstream& file);

        /**
         * @brief Get the index for a material with a specific name.
         * @param[in] materials The list of materials.
         * @param[in] name The name of the material for which to obtain the index.
         * @return Index to that material in the list that matches the given name or -1 if that material does not exist.
         */
        int64_t GetMaterialIndex(const std::vector<TextureSubmeshMaterial>& materials, std::string name);

        /**
         * @brief Update the @ref aabbVertices based on @ref submeshes.
         */
        void UpdateAABB(void);

        /**
         * @brief Ensure that the normal vector is indeed a normalized vector.
         * @param[inout] nx x component of the normal vector.
         */
        void EnsureCorrectNormal(GLfloat& nx, GLfloat& ny, GLfloat& nz);
};

