#pragma once


#include <Common.hpp>
#include <ShaderWaterMesh.hpp>
#include <ImageTexture2D.hpp>
#include <FileManager.hpp>
#include <WaterMeshParameter.hpp>
#include <WaterMesh.hpp>
#include <WaterMeshMessage.hpp>


class WaterMeshRenderer {
    public:
        /**
         * @brief Generate the water mesh renderer.
         * @return True if success, false otherwise.
         */
        bool Generate(void){
            if(!dudvMap.ReadFromPngFile(FileName(FILENAME_TEXTURE_WATER_DUDVMAP), false)){
                PrintE("Failed to read the du/dv map for water meshes!\n");
                return false;
            }
            if(!normalMap.ReadFromPngFile(FileName(FILENAME_TEXTURE_WATER_NORMALMAP), false)){
                PrintE("Failed to read the normal map for water meshes!\n");
                return false;
            }
            waterMesh.Generate();
            bool success = shaderWaterMesh.Generate();
            success &= dudvMap.Generate(GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true, true, false);
            success &= normalMap.Generate(GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true, true, false);
            if(!success){
                Delete();
            }
            return success;
        }

        /**
         * @brief Delete the water mesh renderer.
         */
        void Delete(void){
            waterMesh.Delete();
            shaderWaterMesh.Delete();
            dudvMap.Delete();
            normalMap.Delete();
        }

        /**
         * @brief Clear the water mesh.
         */
        void Clear(void){
            parameter.Clear();
            waterMesh.ClearMeshData();
        }

        /**
         * @brief Process a water mesh messages.
         * @param[in] message The water mesh message to be processed.
         */
        void ProcessWaterMeshMessage(const WaterMeshMessage& message){
            parameter = message.parameter;
            if(message.updateMesh){
                waterMesh.UpdateMeshData(message.meshVertices, message.meshIndices);
            }
        }

        /**
         * @brief Draw the scene to the G-buffer.
         * @param[in] reflectionTextureID Texture ID of that texture to be used as water reflection texture.
         * @param[in] refractionTextureID Texture ID of that texture to be used as water refraction texture.
         */
        void DrawMesh(GLuint reflectionTextureID, GLuint refractionTextureID){
            if(parameter.visible){
                DEBUG_GLCHECK( glDisable(GL_CULL_FACE); );
                DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE0); );
                DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, reflectionTextureID); );
                DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE1); );
                DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, refractionTextureID); );
                DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE2); );
                dudvMap.BindTexture();
                DEBUG_GLCHECK( glActiveTexture(GL_TEXTURE3); );
                normalMap.BindTexture();
                glm::mat4 modelMatrix(glm::vec4(1,0,0,0), glm::vec4(0,0,-1,0), glm::vec4(0,1,0,0), glm::vec4(parameter.position.x, parameter.position.y, parameter.position.z,1));
                shaderWaterMesh.Use();
                shaderWaterMesh.SetModelMatrix(modelMatrix);
                shaderWaterMesh.SetSpecularColor(parameter.specularColor);
                shaderWaterMesh.SetShininess(parameter.shininess);
                shaderWaterMesh.SetStrengthDuDvNormal(parameter.strengthDuDvNormal1, parameter.strengthDuDvNormal2);
                shaderWaterMesh.SetReflectionColorMultiplier(parameter.reflectionColorMultiplier);
                shaderWaterMesh.SetRefractionColorMultiplier(parameter.refractionColorMultiplier);
                shaderWaterMesh.SetTextureCoordinateScaling(parameter.textureCoordinateScaling1, parameter.textureCoordinateScaling2);
                shaderWaterMesh.SetTextureCoordinateAnimation(parameter.textureCoordinateAnimation1, parameter.textureCoordinateAnimation2);
                shaderWaterMesh.SetDistortionWaterDepthScale(parameter.distortionWaterDepthScale);
                waterMesh.Draw();
            }
        }

        /**
         * @brief Get the water level of the water mesh.
         * @return The y-position of the water mesh.
         */
        GLfloat GetWaterLevel(void) const { return parameter.position.y; }

        /**
         * @brief Get the clipping plane to render everything above the surface.
         * @return The clipping plane.
         */
        glm::vec4 GetClippingPlaneAboveSurface(void) const { return glm::vec4(0.0, 1.0, 0.0, -parameter.position.y + parameter.clippingPlaneOffset); }

        /**
         * @brief Get the clipping plane to render everything below the surface.
         * @return The clipping plane.
         */
        glm::vec4 GetClippingPlaneBelowSurface(void) const { return glm::vec4(0.0, -1.0, 0.0, parameter.position.y + parameter.clippingPlaneOffset); }

    private:
        WaterMeshParameter parameter;      // The parameter of the water mesh.
        WaterMesh waterMesh;               // The actual water mesh.
        ShaderWaterMesh shaderWaterMesh;   // The water mesh shader.
        ImageTexture2D dudvMap;            // The du/dv distortion map for the water mesh.
        ImageTexture2D normalMap;          // The normal map for the water mesh.

        /**
         * @brief Update the water mesh.
         * @param[in] vertices The vertices to be used as the water mesh data.
         * @param[in] indices The array of zero-based indices to vertices representing the triangles of the mesh.
         */
        void UpdateMesh(const std::vector<WaterMeshVertex>& vertices, const std::vector<GLuint>& indices){
            waterMesh.UpdateMeshData(vertices, indices);
        }
};

