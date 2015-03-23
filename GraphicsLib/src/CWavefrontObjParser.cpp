#include "CHash.h"
#include "CWavefrontObjParser.h"
#include "stddef.h"
#include <string.h>
#include "CHashMap.h"
#include <iostream>
//#include "UtilTextFunctions.h"
#include "CMesh.h"
#include "CVector3f.h"
//#define USE_TIMER
#ifdef USE_TIMER
#include "../../Foundation/CHighResolutionTimer.h"
#endif


// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
// Tips on the data that OpenGL will need.

bool CWavefrontObjParser::openFile(const char* pFileName)
{
	std::cout << "Reading file:" <<pFileName << std::endl;
	int err = fopen_s(&mPfile,pFileName,"r");
	if (err != 0)
	{		
		
		throw std::exception("Problem reading file", 0);
		return false;
	}

	return true;
}

// http://www.cs.bu.edu/teaching/c/file-io/intro/
bool CWavefrontObjParser::initParser()
{
	
	return true;
} 

bool CWavefrontObjParser::headerCompare(char header)
{
	
	//int ck = 
	//fprintf(mPdebugfile,"Checked file is %i...\n",ck);
	
	if (fscanf_s(mPfile,"%s",mReadHeader,sizeof(mReadHeader)) != EOF )
	{ // only do this if it isn't the end of the file!
		
		std::cout << "Comparing read " << mReadHeader << "with user's " << header;

		// a zero value means it's equal
		if (strcmp(mReadHeader,&header) == 0)
		{
			std::cout << "Matched!\n";
			
			// analyse for f format
			if(strcmp(mReadHeader,"f") == 0)
			{
				int a,b;
				fscanf_s(mPfile,"%d//%d",&a,&b);
				std::cout << a << " and " << b << "was retrieved.\n\n";
			}
				
			else if(strcmp(mReadHeader,"v") == 0)
			{
				float x,y,z;
				fscanf_s(mPfile,"%f%f%f",&x,&y,&z);
				std::cout << x << ", " << y << ", " << z << " were retrived. " << std::endl;				
			}
			return true;
		} else 
		{
			std::cout << "Didn't match..." << std::endl;
			return false;
		}
	} 
	else
	{
		// check if EOF is really eof or an error
		if (feof(mPfile)) 
			std::cout << "EOF!!" << std::endl;
		if (ferror(mPfile)) 
			std::cout << "File Error!!" << std::endl;
		return false;
	}
}


void splitInput(std::vector<std::string> &splits, const std::string &input, char delim)
{
	size_t last_idx = 0;
	size_t idx = 0 ;
 	while (idx < input.length())
 	{
 		if (input[idx] == delim)
 		{
			if (idx != last_idx)
 				splits.push_back(input.substr(last_idx, idx - last_idx));
 			++idx;
 			last_idx = idx;
 		}
 		++idx;
 	}
   
 	if (last_idx != idx)
 	{
 		splits.push_back(input.substr(last_idx, idx - last_idx));
 		last_idx = idx;
 	}
 }

CVector3f CWavefrontObjParser::mesh_scale_factor(1,1,1);


bool isEOL(char ch)
{
	return (ch == 0 || ch == '\t' || ch == '\n');
}

bool IsWhiteSpace(char c)
{
	return (isspace((unsigned char)c) || c == '\n' || c == '\r');
}
///! Thse values returns flags if parsing passed End of line or End of file
enum
{
	EPF_PASSED_EOL = 0x1,	///<Passed end of line
	EPF_PASSED_EOF = 0x2,   ///<Passed end of file
};


unsigned int skipWhiteSpace(const char *&str)
{
	unsigned int flags = 0;
	while (1)
	{
		if (IsWhiteSpace(*str))
		{
			++str;
		}
		else if (isEOL(*str))
		{
			while (isEOL(*str))
				++str;

			flags |= EPF_PASSED_EOL;
		}
		else if (*str == 0)
		{
			flags |= EPF_PASSED_EOF;
			break;
		}
		else
			break;
	}
	return flags;
}



const char* parseTo(const char *c, char test_ch)
{
	const char *cc = c;
	while ((*cc != test_ch) && (*cc != 0))
	{
		++cc;
	}
	return cc;
}

inline int  toInt(char ch) { return ch - '0'; }
inline bool isDigit(char ch) { return (ch >= '0') && (ch <= '9'); }
int atoiSimpleCount(const char *str, int count)
{
	int res = 0;
	const char *end = &str[count];
	bool negate = false;
	const char *ch = str;
	int multiply = 1;
	skipWhiteSpace(ch);
	if (*ch == '-')
		negate = true;

	while ((ch != end) && (*ch != 0) && isDigit(*ch))
	{
		res = (res * 10) + toInt(*ch);
		++ch;
	}
	return negate ? -res : res;
}

void CWavefrontObjParser::parseMesh(CMesh &mesh, bool flip)
{
	
	std::cout << "Parsing mesh" << std::endl;
	struct sUniqueVertexIndex
	{
		sUniqueVertexIndex(){}
		sUniqueVertexIndex(size_t pos,size_t normal,size_t uv) : mPosition(pos), mNormal(normal), mUv(uv){}
		sUniqueVertexIndex(const sUniqueVertexIndex &other) : mPosition(other.mPosition), mNormal(other.mNormal), mUv(other.mUv){}

		CHash GetHash() const 
		{
			CHash hash(mPosition);
			hash.next_hash(mNormal);
			hash.next_hash(mUv);
			return hash;
		}
		size_t mPosition;
		size_t mNormal;
		size_t mUv;
	};

	std::vector<CVector3f> positions;
	std::vector<CVector3f> normals;
	std::vector<CVector2f> uvs;

	size_t vertex_capacity = mesh.m_vertex_capacity;
	positions.reserve(vertex_capacity);
	normals.reserve(vertex_capacity);
	uvs.reserve(vertex_capacity);
	CVector3f t_vertex;
	CVector3f t_normal;
	CVector2f t_uv;
	enum {MAX_FACE_EDGE_COUNT = 64};
	
	
	int	  tmpFace[MAX_FACE_EDGE_COUNT * 3];

	std::vector<sUniqueVertexIndex> vVertexIndices;

	
	CHashMapInt *sUniqueVertices = new CHashMapInt(); 	

	//TODO: read these in properely
	
	mesh.mVertexBuffer.Lock();
	mesh.mIndexBuffer.Lock();
	// The while-read only consumes the first part of the header. 
#ifdef USETIMER
	CHighResolutionTimer ctimer;
	ctimer.StartHRTimer();
#endif
	while(fscanf_s(mPfile,"%s",mReadHeader,sizeof(mReadHeader)) != EOF )
	{
		// Match a Vertex
		if (strcmp(mReadHeader,"v") == 0)
		{
			fscanf_s(mPfile,"%f%f%f",&t_vertex[0],&t_vertex[1],&t_vertex[2]);
			// range-based looper for each element (doesn't work for this compiler)			
			positions.push_back(t_vertex * mesh_scale_factor);
		}
		else
		if (strcmp(mReadHeader,"vn") == 0)
		{
			fscanf_s(mPfile,"%f%f%f",&t_normal[0],&t_normal[1],&t_normal[2]);
			normals.push_back(t_normal);
		}
		else
		if (strcmp(mReadHeader,"vt") == 0)
		{
			fscanf_s(mPfile,"%f%f",&t_uv.x,&t_uv.y);
			uvs.push_back(t_uv);
		}
		else
		//mMesh.beg
		
		if (strcmp(mReadHeader,"f") == 0)
		{
			char szLine[4096];
			fgets(szLine, 4096, mPfile);
			int cidx = 0;

			int pcidx = 0;
			
			int element_count = 0;
			int max_element_count =0;
	
			const char *ch = szLine;
			while (!isEOL(*ch))
			{
				if (IsWhiteSpace(*ch))
				{
					if (element_count > max_element_count)
						max_element_count = element_count;
					element_count = 0;
					skipWhiteSpace(ch);
				}
				
				const char *startch = ch;
				while (isalnum(*ch)) {++ch;}				
				tmpFace[cidx++] = atoiSimpleCount(startch, ch - startch);
				if (*ch == '/')
				{
					++element_count;
					while (*ch=='/') {++ch;}
				}			
			}

			element_count = max_element_count + 1;
			size_t vcount = (cidx) / element_count;
			
			std::vector<size_t> polygon_indices;
			int *cface= tmpFace;
			if (uvs.size() == 0)
				uvs.push_back(CVector2f(0,0));
			
			for (unsigned int i = 0; i < vcount; ++i)
			{

				sUniqueVertexIndex vref = (element_count == 2) ? sUniqueVertexIndex(cface[0]-1, cface[1]-1, 0) : sUniqueVertexIndex(cface[0]-1, cface[2]-1, cface[1]-1);
				cface += element_count;
				CHash hash = vref.GetHash();
				//hash.next_hash(t_uvIndex[i])
				auto itr = sUniqueVertices->find(hash);
				size_t vIndex = 0;
				if (itr == sUniqueVertices->end())
				{
					vIndex = vVertexIndices.size(); //could 
					sUniqueVertices->insert(CHashMapInt::value_type(hash, vIndex));
					vVertexIndices.push_back(vref); //OBJ starts indices at 1 but C++ starts at 0						
					if ((vref.mPosition>positions.size()) || (vref.mNormal>normals.size()) || (vref.mUv>uvs.size()))
					{
						int a = 10;
					}
					
					mesh.mVertexBuffer.AddVertex(SVertex_P_D4B_N_UV(positions[vref.mPosition], vref.mNormal < normals.size() ? normals[vref.mNormal] : CVector3f(1,0,0), vref.mUv < uvs.size() ? uvs[vref.mUv] : CVector2f(0,0), 0xFFFFFFFF));
					//add 
				}
				else
				{
					vIndex = itr->second;
				}
				
				polygon_indices.push_back(vIndex);
			}	
		
			mesh.add_polygon(polygon_indices, flip);
		}
		
	}

	if (normals.size() == 0)
		mesh.calculate_normals();
#ifdef USE_TIMER
	ctimer.StopHRTimer();
	std::cout << "Obj loader took " << ctimer.GetElapsedTime() << std::endl;
	ctimer.StartHRTimer();
#endif
	mesh.mIndexBuffer.UnLock(); //Copy back to GPU
	mesh.mVertexBuffer.UnLock();
	//mesh.CalculateGeometryMetrics();
#ifdef USE_TIMER
	ctimer.StopHRTimer();
	std::cout << "Uploading data to GPU took " << ctimer.GetElapsedTime() << std::endl;
	ctimer.StartHRTimer();
#endif 
	//Very, very slow in windows and in debug (fine in release, fine in Linux) so for the sake of sanity, I'm causing a memory leak in debug.
#ifndef _DEBUG
	sUniqueVertices->clear();
	delete sUniqueVertices;
#endif
	#ifdef USE_TIMER
	ctimer.StopHRTimer();
	std::cout << "Clearing map took " << ctimer.GetElapsedTime() << std::endl;
	#endif
	fclose(mPfile);
}
