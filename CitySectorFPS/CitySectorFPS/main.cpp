#include "main.h"

#include "CMesh.h"
#include "matrix.h"
#include "../FreeImage/FreeImage.h"
#include "TextureManager.h"
size_t segment_count, slice_count;

int mvp_matrix_handle = -1;
Matrix4 mat;

CMesh my_test_mesh;
CitySectorFPS::CitySectorFPS()
	: SampleApplication("HelloTriangle", 1280, 720)
{
}



struct sShaderHandles
{
	int uMvpMatrix;
	int aTexCoord;
	int aPosition;
	int TextureSampler;


	void Init(int ProgramHandle)
	{
		uMvpMatrix = glGetUniformLocation(ProgramHandle, "u_mvpmatrix");		
		TextureSampler = glGetUniformLocation(ProgramHandle, "TextureSampler");

		aTexCoord = glGetUniformLocation(ProgramHandle, "aTexCoord");
		aPosition = glGetUniformLocation(ProgramHandle, "aPosition");
	}

	void Use(GLfloat *vertices, GLfloat *texturecoordinates, Matrix4 &mat, int texid)
	{
		glVertexAttribPointer(aPosition, 3, GL_FLOAT, GL_FALSE, 0, vertices);
		glEnableVertexAttribArray(aPosition);
		glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, GL_FALSE, 0, texturecoordinates);
		glEnableVertexAttribArray(aTexCoord);

		glUniformMatrix4fv(uMvpMatrix, 1, false, (const float*)&mat);
		glUniform1i(TextureSampler, texid);


	}
};

sShaderHandles gShaderHandles;

bool CitySectorFPS::initialize()
{
	const std::string vs = SHADER_SOURCE
		(
		precision mediump float;
		uniform mat4 u_mvpmatrix;
		attribute vec2 aTexCoord;
		attribute vec4 aPosition;
		varying vec3 vWorldPosition;
		varying vec2 vTexCoord;
	void main()
	{
		gl_Position = aPosition;
		vWorldPosition = aPosition.xyz;
		vTexCoord = aTexCoord;
	}
	);

	const std::string fs = SHADER_SOURCE
		(
		uniform sampler2D TextureSampler;
		precision mediump float;
		varying vec3 vWorldPosition;
		varying vec2 vTexCoord;
		
	void main()
	{
		float d = length(vWorldPosition);
		vec4 texcolour = texture2D(TextureSampler, vTexCoord);
		//d = d < 2.0 ? 0.0 : 1.0; // (d* d*d*d) * 100.0;
		gl_FragColor = vec4(d, d, 0.0, 1.0) * texcolour;
	}
	);

	mProgram = CompileProgram(vs, fs);
	if (!mProgram)
	{
		return false;
	}

	gShaderHandles.Init(mProgram);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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
		0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
	};

	GLfloat texcoords[] =
	{
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0
	};

	// Set the viewport
	glViewport(0, 0, getWindow()->getWidth(), getWindow()->getHeight());

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	TextureManager::Inst()->BindTexture(0);
	// Use the program object
	glUseProgram(mProgram);
	TextureManager::Inst()->BindTexture(0);

	gShaderHandles.Use(vertices, texcoords, mat, 0);

	// Load the vertex data
	
	my_test_mesh.draw();

	glDrawArrays(GL_TRIANGLES, 0, 3);
}


int main(int argc, char **argv)
{
	CitySectorFPS app;
	
	mat.perspective(90.0, 1.0, 0.1f, 1000.0f);
	//TODO: make sure this uses relative paths some day
	TextureManager::Inst()->LoadTexture("C:\\Users\\Brython\\Documents\\Code\\CitySectorFPS\\data\\testimage.png", 0);

	
	/*try
	{*/
		my_test_mesh.create_prism(CVector3f(0, 0, 0), 20, 40, segment_count, slice_count);
	/*}
	catch (COutOfBoundsException e)
	{
		std::cout << "Out of bounds error in file " << e.m_src_file_name << " on line " << e.m_line << " had index " << e.m_index << " but array only has capacity of " << e.m_capacity << std::endl;
		my_test_mesh.destroy();
		exit(0);
		//TODO: send email to Brython
	}*/


	app.initialize();
	return app.run();
}
