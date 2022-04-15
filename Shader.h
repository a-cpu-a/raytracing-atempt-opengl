#pragma once

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <iostream>

struct vec3
{
	float x;
	float y;
	float z;
	vec3(float values)
	{
		x = values;
		y = values;
		z = values;
	}
	vec3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	vec3 operator*(const float &val)
	{
		return vec3(
			x * val,
			y * val,
			z * val);
	}
	vec3 operator+(const vec3& val)
	{
		return vec3(
			x + val.x,
			y + val.y,
			z + val.z);
	}

	vec3 operator +=(const vec3 &val)
	{
		return vec3(
			x + val.x,
			y + val.y,
			z + val.z);
	}
	vec3 operator -=(const vec3& val)
	{
		return vec3(
			x - val.x,
			y - val.y,
			z - val.z);
	}
};

class Shader
{
public:
	bool failed;
private:
	std::string fragment;
	std::string vertex;

	GLuint m_id = 0;
	std::string m_getTextFromFile(std::string location)
	{
		std::ifstream stream(location);

		std::string line;
		std::string contents;
		while (getline(stream, line))
		{
			contents = contents + line + "\n";
		}
		//std::cout << contents << "\n";
		stream.close();

		return contents;
	}
	GLuint m_compileShader(GLuint type, std::string& source)
	{
		const GLuint id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			int lenght;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
			char* message = (char*)_malloca(lenght * sizeof(char));//
			glGetShaderInfoLog(id, lenght, &lenght, message);

			std::cout << "ERROR IN SHADER: " << type << "\n";
			std::cout << message << "\n";

			//free(message);//

			//glDeleteShader(id);
			failed = true;
			return id;
		}

		return id;
	}
public:
	Shader(const std::string _vertex, const std::string _fragment)
	{
		m_id = glCreateProgram();

		vertex = _vertex;
		fragment = _fragment;

		reload();
	}
	Shader()
	{}
	~Shader()
	{}
	void reload()
	{
		failed = false;
		glDeleteProgram(m_id);
		m_id = glCreateProgram();
		int success;
		std::string vshSource = m_getTextFromFile(vertex); //"assets/shaders/base.vsh"
		std::string fshSource = m_getTextFromFile(fragment); //"assets/shaders/base.fsh"
		const GLuint vsh = m_compileShader(GL_VERTEX_SHADER, vshSource);
		const GLuint fsh = m_compileShader(GL_FRAGMENT_SHADER, fshSource);

		glAttachShader(m_id, vsh);
		glAttachShader(m_id, fsh);
		glLinkProgram(m_id);
		glValidateProgram(m_id);

		glGetProgramiv(m_id, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{

			int lenght;
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &lenght);
			char* message = (char*)_malloca(lenght * sizeof(char));
			glGetProgramInfoLog(m_id, lenght, &lenght, message);

			std::cout << "ERROR IN PROGRAM\n";
			std::cout << message << "\n";

		}
		glDeleteProgram(vsh);
		glDeleteProgram(fsh);
	}
	void bind()
	{
		glUseProgram(m_id);
	}

	GLint getUniformLocation(const std::string name)
	{
		const GLint id = glGetUniformLocation(m_id, name.c_str());
		if (id == -1)
			std::cout << "[ERROR] SHADER UNIFORM '" << name << "' MISSING FROM SHADER: " << m_id << "\n";
		return id;
	}

	//setters
	void setUniformFLOAT(const GLint id, const float value)
	{
		glUniform1f(id, value);
	}

	void setUniformVEC3(const GLint id, const vec3 value)
	{
		glUniform3f(id, value.x, value.y, value.z);
	}

	void setUniformUINT(const GLint id, const uint32_t value)
	{
		glUniform1ui(id, value);
	}

	void setUniformINT(const GLint id, const int value)
	{
		glUniform1i(id, value);
	}
};

