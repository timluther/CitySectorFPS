#include "gl_includes.h"
#include <iostream>
#include <sstream>
#include "Utility.h"


const char *GetErrorCodeText(unsigned int code)
{
	const char* glErrorString = "Unknown Error";
	switch (code)
	{
	case 0x8031://case GL_TABLE_TOO_LARGE:
		glErrorString = "Table too large";
		break;
#ifdef OPENGL_ES_2
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		glErrorString = "Invalid framebuffer operation";
		break;
#endif
	case GL_OUT_OF_MEMORY:
		glErrorString = "Out of memory";
		break;
	case GL_INVALID_ENUM:
		glErrorString = "Invalid Enum";
		break;
	case GL_INVALID_VALUE:
		glErrorString = "Invalid Value";
		break;
	case GL_INVALID_OPERATION:
		glErrorString = "Invalid Operation";
		break;
	default:
		glErrorString = "Unknown operation";
		break;
#ifdef OPENGL_ES_2
#endif
#ifdef OPENGL_ES_1
	case GL_STACK_OVERFLOW:
		glErrorString = "Stack Overflow";
		break;
	case GL_STACK_UNDERFLOW:
		glErrorString = "Stack Underflow";
		break;
#endif

	}
	return glErrorString;
}


/********************************************//**
* \brief 	PerformShaderErrorCheck
* \param 	Shader[in] <TODO>
* \details 	<TODO>
***********************************************/
void PerformShaderErrorCheck(GLuint Shader)
{
	GLint iLen;
	char *sDebugSource = NULL;
	char *sErrorLog = NULL;
	if (Shader == 0)
	{
		glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &iLen);
		if (iLen >= 0)
		{
			sErrorLog = new char[iLen + 1];
			GLsizei actual_size;
			std::cout << "Problem with Shader \n";
			glGetShaderInfoLog(Shader, iLen, &actual_size, sErrorLog);
			if (sErrorLog)
			{
				std::cout << sErrorLog << std::endl;
				delete[] sErrorLog;
			}
		}
		else
			std::cout << "Could not get length of shader error buffer " << std::endl;
	}	
}

bool CheckGLErrors(const char* argMessage)
{
	int glErr = glGetError();
	if (glErr)
	{				
		const char * outstr = GetErrorCodeText(glErr);
		std::cout << " GL ERROR " << glErr << "(" << outstr <<"):" << argMessage  << std::endl;
		return false;
	}
	return true;
}


bool CheckGLThrow(const char* argFilename, const char* argMessage)
{
	int glErr = glGetError();
	if (glErr)
	{	
		std::stringstream str;					
		throw CException(argFilename, 0,0,0);		
		return false;
	}
	return true;
}


