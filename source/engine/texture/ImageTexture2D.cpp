#include <ImageTexture2D.hpp>
#include <thirdparty/lodepng/lodepng.h>


ImageTexture2D::ImageTexture2D(){
    textureID = 0;
    SetDefaultDataRGB({0,0,0});
}

void ImageTexture2D::SetDefaultDataRGBA(std::array<uint8_t,4> pixelColorRGBA){
    imageWidth = 1;
    imageHeight = 1;
    imageData.clear();
    imageData.push_back(pixelColorRGBA[0]);
    imageData.push_back(pixelColorRGBA[1]);
    imageData.push_back(pixelColorRGBA[2]);
    imageData.push_back(pixelColorRGBA[3]);
    containsAlpha = true;
    isTransparent = (pixelColorRGBA[3] < 0xFF);
}

void ImageTexture2D::SetDefaultDataRGB(std::array<uint8_t,3> pixelColorRGB){
    imageWidth = 1;
    imageHeight = 1;
    imageData.clear();
    imageData.push_back(pixelColorRGB[0]);
    imageData.push_back(pixelColorRGB[1]);
    imageData.push_back(pixelColorRGB[2]);
    containsAlpha = false;
    isTransparent = false;
}

bool ImageTexture2D::ReadFromPngFile(std::string filename, bool useAlpha){
    if(useAlpha){
        SetDefaultDataRGBA({0,0,0,0});
    }
    else{
        SetDefaultDataRGB({0,0,0});
    }
    std::vector<uint8_t> pngPixels;
    unsigned int pngWidth, pngHeight;
    unsigned int error = lodepng::decode(pngPixels, pngWidth, pngHeight, filename, useAlpha ? LCT_RGBA : LCT_RGB, 8);
    if(error){
        PrintE("Could not read png file \"%s\" (error=%u)\n", filename.c_str(), error);
        return false;
    }
    imageWidth = static_cast<GLuint>(pngWidth);
    imageHeight = static_cast<GLuint>(pngHeight);
    imageData.clear();
    size_t wn = static_cast<size_t>(useAlpha ? 4 : 3) * static_cast<size_t>(imageWidth);
    size_t h = static_cast<size_t>(imageHeight);
    for(size_t i = 0; i < h; ++i){
        size_t iStart = (h - i - 1) * wn;
        size_t iEnd = (h - i) * wn;
        imageData.insert(imageData.end(), pngPixels.begin() + iStart, pngPixels.begin() + iEnd);
    }
    CheckTransparency();
    return true;
}

void ImageTexture2D::CheckTransparency(void){
    isTransparent = false;
    if(containsAlpha){
        for(size_t n = 3; n < imageData.size(); n += 4){
            isTransparent |= (imageData[n] < 0xFF);
        }
    }
}

bool ImageTexture2D::Generate(GLenum textureWrapping, GLenum textureMinFilter, GLenum textureMaxFilter, bool generateMipmap, bool useAnisotropy, bool useSRGB){
    if(imageData.empty()){
        return false;
    }
    DEBUG_GLCHECK( glGenTextures(1, &textureID); );
    DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
    DEBUG_GLCHECK( glTexImage2D(GL_TEXTURE_2D, 0, (containsAlpha ? (useSRGB ? GL_SRGB_ALPHA : GL_RGBA) : (useSRGB ? GL_SRGB : GL_RGB)), imageWidth, imageHeight, 0, (containsAlpha ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, &imageData[0]); );
    if(generateMipmap){
        DEBUG_GLCHECK( glGenerateMipmap(GL_TEXTURE_2D); );
    }
    DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrapping); );
    DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrapping); );
    DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureMinFilter); );
    DEBUG_GLCHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureMaxFilter); );
    if(useAnisotropy){
        GLfloat maxAnisotropy;
        DEBUG_GLCHECK( glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy); );
        DEBUG_GLCHECK( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy); );
    }
    imageData.clear();
    return true;
}

void ImageTexture2D::Delete(void){
    if(textureID){
        DEBUG_GLCHECK( glDeleteTextures(1, &textureID); );
        textureID = 0;
    }
}

GLuint ImageTexture2D::GetWidth(void){
    return imageWidth;
}

GLuint ImageTexture2D::GetHeight(void){
    return imageHeight;
}

void ImageTexture2D::BindTexture(void){
    DEBUG_GLCHECK( glBindTexture(GL_TEXTURE_2D, textureID); );
}

bool ImageTexture2D::IsTransparent(void){
    return isTransparent;
}

