/*
	Created 	Frank Luna (c) 2008 All Rights Reserved.
				Additional functionality - Elinor Townsend 2011
*/

/*	
	Name		Sky Sphere
	Brief		Definition of Box Class used for creating a box
*/

#ifndef BOX_H
#define BOX_H

#include <d3dx10.h>

class Box
{
public:
	Box();
	~Box();

	bool initialise(ID3D10Device* device, float scale);
	void render();
	DWORD getNumVertices() const { return verticesNo_; };
	DWORD getNumFaces() const { return facesNo_; };
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
	DWORD verticesNo_;
	DWORD facesNo_;

	ID3D10Device* d3dDevice_;
	ID3D10Buffer* vertexBuffer_;
	ID3D10Buffer* indexBuffer_;

	D3DXMATRIX world_;
	D3DXVECTOR3 pos_, theta_, scale_;
};

#endif // BOX_H
