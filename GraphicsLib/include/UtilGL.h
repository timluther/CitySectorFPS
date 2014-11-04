/********************************************//**
 * \copyright (c) 2014 
 * \file   	UtilGL.h
 * \Author 	Tim Lewis (tim@totga-games.com)
 * \see    	Project: Anthracite
 * \date   	11-09-2014
 * \brief  	UtilGL
 * UtilGL is a general utility file (i.e function orphanage) 
 * for miscellaneous opengl functions
 ***********************************************/
#ifndef UTIL_GL_H_INCLUDED
#define UTIL_GL_H_INCLUDED

bool CheckGLErrors(const char* argMessage);
bool CheckGLThrow(const char* argFilename, const char* argMessage);

#define GL_CHECK(x) \
	x; \
	{ \
	GLenum glError = glGetError(); \
	if(glError != GL_NO_ERROR) { \
	fprintf(stderr, "glGetError() = %i (0x%.8x) at line %i\n", glError, glError, __LINE__); \
	} \
	}
#define EGL_CHECK(x) \
	x; \
	{ \
	EGLint eglError = eglGetError(); \
	if(eglError != EGL_SUCCESS) { \
	fprintf(stderr, "eglGetError() = %i (0x%.8x) at line %i\n", eglError, eglError, __LINE__); \
	} \
	}




#endif