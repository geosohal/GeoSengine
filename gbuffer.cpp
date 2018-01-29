// gbuffer.cpp
// contains 4 textures which are to be used as buffers
// tutorial used as reference: http://ogldev.atspace.co.uk/www/tutorial35/tutorial35.html



#include "openglfunctions.h"
#include <QtOpenGL/QtOpenGL>
#include "gbuffer.h"

#define CHECKERRORNOX {GLenum err = GL::glGetError(); if (err != GL_NO_ERROR) { fprintf(stderr, "OpenGL gerror (at line %d):\n", __LINE__); } }
//#define BUFFERDEBUG
GBuffer::GBuffer()
{
	frameBufferObj = 0;
	depthTexture = 0;
//	textures[] = { (GLuint)GBUFFER_TYPE_NORMAL, (GLuint)GBUFFER_TYPE_SPEC, (GLuint)GBUFFER_TYPE_DIFFUSE,
	//	(GLuint)GBUFFER_TYPE_POS };
}

GBuffer::~GBuffer()
{
	if (frameBufferObj != 0) {
        GL::glDeleteFramebuffers(1, &frameBufferObj);
	}
	if (textures[0] != 0) {
        GL::glDeleteTextures(GBUFFER_NUMTEXTURES, textures);
	}
	if (depthTexture != 0) {
        GL::glDeleteTextures(1, &depthTexture);
	}
}
bool GBuffer::Initialize(int WindowWidth, int WindowHeight)
{
	// generate the FBO
    GL::glGenFramebuffers(1, &frameBufferObj);
#ifdef BUFFERDEBUG
    GL::glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObj);
#else
    GL::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObj);
#endif
	// generate the gbuffer textures for the vertex attributes
    GL::glGenTextures(GBUFFER_NUMTEXTURES, textures);
    GL::glGenTextures(1, &depthTexture);
	
	for (unsigned int i = 0; i < GBUFFER_NUMTEXTURES; i++) {
        GL::glBindTexture(GL_TEXTURE_2D, textures[i]);

        GL::glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
#ifndef BUFFERDEBUG
        GL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
        GL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);
#endif // !BUFFERDEBUG
        GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
	}


	// initialize depth buffer texture
    GL::glBindTexture(GL_TEXTURE_2D, depthTexture);
    GL::glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
		NULL);
    GL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int)GL_CLAMP_TO_EDGE);
    GL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)GL_CLAMP_TO_EDGE);
    GL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_LINEAR);
    GL::glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_LINEAR);
    GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	
	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    GL::glDrawBuffers(GBUFFER_NUMTEXTURES, DrawBuffers);	// enable writing to all 4 buffers

    GLenum Status = GL::glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}

	// restore default FBO  (so that further changes will not affect our G buffer)
    GL::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

void GBuffer::DrawBind()
{
    GL::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObj);
}

void GBuffer::ReadBind()
{
#ifndef BUFFERDEBUG
    GL::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	for (unsigned int i = 0; i < GBUFFER_NUMTEXTURES; i++) {
        GL::glActiveTexture(GL_TEXTURE0 + i);
        GL::glBindTexture(GL_TEXTURE_2D, textures[GBUFFER_TYPE_NORMAL + i]);

        GLenum err = GL::glGetError();
		if (err != GL_NO_ERROR)
			continue;;

		CHECKERRORNOX
	}
#else
        GL::glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferObj);
		CHECKERRORNOX
#endif
}
void GBuffer::SetDepthReadBuffer()
{
    GL::glReadBuffer(GL_DEPTH_ATTACHMENT);
}
void GBuffer::SetReadBuffer(GBUFFER_TYPE TextureType)
{
    GL::glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
	CHECKERRORNOX
}

void GBuffer::BindDepthForRead(GLenum TU)
{
    GL::glActiveTexture(TU);
    GL::glBindTexture(GL_TEXTURE_2D, depthTexture);
	CHECKERRORNOX
}
