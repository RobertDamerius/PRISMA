#pragma once


#include <Common.hpp>
#include <AtmosphereProperties.hpp>
#include <VectorUtils.hpp>
#include <Camera.hpp>
#include <AtmosphereMessage.hpp>


/**
 * @brief Represents the atmosphere based on Rayleigh and Mie scattering.
 */
class Atmosphere {
    public:
        /**
         * @brief Construct a new atmosphere object.
         * @details All parameters are set to default values.
         */
        Atmosphere(){
            properties.Clear();
            ubo = 0;
        }

        /**
         * @brief Generate the atmosphere.
         * @param[in] bindingPoint The binding point to be used for atmosphere parameters.
         */
        void Generate(GLuint bindingPoint){
            GenerateUniformBufferObject(bindingPoint);
        }

        /**
         * @brief Delete the atmosphere.
         */
        void Delete(void){
            DeleteUniformBufferObject();
        }

        /**
         * @brief Clear the atmosphere and set all properties to default values.
         */
        void Clear(void){
            properties.Clear();
        }

        /**
         * @brief Apply the atmosphere to the environment shader.
         * @param[in] shader The environment shader.
         * @param[in] camera The view camera.
         */
        template <class T> void Apply(const T& shader, const Camera& camera){
            UpdateUniformBufferObject(camera.GetViewMatrix());
            shader.SetNumRayTracingSteps(properties.numRayTracingSteps);
        }

        /**
         * @brief Process an atmosphere message.
         * @param[in] message The atmosphere message to be processed.
         */
        void ProcessAtmosphereMessage(const AtmosphereMessage& message){
            properties = message.atmosphereProperties;
        }

        /**
         * @brief Get the sun light direction.
         * @return The sun light direction starting from the light source.
         */
        glm::vec3 GetSunLightDirection(void){
            return properties.sunLightDirection;
        }

    private:
        GLuint ubo;                        // Uniform buffer object for atmosphere data.
        AtmosphereProperties properties;   // The atmosphere properties.

        /**
         * @brief Update the uniform buffer object.
         * @param[in] cameraViewMatrix The view matrix of the viewing camera.
         * @details This member function normalizes the @ref sunLightDirection and calculates the sun color based on atmospheric scattering and @ref sunColorMix.
         */
        void UpdateUniformBufferObject(glm::mat4 cameraViewMatrix){
            // ensure correctly normalized sun light direction
            properties.sunLightDirection = VectorUtils::Normalize(properties.sunLightDirection, glm::vec3(0.0f, -1.0f, 0.0f));

            // transform to view space
            glm::vec3 sunDirection = glm::mat3(cameraViewMatrix) * properties.sunLightDirection;

            // calculate the final sun color
            glm::vec3 sunColor;
            if(properties.useMieForSunColor){
                sunColor = CalculateSunColorIncludingMie();
            }
            else{
                sunColor = CalculateSunColor();
            }
            sunColor = glm::min(glm::vec3(1.0f), sunColor * GetSunHorizonScale());
            sunColor = glm::mix(sunColor, glm::vec3(properties.sunColorMix), properties.sunColorMix.a);
            sunColor *= properties.sunLightStrength;

            // update buffer data
            GLfloat buffer[40];
            buffer[0] = properties.sunLightDirection.x;
            buffer[1] = properties.sunLightDirection.y;
            buffer[2] = properties.sunLightDirection.z;
            buffer[3] = properties.sunDiscIntensity;
            buffer[4] = sunDirection.x;
            buffer[5] = sunDirection.y;
            buffer[6] = sunDirection.z;
            buffer[7] = sunColor.r;
            buffer[8] = sunColor.g;
            buffer[9] = sunColor.b;
            buffer[10] = properties.skyColorMix.r;
            buffer[11] = properties.skyColorMix.g;
            buffer[12] = properties.skyColorMix.b;
            buffer[13] = properties.skyColorMix.a;
            buffer[14] = properties.eyePosition.x;
            buffer[15] = properties.eyePosition.y;
            buffer[16] = properties.eyePosition.z;
            buffer[17] = properties.sunAtmosphereIntensity;
            buffer[18] = properties.radiusPlanet;
            buffer[19] = properties.radiusAtmosphere;
            buffer[20] = properties.coefficientRayleigh.x;
            buffer[21] = properties.coefficientRayleigh.y;
            buffer[22] = properties.coefficientRayleigh.z;
            buffer[23] = properties.coefficientMie;
            buffer[24] = properties.scaleHeightRayleigh;
            buffer[25] = properties.scaleHeightMie;
            buffer[26] = properties.scatterAmountMie;
            buffer[27] = properties.atmosphereExposure;
            buffer[28] = properties.cosSunDiscInnerCutOff;
            buffer[29] = properties.cosSunDiscOuterCutOff;
            buffer[30] = properties.fogDepthBegin;
            buffer[31] = properties.fogDepthEnd;
            buffer[32] = properties.fogDensityBegin;
            buffer[33] = properties.fogDensityEnd;
            buffer[34] = properties.depthFogColor.r;
            buffer[35] = properties.depthFogColor.g;
            buffer[36] = properties.depthFogColor.b;
            buffer[37] = properties.depthBlendOutDistanceMin;
            buffer[38] = properties.depthBlendOutDistanceRange;
            buffer[39] = 0.0f;
            DEBUG_GLCHECK( glBindBuffer(GL_UNIFORM_BUFFER, ubo); );
            DEBUG_GLCHECK( glBufferSubData(GL_UNIFORM_BUFFER, 0, 160, &buffer[0]); );
        }

        /**
         * @brief Generate the uniform buffer object.
         * @param[in] bindingPoint The binding point to be used for the uniform buffer object.
         */
        void GenerateUniformBufferObject(GLuint bindingPoint){
            DEBUG_GLCHECK( glGenBuffers(1, &ubo); );
            DEBUG_GLCHECK( glBindBuffer(GL_UNIFORM_BUFFER, ubo); );
            DEBUG_GLCHECK( glBufferData(GL_UNIFORM_BUFFER, 160, nullptr, GL_DYNAMIC_DRAW); ); // 40 floats = 160 bytes
            DEBUG_GLCHECK( glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo); );
        }

        /**
         * @brief Delete the uniform buffer object.
         */
        void DeleteUniformBufferObject(void){
            if(ubo){
                DEBUG_GLCHECK( glDeleteBuffers(1, &ubo); );
                ubo = 0;
            }
        }

        /**
         * @brief Calculate the sun color based on atmospheric scattering (Rayleigh scattering only).
         * @return The sun color.
         */
        glm::vec3 CalculateSunColor(void){
            // phase function
            constexpr GLfloat phaseRayleigh = 3.0f / (8.0f * 3.14159265358979323f);

            // find ray intersection in atmosphere
            glm::vec3 directionToSun = -properties.sunLightDirection;
            glm::vec2 intersectAtmosphere = RaySphereIntersection(properties.eyePosition, directionToSun, properties.radiusAtmosphere);
            if(intersectAtmosphere.y <= 0.0f){
                return glm::vec3(phaseRayleigh * properties.sunAtmosphereIntensity);
            }
            GLfloat intersectBegin = std::max(0.0f, intersectAtmosphere.x);
            GLfloat intersectEnd = intersectAtmosphere.y;

            // calculate step size
            constexpr uint32_t numSteps = 32;
            GLfloat stepSize = (intersectEnd - intersectBegin) / static_cast<GLfloat>(numSteps);

            // ray tracing through atmosphere
            GLfloat rayTime = 0.0;
            GLfloat opticalDepthRayleigh = 0.0f;
            for(uint32_t k = 0; k < numSteps; ++k){
                glm::vec3 p = properties.eyePosition + directionToSun * (rayTime + stepSize * 0.5f);
                GLfloat altitude = glm::length(p) - properties.radiusPlanet;
                opticalDepthRayleigh += std::exp(-altitude / properties.scaleHeightRayleigh) * stepSize;
                rayTime += stepSize;
            }
            glm::vec3 attenuation = glm::exp(-properties.coefficientRayleigh * opticalDepthRayleigh);
            return attenuation * (phaseRayleigh * properties.sunAtmosphereIntensity);
        }

        /**
         * @brief Calculate the sun color including the Mie scattering.
         * @return The sun color.
         */
        glm::vec3 CalculateSunColorIncludingMie(void){
            // phase functions
            GLfloat gg = properties.scatterAmountMie * properties.scatterAmountMie;
            constexpr GLfloat phaseRayleigh = 3.0f / (8.0f * 3.14159265358979323f);
            GLfloat phaseMie = phaseRayleigh * ((1.0f - gg) * 2.0f) / (std::pow(1.0f + gg - 2.0f * properties.scatterAmountMie, 1.5f) * (2.0f + gg));

            // find ray intersection in atmosphere
            glm::vec3 directionToSun = -properties.sunLightDirection;
            glm::vec2 intersectAtmosphere = RaySphereIntersection(properties.eyePosition, directionToSun, properties.radiusAtmosphere);
            if(intersectAtmosphere.y <= 0.0f){
                return glm::vec3((phaseRayleigh + phaseMie) * properties.sunAtmosphereIntensity);
            }
            GLfloat intersectBegin = std::max(0.0f, intersectAtmosphere.x);
            GLfloat intersectEnd = intersectAtmosphere.y;

            // calculate step size
            constexpr uint32_t numSteps = 20;
            GLfloat stepSize = (intersectEnd - intersectBegin) / static_cast<GLfloat>(numSteps);

            // ray tracing through atmosphere
            GLfloat rayTime = 0.0f;
            GLfloat opticalDepthRayleigh = 0.0f;
            GLfloat opticalDepthMie = 0.0f;
            for(uint32_t k = 0; k < numSteps; ++k){
                glm::vec3 p = properties.eyePosition + directionToSun * (rayTime + stepSize * 0.5f);
                GLfloat altitude = glm::length(p) - properties.radiusPlanet;
                opticalDepthRayleigh += std::exp(-altitude / properties.scaleHeightRayleigh) * stepSize;
                opticalDepthMie += std::exp(-altitude / properties.scaleHeightMie) * stepSize;
                rayTime += stepSize;
            }
            glm::vec3 attenuation = glm::exp(-properties.coefficientRayleigh * opticalDepthRayleigh - properties.coefficientMie * opticalDepthMie);
            return attenuation * ((phaseRayleigh + phaseMie) * properties.sunAtmosphereIntensity);
        }

        /**
         * @brief Get the scale factor for the sun at the horizon.
         * @return Scale factor in range [0, 1] where 0 means no sun and 1 means full sun.
         */
        GLfloat GetSunHorizonScale(void){
            glm::vec3 directionToSun = -properties.sunLightDirection;
            glm::vec3 planetTangent = GetPlanetTangentDirection(directionToSun);
            GLfloat L = glm::length(properties.eyePosition);
            glm::vec3 directionToPlanet(0.0);
            if(L > std::numeric_limits<GLfloat>::epsilon()){
                directionToPlanet = -properties.eyePosition / L;
            }
            GLfloat outerCutOff = std::acos(properties.cosSunDiscOuterCutOff);
            GLfloat cosTheta = glm::dot(planetTangent, directionToPlanet);
            GLfloat c1 = std::cos(std::acos(cosTheta) + outerCutOff);
            GLfloat c2 = std::cos(std::acos(cosTheta) - outerCutOff);
            GLfloat c = glm::dot(directionToSun, directionToPlanet);
            return glm::smoothstep(c2, c1, c);
        }

        /**
         * @brief Get the normalized direction vector to the tangent of the planet.
         * @param[in] directionToSun Normalized direction vector to the sun (world-space).
         * @return Tangent vector (world-space).
         */
        glm::vec3 GetPlanetTangentDirection(glm::vec3 directionToSun){
            glm::vec3 planetTangent(0.0f);
            GLfloat rayLen = glm::length(properties.eyePosition);
            if(rayLen > properties.radiusPlanet){
                glm::vec3 directionToPlanet = -properties.eyePosition / rayLen;
                glm::vec3 a = glm::cross(directionToPlanet, directionToSun);
                GLfloat L = glm::length(a);
                if(L < 0.001f){
                    directionToSun = glm::vec3(-directionToPlanet.y, directionToPlanet.x, directionToPlanet.z);
                    a = glm::cross(directionToPlanet, directionToSun);
                    L = glm::length(a);
                    if(L < 0.001f){
                        directionToSun = glm::vec3(directionToPlanet.z, directionToPlanet.y, -directionToPlanet.x);
                        a = glm::cross(directionToPlanet, directionToSun);
                        L = glm::length(a);
                    }
                }
                a /= L;
                glm::vec3 r = glm::cross(a, directionToPlanet);
                glm::mat3 M(directionToPlanet, r, a);
                glm::vec3 f(0.0f);
                f.y = properties.radiusPlanet / rayLen;
                f.x = std::sqrt(1.0f - f.y * f.y);
                planetTangent = M * f;
            }
            return planetTangent;
        }

        /**
         * @brief Calculate the intersection of a ray with a sphere.
         * @param[in] p Position of the ray origin relative to the center of the sphere.
         * @param[in] dir The normalized direction vector of the ray.
         * @param[in] r Radius of the sphere.
         * @return Two ray length values (L1, L2) where the ray "p + L*dir" intersects with the sphere.
         * If an intersection point exists L2 >= L1. If no intersection exists, L1 > L2.
         */
        glm::vec2 RaySphereIntersection(glm::vec3 p, glm::vec3 dir, GLfloat r){
            GLfloat b = glm::dot(p, dir);
            GLfloat d = r * r + b * b - glm::dot(p, p);
            glm::vec2 result = glm::vec2(1.0e5f, -1.0e5f);
            if(d >= 0.0f){
                GLfloat ds = std::sqrt(d);
                result.x = -ds - b;
                result.y = ds - b;
            }
            return result;
        }
};

