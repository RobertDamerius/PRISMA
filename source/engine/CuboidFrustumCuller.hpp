#pragma once


#include <Common.hpp>
#include <AABB.hpp>


/**
 * @brief This class is used to check whether a cuboid is visible by the cameras frustum or not.
 * @note The cuboid must be aligned to the world space axes.
 */
class CuboidFrustumCuller {
    public:
        /**
         * @brief Create a cuboid frustum culler using the cameras projection-view matrix.
         * @param[in] cameraProjectionViewMatrix The projection-view matrix of the camera.
         */
        explicit CuboidFrustumCuller(const glm::mat4& cameraProjectionViewMatrix){
            const GLfloat *pMat = glm::value_ptr(cameraProjectionViewMatrix);
            cullInfo[0][0] = pMat[3] + pMat[1];
            cullInfo[0][1] = pMat[7] + pMat[5];
            cullInfo[0][2] = pMat[11] + pMat[9];
            cullInfo[0][3] = -(pMat[15] + pMat[13]);
            cullInfo[1][0] = pMat[3] - pMat[1];
            cullInfo[1][1] = pMat[7] - pMat[5];
            cullInfo[1][2] = pMat[11] - pMat[9];
            cullInfo[1][3] = pMat[13] - pMat[15];
            cullInfo[2][0] = pMat[3] + pMat[0];
            cullInfo[2][1] = pMat[7] + pMat[4];
            cullInfo[2][2] = pMat[11] + pMat[8];
            cullInfo[2][3] = -(pMat[15] + pMat[12]);
            cullInfo[3][0] = pMat[3] - pMat[0];
            cullInfo[3][1] = pMat[7] - pMat[4];
            cullInfo[3][2] = pMat[11] - pMat[8];
            cullInfo[3][3] = pMat[12] - pMat[15];
            cullInfo[4][0] = pMat[3] - pMat[2];
            cullInfo[4][1] = pMat[7] - pMat[6];
            cullInfo[4][2] = pMat[11] - pMat[10];
            cullInfo[4][3] = pMat[14] - pMat[15];
            cullInfo[5][0] = pMat[3];
            cullInfo[5][1] = pMat[7];
            cullInfo[5][2] = pMat[11];
            cullInfo[5][3] = -pMat[15];
        }

        /**
         * @brief Check whether a cuboid is visible or not.
         * @param[in] cuboidLowestPosition The lowest position of the cuboid.
         * @param[in] cuboidDimension The dimension of the cuboid.
         * @return True if cuboid is visible, false otherwise.
         */
        bool IsVisible(glm::vec3 cuboidLowestPosition, glm::vec3 cuboidDimension){
            // Check all six planes
            for(unsigned plane = 0; plane != 6; plane++){
                if(cullInfo[plane][0] * (cullInfo[plane][0] > 0.0f ? cuboidLowestPosition[0] + cuboidDimension.x : cuboidLowestPosition[0]) + cullInfo[plane][1] * (cullInfo[plane][1] > 0.0f ? cuboidLowestPosition[1] + cuboidDimension.y : cuboidLowestPosition[1]) + cullInfo[plane][2] * (cullInfo[plane][2] > 0.0f ? cuboidLowestPosition[2] + cuboidDimension.z : cuboidLowestPosition[2]) < cullInfo[plane][3]){
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Check whether a cuboid is visible or not.
         * @param[in] aabb The axis-aligned bounding box that represents to cuboid to be considered.
         * @return True if cuboid is visible, false otherwise.
         */
        bool IsVisible(AABB aabb){
            return IsVisible(aabb.lowestPosition, aabb.dimension);
        }

    private:
        GLfloat cullInfo[6][4];   // Culling information, assigned at construction.
};

