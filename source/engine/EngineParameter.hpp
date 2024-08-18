#pragma once


#include <Common.hpp>


/**
 * @brief Represents the runtime parameter for the PRISMA engine.
 */
class EngineParameter {
    public:
        GLfloat ambientOcclusionRadius;      // Radius for the ambient occlusion.
        GLfloat ambientOcclusionBias;        // Bias for ambient occlusion.
        GLfloat ambientOcclusionEnhance;     // Enhancement of the ambient occlusion strength.
        GLfloat removeShadowOnWater;         // Amount for removing shadow on water.
        GLfloat bloomBrightnessThreshold;    // Brightness threshold for bloom effects.
        GLfloat bloomStrength;               // Bloom strength during post-processing.
        GLfloat gamma;                       // Gamma value of the monitor.
        GLfloat ditheringNoiseGranularity;   // Noise granularity for the dithering noise during post-processing.
        GLfloat shadowBiasMin;               // Minimum value for shadow bias.
        GLfloat shadowBiasMax;               // Maximum value for shadow bias.
        GLfloat exposure;                    // Exposure value during post-processing.
        GLfloat toneMappingStrength;         // Tone mapping strength during post-processing.


        /**
         * @brief Construct a new PRISMA runtime engine parameter object and set default values.
         */
        EngineParameter(){ Clear(); }

        /**
         * @brief Clear parameters and set default values.
         */
        void Clear(void){
            ambientOcclusionRadius = 2.0f;
            ambientOcclusionBias = 0.025f;
            ambientOcclusionEnhance = 1.0f;
            removeShadowOnWater = 1.0f;
            bloomBrightnessThreshold = 1.0f;
            bloomStrength = 0.1f;
            gamma = 2.2f;
            ditheringNoiseGranularity = 0.5f / 255.0f;
            shadowBiasMin = 0.0001f;
            shadowBiasMax = 0.005f;
            exposure = 1.0f;
            toneMappingStrength = 0.42f;
        }
};

