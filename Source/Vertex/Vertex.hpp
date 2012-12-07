/*
	Created 	Frank Luna (C) 2008 All Rights Reserved.
*/

/*	
	Name		Vertex
	Brief		Custom vertex structures to hold vertex data
*/

#ifndef VERTEX_H
#define VERTEX_H

/*
	Name		Vertex
	Syntax		Vertex
	Brief		A vertex structure to hold position, normal and texture 
				coordinates
*/
struct Vertex
{
	Vertex(){}
	Vertex(	float x,	float y,	float z, 
			float nx,	float ny,	float nz, 
			float u,	float v)
			: pos(x, y, z),	normal(nx, ny, nz),	texC(u, v){}

	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texC;
};

/*
	Name		ParticleVertex
	Syntax		ParticleVertex
	Brief		A vertex structure to hold position, velocity, size age and type 
				of particles
*/
struct ParticleVertex
{
	D3DXVECTOR3 initialPos;
	D3DXVECTOR3 initialVel;
	D3DXVECTOR2 size;
	float age;
	unsigned int type;
};

/*
	Name		MeshVertex
	Syntax		MeshVertex
	Brief		A vertex structure to hold tangent, position, normal, and 
				texture coordinates
*/
struct MeshVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 tangent;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texC;
};

#endif 