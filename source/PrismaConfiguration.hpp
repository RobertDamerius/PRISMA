#pragma once


#include <Common.hpp>


class PrismaConfiguration {
    public:
        struct {
            uint32_t width;                                           // Width of the window in pixels.
            uint32_t height;                                          // Height of the window in pixels.
            bool fullscreen;                                          // True if fullscreen should be enabled, false otherwise.
            uint32_t fullscreenMonitorIndex;                          // The index of the monitor to be used in fullscreen mode.
            bool resizable;                                           // True if window should be resizable, false otherwise.
            bool visible;                                             // True if the window should be visible, false otherwise.
            bool decorated;                                           // True if the window should be decorated, false otherwise.
            bool focused;                                             // True if the window should be focused, false otherwise.
            bool maximized;                                           // True if the window should be maximized, false otherwise.
            uint32_t glMajorVersion;                                  // The Open-GL major version to be set for the window context.
            uint32_t glMinorVersion;                                  // The Open-GL minor version to be set for the window context.
            std::string title;                                        // The window title to be set.
            bool showFPSInTitle;                                      // True if FPS should be shown in title.
            bool closeWithEscape;                                     // True if the window should be closed with the escape key of the keyboard.
            bool allowManualCameraControl;                            // True if the camera is allowed to be controlled manually.
        } window;
        struct {
            bool enableWaterReflection;                               // True if water reflection should be enabled, false otherwise.
            bool enableAtmosphericScattering;                         // True if atmospheric scattering should be enabled, false otherwise.
            bool enableShadowMapping;                                 // True if shadow mapping should be enabled, false otherwise.
            bool enableAmbientOcclusion;                              // True if ambient occlusion should be enabled, false otherwise.
            bool enableBloom;                                         // True if bloom should be enabled, false otherwise.
            GLuint maxNumDirectionalLights;                           // Maximum number of directional lights.
            GLuint maxNumPointLights;                                 // Maximum number of point lights.
            GLuint maxNumSpotLights;                                  // Maximum number of spot lights.
            GLuint ssaoKernelSize;                                    // The kernel size for SSAO.
            std::vector<GLfloat> shadowFrustumSplitLevels;            // Normalized frustum split levels for cascaded shadow mapping.
            GLsizei shadowMapResolution;                              // Resolution of the shadow map in pixels.
            GLuint maxNumBloomMips;                                   // The maximum number of bloom mips.
        } engine;
        struct {
            uint16_t localPort;                                       // The local port to be bound.
            std::array<uint8_t,4> interfaceAddress;                   // IPv4 address of the network interface to be used for joining the multicast group.
            std::string interfaceName;                                // Name of the network interface to be used for joining the multicast group. If this string is empty, @ref interfaceAddress is used.
            int32_t socketPriority;                                   // The socket priority (linux only, in range [0 (lowest), 6 (greatest)]).
            uint32_t socketErrorRetryTimeMs;                          // Time in milliseconds to wait in case of an socket error before trying again.
            struct {
                std::vector<std::array<uint8_t,4>> multicastGroups;   // The group addresses to be joined.
            } receive;
            struct {
                std::vector<std::array<uint16_t,5>> destinationAddresses;   // The destination addresses {ipA, ipB, ipC, ipD, port} to which to send messages.
                uint8_t timeToLive;                                         // The time-to-live (TTL) value to be set for multicast messages.
            } send;
        } network;

        /**
         * @brief Construct a new PRISMA configuration and set default values.
         */
        PrismaConfiguration();

        /**
         * @brief Clear the configuration and set default values.
         */
        void Clear(void);

        /**
         * @brief Read the configuration from the configuration file.
         * @param[in] alternativeConfigurationFile The filename of an alternative configuration file. If this string is empty, then the default configuration file is used.
         * @return True if configuration file was read successfully, false otherwise.
         */
        bool ReadFromFile(std::string alternativeConfigurationFile);

        /**
         * @brief Get the original file content of the PRISMA configuration file that has been read via @ref ReadFromFile.
         * @return Text of the configuration file that has been read via @ref ReadFromFile.
         */
        std::string GetOriginalFileContent(void);

    private:
        std::string originalFileContent;
};


extern PrismaConfiguration prismaConfiguration;

