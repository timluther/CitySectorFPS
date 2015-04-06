#include "main.h"

#include "CMesh.h"
#include "matrix.h"
#include "../FreeImage/FreeImage.h"
#include "TextureManager.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "CWavefrontObjParser.h"
#include "UtilLogger.h"

size_t segment_count = 10, slice_count = 10;

float aspectRatio = 1.0;
int mvp_matrix_handle = -1;
int m_matrix_handle = -1;
Matrix4 mat;
Matrix4 objectmat;

CVector3f lightDirection = CVector3f(1, 0.4, 0.2).normal();
CVector3f skyColour(0.7f, 0.8f, 0.9f);
CVector3f groundColour(0.3f, 0.2f, 0.1f);

CMesh *house_mesh = NULL;
CMesh *my_test_mesh = NULL;
CitySectorFPS::CitySectorFPS()
	: SampleApplication("HelloTriangle", 1280, 720)
{
}

struct sShaderHandles
{
	int uMvpMatrix;
	int uMMatrix;
	int aTexCoord;
	int aPosition;
	int aNormal;
	int TextureSampler;
	int MaterialColour;

	int ulightNormal;
	int uSkyColour;
	int uGroundColour;


	void Init(int ProgramHandle)
	{
		MaterialColour = glGetUniformLocation(ProgramHandle, "MaterialColour");
		uMMatrix = glGetUniformLocation(ProgramHandle, "u_mmatrix");
		uMvpMatrix = glGetUniformLocation(ProgramHandle, "u_mvpmatrix");		
		TextureSampler = glGetUniformLocation(ProgramHandle, "TextureSampler");
		ulightNormal = glGetUniformLocation(ProgramHandle, "uLightNormal");
		uSkyColour = glGetUniformLocation(ProgramHandle, "uSkyColour");
		uGroundColour = glGetUniformLocation(ProgramHandle, "uGroundColour");
		aNormal = glGetAttribLocation(ProgramHandle, "aNormal");
		aTexCoord = glGetAttribLocation(ProgramHandle, "aTexCoord");
		aPosition = glGetAttribLocation(ProgramHandle, "aPosition");

	}

	void ApplyUniforms(const Matrix4 &mat, const Matrix4 &MVPmatrix, const Matrix4 &WorldMatrix, int texid)
	{
		if (uMvpMatrix != -1)
			glUniformMatrix4fv(uMvpMatrix, 1, false, (const float*)&MVPmatrix);

		if (uMMatrix != -1)
			glUniformMatrix4fv(uMMatrix, 1, false, (const float*)&WorldMatrix);
		if (MaterialColour != -1)
			glUniform4f(MaterialColour, 1.0f, 1.0f, 1.0f, 1.0f);

		if (ulightNormal != -1)
			glUniform3f(ulightNormal, lightDirection.x, lightDirection.y, lightDirection.z);
		if (uSkyColour != -1)
			glUniform3f(uSkyColour, skyColour.x, skyColour.y, skyColour.z);
		if (uGroundColour != -1)
			glUniform3f(uGroundColour, groundColour.x, groundColour.y, groundColour.z);


		CheckGLErrors("Error has occured lol7");

		if (MaterialColour != -1)
			glUniform4f(MaterialColour, 1.0f, 1.0f, 1.0f, 1.0f);
		unsigned int GLtexid = TextureManager::Inst()->GetTextureId(texid);

		if (TextureSampler != -1)
		{
			unsigned int GLtexid = TextureManager::Inst()->GetTextureId(texid);
			glUniform1i(TextureSampler, 0);
		}
	}

#define BUFFER_OFFSET(i) ((char*)NULL +(i << 2))
	void Use(GLfloat *vertices, GLfloat *texturecoordinates, GLfloat *normals, Matrix4 &MVPmatrix, const Matrix4 &WorldMatrix, int texid)
	{
		int stride = 36;
		CheckGLErrors("Error has occured lol3");
		glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices);		
		if (aPosition != -1)
			glEnableVertexAttribArray(aPosition);
		if (aNormal != -1)
			glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, 0, normals);
		glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texturecoordinates);
		if (aTexCoord != -1)
			glEnableVertexAttribArray(aTexCoord);
		CheckGLErrors("Error has occured lol4");
		ApplyUniforms(mat, MVPmatrix, WorldMatrix, texid);				
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
	
		glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, buffer.GetVertexSize(), (void*)0);			//these are offsets in to a data structure 
		if (aPosition != -1)
			glEnableVertexAttribArray(aPosition);
		CheckGLErrors("Error has occured lol9");


		glVertexAttribPointer(aNormal, 2, GL_FLOAT, GL_FALSE, buffer.GetVertexSize(), (void*)12);
		if (aNormal != -1)
			glEnableVertexAttribArray(aNormal);

		glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, buffer.GetVertexSize(), (void*)24);
		if (aTexCoord != -1)
			glEnableVertexAttribArray(aTexCoord);


		ApplyUniforms(mat, MVPmatrix, Matrix4(), texid);
		
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

std::string PathOrParentOf(const std::string &filename)
{
	size_t idx = filename.find_last_of('\\');
	if (idx == filename.length() - 1)
		idx = filename.find_last_of('\\', filename.length() - 2);
	if (idx != std::string::npos)
	{
		return filename.substr(0, idx);
	}
}

COutputdebugStringStream outputlog(128);
std::ofstream			 mLog;

bool CitySectorFPS::initialize()
{	
	LOGinit(&mLog);
	LOG.rdbuf(&outputlog);

	std::cout << "Starting City Sector" << std::endl;
	//std::string basestr = "C:\\Users\\Brython\\Documents\\Code\\CitySectorFPS\\data\\";

	char module_filename[512];
	GetModuleFileNameA(NULL, module_filename, 512);
	std::string ApplicationDataDir = PathOrParentOf(PathOrParentOf(PathOrParentOf(module_filename)));
	ApplicationDataDir += "\\data\\";

	mProgram = LoadShadersFromFile((ApplicationDataDir + "simpleshader").c_str());
	mLightingProgram = LoadShadersFromFile((ApplicationDataDir + "littextured").c_str());

	if (!mProgram)
	{
		return false;
	}

	
	CWavefrontObjParser parser;
	parser.initParser();

	//parser.openFile((ApplicationDataDir + "houseshaded.obj").c_str());             ///<Open a file for reading     
	parser.openFile((ApplicationDataDir + "testshapefromblender.obj").c_str());             ///<Open a file for reading     

	house_mesh = new CMesh(100000,32000*6);
	parser.parseMesh(*house_mesh, false);	  ///<Parse the mesh in to a CCmpMesh structure               	


	gShaderHandles.Init(mProgram);
	gLitShaderHandles.Init(mLightingProgram);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	house_mesh->fill_GPU_buffers();
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
	//C:\\Users\\Brython\\Documents\\Code\\CitySectorFPS\\
		
	TextureManager::Inst()->LoadTexture((ApplicationDataDir + "testimage.png").c_str(), 0);
	TextureManager::Inst()->LoadTexture((ApplicationDataDir + "HouseAmbientOcclusion.png").c_str(), 1);


	return true;
}



CVector3f camPos(0, 0, -100.0f);
CVector3f camOrientation(0, 0, 0);
CVector3f objectSpin(0, 0, 0);
CVector3f objectOrientation(0, 0, 0);

CVector3f viewDirection(0, 0, 1);
CVector3f viewStrafeDirection(1, 0, 0);
CVector3f viewUpDirection(0, 1, 0);

enum EDepthFunction
{
	ZF_NEVER,
	ZF_LESS,
	ZF_LESS_EQUAL,
	ZF_EQUAL,
	ZF_GREATER_EQUAL,
	ZF_GREATER,
	ZF_ALWAYS
};


void SetZFunc(EDepthFunction func)
{
	if (func == ZF_NEVER)
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);
	}
	else if (func == ZF_LESS)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
	else if (func == ZF_LESS_EQUAL)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}
	else if (func == ZF_EQUAL)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_EQUAL);
	}
	else if (func == ZF_GREATER_EQUAL)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GEQUAL);
	}
	else if (func == ZF_GREATER)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GREATER);
	}
	else if (func == ZF_ALWAYS)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);
	}
	else
	{
	
	}

}



void CitySectorFPS::handleEvent(Event *event)
{
	static float speed = 1.0;
	static float mouseSensitivity = 0.05f;
	static int buttonstatus = 0;
	static int lastMouseX = 0;
	static int lastMouseY = 0;
	switch (event->Type)
	{
		case Event::EVENT_MOUSE_BUTTON_PRESSED:
		{
			buttonstatus |= 1 << event->MouseButton.Button;						
			break;
		}
		case Event::EVENT_MOUSE_BUTTON_RELEASED:
		{
			buttonstatus &= ~(1 << event->MouseButton.Button);
			
			break;
		}
		case Event::EVENT_MOUSE_MOVED:
		{
			if (buttonstatus != 0)
			{
				int MouseDeltaX = event->MouseMove.X - lastMouseX;
				int MouseDeltaY = event->MouseMove.Y - lastMouseY;
				camOrientation.y += float(MouseDeltaX) * mouseSensitivity;
				camOrientation.x += float(MouseDeltaY) * mouseSensitivity;
			
			}				 
			lastMouseX = event->MouseMove.X;
			lastMouseY = event->MouseMove.Y;
			break;
		}
		case Event::EVENT_KEY_PRESSED:
		{
			switch (event->Key.Code)
			{
			case KEY_G:
				objectSpin.x -= 0.1;break;
			case KEY_H:
				objectSpin.x += 0.1;break;
			case KEY_W:
				camPos += viewDirection * speed; break;
			case KEY_S:
				camPos -= viewDirection * speed; break;
			case KEY_A:
				camPos += viewStrafeDirection * speed; break;
			case KEY_D:
				camPos -= viewStrafeDirection * speed; break;
			}
			std::cout << "cam pos: " << camPos.x << ", " << camPos.y << ", " << camPos.z << std::endl;
			break;
		}
	}
}

void CitySectorFPS::destroy()
{
	glDeleteProgram(mProgram);
}


void CitySectorFPS::draw()
{
	GLfloat vertices[] =
	{
		-1.0f, -1.0f, 10000.0f,
		1.0f, -1.0f, 10000.0f,
		1.0f, 1.0f, 10000.0f,
		-1.0f, 1.0f, 10000.0f,
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
	aspectRatio = getWindow()->getWidth() / (float)getWindow()->getHeight();

	// Clear the color buffer
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClearDepthf(10000.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	CheckGLErrors("Error has occured alpha 1");
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	CheckGLErrors("Error has occured alpha 2");
	glUseProgram(mLightingProgram);
	
	Matrix4 objmat = Matrix4::identity();
	/*objmat *= objmat.rotate(objectOrientation.x, CVector3f(1, 0, 0));
	objmat *= objmat.rotate(objectOrientation.y, CVector3f(0, 1, 0));
	objmat *= objmat.rotate(objectOrientation.z, CVector3f(0, 0, 1));*/

	objectOrientation += objectSpin;
	Matrix4 viewmatrix = Matrix4::identity();
	viewmatrix *= viewmatrix.rotate(camOrientation.x, CVector3f(1, 0, 0));
	viewmatrix *= viewmatrix.rotate(camOrientation.y, CVector3f(0, 1, 0));
	

	viewDirection = viewmatrix.column(2);
	viewStrafeDirection = viewmatrix.column(0);// CVector3f(0, 1, 0).cross(viewDirection);
//	CVector3f testviewStrafeDirection = viewmatrix.column(0);
	//viewmatrix *= viewmatrix.rotate(camOrientation.z, CVector3f(0, 0, 1));
	viewmatrix *= Matrix4::translate(camPos);
	
	Matrix4 projmat = Matrix4::perspective(50.0f, aspectRatio, 0.1, 10000.0f);
	//Matrix4 projmat = Matrix4::ortho(-100, 100, -100, 100, 0.1, 10000.0f);

	Matrix4 MVP = projmat * viewmatrix;// *objmat;//

	//Matrix4 MVP = objmat * viewmatrix * projmat;
	CheckGLErrors("Error has occured alpha");
	//gLitShaderHandles.Use(my_test_mesh->mVertexBuffer, objmat, MVP, 0);
	//my_test_mesh->draw();
	SetZFunc(ZF_LESS_EQUAL);
	gLitShaderHandles.Use(house_mesh->mVertexBuffer, objmat, MVP, 0);
	TextureManager::Inst()->BindTexture(0, 1);
	house_mesh->draw();
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
