#pragma once


#include <Common.hpp>


class ImageTexture2D {
    public:
        /**
         * @brief Construct a new 2D image texture and set default data.
         * @details The default image data represents a 1 pixel image with RGB = {0,0,0}.
         */
        ImageTexture2D();

        /**
         * @brief Set default data for the 2D texture by specifying the color of one pixel.
         * @param[in] pixelColorRGBA The red, green, blue, alpha values for the pixel color.
         * @details The default texture has one pixel.
         */
        void SetDefaultDataRGBA(std::array<uint8_t,4> pixelColorRGBA);

        /**
         * @brief Set default data for the 2D texture by specifying the color of one pixel.
         * @param[in] pixelColorRGB The red, green, blue values for the pixel color.
         * @details The default texture has one pixel.
         */
        void SetDefaultDataRGB(std::array<uint8_t,3> pixelColorRGB);

        /**
         * @brief Read the image data from a png file.
         * @param[in] filename The png file from which to read the data.
         * @param[in] useAlpha True if pixel format should contain alpha, false otherwise.
         * @return True if success, false otherwise.
         * @details If this function fails, the image data represents a 1 pixel image with either RGB = {0,0,0} or RGBA = {0,0,0,0} depending on the useAlpha parameter.
         */
        bool ReadFromPngFile(std::string filename, bool useAlpha);

        /**
         * @brief Generate the GL content and free memory (clear image data).
         * @param[in] textureWrapping Texture wrapping for s- and t-coordinates, e.g. GL_REPEAT.
         * @param[in] textureMinFilter Texture filtering for GL_TEXTURE_MIN_FILTER, e.g. GL_LINEAR_MIPMAP_LINEAR.
         * @param[in] textureMaxFilter Texture filtering for GL_TEXTURE_MAG_FILTER, e.g. GL_LINEAR.
         * @param[in] generateMipmap True if mipmaps should be generated.
         * @param[in] useAnisotropy True if anisotropic filtering should be enabled.
         * @param[in] useSRGB True if SRGB or SRGB_ALPHA should be used as internal format. Usually, set this to true for diffuse maps.
         * @return True if success, false otherwise.
         */
        bool Generate(GLenum textureWrapping, GLenum textureMinFilter, GLenum textureMaxFilter, bool generateMipmap, bool useAnisotropy, bool useSRGB);

        /**
         * @brief Delete all GL content.
         */
        void Delete(void);

        /**
         * @brief Bind the texture.
         */
        void BindTexture(void);

        /**
         * @brief Check whether the image data contains at least one pixel with an alpha value of less than 0xFF.
         * @return True if at least one pixel contains an alpha value of less than 0xFF, false otherwise. If the image data does not contain alpha values, false is returned.
         */
        bool IsTransparent(void);

        /**
         * @brief Get the image width.
         * @return Image width in pixels.
         */
        GLuint GetWidth(void);

        /**
         * @brief Get the image height.
         * @return Image height in pixels.
         */
        GLuint GetHeight(void);

    private:
        GLuint textureID;                 // The OpenGL texture ID.
        GLuint imageWidth;                // Image width in pixels.
        GLuint imageHeight;               // Image height in pixels.
        std::vector<uint8_t> imageData;   // Image data stored as R,G,B,A and pixel by pixel.
        bool containsAlpha;               // True if the @ref imageData contains alpha and has format RGBA, false otherwise.
        bool isTransparent;               // True if there's at least one pixel in @ref imageData, that has an alpha of less than 0xFF.

        /**
         * @brief Check whether at least one pixel in @ref imageData contains an alpha value of less than 0xFF.
         * @details This member function sets the @ref isTransparent flag.
         */
        void CheckTransparency(void);
};

