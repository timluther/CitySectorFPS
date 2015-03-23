#ifdef COBJPARSER_EXPORTS
#define COBJPARSER_API __declspec(dllexport) 
#else
#define COBJPARSER_API __declspec(dllimport) 
#endif

//Derived from CObjParser with fixes and made to work with 'CCmpMesh'

#ifndef CWavefrontObjParser_h_included
#define CWavefrontObjParser_h_included

#include <vector>
#include <stdio.h>


#include "CMesh.h"

/********************************************//**
* \brief 	CWavefrontObjParser is a class used for parsing wavefront 'obj' 3d object/scene files
* \details 	This class enables the loading of 3d models using the popular wavefront obj format

***********************************************/
class CWavefrontObjParser
{
	private:
		char  mReadHeader[256];                         ///<File header                            
		FILE *mPfile;                                    ///<File pointer
	
	public:
		bool initParser();                                ///<Initialise the parser         
		bool openFile(const char* pFileName);             ///<Open a file for reading     
		bool headerCompare(char header);                  ///<                      
		void parseMesh(CMesh &mesh, bool flip = false);	  ///<Parse the mesh in to a CCmpMesh structure               
		static CVector3f mesh_scale_factor;				  ///<Mesh scale factor - usually set to 1,1,1 but can be changed if required
};


#endif