#include "gl_includes.h"
#include "CVertexBuffer.h"
#include <malloc.h>
#include <iostream>

#include <string>

class COGLGraphics;

	CVertexBuffer::CVertexBuffer(EVertexType format, size_t size, unsigned int flags)
		:mFlags(flags)
		,mCapacity(size)		
		,mFormat(format)
		,mVertexSize(get_vertex_size(format))
		,mData(NULL)
		,mGlHandle(-1)
		,mOglFlags()
		,mLockSize(0)
		,mLockOffset(0)
		,mCount(0)		
	{
		CreateGPUBuffer();
	}

	CVertexBuffer::~CVertexBuffer()
	{
		glDeleteBuffers(1, &mGlHandle);
		free(mData);
	}
	
	void  CVertexBuffer::EnsureCapacity(size_t capacity)
	{	
		if (!mData)
		{
			mCapacity = capacity;
			mData = malloc(mVertexSize * mCapacity);
			return;
		}
		if (capacity > mCapacity) 
		{	
			size_t new_size = capacity + (mCapacity >> 1); //to ensure we don't allocate and re-allocate too often, allocate more memory using this policy (new desired capacity plus half of old)
			void *new_data = malloc(mVertexSize * new_size);			
			std::memcpy(new_data, mData, mCapacity * mVertexSize);
			free(mData);
			mCapacity = new_size;
			mData = new_data;
		}
	}
	
	void *CVertexBuffer::Lock(unsigned int flags)
	{
		flags |= mFlags;	
		if (!mData)
			mData = malloc(mVertexSize * mCapacity);
		mLockSize = mCapacity;
		mLockOffset = 0;
		return mData;		
	}

	void* CVertexBuffer::Lock(size_t startidx, size_t size, unsigned int flags)
	{
		assert((startidx + size) <= mCapacity);
		flags |= mFlags;
		if (!mData)
			mData = malloc(mVertexSize * mCapacity);
		mLockSize = size;
		mLockOffset = startidx;
		unsigned int oglflags = 0;
		return &((char*)mData)[startidx * mVertexSize];
	}

	unsigned int CVertexBuffer::UnLock()
	{
		if (mLockSize == 0)
		{
			mLockSize = mCount;
			mLockOffset = 0;
		}
		glBindBuffer(GL_ARRAY_BUFFER, mGlHandle);

		if (mLockOffset == 0)
			glBufferData(GL_ARRAY_BUFFER, mLockSize * mVertexSize, mData, mOglFlags);
		else
			glBufferSubData(GL_ARRAY_BUFFER, mLockOffset, mLockSize * mVertexSize, &((char*)mData)[mLockOffset * mVertexSize]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);		
		return 0;
	}


	unsigned int CVertexBuffer::CreateGPUBuffer()
	{
		
		mVertexSize=  get_vertex_size(mFormat);
		if ((mFlags & EVB_KEEP_BUFFER_FLAGS) != 0)
			mData = malloc(mVertexSize * mCapacity);

		glGenBuffers(1, &mGlHandle);
		glBindBuffer(GL_ARRAY_BUFFER, mGlHandle);
		if (mGlHandle == 0xFFFFFFFF)
		{
			std::cout << "Error creating vertex buffer" << std::endl;
		}
		//Flags of form 'EVB_XXXX', e.g. EVB_WRITE_ONLY | EVB_DYNAMIC | EVB_DONT_CLEAR
		mOglFlags = ((mFlags & EVB_DYNAMIC) != 0)?GL_DYNAMIC_DRAW:GL_STATIC_DRAW;//((mFlags & IVBF_dynamic) == 0) ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
		CheckGLErrors2("Error before sending data to Vertex Buffer Object");
		if (mData)
			glBufferData(GL_ARRAY_BUFFER,mCapacity << 2, mData, mOglFlags);
		CheckGLErrors2("Error creating bertex buffer Object");
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return 1;
	}

	void CVertexBuffer::Resize(unsigned int new_size, EVertexType new_format)
	{
		if ((new_size > mCapacity) || (new_format != mFormat))
		{
			if (mData)
				free((char*) mData);
			mFormat =  new_format;
			//mVbDecl = COGLGraphics::mD3DVertexDecls[mFormat];
			mVertexSize=  get_vertex_size(mFormat);
			//SAFE_RELEASE(mD3Dvb);
			mCapacity = new_size;
			if ((mFlags & EVB_KEEP_BUFFER_FLAGS) != 0)
				mData = malloc(mVertexSize * mCapacity);
		}
	}
