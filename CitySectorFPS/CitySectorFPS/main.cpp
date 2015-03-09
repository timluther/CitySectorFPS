#include "main.h"

#include "CMesh.h"
#include "matrix.h"
#include "../FreeImage/FreeImage.h"
#include "TextureManager.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
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

#define BUFFER_OFFSET(i) ((char*)NULL +(i << 2))
	void Use(GLfloat *vertices, GLfloat *texturecoordinates, GLfloat *normals, Matrix4 &MVPmatrix, Matrix4 &WorldMatrix, int texid)
	{
		int stride = 36;
		CheckGLErrors("Error has occured lol3");
		glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));		
		if (aPosition != -1)
			glEnableVertexAttribArray(aPosition);
		if (aNormal != -1)
			glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(3));
		glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(6));
		if (aTexCoord != -1)
			glEnableVertexAttribArray(aTexCoord);
		CheckGLErrors("Error has occured lol4");
		if (uMvpMatrix != -1)
			glUniformMatrix4fv(uMvpMatrix, 1, false, (const float*)&MVPmatrix);

		if (uMMatrix != -1)
			glUniformMatrix4fv(uMMatrix, 1, false, (const float*)&WorldMatrix);
		if (MaterialColour != -1)
			glUniform4f(MaterialColour, 1.0f, 1.0f, 1.0f, 1.0f);
		unsigned int GLtexid = TextureManager::Inst()->GetTextureId(texid);
		if (TextureSampler != -1)
			glUniform1i(TextureSampler, 0);
		CheckGLErrors("Error has occured lol5");

	}

	void Deinit()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);		
	}
	
	void Use(const CVertexBuffer &buffer, const Matrix4 &mat, const Matrix4 &MVPmatrix, int texid)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer.GetGLHandle());
		CheckGLErrors("Error has occured lol6");
	
		glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		if (aPosition != -1)
			glEnableVertexAttribArray(aPosition);
		CheckGLErrors("Error has occured lol9");
/*		glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 0,(void*) 24);
		if (aTexCoord != -1)
			glEnableVertexAttribArray(aTexCoord);*/
		CheckGLErrors("Error has occured lol7");
		glUniformMatrix4fv(uMvpMatrix, 1, GL_FALSE, (const float*)&MVPmatrix.data);
		if (uMMatrix != -1)
			glUniformMatrix4fv(uMMatrix, 1, GL_FALSE, (const float*)&mat.data);
		CheckGLErrors("Error has occured lol7b");
		if (MaterialColour != -1)
			glUniform4f(MaterialColour, 1.0f, 1.0f, 1.0f, 1.0f);
		if (TextureSampler != -1)
		{
			unsigned int GLtexid = TextureManager::Inst()->GetTextureId(texid);
			glUniform1i(TextureSampler, 0);
		}
		CheckGLErrors("Error has occured lol8");

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


int LoadShadersFromFile(const char *filename)
{
	std::string filebase = std::string(filename);
	std::string vs = LoadFileAsString((filebase + ".vert").c_str());
	std::string fs = LoadFileAsString((filebase + ".frag").c_str());
	std::cout << "Attempting to load shaders from " << filebase << std::endl;
	return CompileProgram(vs, fs);

}


bool CitySectorFPS::initialize()
{	
	
	mProgram = LoadShadersFromFile("C:\\Users\\Brython\\Documents\\Code\\CitySectorFPS\\data\\simpleshader");
	mLightingProgram = LoadShadersFromFile("C:\\Users\\Brython\\Documents\\Code\\CitySectorFPS\\data\\littextured");  

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
	//my_test_mesh->add_prism_element_count(segment_count, slice_count, v_count, t_count);
	//my_test_mesh->create_arrays(v_count, t_count * 6);
	//my_test_mesh->create_prism(CVector3f(1, 1, 1), 20, 1, segment_count, slice_count);

	my_test_mesh->create_arrays(8, 12);
	my_test_mesh->create_cube(CVector3f(-10, -10, -10), CVector3f(10, 10, 10));

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

Vector3 objectOrientation(0, 0, 0);
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
	CheckGLErrors("Error has occured lol2");

	// Load the vertex data
	TextureManager::Inst()->BindTexture(0, 0);
	static unsigned char indices[] = { 0, 1, 3, 3, 1, 2 };
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glUseProgram(mLightingProgram);
	
	Matrix4 objmat;
	objmat *= objmat.rotate(objectOrientation.x, Vector3(1, 0, 0));
	objmat *= objmat.rotate(objectOrientation.y, Vector3(0, 1, 0));
	objmat *= objmat.rotate(objectOrientation.z, Vector3(0, 0, 1));

	objectOrientation.x += 0.01f;
	objectOrientation.y += 0.1f;

	Vector3 campos(0, 0, -1000.0f);
	Matrix4 viewmatrix = Matrix4::translate(campos);

	Matrix4 projmat = Matrix4::perspective(50.0f, 1000.0, 0.1, 10000.0f);
	//Matrix4 projmat = Matrix4::ortho(-100, 100, -100, 100, 0.1, 10000.0f);

	Matrix4 MVP = projmat *viewmatrix * objmat;
	gLitShaderHandles.Use(my_test_mesh->mVertexBuffer, objmat, MVP, 0);
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
