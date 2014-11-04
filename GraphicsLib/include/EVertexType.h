/********************************************//**
 * \copyright (c) 2014 
 * \file   	EVertexType.h
 * \Author 	Tim Lewis (tim@totga-games.com)
 * \see    	Project: Anthracite
 * \date   	11-09-2014
 * \brief  	EVertexType
 * This file contains vertex type definitions.
 * Currently, for this project, it contains just one.
 * However, should this need to be extended, this file should be updated
 * accordingly.
 * The enum 'list', struct and the functions that follow it define all the information
 * that other objects need to handle a new vertex type appropriate for your needs
 ***********************************************/




#ifndef EVertexType_h_included
#define EVertexType_h_included


#include <string>
#include <cstring>

#include "CVector3f.h"
#include "CVector2f.h"
 

enum EVertexType
{
	EVT_P_D4B_N_UV
};

/********************************************//**
* \brief 	SVertex_P_N_UV_D4B is a vertex struct
* \details 	
  SVertex_P_N_UV_D4B contains one position (3D), a normal, one UV coord (2D) and a colour (8bpp RGBA)
***********************************************/
struct SVertex_P_N_UV_D4B
{
	enum {VERTEX_TYPE = EVT_P_D4B_N_UV};		
	
	SVertex_P_N_UV_D4B(const CVector3f &position, const CVector3f &normal, const CVector2f &uv, unsigned int colour)
		:mPosition(position)
		,mNormal(normal)
		,mUv(uv)
		,mColour(colour)
	{
	}

	CVector3f mPosition;	 
	CVector3f mNormal;
	CVector2f mUv;
	unsigned int mColour;	
};




inline size_t get_vertex_size(EVertexType vt)
{
	switch (vt)
	{
		case EVT_P_D4B_N_UV: return sizeof(SVertex_P_N_UV_D4B);
	}
	return 0;
}


#define offsetof_macro(st, m) ((size_t)(&((st *)0)->m))

inline size_t get_position_offset(EVertexType vt)
{
	switch (vt)
	{
		case EVT_P_D4B_N_UV: return offsetof_macro(SVertex_P_N_UV_D4B, mPosition);
	}
	return 0;
}


inline size_t get_uv_offset(EVertexType vt, size_t channel)
{
	switch (vt)
	{
		case EVT_P_D4B_N_UV: return offsetof_macro(SVertex_P_N_UV_D4B, mUv);
		
	}
	return 0;
}



inline size_t get_normal_offset(EVertexType vt)
{
	switch (vt)
	{
		case EVT_P_D4B_N_UV: return offsetof_macro(SVertex_P_N_UV_D4B, mNormal);
		
	}
	return 0;
}
#endif
