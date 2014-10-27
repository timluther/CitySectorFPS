#include "UtilAssert.h"
#include "CIndexBuffer.h"
#include <string>


template <typename INDEX>
CIndexBuffer<INDEX>::CIndexBuffer(size_t size)
		:mCapacity(size)
		,mCount(0)
		,mData(NULL)
		,mGlHandle(0xFFFFFFFF)
{

}

template <typename INDEX>
CIndexBuffer<INDEX>::~CIndexBuffer()
{
	glDeleteBuffers(1, &mGlHandle);
	free(mData);	 
}

template <typename INDEX>
size_t CIndexBuffer<INDEX>::AddIndex(size_t index)
{
	EnsureCapacity(mCount + 1);
	mData[mCount++] = index;
	return mCount - 1;
}

template <typename INDEX>
size_t CIndexBuffer<INDEX>::AddTriangle(size_t a, size_t b, size_t c)
{
	EnsureCapacity(mCount + 3);
	mData[mCount++] = a;
	mData[mCount++] = b;
	mData[mCount++] = c;
	return mCount - 3;
}

template <typename INDEX>
size_t CIndexBuffer<INDEX>::AddQuad(size_t a, size_t  b, size_t c, size_t d)
{
	EnsureCapacity(mCount + 4);
	mData[mCount++] = a;
	mData[mCount++] = b;
	mData[mCount++] = c;
	mData[mCount++] = d;
	return mCount - 4;
}



template <typename INDEX>
void CIndexBuffer<INDEX>::Clear()
{
	mCount = 0;
}

template <typename INDEX>
void CIndexBuffer<INDEX>::Resize(unsigned int new_size)
{
	if (new_size > mCapacity) 
	{	
		delete [] mData;
		mCapacity = new_size;
		mData = NULL;
	}
}

template <typename INDEX>
void  CIndexBuffer<INDEX>::EnsureCapacity(size_t capacity)
{	
	if (!mData)
	{
		mCapacity = capacity;
		mData =  new INDEX[mCapacity];
		return;
	}
	if (capacity > mCapacity) 
	{	
		size_t new_size = capacity + (mCapacity >> 1); //to ensure we don't allocate and re-allocate too often, allocate more memory using this policy (new desired capacity plus half of old)
	
		INDEX *new_data = new INDEX[new_size];
		std::memcpy(new_data, mData, mCapacity * sizeof(INDEX));
		delete [] mData;
		mCapacity = new_size;
		mData = new_data;
	}
}

template <typename INDEX>
INDEX *CIndexBuffer<INDEX>::Lock()
{	
	if (!mData)
	{
		mData = new INDEX[mCapacity];		
	}	
	return mData;
}

template <typename INDEX>
void CIndexBuffer<INDEX>::UnLock()
{	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGlHandle);	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mCapacity * sizeof(INDEX), mData, GL_STATIC_DRAW);        
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  	
}

template <typename INDEX>
unsigned int CIndexBuffer<INDEX>::CreateGPUBuffer()
{	
	glGenBuffers(1, &mGlHandle);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mGlHandle);	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mCapacity * sizeof(INDEX), mData, GL_STATIC_DRAW);        
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  	
	return 1;
}

template <typename INDEX>
void CIndexBuffer<INDEX>::Use()
{	
	if (mGlHandle == 0xFFFFFFFF)
		CreateGPUBuffer();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (mCapacity > 0) ? mGlHandle : 0);		
}

