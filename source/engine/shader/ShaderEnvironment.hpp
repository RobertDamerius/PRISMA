#pragma once


#include <Common.hpp>
#include <Shader.hpp>
#include <FileManager.hpp>
#include <DirectionalLight.hpp>
#include <PointLight.hpp>
#include <SpotLight.hpp>
#include <PrismaConfiguration.hpp>


class ShaderEnvironment: protected Shader {
    public:
        using Shader::Use;

        /**
         * @brief Construct an environment shader.
         */
        ShaderEnvironment(): locationAmbientLightColor(0), locationNumDirectionalLights(0), locationNumPointLights(0), locationNumSpotLights(0), locationBrightnessThreshold(0), locationNumRayTracingSteps(0), locationAmbientOcclusionEnhance(0), locationShadowBiasMinMax(0), locationRemoveShadowOnWater(0), maxNumDirectionalLights(0), maxNumPointLights(0), maxNumSpotLights(0), numShadowCascades(0) {}

        /**
         * @brief Generate the shader.
         * @param[in] maxNumDirectionalLights The maximum number of directional lights to be used.
         * @param[in] maxNumPointLights The maximum number of point lights to be used.
         * @param[in] maxNumSpotLights The maximum number of spot lights to be used.
         * @param[in] numShadowCascades The number of cascaded shadow maps in use.
         * @return True if success, false otherwise.
         */
        bool Generate(GLuint maxNumDirectionalLights, GLuint maxNumPointLights, GLuint maxNumSpotLights, GLuint numShadowCascades){
            this->numShadowCascades = numShadowCascades;
            this->maxNumDirectionalLights = maxNumDirectionalLights;
            this->maxNumPointLights = maxNumPointLights;
            this->maxNumSpotLights = maxNumSpotLights;
            std::vector<std::pair<std::string, std::string>> replacement;
            std::string configurationMacros = "";
            if(prismaConfiguration.engine.enableAtmosphericScattering){
                configurationMacros += "#define PRISMA_CONFIGURATION_ENABLE_ATMOSPHERIC_SCATTERING\n";
            }
            if(prismaConfiguration.engine.enableShadowMapping){
                configurationMacros += "#define PRISMA_CONFIGURATION_ENABLE_SHADOW_MAPPING\n";
            }
            if(prismaConfiguration.engine.enableAmbientOcclusion){
                configurationMacros += "#define PRISMA_CONFIGURATION_ENABLE_AMBIENT_OCCLUSION\n";
            }
            replacement.push_back(std::pair<std::string, std::string>("$PRISMA_CONFIGURATION_MACROS$", configurationMacros));
            replacement.push_back(std::pair<std::string, std::string>("$UBO_CAMERA$", std::to_string(UBO_CAMERA)));
            replacement.push_back(std::pair<std::string, std::string>("$UBO_LIGHTMATRICES$",std::to_string(UBO_LIGHTMATRICES)));
            replacement.push_back(std::pair<std::string, std::string>("$UBO_ATMOSPHERE$",std::to_string(UBO_ATMOSPHERE)));
            replacement.push_back(std::pair<std::string, std::string>("$SSBO_DIRECTIONALLIGHTS$", std::to_string(SSBO_DIRECTIONALLIGHTS)));
            replacement.push_back(std::pair<std::string, std::string>("$SSBO_POINTLIGHTS$", std::to_string(SSBO_POINTLIGHTS)));
            replacement.push_back(std::pair<std::string, std::string>("$SSBO_SPOTLIGHTS$", std::to_string(SSBO_SPOTLIGHTS)));
            replacement.push_back(std::pair<std::string, std::string>("$MAX_NUMBER_DIRECTIONALLIGHTS$", std::to_string(this->maxNumDirectionalLights)));
            replacement.push_back(std::pair<std::string, std::string>("$MAX_NUMBER_POINTLIGHTS$", std::to_string(this->maxNumPointLights)));
            replacement.push_back(std::pair<std::string, std::string>("$MAX_NUMBER_SPOTLIGHTS$", std::to_string(this->maxNumSpotLights)));
            replacement.push_back(std::pair<std::string, std::string>("$NUMBER_OF_SHADOW_CASCADES$",std::to_string(this->numShadowCascades)));
            if(!Shader::Generate(FileName(FILENAME_SHADER_ENVIRONMENT), replacement)){
                PrintE("Could not generate environment shader!\n");
                return false;
            }

            // get all uniform locations
            Use();
            locationAmbientLightColor = GetUniformLocation("ambientLightColor");
            locationNumDirectionalLights = GetUniformLocation("numDirectionalLights");
            locationNumPointLights = GetUniformLocation("numPointLights");
            locationNumSpotLights = GetUniformLocation("numSpotLights");
            locationBrightnessThreshold = GetUniformLocation("brightnessThreshold");
            locationNumRayTracingSteps = GetUniformLocation("numRayTracingSteps");
            locationAmbientOcclusionEnhance = GetUniformLocation("ambientOcclusionEnhance");
            locationShadowBiasMinMax = GetUniformLocation("shadowBiasMinMax");
            locationRemoveShadowOnWater = GetUniformLocation("removeShadowOnWater");
            locationShadowCascadeFarPlanes.resize(this->numShadowCascades);
            for(GLuint i = 0; i < this->numShadowCascades; ++i){
                locationShadowCascadeFarPlanes[i] = GetUniformLocation("shadowCascadeFarPlanes[" + std::to_string(i) + "]");
            }

            // upload default uniform values
            SetNumDirectionalLights(0);
            SetNumPointLights(0);
            SetNumSpotLights(0);
            SetAmbientLightColor(glm::vec3(0.0f));
            SetNumRayTracingSteps(glm::uvec2(0));
            SetShadowBiasMinMax(glm::vec2(0.0f));
            return true;
        }

        /**
         * @brief Delete the shader.
         */
        void Delete(void){
            Shader::Delete();
            locationAmbientLightColor = 0;
            locationNumDirectionalLights = 0;
            locationNumPointLights = 0;
            locationNumSpotLights = 0;
            locationBrightnessThreshold = 0;
            locationNumRayTracingSteps = 0;
            locationAmbientOcclusionEnhance = 0;
            locationShadowBiasMinMax = 0;
            locationRemoveShadowOnWater = 0;
            maxNumDirectionalLights = 0;
            maxNumPointLights = 0;
            maxNumSpotLights = 0;
            numShadowCascades = 0;
            locationShadowCascadeFarPlanes.clear();
        }

        /**
         * @brief Set the number of directional lights to be rendered.
         * @param numDirectionalLights Number of directional lights.
         */
        void SetNumDirectionalLights(GLuint numDirectionalLights) const {
            Uniform1ui(locationNumDirectionalLights, std::min(numDirectionalLights, maxNumDirectionalLights));
        }

        /**
         * @brief Set the number of point lights to be rendered.
         * @param numPointLights Number of point lights.
         */
        void SetNumPointLights(GLuint numPointLights) const {
            Uniform1ui(locationNumPointLights, std::min(numPointLights, maxNumPointLights));
        }

        /**
         * @brief Set the number of spot lights to be rendered.
         * @param numSpotLights Number of spot lights.
         */
        void SetNumSpotLights(GLuint numSpotLights) const {
            Uniform1ui(locationNumSpotLights, std::min(numSpotLights, maxNumSpotLights));
        }

        /**
         * @brief Set the ambient light color.
         * @param[in] ambientLightColor The ambient light color to be set. 
         */
        void SetAmbientLightColor(glm::vec3 ambientLightColor) const {
            Uniform3f(locationAmbientLightColor, ambientLightColor);
        }

        /**
         * @brief Set the brightness threshold.
         * @param[in] brightnessThreshold The brightness threshold.
         */
        void SetBrightnessThreshold(GLfloat brightnessThreshold) const {
            Uniform1f(locationBrightnessThreshold, brightnessThreshold);
        }

        /**
         * @brief Set the far planes of the shadow cascades.
         * @param[in] farPlanes Far planes to be set.
         */
        void SetShadowCascadeFarPlanes(std::vector<GLfloat> farPlanes) const {
            GLuint N = std::min(static_cast<GLuint>(farPlanes.size()), numShadowCascades);
            for(GLuint i = 0; i < N; ++i){
                Uniform1f(locationShadowCascadeFarPlanes[i], farPlanes[i]);
            }
        }

        /**
         * @brief Set the number of ray tracing steps for the atmospheric scattering calculation.
         * @param[in] numRayTracingSteps Number of primary and secondary ray tracing steps.
         */
        void SetNumRayTracingSteps(glm::uvec2 numRayTracingSteps) const {
            Uniform2ui(locationNumRayTracingSteps, numRayTracingSteps.x, numRayTracingSteps.y);
        }

        /**
         * @brief Set the ambient occlusion enhance value.
         * @param[in] ambientOcclusionEnhance Ambient occlusion enhance value to be set.
         */
        void SetAmbientOcclusionEnhance(GLfloat ambientOcclusionEnhance) const {
            Uniform1f(locationAmbientOcclusionEnhance, ambientOcclusionEnhance);
        }

        /**
         * @brief Set the remove shadow on water value.
         * @param[in] removeShadowOnWater The remove shadow on water value to be set.
         */
        void SetRemoveShadowOnWater(GLfloat removeShadowOnWater) const {
            Uniform1f(locationRemoveShadowOnWater, removeShadowOnWater);
        }

        /**
         * @brief Set the shadow bias min/max values.
         * @param[in] shadowBiasMinMax Shadow bias min/max values to be set.
         */
        void SetShadowBiasMinMax(glm::vec2 shadowBiasMinMax) const {
            Uniform2f(locationShadowBiasMinMax, shadowBiasMinMax);
        }

    protected:
        GLint locationAmbientLightColor;                     // Uniform location for ambientLightColor.
        GLint locationNumDirectionalLights;                  // Uniform location for numDirectionalLights.
        GLint locationNumPointLights;                        // Uniform location for numPointLights.
        GLint locationNumSpotLights;                         // Uniform location for numSpotLights.
        GLint locationBrightnessThreshold;                   // Uniform location for brightnessThreshold.
        GLint locationNumRayTracingSteps;                    // Uniform location for numRayTracingSteps.
        GLint locationAmbientOcclusionEnhance;               // Uniform location for ambientOcclusionEnhance.
        GLint locationShadowBiasMinMax;                      // Uniform location for shadowBiasMinMax.
        GLint locationRemoveShadowOnWater;                   // Uniform location for removeShadowOnWater.
        GLuint maxNumDirectionalLights;                      // The maximum number of directional lights that can be set.
        GLuint maxNumPointLights;                            // The maximum number of point lights that can be set.
        GLuint maxNumSpotLights;                             // The maximum number of spot lights that can be set.
        GLuint numShadowCascades;                            // Number of shadow cascades in use.
        std::vector<GLint> locationShadowCascadeFarPlanes;   // Uniform locations for shadowCascadeFarPlanes[].
};

