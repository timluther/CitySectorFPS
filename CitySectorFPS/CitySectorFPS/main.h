#include "SampleApplication.h"
#include "shader_utils.h"

class CitySectorFPS: public SampleApplication
{
public:
	CitySectorFPS::CitySectorFPS();
	virtual bool initialize();
	virtual void destroy();
	virtual void draw();

private:
	GLuint mProgram;
};

int main(int argc, char **argv);