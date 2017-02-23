#pragma once

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	GLuint program;

	Shader(const GLchar* vertexPath, const GLchar* tessControlPath, const GLchar* tessEvalPath, const GLchar* geometryPath, const GLchar* fragmentPath)
	{		
		GLuint vertex;
		vertex = getShader(vertexPath, GL_VERTEX_SHADER);
		checkCompileErrors(vertex, "VERTEX");
		
		GLuint tessControl;
		tessControl = getShader(tessControlPath, GL_TESS_CONTROL_SHADER);
		checkCompileErrors(tessControl, "TESSCONTROL");

		GLuint tessEval;
		tessEval = getShader(tessEvalPath, GL_TESS_EVALUATION_SHADER);
		checkCompileErrors(tessEval, "TESSEVAL");

		GLuint geometry;
		geometry = getShader(geometryPath, GL_GEOMETRY_SHADER);
		checkCompileErrors(geometry, "GEOMETRY");

		GLuint fragment;
		fragment = getShader(fragmentPath, GL_FRAGMENT_SHADER);
		checkCompileErrors(fragment, "FRAGMENT");

		this->program = glCreateProgram();
		glAttachShader(this->program, vertex);
		glAttachShader(this->program, tessControl);
		glAttachShader(this->program, tessEval);
		glAttachShader(this->program, geometry);
		glAttachShader(this->program, fragment);
		glLinkProgram(this->program);
		checkCompileErrors(this->program, "PROGRAM");
		
		glDeleteShader(vertex);
		glDeleteShader(tessControl);
		glDeleteShader(tessEval);
		glDeleteShader(geometry);
		glDeleteShader(fragment);
	}

	void use() { glUseProgram(this->program); }

private:
	GLuint getShader(const GLchar* shaderPath, GLenum shaderType)
	{
		
		std::ifstream shaderFile;
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		std::string shaderCode;
		shaderCode = shaderStream.str();
		const GLchar* shaderChar = shaderCode.c_str();

		GLuint shader;
		shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &shaderChar, NULL);
		glCompileShader(shader);
		return shader;
	}

	void checkCompileErrors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
			}
		}
	}
};