#pragma once


#include <Common.hpp>
#include <MessageBaseType.hpp>
#include <EngineParameter.hpp>
#include <NetworkUtils.hpp>


/**
 * @brief Protocol definition for the engine parameter message.
 */
#pragma pack(push, 1)
union EngineParameterMessageUnion {
    struct EngineParameterMessageStruct {
        uint8_t messageType;                 // Message type, must be @ref MESSAGE_TYPE_ENGINE_PARAMETER.
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
    } protocol;
    uint8_t bytes[sizeof(EngineParameterMessageUnion::EngineParameterMessageStruct)];
};
#pragma pack(pop)


/**
 * @brief Represents the engine parameter message.
 */
class EngineParameterMessage: public MessageBaseType {
    public:
        EngineParameter engineParameter;   // Runtime parameter for the PRISMA engine.

        /**
         * @brief Construct a new engine parameter message.
         */
        EngineParameterMessage(): MessageBaseType(MESSAGE_TYPE_ENGINE_PARAMETER){}

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        int32_t Deserialize(const uint8_t* bytes, int32_t length){
            if(length < static_cast<int32_t>(sizeof(EngineParameterMessageUnion))){
                return 0;
            }
            const EngineParameterMessageUnion* msg = reinterpret_cast<const EngineParameterMessageUnion*>(&bytes[0]);
            if(messageType != msg->protocol.messageType){
                return 0;
            }
            engineParameter.ambientOcclusionRadius    = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.ambientOcclusionRadius);
            engineParameter.ambientOcclusionBias      = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.ambientOcclusionBias);
            engineParameter.ambientOcclusionEnhance   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.ambientOcclusionEnhance);
            engineParameter.removeShadowOnWater       = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.removeShadowOnWater);
            engineParameter.bloomBrightnessThreshold  = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.bloomBrightnessThreshold);
            engineParameter.bloomStrength             = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.bloomStrength);
            engineParameter.gamma                     = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.gamma);
            engineParameter.ditheringNoiseGranularity = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.ditheringNoiseGranularity);
            engineParameter.shadowBiasMin             = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.shadowBiasMin);
            engineParameter.shadowBiasMax             = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.shadowBiasMax);
            engineParameter.exposure                  = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.exposure);
            engineParameter.toneMappingStrength       = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.toneMappingStrength);
            return static_cast<int32_t>(sizeof(EngineParameterMessageUnion));
        }

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        bool IsValid(void){
            bool valid = true;
            valid &= std::isfinite(engineParameter.ambientOcclusionRadius);
            valid &= std::isfinite(engineParameter.ambientOcclusionBias);
            valid &= std::isfinite(engineParameter.ambientOcclusionEnhance);
            valid &= std::isfinite(engineParameter.removeShadowOnWater);
            valid &= std::isfinite(engineParameter.bloomBrightnessThreshold);
            valid &= std::isfinite(engineParameter.bloomStrength);
            valid &= std::isfinite(engineParameter.gamma);
            valid &= std::isfinite(engineParameter.ditheringNoiseGranularity);
            valid &= std::isfinite(engineParameter.shadowBiasMin);
            valid &= std::isfinite(engineParameter.shadowBiasMax);
            valid &= std::isfinite(engineParameter.exposure);
            valid &= std::isfinite(engineParameter.toneMappingStrength);
            return valid;
        }
};

