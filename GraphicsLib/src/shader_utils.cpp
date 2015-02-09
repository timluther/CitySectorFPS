//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "shader_utils.h"
#include "UtilGL.h"
#include <vector>
#include <iostream>
#include <fstream>

static std::string ReadFileToString(const std::string &source)
{
    std::ifstream stream(source);
    if (!stream)
    {
        std::cerr << "Failed to load shader file: " << source;
        return "";
    }

    std::string result;

    stream.seekg(0, std::ios::end);
    result.reserve(stream.tellg());
    stream.seekg(0, std::ios::beg);

    result.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

    return result;
}

GLuint CompileShader(GLenum type, const std::string &source)
{
    GLuint shader = glCreateShader(type);

    const char *sourceArray[1] = { source.c_str() };
    glShaderSource(shader, 1, sourceArray, NULL);
    glCompileShader(shader);

    GLint compileResult;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

    if (compileResult != GL_TRUE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog(infoLogLength);
        glGetShaderInfoLog(shader, infoLog.size(), NULL, infoLog.data());

        std::cerr << "shader compilation failed: " << infoLog.data();
		OutputDebugStringA(infoLog.data());

        glDeleteShader(shader);
        shader = 0;
    }

    return shader;
}

GLuint CompileShaderFromFile(GLenum type, const std::string &sourcePath)
{
    std::string source = ReadFileToString(sourcePath);
    if (source.empty())
    {
        return 0;
    }

    return CompileShader(type, source);
}


void CheckShader(unsigned int shader)
{
	int iLen = 0;
	GL_CHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &iLen));
	if (iLen > 0)
	{
		char *sErrorLog = (char*)malloc(iLen);
		GL_CHECK(glGetShaderInfoLog(shader, iLen, NULL, sErrorLog));
		printf("Log START:\n%s\nLog END\n\n", sErrorLog);
		std::cout << sErrorLog << std::endl;
		OutputDebugStringA(sErrorLog);
	}
}



void CheckLinkStatus(unsigned int mShaderProgram) 
{
	GLint link_ok, validate_ok;
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &link_ok);

	char * sErrorLog = NULL;
	if (!link_ok)
	{
		int iLen;
		GL_CHECK(glGetProgramiv(mShaderProgram, GL_INFO_LOG_LENGTH, &iLen));
		if (iLen != 0)
		{
			sErrorLog = (char*)malloc(iLen + 1);

			GLsizei actual_size;
			std::cout << "Problem with Shader \n";
			GL_CHECK(glGetProgramInfoLog(mShaderProgram, iLen, &actual_size, sErrorLog));
		}
		else
			sErrorLog = "Unknown Error at shader link time.";
		std::cout << sErrorLog << std::endl;
		//throw CException(0, 0, mDebugShaderPath, NULL, sErrorLog, XPMERR_LINKER_ERROR, ECEC_SCRIPT, NULL);
		delete[]sErrorLog;
		//return false;
	}

	glValidateProgram(mShaderProgram);
	glGetProgramiv(mShaderProgram, GL_VALIDATE_STATUS, &validate_ok);
	if (!validate_ok)
	{
		int iLen;
		glGetProgramiv(mShaderProgram, GL_INFO_LOG_LENGTH, &iLen);
		CheckGLErrors("Problems linking shader");
		sErrorLog = (char*)malloc(iLen + 1);
		GLsizei actual_size;
		std::cout << "Problem with Shader Validation \n";
		GL_CHECK(glGetProgramInfoLog(mShaderProgram, iLen, &actual_size, sErrorLog));


		//throw CException(0, 0, mDebugShaderPath, NULL, sErrorLog, XPMERR_LINKER_ERROR, ECEC_SCRIPT, NULL);
		delete[]sErrorLog;
		//return false;
	}
}


GLuint CompileProgram(const std::string &vsSource, const std::string &fsSource)
{
    GLuint program = glCreateProgram();
	program = glCreateProgram();
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vsSource);
	std::cout << "Checking vertex shader " << std::endl;
	PerformShaderErrorCheck(vs);
	
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSource);
	std::cout << "Checking fragment shader " << std::endl;
	PerformShaderErrorCheck(fs);

    if (vs == 0 || fs == 0)
    {
        glDeleteShader(fs);
        glDeleteShader(vs);
        glDeleteProgram(program);
        return 0;
    }

    glAttachShader(program, vs);
    glDeleteShader(vs);

    glAttachShader(program, fs);
    glDeleteShader(fs);

    glLinkProgram(program);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == 0)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::vector<GLchar> infoLog(infoLogLength);
        glGetProgramInfoLog(program, infoLog.size(), NULL, infoLog.data());

        std::cerr << "program link failed: " << infoLog.data();

        glDeleteProgram(program);
        return 0;
    }

    return program;
}

GLuint CompileProgramFromFiles(const std::string &vsPath, const std::string &fsPath)
{
    std::string vsSource = ReadFileToString(vsPath);
    std::string fsSource = ReadFileToString(fsPath);
    if (vsSource.empty() || fsSource.empty())
    {
        return 0;
    }

    return CompileProgram(vsSource, fsSource);
}
