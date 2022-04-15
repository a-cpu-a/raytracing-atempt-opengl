#pragma once

#include <string>

#include <GL/glew.h>

struct FrameBuffer
{
	GLuint id;
	GLuint texId;

	FrameBuffer(size_t scrW, size_t scrH, GLenum i, GLuint _id)
	{
		id = _id;
		glBindFramebuffer(GL_FRAMEBUFFER, id);

		//tex
		{
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scrW, scrH, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, scrW, scrH, 0, GL_RGB, GL_FLOAT, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			glActiveTexture(GL_TEXTURE0 + i);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);

		}

//		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	~FrameBuffer()
	{
		glDeleteFramebuffers(1, &id);
	}

	void resize(size_t scrW, size_t scrH)
	{
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scrW, scrH, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, scrW, scrH, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	void bind()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
	}
	void unBind()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	FrameBuffer() {}
};