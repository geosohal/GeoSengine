#include "aoBuffer.h"
#include "openglfunctions.h"


#define CHECKERROR {GLenum err = GL::glGetError(); if (err != GL_NO_ERROR) { fprintf(stderr, "shOpenGL error (at line %d): %s\n", __LINE__, GL::gluErrorString(err)); } }


AoFbo::AoFbo()
{
	aoTexture = aoTextureF = 0;
	aoFbo = 0;
}
AoFbo::~AoFbo()
{
	if (aoFbo != 0)
        GL::glDeleteFramebuffers(1, &aoFbo);
	if (aoTexture != 0)
        GL::glDeleteTextures(1, &aoTexture);
	if (aoTextureF != 0)
        GL::glDeleteTextures(1, &aoTextureF);
}
void AoFbo::CreateFBO(const int w, const int h)
{
    GL::glGenFramebuffers(1, &aoFbo);
    GL::glBindFramebuffer(GL_FRAMEBUFFER, aoFbo);

	// unfiltered ao texture, store AOfactors in R component
    GL::glGenTextures(1, &aoTexture);
    GL::glBindTexture(GL_TEXTURE_2D, aoTexture);
    GL::glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_R32F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    GL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_LINEAR);
    GL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_LINEAR);
	// gbuffer leaves off at GL_COLOR_ATTACHMENT3 and it did textures 0-3?
    GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, aoTexture, 0);

    GL::glGenTextures(1, &aoTextureF);
    GL::glBindTexture(GL_TEXTURE_2D, aoTextureF);
    GL::glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_R32F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    GL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_LINEAR);
    GL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_LINEAR);
    GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, aoTextureF, 0);

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
    GL::glDrawBuffers(1, DrawBuffers);

	// Check for completeness/correctness
    int status = (int)GL::glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
	if (status != int(GL_FRAMEBUFFER_COMPLETE_EXT))
        printf("aoFBO Error: %d\n", status);
	// restore default FBO  (so that further changes will not affect our G buffer)
    GL::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
void AoFbo::BindForWrite()
{
    GL::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, aoFbo);
}

void AoFbo::UnbindWrite()
{
    GL::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
void AoFbo::BindForRead(GLenum textureUnit)
{
    GL::glActiveTexture(textureUnit);
    GL::glBindTexture(GL_TEXTURE_2D, aoTexture);
}
void AoFbo::BindBlurredForRead(GLenum textureUnit)
{
    GL::glActiveTexture(textureUnit);
    GL::glBindTexture(GL_TEXTURE_2D, aoTextureF);
}
