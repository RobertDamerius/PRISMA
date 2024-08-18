#pragma once


#include <Common.hpp>


/**
 * @brief Different types of camera modes.
 */
enum CameraMode : uint8_t {
    CAMERA_MODE_PERSPECTIVE = 0x00,
    CAMERA_MODE_ORTHOGRAPHIC = 0x01
};


class CameraProperties {
    public:
        CameraMode mode;                 // The camera mode.
        glm::vec3 position;              // The position in world space coordinates.
        glm::vec4 quaternion;            // The unit quaternion representing the orientation of the camera (stored as x,y,z,w).
        GLfloat clipNear;                // The near clipping plane.
        GLfloat clipFar;                 // The far clipping plane.
        GLfloat orthographicLeft;        // The left border limit for orthographic projection.
        GLfloat orthographicRight;       // The right border limit for orthographic projection.
        GLfloat orthographicBottom;      // The bottom border limit for orthographic projection.
        GLfloat orthographicTop;         // The top border limit for orthographic projection.
        GLfloat perspectiveFieldOfView;  // The field of view angle in radians for perspective projection.

        /**
         * @brief Construct a new camera properties object.
         */
        CameraProperties(){
            mode = CAMERA_MODE_PERSPECTIVE;
            position = glm::vec3(0.0f);
            quaternion = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            clipNear = 0.1f;
            clipFar = 1000.0f;
            orthographicLeft = -1.0f;
            orthographicRight = 1.0f;
            orthographicBottom = -1.0f;
            orthographicTop = 1.0f;
            perspectiveFieldOfView = glm::radians(70.0f);
        }
};


/**
 * @brief Represents a camera with either perspective or orthographic projection.
 */
class Camera: public CameraProperties {
    public:
        /**
         * @brief Construct a new camera object.
         */
        Camera(){
            ubo = 0;
            aspect = 1.0f;
            storedAspect = 1.0f;
        }

        /**
         * @brief Set aspect ratio for perspective mode based on the framebuffer size.
         * @param[in] width Width in pixels.
         * @param[in] height Height in pixels.
         */
        void Resize(GLsizei width, GLsizei height){
            aspect = static_cast<GLfloat>(width) / static_cast<GLfloat>(height);
        }

        /**
         * @brief Get the current aspect ratio of the camera.
         * @return Aspect ratio for perspective projection (independent of the current @ref mode).
         */
        GLfloat GetAspectRatio(void) const {
            return aspect;
        }

        /**
         * @brief Get the view matrix.
         * @return The 4x4 view matrix.
         */
        glm::mat4 GetViewMatrix(void) const {
            glm::mat4 C = CalculateRotationMatrix();
            glm::vec3 R = C[0]; // right
            glm::vec3 U = C[1]; // up
            glm::vec3 B = C[2]; // back
            glm::mat4 V;
            V[0] = glm::vec4(R.x, U.x, B.x, 0.0f);
            V[1] = glm::vec4(R.y, U.y, B.y, 0.0f);
            V[2] = glm::vec4(R.z, U.z, B.z, 0.0f);
            V[3] = glm::vec4(-glm::dot(R, position), -glm::dot(U, position), -glm::dot(B, position), 1.0f);
            return V;
        }

        /**
         * @brief Get the projection matrix only.
         * @return The 4x4 projection matrix depending on the @ref mode.
         */
        glm::mat4 GetProjectionMatrix(void) const {
            glm::mat4 projectionMatrix(1.0f);
            switch(mode){
                case CAMERA_MODE_PERSPECTIVE:
                    projectionMatrix = glm::perspective(perspectiveFieldOfView, aspect, clipNear, clipFar);
                    break;
                case CAMERA_MODE_ORTHOGRAPHIC:
                    projectionMatrix = glm::ortho(orthographicLeft, orthographicRight, orthographicBottom, orthographicTop, clipNear, clipFar);
                    break;
            }
            return projectionMatrix;
        }

        /**
         * @brief Get the projection-view matrix.
         * @return 4x4 projection-view matrix.
         */
        glm::mat4 GetProjectionViewMatrix(void) const {
            return GetProjectionMatrix() * GetViewMatrix();
        }

        /**
         * @brief Get the view direction vector.
         * @return The view direction vector.
         */
        glm::vec3 GetViewDirection(void) const {
            glm::mat4 C = CalculateRotationMatrix();
            return -C[2];
        }

        /**
         * @brief Get the up direction vector.
         * @return The up direction vector.
         */
        glm::vec3 GetUpDirection(void) const {
            glm::mat4 C = CalculateRotationMatrix();
            return C[1];
        }

        /**
         * @brief Get the euler angles according to the Y-X-Z convention.
         * @return Euler angles in radians given as {a_x, a_y, a_z} where a_i denotes the rotation angle around the i-axis.
         * @details The rotation order is Y -> X -> Z.
         */
        glm::vec3 GetEulerAnglesYXZ(void) const {
            glm::mat3 C = CalculateRotationMatrix();
            GLfloat x = 2.0 * std::atan2(std::sqrt(1.0f - C[2][1]), std::sqrt(1.0f + C[2][1])) - 3.14159265358979323846f / 2.0f;
            GLfloat y = -std::atan2(-C[2][0], C[2][2]);
            GLfloat z = -std::atan2(-C[0][1], C[1][1]);
            if(!std::isfinite(x)) x = -3.14159265358979323846f / 2.0f;
            if(!std::isfinite(y)) y = 0.0f;
            if(!std::isfinite(z)) z = 0.0f;
            return glm::vec3(x, y, z);
        }

        /**
         * @brief Set the euler angles according to the Y-X-Z convention.
         * @param[in] Euler angles in radians represented as {a_x, a_y, a_z} where a_i denotes the rotation angle around the i-axis.
         * @details The rotation order is Y -> X -> Z.
         */
        void SetEulerAnglesYXZ(glm::vec3 angles){
            GLfloat xHalf = 0.5f * angles.x;
            GLfloat yHalf = 0.5f * angles.y;
            GLfloat zHalf = 0.5f * angles.z;
            GLfloat cp = std::cos(xHalf);
            GLfloat sp = std::sin(xHalf);
            GLfloat cy = std::cos(yHalf);
            GLfloat sy = std::sin(yHalf);
            GLfloat cr = std::cos(zHalf);
            GLfloat sr = std::sin(zHalf);
            quaternion.w = cr*cp*cy + sr*sp*sy;
            quaternion.z = sr*cp*cy - cr*sp*sy;
            quaternion.x = cr*sp*cy + sr*cp*sy;
            quaternion.y = cr*cp*sy - sr*sp*cy;
        }

        /**
         * @brief Calculate the 3-by-3 rotation matrix.
         * @return The rotation matrix that describes the rotational transformation from the camera view space to the world space.
         */
        glm::mat3 CalculateRotationMatrix(void) const {
            GLfloat q0q0 = quaternion.w * quaternion.w;
            GLfloat q1q1 = quaternion.x * quaternion.x;
            GLfloat q2q2 = quaternion.y * quaternion.y;
            GLfloat q3q3 = quaternion.z * quaternion.z;
            GLfloat q1q2 = quaternion.x * quaternion.y;
            GLfloat q0q3 = quaternion.w * quaternion.z;
            GLfloat q1q3 = quaternion.x * quaternion.z;
            GLfloat q0q2 = quaternion.w * quaternion.y;
            GLfloat q2q3 = quaternion.y * quaternion.z;
            GLfloat q0q1 = quaternion.w * quaternion.x;
            glm::mat3 Cb2n; // column-major order, elements stored as mat[col][row]
            Cb2n[0][0] = q0q0+q1q1-q2q2-q3q3;
            Cb2n[0][1] = q1q2+q1q2+q0q3+q0q3;
            Cb2n[0][2] = q1q3+q1q3-q0q2-q0q2;
            Cb2n[1][0] = q1q2+q1q2-q0q3-q0q3;
            Cb2n[1][1] = q0q0-q1q1+q2q2-q3q3;
            Cb2n[1][2] = q2q3+q2q3+q0q1+q0q1;
            Cb2n[2][0] = q1q3+q1q3+q0q2+q0q2;
            Cb2n[2][1] = q2q3+q2q3-q0q1-q0q1;
            Cb2n[2][2] = q0q0-q1q1-q2q2+q3q3;
            return Cb2n;
        }

        /**
         * @brief Generate the uniform buffer object.
         * @param[in] bindingPoint The bining point for the uniform buffer object. Must be the same in the shader.
         * @details The shader must use the following uniform block:
         * layout (std140, binding = $UBO_CAMERA$) uniform Camera {
         *     mat4 cameraViewMatrix;
         *     mat4 cameraProjectionMatrix;
         *     mat4 cameraInvProjectionMatrix;
         *     vec4 cameraClippingInfo; // x: near, y: far
         * };
         */
        void GenerateUniformBufferObject(GLuint bindingPoint){
            DEBUG_GLCHECK( glGenBuffers(1, &ubo); );
            DEBUG_GLCHECK( glBindBuffer(GL_UNIFORM_BUFFER, ubo); );
            DEBUG_GLCHECK( glBufferData(GL_UNIFORM_BUFFER, 208, nullptr, GL_DYNAMIC_DRAW); ); // 52 floats = 208 bytes
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
         * @brief Update the uniform buffer object (complete uniform block buffer data).
         */
        void UpdateUniformBufferObject(void){
            // calculate matrices
            glm::mat4 matProjection = GetProjectionMatrix();
            glm::mat4 matView = GetViewMatrix();
            glm::mat4 matInvProjection = glm::mat4(glm::inverse(matProjection));

            // update buffer data
            GLfloat buffer[52];
            memcpy(&buffer[0], glm::value_ptr(matView), 64);
            memcpy(&buffer[16], glm::value_ptr(matProjection), 64);
            memcpy(&buffer[32], glm::value_ptr(matInvProjection), 64);
            buffer[48] = clipNear;
            buffer[49] = clipFar;
            buffer[50] = 0.0f;
            buffer[51] = 0.0f;
            DEBUG_GLCHECK( glBindBuffer(GL_UNIFORM_BUFFER, ubo); );
            DEBUG_GLCHECK( glBufferSubData(GL_UNIFORM_BUFFER, 0, 208, &buffer[0]); ); // 52 floats = 208 bytes
        }

        /**
         * @brief Store the current camera properties.
         */
        void Store(void){
            storedProperties.mode = this->mode;
            storedProperties.position = this->position;
            storedProperties.quaternion = this->quaternion;
            storedProperties.clipNear = this->clipNear;
            storedProperties.clipFar = this->clipFar;
            storedProperties.orthographicLeft = this->orthographicLeft;
            storedProperties.orthographicRight = this->orthographicRight;
            storedProperties.orthographicBottom = this->orthographicBottom;
            storedProperties.orthographicTop = this->orthographicTop;
            storedProperties.perspectiveFieldOfView = this->perspectiveFieldOfView;
            storedAspect = this->aspect;
        }

        /**
         * @brief Restore the camera properties that have been stored via @ref Store.
         */
        void Restore(void){
            this->mode = storedProperties.mode;
            this->position = storedProperties.position;
            this->quaternion = storedProperties.quaternion;
            this->clipNear = storedProperties.clipNear;
            this->clipFar = storedProperties.clipFar;
            this->orthographicLeft = storedProperties.orthographicLeft;
            this->orthographicRight = storedProperties.orthographicRight;
            this->orthographicBottom = storedProperties.orthographicBottom;
            this->orthographicTop = storedProperties.orthographicTop;
            this->perspectiveFieldOfView = storedProperties.perspectiveFieldOfView;
            this->aspect = storedAspect;
        }

        /**
         * @brief Mirror the camera at the X-Z-plane.
         * @param[in] yLevel The y level of the X-Z-plane.
         * @details Mirrors the y-position of the camera and inverts the pitch angle.
         */
        void MirrorAtXZPlane(GLfloat yLevel){
            GLfloat camDistanceFromPlane = position.y - yLevel;
            position.y -= 2.0f * camDistanceFromPlane;
            glm::vec3 angles = GetEulerAnglesYXZ();
            angles.x = -angles.x;
            SetEulerAnglesYXZ(angles);
        }

    private:
        GLuint ubo;                          // The uniform buffer object.
        GLfloat aspect;                      // The aspect ratio for perspective projection (width / height).
        GLfloat storedAspect;                // Hold the stored camera aspect.
        CameraProperties storedProperties;   // Hold the stored camera properties.
};

