#pragma once


#include <Common.hpp>
#include <MessageBaseType.hpp>
#include <WaterMeshParameter.hpp>
#include <WaterMesh.hpp>
#include <NonConvexPolygon.hpp>
#include <NetworkUtils.hpp>


/**
 * @brief Protocol definition for the water mesh message.
 */
#pragma pack(push, 1)
union WaterMeshMessageUnion {
    struct WaterMeshMessageStruct {
        uint8_t messageType;                      // Message type, must be @ref MESSAGE_TYPE_WATER_MESH.
        uint8_t updateMesh:1;                     // bit0: True if the mesh should be updated, false otherwise.
        uint8_t visible:1;                        // bit1: True if the mesh should be visible, false otherwise.
        uint8_t unused:6;                         // bit2-bit7: unused.
        GLfloat position[3];                      // World space position of the water mesh.
        GLfloat textureCoordinateScaling1;        // Scaling (1) for texture coordinates of the texture maps.
        GLfloat textureCoordinateScaling2;        // Scaling (2) for texture coordinates of the texture maps.
        GLfloat textureCoordinateAnimation1[2];   // Texture coordinate animation time (1) for the texture maps.
        GLfloat textureCoordinateAnimation2[2];   // Texture coordinate animation time (2) for the texture maps.
        GLfloat specularColor[3];                 // The specular color of the water mesh.
        GLfloat shininess;                        // The shininess value of the water mesh.
        GLfloat reflectionColorMultiplier[3];     // Color multiplier for the reflection texture.
        GLfloat refractionColorMultiplier[3];     // Color multiplier for the refraction texture.
        GLfloat clippingPlaneOffset;              // Offset for the clipping plane separating reflection and refraction.
        GLfloat strengthDuDvNormal1[2];           // Strength (1) for the distortion du/dv-texture map and for the normal texture map of the water mesh.
        GLfloat strengthDuDvNormal2[2];           // Strength (2) for the distortion du/dv-texture map and for the normal texture map of the water mesh.
        GLfloat distortionWaterDepthScale;        // Multiply distortion with the scaled water depth to reduce artefacts near the water edge.
        // variable mesh data
    } protocol;
    uint8_t bytes[sizeof(WaterMeshMessageUnion::WaterMeshMessageStruct)];
};
union WaterMeshMessageVertexUnion {
    struct WaterMeshMessageVertexStruct {
        GLfloat x;
        GLfloat y;
    } point;
    uint8_t bytes[sizeof(WaterMeshMessageVertexUnion::WaterMeshMessageVertexStruct)];
};
#pragma pack(pop)


/**
 * @brief Represents the water mesh message.
 */
class WaterMeshMessage: public MessageBaseType {
    public:
        bool updateMesh;                             // True if the mesh should be updated, false otherwise.
        WaterMeshParameter parameter;                // Parameter of the water mesh.
        std::vector<WaterMeshVertex> meshVertices;   // Vertices of the water mesh.
        std::vector<GLuint> meshIndices;             // Indices to that vertices representing the triangles of the water mesh.

        /**
         * @brief Construct a new water mesh message.
         */
        WaterMeshMessage(): MessageBaseType(MESSAGE_TYPE_WATER_MESH){
            updateMesh = true;
        }

        /**
         * @brief Deserialize the message.
         * @param[in] bytes The bytes containing the message.
         * @param[in] length The number of bytes containing the message to be deserialized.
         * @return The number of bytes that have been deserialized.
         */
        int32_t Deserialize(const uint8_t* bytes, int32_t length){
            if(length < static_cast<int32_t>(sizeof(WaterMeshMessageUnion))){
                return 0;
            }
            const WaterMeshMessageUnion* msg = reinterpret_cast<const WaterMeshMessageUnion*>(&bytes[0]);
            if(messageType != msg->protocol.messageType){
                return 0;
            }
            updateMesh                              = static_cast<bool>(msg->protocol.updateMesh);
            parameter.visible                       = static_cast<bool>(msg->protocol.visible);
            parameter.position.x                    = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[0]);
            parameter.position.y                    = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[1]);
            parameter.position.z                    = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.position[2]);
            parameter.textureCoordinateScaling1     = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.textureCoordinateScaling1);
            parameter.textureCoordinateScaling2     = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.textureCoordinateScaling2);
            parameter.textureCoordinateAnimation1.x = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.textureCoordinateAnimation1[0]);
            parameter.textureCoordinateAnimation1.y = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.textureCoordinateAnimation1[1]);
            parameter.textureCoordinateAnimation2.x = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.textureCoordinateAnimation2[0]);
            parameter.textureCoordinateAnimation2.y = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.textureCoordinateAnimation2[1]);
            parameter.specularColor.r               = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.specularColor[0]);
            parameter.specularColor.g               = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.specularColor[1]);
            parameter.specularColor.b               = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.specularColor[2]);
            parameter.shininess                     = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.shininess);
            parameter.reflectionColorMultiplier.r   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.reflectionColorMultiplier[0]);
            parameter.reflectionColorMultiplier.g   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.reflectionColorMultiplier[1]);
            parameter.reflectionColorMultiplier.b   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.reflectionColorMultiplier[2]);
            parameter.refractionColorMultiplier.r   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.refractionColorMultiplier[0]);
            parameter.refractionColorMultiplier.g   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.refractionColorMultiplier[1]);
            parameter.refractionColorMultiplier.b   = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.refractionColorMultiplier[2]);
            parameter.clippingPlaneOffset           = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.clippingPlaneOffset);
            parameter.strengthDuDvNormal1.x         = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.strengthDuDvNormal1[0]);
            parameter.strengthDuDvNormal1.y         = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.strengthDuDvNormal1[1]);
            parameter.strengthDuDvNormal2.x         = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.strengthDuDvNormal2[0]);
            parameter.strengthDuDvNormal2.y         = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.strengthDuDvNormal2[1]);
            parameter.distortionWaterDepthScale     = NetworkUtils::NetworkToNativeByteOrder(msg->protocol.distortionWaterDepthScale);

            // deserialize variable mesh data
            int32_t result = static_cast<int32_t>(sizeof(WaterMeshMessageUnion));
            if((result + 2) > length){
                return 0;
            }
            int32_t numPolygons = static_cast<int32_t>(NetworkUtils::NetworkToNativeByteOrder(*reinterpret_cast<const uint16_t*>(&bytes[result])));
            result += 2;
            meshVertices.clear();
            meshIndices.clear();
            for(int32_t p = 0; p < numPolygons; ++p){
                if((result + 2) > length){
                    return 0;
                }
                int32_t numVertices = static_cast<int32_t>(NetworkUtils::NetworkToNativeByteOrder(*reinterpret_cast<const uint16_t*>(&bytes[result])));
                result += 2;
                if((result + numVertices * sizeof(WaterMeshMessageVertexUnion)) > static_cast<size_t>(length)){
                    return 0;
                }
                NonConvexPolygon polygon;
                polygon.verticesAndHoles.resize(1);
                for(int32_t v = 0; v < numVertices; ++v){
                    const WaterMeshMessageVertexUnion* vertex = reinterpret_cast<const WaterMeshMessageVertexUnion*>(&bytes[result]);
                    polygon.verticesAndHoles[0].push_back({NetworkUtils::NetworkToNativeByteOrder(vertex->point.x), NetworkUtils::NetworkToNativeByteOrder(vertex->point.y)});
                    result += sizeof(WaterMeshMessageVertexUnion);
                }
                polygon.Triangulate(meshVertices, meshIndices);
            }
            return result;
        }

        /**
         * @brief Check if the message is valid.
         * @return True if the message is valid, false otherwise.
         */
        bool IsValid(void){
            bool valid = true;
            valid &= std::isfinite(parameter.position.x);
            valid &= std::isfinite(parameter.position.y);
            valid &= std::isfinite(parameter.position.z);
            valid &= std::isfinite(parameter.textureCoordinateScaling1);
            valid &= std::isfinite(parameter.textureCoordinateScaling2);
            valid &= std::isfinite(parameter.textureCoordinateAnimation1.x);
            valid &= std::isfinite(parameter.textureCoordinateAnimation1.y);
            valid &= std::isfinite(parameter.textureCoordinateAnimation2.x);
            valid &= std::isfinite(parameter.textureCoordinateAnimation2.y);
            valid &= std::isfinite(parameter.specularColor.r);
            valid &= std::isfinite(parameter.specularColor.g);
            valid &= std::isfinite(parameter.specularColor.b);
            valid &= std::isfinite(parameter.shininess);
            valid &= std::isfinite(parameter.reflectionColorMultiplier.r);
            valid &= std::isfinite(parameter.reflectionColorMultiplier.g);
            valid &= std::isfinite(parameter.reflectionColorMultiplier.b);
            valid &= std::isfinite(parameter.refractionColorMultiplier.r);
            valid &= std::isfinite(parameter.refractionColorMultiplier.g);
            valid &= std::isfinite(parameter.refractionColorMultiplier.b);
            valid &= std::isfinite(parameter.clippingPlaneOffset);
            valid &= std::isfinite(parameter.strengthDuDvNormal1.x);
            valid &= std::isfinite(parameter.strengthDuDvNormal1.y);
            valid &= std::isfinite(parameter.strengthDuDvNormal2.x);
            valid &= std::isfinite(parameter.strengthDuDvNormal2.y);
            valid &= std::isfinite(parameter.distortionWaterDepthScale);
            return valid;
        }
};

