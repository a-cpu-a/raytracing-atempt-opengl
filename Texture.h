#pragma once

#include <string>

#include <GL/glew.h>

struct Texture
{
	int32_t w;
	int32_t h;
	int32_t c;

	GLuint id;

	std::string filePath;
	

	Texture(std::string _filePath, GLenum i)
	{
		filePath = _filePath;

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		float col[4] = { 0.0f,0.0f,0.0f,0.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, col);

		reload();

		glActiveTexture(GL_TEXTURE0 + i);
	}

	Texture()
	{

	}

	void reload()
	{
		glBindTexture(GL_TEXTURE_2D, id);

		uint8_t* buffer = stbi_load(filePath.c_str(), &w, &h, &c, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

		stbi_image_free(buffer);
	}
};