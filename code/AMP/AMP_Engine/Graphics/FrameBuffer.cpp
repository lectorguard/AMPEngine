#include "pch.h"
#include "FrameBuffer.h"
#include "AMP_Engine.h"

amp::FrameBuffer::~FrameBuffer()
{
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteTextures(1, &texColorBuffer);
	glDeleteFramebuffers(1, &mainFramebuffer);
	glDeleteProgram(shaderProgram);
	GLHelper::ErrorCheck();
	resetDrawCallback();
}

void amp::FrameBuffer::enable(AMP_Engine* engine)
{
	assert(engine);//Initialization of the engine was not correct
	this->engine = engine;
	GetShader();
	CreateFrameBuffer();
	CreateFrameBufferTexture();
	CreateRenderBuffer();
	SetRenderBuffer();
	CreateFrameRenderBuffer();
	GLHelper::SetBuffer(vertexBuffer, 6, verticesArray);
	GLHelper::SetBuffer(uvBuffer, 6, uvsArray);
	SetFragmentShaderFastest();
	GLHelper::ErrorCheck();
}

void amp::FrameBuffer::clear(bool isLowestLayer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if (isLowestLayer)glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	else glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	GLHelper::ErrorCheck();
}

void amp::FrameBuffer::drawFrameBufferContent()
{
	DisableDepthTesting();
	EnableBlend();
	BindFrameBuffer();
	glUseProgram(shaderProgram);
	GLHelper::SetUniformInt(shaderProgram, "ShadingType", shadingType);
	if (drawCallback)drawCallback(shaderProgram);
	GLHelper::BindArrayBuffer(0, 2, vertexBuffer);
	GLHelper::BindArrayBuffer(1, 2, uvBuffer);
	GLHelper::BindTexture(texColorBuffer, TextureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	DisableBlend();
	EnableDepthTesting();
	GLHelper::ErrorCheck();
}

void amp::FrameBuffer::setShaderPath(std::string path)
{
	this->shaderPath = path;
}

void amp::FrameBuffer::setShadingType(ShadingType type)
{
	this->shadingType = type;
}

amp::ShadingType amp::FrameBuffer::getShadingType()
{
	return this->shadingType;
}

void amp::FrameBuffer::SetFragmentShaderFastest()
{
	glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_FASTEST);
}

void amp::FrameBuffer::CreateFrameRenderBuffer()
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void amp::FrameBuffer::SetRenderBuffer()
{
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, engine->ampContext.getWidth(), engine->ampContext.getHeight());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void amp::FrameBuffer::CreateRenderBuffer()
{
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
}

void amp::FrameBuffer::CreateFrameBuffer()
{
	glGenFramebuffers(1, &mainFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);
}

void amp::FrameBuffer::CreateFrameBufferTexture()
{
	TexAttributes attrib;
	attrib.height = engine->ampContext.getHeight();
	attrib.width = engine->ampContext.getWidth();
	attrib.numberOfComponents = 3;
	attrib.data = NULL;
	TextureID = GLHelper::SetTexture(texColorBuffer, &attrib, shaderProgram, "FrameTex");
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
}

void amp::FrameBuffer::GetShader()
{
	ShaderAttributes* attrib = engine->ampShader.getShaderProgramFromPath(shaderPath.c_str());
	shaderProgram = attrib->ShaderProgram;
	attrib->isSelfOrganized = true;
	delete attrib;
	attrib = nullptr;
}

void amp::FrameBuffer::EnableDepthTesting()
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void amp::FrameBuffer::DisableBlend()
{
	glDisable(GL_BLEND);
}

void amp::FrameBuffer::BindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void amp::FrameBuffer::EnableBlend()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

void amp::FrameBuffer::DisableDepthTesting()
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
}
