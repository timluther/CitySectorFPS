//**********************************************
//Singleton Texture Manager class
//Written by Ben English
//benjamin.english@oit.edu
//
//For use with OpenGL and the FreeImage library
//**********************************************

#ifndef TextureManager_H
#define TextureManager_H

#include <windows.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "../FreeImage/FreeImage.h"
#include <map>

class TextureManager
{
public:
	static TextureManager* Inst();
	virtual ~TextureManager();

	//load a texture an make it the current texture
	//if texID is already in use, it will be unloaded and replaced with this texture
	bool LoadTexture(const char* filename,	//where to load the file from
		const unsigned int texID,			//arbitrary id you will reference the texture by
											//does not have to be generated with glGenTextures
		GLenum image_format = GL_RGBA,		//format the image is in
		GLint internal_format = GL_RGBA,		//format to store the image in
		GLint level = 0,					//mipmapping level
		GLint border = 0);					//border size

	//free the memory for a texture
	bool UnloadTexture(unsigned int texID);

	//set the current texture
	bool BindTexture(const unsigned int slot, unsigned int texID);

	//free all texture memory
	void UnloadAllTextures();

	GLuint GetTextureId(unsigned int id);

protected:
	TextureManager();
	TextureManager(const TextureManager& tm);
	TextureManager& operator=(const TextureManager& tm);

	static TextureManager* m_inst;
	std::map<unsigned int, GLuint> m_texID;
};

#endif