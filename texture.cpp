///////////////////////////////////////////////////////////////////////
// A slight encapsulation of an OpenGL texture. This contains a method
// to read an image file into a texture, and methods to bind a texture
// to a shader for use, and unbind when done.
////////////////////////////////////////////////////////////////////////

#include "math.h"
#include <fstream>
#include <stdlib.h>

#include "texture.h"

#include "openglfunctions.h"


#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "libs/stb_image.h"

#define CHECKERRORNOX {GLenum err = GL::glGetError(); if (err != GL_NO_ERROR) { fprintf(stderr, "YTOpenGL error (at line %d): %i\n", __LINE__, err); } }
Texture::Texture(const std::string &path)
{
    fileName = path;
    textureId = 0;
}

bool Texture::Load(bool isHDR)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, n;
    unsigned char* image = stbi_load(fileName.c_str(), &width, &height, &n, 4);
    if (!image) {
        printf("\nRead error on file %s:\n  %s\n\n", fileName.c_str(), stbi_failure_reason());
        return false;
        //exit(-1);
    }

    // Here we create MIPMAP and set some useful modes for the texture
    GL::glGenTextures(1, &textureId);   // Get an integer id for thi texture from OpenGL
    GL::glBindTexture(GL_TEXTURE_2D, textureId);

    if (!isHDR)
        GL::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    else //hdr image
        GL::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, width, height, 0, GL_RGB, GL_FLOAT, image);
    CHECKERRORNOX
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 100);
    GL::glGenerateMipmap(GL_TEXTURE_2D);
    CHECKERRORNOX
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_LINEAR);
    GL::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_LINEAR_MIPMAP_LINEAR);
    GL::glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image);
    CHECKERRORNOX
    return true;
}

bool Texture::Load()
{
    return Load(false);
}

void Texture::Bind(GLenum unit)
{
    GL::glActiveTexture( unit);
    GL::glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::Unbind()
{
    GL::glBindTexture(GL_TEXTURE_2D, 0);
}
