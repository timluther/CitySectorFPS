#include "main.h"

CitySectorFPS::CitySectorFPS()
	: SampleApplication("HelloTriangle", 1280, 720)
{
}

bool CitySectorFPS::initialize()
{
	const std::string vs = SHADER_SOURCE
		(
		attribute vec4 vPosition;
	void main()
	{
		gl_Position = vPosition;
	}
	);

	const std::string fs = SHADER_SOURCE
		(
		precision mediump float;
	void main()
	{
		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
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

	glDrawArrays(GL_TRIANGLES, 0, 3);
}


int main(int argc, char **argv)
{
	CitySectorFPS app;
	app.initialize();
	return app.run();
}
