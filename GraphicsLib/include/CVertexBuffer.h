/********************************************//**
 * \copyright (c) 2014 
 * \file   	CVertexBuffer.h
 * \Author 	Tim Lewis (tim@totga-games.com)
 * \see    	Project: Anthracite
 * \date   	11-09-2014
 * \brief  	CVertexBuffer
 * CVertexBuffer is a class used to encapsulate opengl's vertex buffer objects.
 ***********************************************/

#ifndef CVertexBuffer_h_included
#define CVertexBuffer_h_included
#include "EVertexType.h"
#include <assert.h>
class COGLGraphics;


/********************************************//**
* \brief 	EVertexBufferFlags is an enum used to determine access modes to this vertex buffer
* \details 	EVertexBufferFlags is an enum used to determine access modes to this vertex buffer
***********************************************/
enum EVertexBufferFlags
{
	EVB_DONT_CLEAR	  = 0x1,  //when buffers are flushed, don't clear this buffer
	EVB_DYNAMIC		  = 0x2,  //indicates the VB will be dynamically updated
	EVB_LOCK_DISCARD  = 0x4,  //Buffer can be discarded after lock
	EVB_WRITE_ONLY	  = 0x8,  //No reads from locked buffers
	EVB_NO_OVERWRITE  = 0x10, //Buffer will be written to sequentially, speed hint.
	EVB_POINTS		  = 0x20, //Buffer will use old-school point list
	EVB_NOT_USING_FVF = 0x40,  //not using FVF codes

	EVB_KEEP_BUFFER_FLAGS = EVB_DONT_CLEAR
};


/********************************************//**
* \brief 	CVertexBuffer  is a class used to encapsulate opengl's vertex buffer objects.
* \details 	CVertexBuffer 
 * As with CIndexBuffer, this class acts as a buffer between CPU memory and GPU memory.
 * Modern graphics hardware requires its own (very fast) memory and we have to manage how
 * we transfer data to the GPU (preferably as infrequently as possible as this is a potential performance bottleneck)
 * This class handles this task for vertex information as well as providing some useful functionality for applications
 * using vertex buffers.
***********************************************/

class CVertexBuffer
{
public:
	typedef SVertex_P_D4B_N_UV VertexType;
		
	CVertexBuffer(EVertexType format, size_t capacity,unsigned int flags);                                ///< Constructor with format, capacity amd flags
	~CVertexBuffer();                                                                                     ///< Destructor - releases CPU and GPU data.
	unsigned int GetFlags() const {return mFlags;}                                                       ///< Returns our flags (see 'EVertexBufferFlags')
	unsigned int CreateGPUBuffer();                                                                       ///< Creates the GPU side buffer
	size_t		 GetVertexSize() const {return mVertexSize;}                                            ///< Returns the size of each vertex (in bytes)
	EVertexType	 GetVertexFormat() const {return mFormat;}                                               ///< Returns the enum representing our vertex format
	void		 Resize(unsigned int new_size, EVertexType format);                                       ///< Resize the vertex buffer
	void*		 Lock(size_t startidx, size_t size, unsigned int flags=0);                                ///< Lock the vertex buffer (with optional range)
	void*		 Lock(unsigned int flags=0);                                                              ///< Lock the vertex buffer
	unsigned int UnLock(); 																				  ///< Unlock the vertex buffer (also transmits changes to GPU)
	unsigned int GetGLHandle() const {return mGlHandle;}                                                ///< Get the OpenGL handle to the GPU vertex buffer
	size_t		 Size() const {return mCapacity;}                                                        ///< return the capacity of the vertex buffer
	size_t		 GetCapacity() const {return mCapacity;}                                                 ///< synonym for above
	void		 Clear() {mCount = 0;}                                                                   ///< Clears the vertex buffer - does not release any memory, just resets the count variable
	size_t		 GetCount() const {return mCount;}                                                       ///< returns the vertex count - the number of vertices we're actually using
	void		 EnsureCapacity(size_t count);                                                            ///< Ensure that the vertex buffer has at least 'count' capacity
	template <class VERTEX_TYPE>                                                                          ///< Templatized function for adding a vertex (defined below this class)
	void		  AddVertex(const VERTEX_TYPE &vertex);
	
protected:		
	unsigned int			   mGlHandle;		///<The opengl handle to the GPU vertex buffer
	unsigned int			   mFlags;			///<Flags of form 'EVB_XXXX', e.g. EVB_WRITE_ONLY | EVB_DYNAMIC | EVB_DONT_CLEAR
	unsigned int			   mOglFlags;     ///OpenGL has its own set of flags, which we calculate from the above variable and cache here
	size_t					   mVertexSize;   ///<Size (in bytes) of each vertex
	size_t					   mLockSize;		///<Size of data that was locked - useful for only copying a portion of the buffer
	size_t					   mLockOffset;	///<Offset in to the buffer to lock
	size_t					   mCount;			///<number of vertices in use
	EVertexType				   mFormat;	 	///<Whislt there's only one vertex type at time of writing, this is built for expansion
	size_t					   mCapacity;      ///<Total capacity of vertex buffer
	void					 * mData;			///<Pointer to CPU side vertex buffer data
	
};

template <class VERTEX_TYPE>                                                                          ///< Templatized function for adding vertex
void CVertexBuffer::AddVertex(const VERTEX_TYPE &vertex)                                                    
{
	EnsureCapacity(mCount + 1);
	ASSERT(mFormat == VERTEX_TYPE::VERTEX_TYPE) //check this is the same vertex format as the mesh			
	((VERTEX_TYPE*)mData)[mCount++] = vertex;		
	if (mCount > mLockSize)
		mLockSize = mCount;
}
#endif