//**********************************************
//Singleton Texture Manager class
//Written by Ben English
//benjamin.english@oit.edu
//
//For use with OpenGL and the FreeImage library
//**********************************************

#include "TextureManager.h"

TextureManager* TextureManager::m_inst(0);

TextureManager* TextureManager::Inst()
{
	if(!m_inst)
		m_inst = new TextureManager();

	return m_inst;
}

TextureManager::TextureManager()
{
	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
		FreeImage_Initialise();
	#endif
}

//these should never be called
//TextureManager::TextureManager(const TextureManager& tm){}
//TextureManager& TextureManager::operator=(const TextureManager& tm){}
	
TextureManager::~TextureManager()
{
	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
		FreeImage_DeInitialise();
	#endif

	UnloadAllTextures();
	m_inst = 0;
}

bool TextureManager::LoadTexture(const char* filename, const unsigned int texID, GLenum image_format, GLint internal_format, GLint level, GLint border)
{
	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);
	//OpenGL's image ID to map to
	GLuint gl_texID;
	
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename, 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(filename);
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		return false;

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename);
	//if the image failed to load, return failure
	if(dib)
	{
		//retrieve the image data
		bits = FreeImage_GetBits(dib);
		//get the image width and height
		width = FreeImage_GetWidth(dib);
		height = FreeImage_GetHeight(dib);
	}
	//if this somehow one of these failed (they shouldn't), return failure
	if ((bits == 0) || (width == 0) || (height == 0))
	{
		width = 256;
		height = 256;
		char *data = new char[width * height * 4];

		int c = 0;
		for (int y = 0; y < height; ++y)
			for (int x = 0; x < height; ++x)
			{

				data[c] = x * y;
				data[c + 1] = x;
				data[c + 2] = y;
				data[c + 3] = 255;
				c += 4;
			}
		bits = (BYTE*)data;
		//return false;
	}
	
	//if this texture ID is in use, unload the current texture
	if(m_texID.find(texID) != m_texID.end())
		glDeleteTextures(1, &(m_texID[texID]));

	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &gl_texID);
	//store the texture ID mapping
	m_texID[texID] = gl_texID;
	//bind to the new texture ID
	//glBindTexture(GL_TEXTURE_2D, gl_texID);
	//store the texture data for OpenGL use

	glBindTexture(GL_TEXTURE_2D, m_texID[texID]);

	glTexImage2D(GL_TEXTURE_2D, level, internal_format, width, height, border, image_format, GL_UNSIGNED_BYTE, bits);
	glGenerateMipmap(GL_TEXTURE_2D);
	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);

	//return success
	return true;
}

GLuint TextureManager::GetTextureId(unsigned int texID)
{
	return m_texID[texID];
}

bool TextureManager::UnloadTexture( unsigned int texID)
{
	bool result(true);
	//if this texture ID mapped, unload it's texture, and remove it from the map
	if(m_texID.find(texID) != m_texID.end())
	{
		glDeleteTextures(1, &(m_texID[texID]));
		m_texID.erase(texID);
	}
	//otherwise, unload failed
	else
	{
		result = false;
	}

	return result;
}


enum eGLMinFiltering
{
	ETGL_NEAREST_MIPMAP_NEAREST = 0x2700,
	ETGL_LINEAR_MIPMAP_NEAREST = 0x2701,
	ETGL_NEAREST_MIPMAP_LINEAR = 0x2702,
	ETGL_LINEAR_MIPMAP_LINEAR = 0x2703,
	ETGL_NEAREST = 0x2600,
	ETGL_LINEAR = 0x2601
};

enum eGLMagFiltering
{
	ETGL_MAG_NEAREST = 0x2600,
	ETGL_MAG_LINEAR = 0x2601
};


enum eGLTexRepeat
{
	ETGL_REPEAT = 0x2901,
	ETGL_CLAMP_TO_EDGE = 0x812F,
	ETGL_MIRRORED_REPEAT = 0x8370
};

bool TextureManager::BindTexture(unsigned int slot,  unsigned int texID)
{
	bool result(true);

	glActiveTexture(GL_TEXTURE0 + slot);			//GL textures have 'slots' (sometimes 8, 16 or more). We must tell GL which slot we're currently using.
													//All subsequent calls to routines like 'glBindTexture' or 'glTexParameterx' will refer to the slot specified here. The Slots start at 
													//'GL_TEXTURE0' and we can add an integer to that value to access subseqent slots.

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ETGL_NEAREST);			//this sets up the filtering mode for texels that are 'minified'. i.e. they further away than a 1:1 mapping between texels and screen pixels
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ETGL_MAG_NEAREST);		//this sets up the filtering mode for texels that are zoom in or magnified. i.e. they are closer to us than a 1:1 texel/screen pixel mapping

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ETGL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ETGL_CLAMP_TO_EDGE);

	//if this texture ID mapped, bind it's texture as current


	if(m_texID.find(texID) != m_texID.end())
		glBindTexture(GL_TEXTURE_2D, m_texID[texID]);
	//otherwise, binding failed
	else
		result = false;

	return result;
}

void TextureManager::UnloadAllTextures()
{
	//start at the begginning of the texture map
	std::map<unsigned int, GLuint>::iterator i = m_texID.begin();

	
	

	//Unload the textures untill the end of the texture map is found
	while(i != m_texID.end())
		UnloadTexture(i->first);

	//clear the texture map
	m_texID.clear();
}