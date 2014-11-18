#include "main.h"

#include "CMesh.h"
#include "matrix.h"

size_t segment_count, slice_count;

int mvp_matrix_handle = -1;
Matrix4 mat;

CMesh my_test_mesh;
CitySectorFPS::CitySectorFPS()
	: SampleApplication("HelloTriangle", 1280, 720)
{
}

bool CitySectorFPS::initialize()
{
	const std::string vs = SHADER_SOURCE
		(
		precision mediump float;
		uniform mat4 u_mvpmatrix;
		attribute vec4 vPosition;
		varying vec3 vWorldPosition;
	void main()
	{
		gl_Position = vPosition;
		vWorldPosition = vPosition.xyz;
	}
	);

	const std::string fs = SHADER_SOURCE
		(
		precision mediump float;
		varying vec3 vWorldPosition;
		
		
	void main()
	{
		float d = length(vWorldPosition);
		//d = d < 2.0 ? 0.0 : 1.0; // (d* d*d*d) * 100.0;
		gl_FragColor = vec4(d, d, 0.0, 1.0);
	}
	);

	mProgram = CompileProgram(vs, fs);
	if (!mProgram)
	{
		return false;
	}

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

	// Set the viewport
	glViewport(0, 0, getWindow()->getWidth(), getWindow()->getHeight());

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Use the program object
	glUseProgram(mProgram);

	// Load the vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(0);
	my_test_mesh.draw();

	glDrawArrays(GL_TRIANGLES, 0, 3);
}


int main(int argc, char **argv)
{
	CitySectorFPS app;

	mat.perspective(90.0, 1.0, 0.1f, 1000.0f);
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
