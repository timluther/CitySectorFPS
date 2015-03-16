#include "SampleApplication.h"
#include "shader_utils.h"

class CitySectorFPS: public SampleApplication
{
public:
	CitySectorFPS::CitySectorFPS();
	virtual bool initialize();
	virtual void destroy();
	virtual void draw();
	virtual void handleEvent(Event *event);
private:
	GLuint mProgram;
	GLuint mLightingProgram;
};

int main(int argc, char **argv);