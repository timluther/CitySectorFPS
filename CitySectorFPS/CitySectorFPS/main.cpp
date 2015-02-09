#include "main.h"

#include "CMesh.h"
#include "matrix.h"
#include "../FreeImage/FreeImage.h"
#include "TextureManager.h"
#include <stdio.h>
size_t segment_count = 10, slice_count = 10;

int mvp_matrix_handle = -1;
int m_matrix_handle = -1;
Matrix4 mat;
Matrix4 objectmat;

CMesh *my_test_mesh = NULL;
CitySectorFPS::CitySectorFPS()
	: SampleApplication("HelloTriangle", 1280, 720)
{
}
/*
struct SVertex_P_D4B_N_UV
{
	enum { VERTEX_TYPE = EVT_P_D4B_N_UV };

	SVertex_P_D4B_N_UV(const CVector3f &position, const CVector3f &normal, const CVector2f &uv, unsigned int colour)
		:mPosition(position)
		, mNormal(normal)
		, mUv(uv)
		, mColour(colour)
	{
	}

	CVector3f mPosition;   //0
	CVector3f mNormal;     //12
	CVector2f mUv;         //24
	unsigned int mColour;  //32
};*/

struct sShaderHandles
{
	int uMvpMatrix;
	int uMMatrix;
	int aTexCoord;
	int aPosition;
	int aNormal;
	int TextureSampler;
	int MaterialColour;

	void Init(int ProgramHandle)
	{
		MaterialColour = glGetUniformLocation(ProgramHandle, "MaterialColour");
		uMMatrix = glGetUniformLocation(ProgramHandle, "u_mmatrix");
		uMvpMatrix = glGetUniformLocation(ProgramHandle, "u_mvpmatrix");		
		TextureSampler = glGetUniformLocation(ProgramHandle, "TextureSampler");
		aNormal = glGetAttribLocation(ProgramHandle, "aNormal");
		aTexCoord = glGetAttribLocation(ProgramHandle, "aTexCoord");
		aPosition = glGetAttribLocation(ProgramHandle, "aPosition");
	}

	void Use(GLfloat *vertices, GLfloat *texturecoordinates, GLfloat *normals, Matrix4 &MVPmatrix, Matrix4 &WorldMatrix, int texid)
	{
		CheckGLErrors("Error has occured lol");
		glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices);		
		if (aPosition != -1)
			glEnableVertexAttribArray(aPosition);
		if (aNormal != -1)
			glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, normals);
		glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texturecoordinates);
		if (aTexCoord != -1)
			glEnableVertexAttribArray(aTexCoord);
		CheckGLErrors("Error has occured lol");
		if (uMvpMatrix != -1)
			glUniformMatrix4fv(uMvpMatrix, 1, false, (const float*)&MVPmatrix);

		if (uMMatrix != -1)
			glUniformMatrix4fv(uMMatrix, 1, false, (const float*)&WorldMatrix);
		if (MaterialColour != -1)
			glUniform4f(MaterialColour, 1.0f, 1.0f, 1.0f, 1.0f);
		unsigned int GLtexid = TextureManager::Inst()->GetTextureId(texid);
		if (TextureSampler != -1)
			glUniform1i(TextureSampler, 0);
		CheckGLErrors("Error has occured lol");

	}

	void Deinit()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);		
	}
	
	void Use(const CVertexBuffer &buffer, Matrix4 &mat, int texid)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer.GetGLHandle());
		CheckGLErrors("Error has occured lol");
	
		glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		if (aPosition != -1)
			glEnableVertexAttribArray(aPosition);
		glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 0,(void*) 24);
		if (aTexCoord != -1)
			glEnableVertexAttribArray(aTexCoord);
		CheckGLErrors("Error has occured lol");
		glUniformMatrix4fv(uMvpMatrix, 1, false, (const float*)&mat);
		glUniform4f(MaterialColour, 1.0f, 1.0f, 1.0f, 1.0f);
		unsigned int GLtexid = TextureManager::Inst()->GetTextureId(texid);
		glUniform1i(TextureSampler, 0);
		CheckGLErrors("Error has occured lol");

	}

};

sShaderHandles gShaderHandles;
sShaderHandles gLitShaderHandles;

//end of file marker is found
inline long fsize_eof(FILE *f)
{
	long old_pos = ftell(f);
	fseek(f, 0, SEEK_END);
	long ret = ftell(f);
	fseek(f, old_pos, SEEK_SET);
	return ret;
}


const std::string &block_read_file(FILE *f, std::string &ret)
{	
	size_t len = fsize_eof(f);
	char *str = new char[len + 1];
	
	fread((void*)str, len, 1, f);
	str[len] = 0;
	ret = str;
	return ret;
}

std::string LoadFileAsString(const char *filename)
{
	std::string str;
	FILE *f = fopen(filename, "rb");
	if (f)
	{
		block_read_file(f, str);
		fclose(f);
	}
	return str;
}

bool CitySectorFPS::initialize()
{	
	std::string vs = LoadFileAsString("C:\\Users\\Brython\\Documents\\Code\\CitySectorFPS\\data\\simpleshader.vert");
	std::string fs = LoadFileAsString("C:\\Users\\Brython\\Documents\\Code\\CitySectorFPS\\data\\simpleshader.frag");
	mProgram = CompileProgram(vs, fs);

	vs = LoadFileAsString("C:\\Users\\Brython\\Documents\\Code\\CitySectorFPS\\data\\littextured.vert");
	fs = LoadFileAsString("C:\\Users\\Brython\\Documents\\Code\\CitySectorFPS\\data\\littextured.frag");
	mLightingProgram = CompileProgram(vs, fs);

	if (!mProgram)
	{
		return false;
	}

	gShaderHandles.Init(mProgram);
	gLitShaderHandles.Init(mLightingProgram);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	my_test_mesh = new CMesh();
	
	unsigned int v_count = 0;
	unsigned int t_count = 0;
	my_test_mesh->add_prism_element_count(segment_count, slice_count, v_count, t_count);
	my_test_mesh->create_arrays(v_count, t_count * 6);
	my_test_mesh->create_prism(CVector3f(0, 0, 0), -20, 10, segment_count, slice_count);
	my_test_mesh->fill_GPU_buffers();
	mat.perspective(90.0, 1.0, 0.1f, 1000.0f);
	//TODO: make sure this uses relative paths some day
	TextureManager::Inst()->LoadTexture("C:\\Users\\Brython\\Documents\\Code\\CitySectorFPS\\data\\testimage.png", 0);
	
	return true;
}

void CitySectorFPS::destroy()
{
	glDeleteProgram(mProgram);
}

void CitySectorFPS::draw()
{
	GLfloat vertices[] =
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
	};

	GLfloat texcoords[] =
	{
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};

	// Set the viewport
	glViewport(0, 0, getWindow()->getWidth(), getWindow()->getHeight());

	// Clear the color buffer
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);

	// Use the program object
	glUseProgram(mProgram);

	//TextureManager::Inst()->BindTexture(0);
	CheckGLErrors("Error has occured lol 1");
	gShaderHandles.Use(vertices, texcoords, NULL, mat, Matrix4(), 0);
	CheckGLErrors("Error has occured lol");

	// Load the vertex data
	TextureManager::Inst()->BindTexture(0, 0);
	static unsigned char indices[] = { 0, 1, 3, 3, 1, 2 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	gLitShaderHandles.Use(my_test_mesh->mVertexBuffer, mat, 0);
	my_test_mesh->draw();
	gShaderHandles.Deinit();
}


int main(int argc, char **argv)
{
	CitySectorFPS app;
	
	

	
	/*try
	{*/
		
	/*}
	catch (COutOfBoundsException e)
	{
		std::cout << "Out of bounds error in file " << e.m_src_file_name << " on line " << e.m_line << " had index " << e.m_index << " but array only has capacity of " << e.m_capacity << std::endl;
		my_test_mesh.destroy();
		exit(0);
		//TODO: send email to Brython
	}*/


	//app.initialize();


	
	return app.run();
}
