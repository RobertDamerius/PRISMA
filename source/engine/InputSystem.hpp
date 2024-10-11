#pragma once


#include <Common.hpp>
#include <Camera.hpp>
#include <PrismaConfiguration.hpp>
#include <EventContainer.hpp>
#include <WindowEventMessage.hpp>


class InputSystem {
    public:
        /**
         * @brief Construct a new input system.
         */
        InputSystem(){
            enableManualCameraControl = false;
            moveForward = false;
            moveReverse = false;
            moveUp = false;
            moveDown = false;
            moveRight = false;
            moveLeft = false;
            integralDeltaCursorPosition = glm::vec2(0.0f);
            previousCursorPosition = glm::vec2(0.0f);
        }

        /**
         * @brief Check whether the manual camera control is enabled or not.
         * @return True if the manual camera control is enabled, false otherwise.
         */
        bool ManualCameraControlEnabled(void){ return enableManualCameraControl; }

        /**
         * @brief Update a given camera based on mouse and keyboard actions if manual camera control is enabled.
         * @param[in] camera The camera to be updated.
         * @param[in] dt The elapsed time to the previous update event in seconds.
         * @return True if the manual camera control mode is enabled and the camera has been update, false otherwise.
         */
        bool UpdateCamera(Camera& camera, double dt){
            if(enableManualCameraControl){
                // camera position
                constexpr GLfloat speed = 10.0f;
                glm::vec3 move = GetNormalizedMoveVector();
                glm::mat3 C = camera.CalculateRotationMatrix();
                glm::vec3 velocity = speed * C * move;
                camera.position += static_cast<GLfloat>(dt) * velocity;

                // camera orientation
                constexpr GLfloat angularSpeed = 0.1f;
                constexpr GLfloat pitchLimit = glm::radians(89.0f);
                glm::vec2 deltaRot = GetDeltaRotationVector();
                glm::vec3 angles = camera.GetEulerAnglesYXZ();
                angles.x = std::clamp(angles.x + static_cast<GLfloat>(dt) * angularSpeed * deltaRot.y, -pitchLimit, pitchLimit);
                angles.y = angles.y - static_cast<GLfloat>(dt) * angularSpeed * deltaRot.x;
                angles.z = 0.0f;
                camera.SetEulerAnglesYXZ(angles);
            }
            return enableManualCameraControl;
        }

        /**
         * @brief The GLFW key callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] key The keyboard key that was pressed or released.
         * @param[in] scancode The system-specific scancode of the key.
         * @param[in] action GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT. Future releases may add more actions.
         * @param[in] mods Bit field describing which modifier keys were held down.
         */
        void CallbackKey(GLFWwindow* wnd, int key, int scancode, int action, int mods){
            eventContainer.keyEvents.push_back(KeyEventData(key, action, mods));
            if(GLFW_KEY_ESCAPE == key){
                if(prismaConfiguration.window.closeWithEscape){
                    glfwSetWindowShouldClose(wnd, GLFW_TRUE);
                }
            }
            else if(GLFW_KEY_F1 == key){
                switch(action){
                    case GLFW_PRESS:
                        enableManualCameraControl = !enableManualCameraControl;
                        if(!enableManualCameraControl){
                            glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        }
                        break;
                }
            }
            else if(GLFW_KEY_W == key){ // forward
                switch(action){
                    case GLFW_PRESS:     moveForward = true;    break;
                    case GLFW_RELEASE:   moveForward = false;   break;
                }
            }
            else if(GLFW_KEY_S == key){ // reverse
                switch(action){
                    case GLFW_PRESS:     moveReverse = true;    break;
                    case GLFW_RELEASE:   moveReverse = false;   break;
                }
            }
            else if(GLFW_KEY_A == key){ // left
                switch(action){
                    case GLFW_PRESS:     moveLeft = true;    break;
                    case GLFW_RELEASE:   moveLeft = false;   break;
                }
            }
            else if(GLFW_KEY_D == key){ // right
                switch(action){
                    case GLFW_PRESS:     moveRight = true;    break;
                    case GLFW_RELEASE:   moveRight = false;   break;
                }
            }
            else if(GLFW_KEY_SPACE == key){ // up
                switch(action){
                    case GLFW_PRESS:     moveUp = true;    break;
                    case GLFW_RELEASE:   moveUp = false;   break;
                }
            }
            else if(GLFW_KEY_LEFT_SHIFT == key){ // down
                switch(action){
                    case GLFW_PRESS:     moveDown = true;    break;
                    case GLFW_RELEASE:   moveDown = false;   break;
                }
            }
            (void)scancode;
        }

        /**
         * @brief The GLFW cursor position callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] xPos The new cursor x-coordinate, relative to the left edge of the content area.
         * @param[in] yPos The new cursor y-coordinate, relative to the top edge of the content area.
         */
        void CallbackCursorPosition(GLFWwindow* wnd, double xPos, double yPos){
            eventContainer.cursorEvents.push_back(CursorEventData(xPos, yPos));
            if(GLFW_CURSOR_DISABLED == glfwGetInputMode(wnd, GLFW_CURSOR)){
                if(firstCapture){
                    firstCapture = false;
                    integralDeltaCursorPosition = glm::vec2(0.0f);
                }
                else{
                    integralDeltaCursorPosition.x += (xPos - previousCursorPosition.x);
                    integralDeltaCursorPosition.y += (yPos - previousCursorPosition.y);
                }
            }
            previousCursorPosition.x = xPos;
            previousCursorPosition.y = yPos;
        }

        /**
         * @brief The GLFW mouse button callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] button The mouse button that was pressed or released.
         * @param[in] action One of GLFW_PRESS or GLFW_RELEASE. Future releases may add more actions.
         * @param[in] mods Bit field describing which modifier keys were held down.
         */
        void CallbackMouseButton(GLFWwindow* wnd, int button, int action, int mods){
            eventContainer.mouseButtonEvents.push_back(MouseButtonEventData(button, action, mods));
            if(enableManualCameraControl && (GLFW_MOUSE_BUTTON_LEFT == button)){
                switch(action){
                    case GLFW_PRESS:
                        glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                        firstCapture = true;
                        break;
                    case GLFW_RELEASE:
                        glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                        break;
                }
            }
        }

        /**
         * @brief The GLFW scroll callback function.
         * @param[in] wnd The window that received the event.
         * @param[in] xoffset The scroll offset along the x-axis.
         * @param[in] yoffset The scroll offset along the y-axis.
         */
        void CallbackScroll(GLFWwindow* wnd, double xoffset, double yoffset){
            eventContainer.scrollEvents.push_back(ScrollEventData(xoffset, yoffset));
            (void)wnd;
        }

        /**
         * @brief Generate a new window event message based on the latest GLFW events. After this operation, the internal @ref eventContainer is cleared.
         * @return The window event message.
         */
        WindowEventMessage GenerateWindowEventMessage(GLFWwindow* wnd){
            WindowEventMessage result;
            int width, height;
            glfwGetFramebufferSize(wnd, &width, &height);
            result.framebufferWidth = static_cast<uint32_t>(width);
            result.framebufferHeight = static_cast<uint32_t>(height);
            result.prismaCameraControlEnabled = enableManualCameraControl;
            result.eventContainer.Swap(eventContainer);
            return result;
        }

    private:
        bool enableManualCameraControl;          // True if manual camera controlling via mouse/keyboard should be enabled.
        bool moveForward;                        // True if moving forward in view space.
        bool moveReverse;                        // True if moving reverse in view space.
        bool moveUp;                             // True if moving up in view space.
        bool moveDown;                           // True if moving down in view space.
        bool moveRight;                          // True if moving right in view space.
        bool moveLeft;                           // True if moving left in view space.
        glm::vec2 integralDeltaCursorPosition;   // The integral delta cursor position.
        glm::vec2 previousCursorPosition;        // The previous cursor position.
        bool firstCapture;                       // True if cursor has been captured by a mouse button event.
        EventContainer eventContainer;           // Stores all callback events.

        /**
         * @brief Get the normalized move vector object.
         * @return The normalized move vector, where the components indicate the amount of movement in view space (x=right, y=up, z=back).
         */
        glm::vec3 GetNormalizedMoveVector(void){
            glm::vec3 moveVector;
            moveVector.x = static_cast<GLfloat>(moveRight) - static_cast<GLfloat>(moveLeft);
            moveVector.y = static_cast<GLfloat>(moveUp) - static_cast<GLfloat>(moveDown);
            moveVector.z = static_cast<GLfloat>(moveReverse) - static_cast<GLfloat>(moveForward);
            GLfloat L = (moveVector.x * moveVector.x) + (moveVector.y * moveVector.y) + (moveVector.z * moveVector.z);
            if(L > std::numeric_limits<GLfloat>::epsilon()){
                moveVector /= std::sqrt(L);
            }
            return moveVector;
        }

        /**
         * @brief Get the delta rotation vector.
         * @return Delta rotation vector that contains delta angles for yaw and pitch.
         */
        glm::vec2 GetDeltaRotationVector(void){
            glm::vec2 result = integralDeltaCursorPosition;
            integralDeltaCursorPosition = glm::vec2(0.0f);
            result.y = -result.y;
            return result;
        }
};

