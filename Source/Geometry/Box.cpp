/*
	Created 	Frank Luna (c) 2008 All Rights Reserved.
				Additional functionality - Elinor Townsend 2011
*/

/*	
	Name		Sky Sphere
	Brief		Definition of Box Class used for creating a box
*/

#include "Geometry/Box.hpp"
#include "Vertex/Vertex.hpp"

#define VERTICES_NO 24
#define FACES_NO 12

/*
	Name		Box::Box
	Syntax		Box()
	Brief		Box constructor
*/
Box::Box()
: verticesNo_(0), facesNo_(0), d3dDevice_(0), vertexBuffer_(0), indexBuffer_(0), 
  scale_(1,1,1), theta_(0,0,0), pos_(0,0,0)
{

}

/*
	Name		Box::~Box
	Syntax		~Box()
	Brief		Box destructor
*/
Box::~Box()
{
	if (vertexBuffer_)
		vertexBuffer_->Release();
	if (indexBuffer_)
		indexBuffer_->Release();
}

/*
	Name		Box::initialise
	Syntax		Box::initialise(ID3D10Device* device, float scale)
	Param		ID3D10Device* device - pointer to the D3D device
	Param		float scale - scale to set for the box
	Return		bool - false if box is not created
	Brief		Creates the vertices and indices for the box
*/
bool Box::initialise(ID3D10Device* device, float scale)
{
	d3dDevice_ = device;
 
	verticesNo_ = VERTICES_NO;
	facesNo_    = FACES_NO; // 2 per quad

	// Create vertex buffer
    Vertex v[VERTICES_NO];

	// Fill in the front face vertex data.
 	v[0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[3] = Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[5] = Vertex( 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[6] = Vertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[7] = Vertex(-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8]  = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[9]  = Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex( 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex( 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex( 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex( 1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[17] = Vertex(-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[18] = Vertex(-1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[19] = Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[21] = Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[22] = Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[23] = Vertex( 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Scale the box
	for (DWORD i = 0; i < verticesNo_; ++i)
		v[i].pos *= scale;


    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * verticesNo_;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = v;
    HRESULT hr = d3dDevice_->CreateBuffer(&vbd, &vinitData, &vertexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(0, "Create Box Vertex Buffer - Failed", "Error", MB_OK);
		return false;
	}

	// Create the index buffer
	DWORD i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * facesNo_ * 3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = i;
	hr = d3dDevice_->CreateBuffer(&ibd, &iinitData, &indexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(0, "Create Box Index Buffer - Failed", "Error", MB_OK);
		return false;
	}

   return true;
}

/*
	Name		Box::render
	Syntax		Box::render()
	Brief		Draws the box
*/
void Box::render()
{
	UINT stride = sizeof(Vertex);
    UINT offset = 0;
    d3dDevice_->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
	d3dDevice_->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R32_UINT, 0);
	d3dDevice_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dDevice_->DrawIndexed(facesNo_ * 3, 0, 0);
}

/*
	Name		Box::setTrans
	Syntax		Box::setTrans()
	Brief		Applies the box's translation to its world matrix
*/
void Box::setTrans()
{
	D3DXMATRIX m;
	D3DXMatrixIdentity(&world_);
	D3DXMatrixScaling(&m, scale_.x, scale_.y, scale_.z);
	world_ *= m;
	D3DXMatrixRotationYawPitchRoll(&m, theta_.y, theta_.x, theta_.z);
	world_ *= m;
	D3DXMatrixTranslation(&m, pos_.x, pos_.y, pos_.z);
	world_ *= m;
}

/*
	Name		Box::increasePosX
	Syntax		Box::increasePosX(float x)
	Param		float x - Value to increase the x position by
	Brief		Increases the box's x position
*/
void Box::increasePosX(float x)
{
	pos_.x += x;
}

/*
	Name		Box::increasePosY
	Syntax		Box::increasePosY(float y)
	Param		float y - Value to increase the y position by
	Brief		Increases the box's y position
*/
void Box::increasePosY(float y)
{
	pos_.y += y;
}

/*
	Name		Box::increasePosZ
	Syntax		Box::increasePosZ(float z)
	Param		float z - Value to increase the z position by
	Brief		Increases the box's z position
*/
void Box::increasePosZ(float z)
{
	pos_.z += z;
}

/*
	Name		Box::setPos
	Syntax		Box::setPos(D3DXVECTOR3 pos)
	Param		D3DXVECTOR3 pos - Position vector to move box to
	Brief		Sets the box's position
*/
void Box::setPos(D3DXVECTOR3 pos)
{
	pos_ = pos;
}

/*
	Name		Box::increaseThetaX
	Syntax		Box::increaseThetaX(float x)
	Param		float x - Value to increase rotation on the x axis by
	Brief		Increases the box's x axis rotation
*/
void Box::increaseThetaX(float x)
{
	theta_.x += x;
}

/*
	Name		Box::increaseThetaY
	Syntax		Box::increaseThetaY(float y)
	Param		float y - Value to increase rotation on the y axis by
	Brief		Increases the box's y axis rotation
*/
void Box::increaseThetaY(float y)
{
	theta_.y += y;
}

/*
	Name		Box::increaseThetaZ
	Syntax		Box::increaseThetaZ(float z)
	Param		float z - Value to increase rotation on the z axis by
	Brief		Increases the box's z axis rotation
*/
void Box::increaseThetaZ(float z)
{
	theta_.z += z;
}

/*
	Name		Box::setTheta
	Syntax		Box::setTheta(float x, float y, float z)
	Param		float x - Value to set rotation on the x axis to
	Param		float y - Value to set rotation on the y axis to
	Param		float z - Value to set rotation on the z axis to
	Brief		Set the box's rotation
*/
void Box::setTheta(float x, float y, float z)
{
	theta_.x = x;
	theta_.y = y;
	theta_.z = z;
}

/*
	Name		Box::increaseScaleX
	Syntax		Box::increaseScaleX(float x)
	Param		float x - Value to increase scale along the z axis by
	Brief		Increases the box's x axis scaling
*/
void Box::increaseScaleX(float x)
{
	scale_.x += x;
}

/*
	Name		Box::increaseScaleY
	Syntax		Box::increaseScaleY(float y)
	Param		float y - Value to increase scale along the y axis by
	Brief		Increases the box's z axis scaling
*/
void Box::increaseScaleY(float y)
{
	scale_.y += y;
}

/*
	Name		Box::increaseScaleZ
	Syntax		Box::increaseScaleZ(float z)
	Param		float z - Value to increase scale along the z axis by
	Brief		Increases the box's z axis scaling
*/
void Box::increaseScaleZ(float z)
{
	scale_.z += z;
}

/*
	Name		Box::setScale
	Syntax		Box::setScale(float x, float y, float z)
	Param		float x - Value to set scale along the x axis to
	Param		float y - Value to set scale along the y axis to
	Param		float z - Value to set scale along the z axis to
	Brief		Set the box's scale
*/
void Box::setScale(float x, float y, float z)
{
	scale_.x = x;
	scale_.y = y;
	scale_.z = z;
}