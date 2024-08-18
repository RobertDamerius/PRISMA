#pragma once


#include <Common.hpp>
#include <MessageBaseType.hpp>
#include <AtmosphereProperties.hpp>
#include <NetworkUtils.hpp>


/**
 * @brief Protocol definition for the atmosphere message.
 */
#pragma pack(push, 1)
union AtmosphereMessageUnion {
    struct AtmosphereMessageStruct {
        uint8_t messageType;                  // Message type, must be @ref MESSAGE_TYPE_ATMOSPHERE.

        /* basic properties */
        GLfloat sunLightDirection[3];         // Sun light direction (normalized) in world-space starting from the sun.
        GLfloat skyColorMix[4];               // Color to be mixed with the atmospheric scattering based sky color.
        GLfloat sunColorMix[4];               // Color to be mixed with the atmospheric scattering based sun color.
        uint8_t useMieForSunColor:1;          // bit0: True if Mie scattering should be used for sun color calculation.
        uint8_t unused:7;                     // bit1-bit7: unused.
        GLfloat sunLightStrength;             // Multiplier for the directional sun light.

        /* Atmospheric Scattering */
        GLfloat eyePosition[3];               // World-space position relative to the center of the planet from where to start the ray-tracing for atmospheric scattering, e.g. vec3(0, 6372000, 0).
        GLfloat sunAtmosphereIntensity;       // Intensity of the sun for atmospheric scattering, e.g. 20.
        GLfloat radiusPlanet;                 // Radius of the planet in meters, e.g. 6371000.
        GLfloat radiusAtmosphere;             // Radius of the atmosphere in meters, e.g. 6471000.
        GLfloat coefficientRayleigh[3];       // Precomputed Rayleigh scattering coefficients for red, green and blue wavelengths in 1/m, e.g. vec3(5.5e-6, 13.0e-6, 22.4e-6).
        GLfloat coefficientMie;               // Precomputed Mie scattering coefficient in 1/m, e.g. 21e-6.
        GLfloat scaleHeightRayleigh;          // Scale height for Rayleigh scattering in meters, e.g. 8000.
        GLfloat scaleHeightMie;               // Scale height for Mie scattering in meters, e.g. 1200.
        GLfloat scatterAmountMie;             // Amount of Mie scattering, usually in range (-1,0,1), e.g. 0.758.
        GLfloat atmosphereExposure;           // Atmosphere exposure value, e.g. 1.
        uint8_t numRayTracingSteps[2];        // Number of ray tracing steps for primary and secondary ray tracing for the atmospheric scattering calculation, e.g. uvec2(16, 8).

        /* Sun Disc */
        GLfloat sunDiscIntensity;             // Intensity of the sun disk, e.g. 42.
        GLfloat cosSunDiscInnerCutOff;        // cos(a1) where a1 is the inner angle where the smooth cut-off of the sun disc begins, e.g. 0.999995670984418.
        GLfloat cosSunDiscOuterCutOff;        // cos(a2) where a2 is the outer angle where the smooth cut-off of the sun disc ends, e.g. 0.999989165999595.

        /* Depth Fog */
        GLfloat fogDepthBegin;                // Depth, where the fog begins.
        GLfloat fogDepthEnd;                  // Depth, where the fog ends.
        GLfloat fogDensityBegin;              // Density of the fog at the beginning.
        GLfloat fogDensityEnd;                // Density of the fog at the end.
        GLfloat depthFogColor[3];             // Color of the depth fog.

        /* Scene Blend-Out */
        GLfloat depthBlendOutDistanceMin;     // Minimum distance of the scene blend out, e.g. the depth where the scene blend out begins.
        GLfloat depthBlendOutDistanceRange;   // Range of the scene blend out distance.
    } protocol;
    uint8_t bytes[sizeof(AtmosphereMessageUnion::AtmosphereMessageStruct)];
};
#pragma pack(pop)


/**
 * @brief Represents the atmosphere message.
 */
class AtmosphereMessage: public MessageBaseType {
    public:
        AtmosphereProperties atmosphereProperties;   // The properties of the atmosphere.

        /**
         * @brief Construct a new atmosphere message.
         */
        AtmosphereMessage(): MessageBaseType(MESSAGE_TYPE_ATMOSPHERE){}

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        int32_t Deserialize(const uint8_t* bytes, int32_t length){
            if(length < static_cast<int32_t>(sizeof(AtmosphereMessageUnion))){
                return 0;
            }
            const AtmosphereMessageUnion* msg = reinterpret_cast<const AtmosphereMessageUnion*>(&bytes[0]);
            if(messageType != msg->protocol.messageType){
                return 0;
            }
            atmosphereProperties.sunLightDirection.x        = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.sunLightDirection[0]);
            atmosphereProperties.sunLightDirection.y        = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.sunLightDirection[1]);
            atmosphereProperties.sunLightDirection.z        = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.sunLightDirection[2]);
            atmosphereProperties.skyColorMix.r              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.skyColorMix[0]);
            atmosphereProperties.skyColorMix.g              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.skyColorMix[1]);
            atmosphereProperties.skyColorMix.b              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.skyColorMix[2]);
            atmosphereProperties.skyColorMix.a              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.skyColorMix[3]);
            atmosphereProperties.sunColorMix.r              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.sunColorMix[0]);
            atmosphereProperties.sunColorMix.g              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.sunColorMix[1]);
            atmosphereProperties.sunColorMix.b              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.sunColorMix[2]);
            atmosphereProperties.sunColorMix.a              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.sunColorMix[3]);
            atmosphereProperties.useMieForSunColor          = static_cast<bool>(msg->protocol.useMieForSunColor);
            atmosphereProperties.sunLightStrength           = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.sunLightStrength);
            atmosphereProperties.eyePosition.x              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.eyePosition[0]);
            atmosphereProperties.eyePosition.y              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.eyePosition[1]);
            atmosphereProperties.eyePosition.z              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.eyePosition[2]);
            atmosphereProperties.sunAtmosphereIntensity     = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.sunAtmosphereIntensity);
            atmosphereProperties.radiusPlanet               = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.radiusPlanet);
            atmosphereProperties.radiusAtmosphere           = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.radiusAtmosphere);
            atmosphereProperties.coefficientRayleigh.x      = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.coefficientRayleigh[0]);
            atmosphereProperties.coefficientRayleigh.y      = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.coefficientRayleigh[1]);
            atmosphereProperties.coefficientRayleigh.z      = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.coefficientRayleigh[2]);
            atmosphereProperties.coefficientMie             = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.coefficientMie);
            atmosphereProperties.scaleHeightRayleigh        = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.scaleHeightRayleigh);
            atmosphereProperties.scaleHeightMie             = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.scaleHeightMie);
            atmosphereProperties.scatterAmountMie           = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.scatterAmountMie);
            atmosphereProperties.atmosphereExposure         = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.atmosphereExposure);
            atmosphereProperties.numRayTracingSteps.x       = static_cast<GLuint>(msg->protocol.numRayTracingSteps[0]);
            atmosphereProperties.numRayTracingSteps.y       = static_cast<GLuint>(msg->protocol.numRayTracingSteps[1]);
            atmosphereProperties.sunDiscIntensity           = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.sunDiscIntensity);
            atmosphereProperties.cosSunDiscInnerCutOff      = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.cosSunDiscInnerCutOff);
            atmosphereProperties.cosSunDiscOuterCutOff      = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.cosSunDiscOuterCutOff);
            atmosphereProperties.fogDepthBegin              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.fogDepthBegin);
            atmosphereProperties.fogDepthEnd                = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.fogDepthEnd);
            atmosphereProperties.fogDensityBegin            = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.fogDensityBegin);
            atmosphereProperties.fogDensityEnd              = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.fogDensityEnd);
            atmosphereProperties.depthFogColor.r            = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.depthFogColor[0]);
            atmosphereProperties.depthFogColor.g            = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.depthFogColor[1]);
            atmosphereProperties.depthFogColor.b            = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.depthFogColor[2]);
            atmosphereProperties.depthBlendOutDistanceMin   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.depthBlendOutDistanceMin);
            atmosphereProperties.depthBlendOutDistanceRange = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.depthBlendOutDistanceRange);
            return static_cast<int32_t>(sizeof(AtmosphereMessageUnion));
        }

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        bool IsValid(void){
            bool valid = true;
            valid &= std::isfinite(atmosphereProperties.sunLightDirection.x);
            valid &= std::isfinite(atmosphereProperties.sunLightDirection.y);
            valid &= std::isfinite(atmosphereProperties.sunLightDirection.z);
            valid &= std::isfinite(atmosphereProperties.skyColorMix.r);
            valid &= std::isfinite(atmosphereProperties.skyColorMix.g);
            valid &= std::isfinite(atmosphereProperties.skyColorMix.b);
            valid &= std::isfinite(atmosphereProperties.skyColorMix.a);
            valid &= std::isfinite(atmosphereProperties.sunColorMix.r);
            valid &= std::isfinite(atmosphereProperties.sunColorMix.g);
            valid &= std::isfinite(atmosphereProperties.sunColorMix.b);
            valid &= std::isfinite(atmosphereProperties.sunColorMix.a);
            valid &= std::isfinite(atmosphereProperties.sunLightStrength);
            valid &= std::isfinite(atmosphereProperties.eyePosition.x);
            valid &= std::isfinite(atmosphereProperties.eyePosition.y);
            valid &= std::isfinite(atmosphereProperties.eyePosition.z);
            valid &= std::isfinite(atmosphereProperties.sunAtmosphereIntensity);
            valid &= std::isfinite(atmosphereProperties.radiusPlanet);
            valid &= std::isfinite(atmosphereProperties.radiusAtmosphere);
            valid &= std::isfinite(atmosphereProperties.coefficientRayleigh.x);
            valid &= std::isfinite(atmosphereProperties.coefficientRayleigh.y);
            valid &= std::isfinite(atmosphereProperties.coefficientRayleigh.z);
            valid &= std::isfinite(atmosphereProperties.coefficientMie);
            valid &= std::isfinite(atmosphereProperties.scaleHeightRayleigh);
            valid &= std::isfinite(atmosphereProperties.scaleHeightMie);
            valid &= std::isfinite(atmosphereProperties.scatterAmountMie);
            valid &= std::isfinite(atmosphereProperties.atmosphereExposure);
            valid &= std::isfinite(atmosphereProperties.sunDiscIntensity);
            valid &= std::isfinite(atmosphereProperties.cosSunDiscInnerCutOff);
            valid &= std::isfinite(atmosphereProperties.cosSunDiscOuterCutOff);
            valid &= std::isfinite(atmosphereProperties.fogDepthBegin);
            valid &= std::isfinite(atmosphereProperties.fogDepthEnd);
            valid &= std::isfinite(atmosphereProperties.fogDensityBegin);
            valid &= std::isfinite(atmosphereProperties.fogDensityEnd);
            valid &= std::isfinite(atmosphereProperties.depthFogColor.r);
            valid &= std::isfinite(atmosphereProperties.depthFogColor.g);
            valid &= std::isfinite(atmosphereProperties.depthFogColor.b);
            valid &= std::isfinite(atmosphereProperties.depthBlendOutDistanceMin);
            valid &= std::isfinite(atmosphereProperties.depthBlendOutDistanceRange);
            return valid;
        }
};

