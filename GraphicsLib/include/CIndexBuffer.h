/********************************************//**
 * \copyright (c) 2014 
 * \file   	CIndexBuffer.h
 * \Author 	Tim Lewis (tim@totga-games.com)
 * \see    	Project: Anthracite
 * \date   	11-09-2014
 * \brief  	CIndexBuffer class
  ***********************************************/
#pragma once

#ifndef CIndexBuffer_h_included
#define CIndexBuffer_h_included 

#include "gl_includes.h"
#include "UtilGL.h"

/********************************************//**
 * \brief  EIndexBufferFlags
 * \details Index buffer usage flags Flags expressed as an enum
 ***********************************************/

enum EIndexBufferFlags
{
	EIB_DONT_CLEAR	  = 0x1, //when buffers are flushed, don't clear this buffer
	EIB_DYNAMIC		  = 0x2, 
	EIB_LOCK_DISCARD  = 0x4,
	EIB_WRITE_ONLY    = 0x8,
	EIB_READ_ONLY     = 0x10,
	EIB_RESIZE_CAN_CHANGE_DATA_TYPE = 0x20
};

class COGLGraphics;

/********************************************//**
 * \brief  CIndexBuffer is used to store mesh index data
 * \details A class that encapsulates an OpenGL index buffer, templatized over the index type.
 *			OpenGL (and other graphics libraries) usually require that data be stored on the GPU's own
  			(very fast) memory for performant code. 
  			This object encapsulates the behaviour of an index buffer, simplifying the managment of index buffer objects
  			and providing some higher level functionality than OpenGL provides.
 ***********************************************/
template <typename INDEX_TYPE>
class CIndexBuffer
{
public:
	typedef INDEX_TYPE index_type;
	
	CIndexBuffer(size_t size);	
	~CIndexBuffer();

	index_type	*Lock();						///<lock the buffer and grab the data.
	void		 UnLock();						///<unlocks the buffer and copies the data to the GPU	
	size_t		 Size() const {return mCapacity;} ///<returns the capacity of the index buffer
	size_t		 Count() const {return mCount;} ///<returns the number of indices in use
	
	//EDataTypeID  IndexType() const{return TGetTypeID<INDEX_TYPE>::TYPE_ID;}			///<returns the type as per our EDataTypeID
	//unsigned int GLIndexType() const {return TGetGLTypeID<INDEX_TYPE>::TYPE_ID;}	///<returns the equivalent GL type code

	void		 Resize(unsigned int new_size);	 ///<Changes the capactiy of the index buffer
	
	void		 Clear();				///<Clears the index buffer - sets the number of indices in use to zero but does not deallocate any memory
	unsigned int CreateGPUBuffer();		///<Creates the GPU side data
	unsigned int GetGLHandle() const {return mGlHandle;} ///<Returns the GL handle for this buffer

	void		 Use();					///<Puts the index buffer in to use - Subsequent appropriate GL draw calls will use the buffer

	size_t		 AddIndex(size_t index);	///<Adds a single index to the index buffer
	size_t		 AddQuad(size_t a, size_t  b, size_t c, size_t d);  ///<Adds a quad using the given indices
	size_t		 AddTriangle(size_t a, size_t b, size_t c);	 ///<Adds a triangle to the index buffer
	size_t		 AddPolygonLoop(size_t start, size_t count); ///<Adds a polygon of 'count' sides to the index buffer using sequential vertices starting at 'start'
	void		 EnsureCapacity(size_t capacity); ///<Ensures we have sufficient capcity in the index buffer
	size_t		 GetCapacity() const {return mCapacity;} ///<Returns the capcity of the index buffer
public:		

	size_t				   mCapacity;	///<The capacity of our index buffer
	size_t				   mCount;     ///<The number of used elements in our index buffer - incremented by 'add_index', 
	unsigned int		   mGlHandle; ///<gl handle to the GPU side copy of the data
	index_type			  *mData;		///<our data (mCapacity in size)	
};


#endif