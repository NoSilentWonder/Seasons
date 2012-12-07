/*
	Created 	Frank Luna (C) 2008 All Rights Reserved.
*/

/*	
	Name		Utility
	Brief		Abridged version of d3dUtil.h/.cpp and TextureMgr.h/.cpp 
				by Frank Luna 
*/

#ifndef UTILITY_H
#define UTILITY_H

#include <d3dx10.h>
#include <string>

// Returns random float in [0, 1)
D3DX10INLINE float randFloat()
{
	return (float)(rand()) / (float)RAND_MAX;
}

// Returns random float in [a, b)
D3DX10INLINE float randFloat(float a, float b)
{
	return a + randFloat()*(b-a);
}

// Returns random vector on the unit sphere
D3DX10INLINE D3DXVECTOR3 randUnitVector3()
{
	D3DXVECTOR3 v(randFloat(), randFloat(), randFloat());
	D3DXVec3Normalize(&v, &v);
	return v;
}

// Prototypes
ID3D10ShaderResourceView* createRandomTexture();
std::string wStringtoString(const std::wstring &wstr);

#endif // UTILITY_H