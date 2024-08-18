#pragma once


#include <Common.hpp>


/**
 * @brief Represents the properties of the atmosphere.
 */
class AtmosphereProperties {
    public:
        /* basic properties */
        glm::vec3 sunLightDirection;          // Sun light direction (normalized) in world-space starting from the sun.
        glm::vec4 skyColorMix;                // Color to be mixed with the atmospheric scattering based sky color.
        glm::vec4 sunColorMix;                // Color to be mixed with the atmospheric scattering based sun color.
        bool useMieForSunColor;               // True if Mie scattering should be used for sun color calculation.
        GLfloat sunLightStrength;             // Multiplier for the directional sun light.

        /* Atmospheric Scattering */
        glm::vec3 eyePosition;                // World-space position relative to the center of the planet from where to start the ray-tracing for atmospheric scattering, e.g. vec3(0, 6372000, 0).
        GLfloat sunAtmosphereIntensity;       // Intensity of the sun for atmospheric scattering, e.g. 20.
        GLfloat radiusPlanet;                 // Radius of the planet in meters, e.g. 6371000.
        GLfloat radiusAtmosphere;             // Radius of the atmosphere in meters, e.g. 6471000.
        glm::vec3 coefficientRayleigh;        // Precomputed Rayleigh scattering coefficients for red, green and blue wavelengths in 1/m, e.g. vec3(5.5e-6, 13.0e-6, 22.4e-6).
        GLfloat coefficientMie;               // Precomputed Mie scattering coefficient in 1/m, e.g. 21e-6.
        GLfloat scaleHeightRayleigh;          // Scale height for Rayleigh scattering in meters, e.g. 8000.
        GLfloat scaleHeightMie;               // Scale height for Mie scattering in meters, e.g. 1200.
        GLfloat scatterAmountMie;             // Amount of Mie scattering, usually in range (-1,0,1), e.g. 0.758.
        GLfloat atmosphereExposure;           // Atmosphere exposure value, e.g. 1.
        glm::uvec2 numRayTracingSteps;        // Number of ray tracing steps for primary and secondary ray tracing for the atmospheric scattering calculation, e.g. uvec2(16, 8).

        /* Sun Disc */
        GLfloat sunDiscIntensity;             // Intensity of the sun disk, e.g. 42.
        GLfloat cosSunDiscInnerCutOff;        // cos(a1) where a1 is the inner angle where the smooth cut-off of the sun disc begins, e.g. 0.999995670984418.
        GLfloat cosSunDiscOuterCutOff;        // cos(a2) where a2 is the outer angle where the smooth cut-off of the sun disc ends, e.g. 0.999989165999595.

        /* Depth Fog */
        GLfloat fogDepthBegin;                // Depth, where the fog begins.
        GLfloat fogDepthEnd;                  // Depth, where the fog ends.
        GLfloat fogDensityBegin;              // Density of the fog at the beginning.
        GLfloat fogDensityEnd;                // Density of the fog at the end.
        glm::vec3 depthFogColor;              // Color of the depth fog.

        /* Scene Blend-Out */
        GLfloat depthBlendOutDistanceMin;     // Minimum distance of the scene blend out, e.g. the depth where the scene blend out begins.
        GLfloat depthBlendOutDistanceRange;   // Range of the scene blend out distance.

        /**
         * @brief Construct a new atmosphere properties object and set default values.
         */
        AtmosphereProperties(){ Clear(); }

        /**
         * @brief Clear the atmosphere and set default values.
         */
        void Clear(void){
            sunLightDirection = glm::vec3(0.0f, -1.0f, 0.0f);
            skyColorMix = glm::vec4(0.01f, 0.01f, 0.01f, 1.0f);
            sunColorMix = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            useMieForSunColor = false;
            sunLightStrength = 1.0f;
            eyePosition = glm::vec3(0.0f, 6372000.0f, 0.0f);
            sunAtmosphereIntensity = 20.0f;
            radiusPlanet = 6371000.0f;
            radiusAtmosphere = 6471000.0f;
            coefficientRayleigh = glm::vec3(5.5e-6f, 13.0e-6f, 22.4e-6f);
            coefficientMie = 21.0e-6f;
            scaleHeightRayleigh = 8000.0f;
            scaleHeightMie = 1200.0f;
            scatterAmountMie = 0.758f;
            atmosphereExposure = 1.0f;
            sunDiscIntensity = 42.0f;
            cosSunDiscInnerCutOff = 0.999995670984418f;
            cosSunDiscOuterCutOff = 0.999989165999595f;
            numRayTracingSteps = glm::uvec2(16, 8);
            fogDepthBegin = 100000.0f;
            fogDepthEnd = 100000.0f;
            fogDensityBegin = 0.0f;
            fogDensityEnd = 0.0f;
            depthFogColor = glm::vec3(0.0f);
            depthBlendOutDistanceMin = 100000.0f;
            depthBlendOutDistanceRange = 0.0f;
        }
};

