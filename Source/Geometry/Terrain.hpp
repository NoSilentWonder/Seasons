/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Terrain
	Brief		Definition of Terrain Class
*/

#ifndef TERRAIN_H
#define TERRAIN_H

#include <d3dx10.h>
#include <d3dx10math.h>
#include <stdio.h>
#include <fstream>

struct Vertex;

class Terrain
{
public:
	Terrain();
	~Terrain();
	bool initialise(ID3D10Device* device, char* heightMapFileName);
	void render(); 
	DWORD getNumVertices() const { return verticesNo_; };
	DWORD getfacesNo_() const { return facesNo_; };
	D3DXMATRIX getWorld() const { return world_; };
	void setTrans();
	void increasePosX(float x);
	void increasePosY(float y);
	void increasePosZ(float z);
	void setPos(D3DXVECTOR3 pos);
	void increaseThetaX(float x);
	void increaseThetaY(float y);
	void increaseThetaZ(float z);
	void setTheta(float x, float y, float z);
	void increaseScaleX(float x);
	void increaseScaleY(float y);
	void increaseScaleZ(float z);
	void setScale(float x, float y, float z);

private:
	bool loadHeightMap(char* heightMapFileName);
	bool loadHeightMapRaw(char* heightMapFileName);
	void smoothHeightMap();
	bool initialiseBuffers();
	void calculateNormals(Vertex* vertices, DWORD* indices);
	void calculateNormalsPerTriangle(Vertex* vertices, DWORD* indices);

	D3DXMATRIX world_;
	D3DXVECTOR3 pos_, theta_, scale_;

	DWORD verticesNo_;
	DWORD facesNo_;

	ID3D10Device* d3dDevice_;
	ID3D10Buffer* vertexBuffer_;
	ID3D10Buffer* indexBuffer_;
	
	UINT width_;
	UINT height_;

	D3DXVECTOR3* heightMap_;
	const int DIMENSIONS;
	const float SMOOTHING_FACTOR;
};

#endif