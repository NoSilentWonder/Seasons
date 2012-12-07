/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Terrain
	Brief		Definition of Terrain Class
*/

#include "Geometry/Terrain.hpp"
#include <vector>
#include <fstream>
#include "Vertex/Vertex.hpp"

/*
	Name		Terrain::Terrain
	Syntax		Terrain()
	Brief		Terrain constructor
*/
Terrain::Terrain() 
: verticesNo_(0), facesNo_(0), d3dDevice_(0), vertexBuffer_(0), indexBuffer_(0), 
  heightMap_(0), scale_(1,1,1), theta_(0,0,0), pos_(0,0,0), width_(0), height_(0),
  DIMENSIONS(257), SMOOTHING_FACTOR(0.1f)
{

}

/*
	Name		Terrain::~Terrain
	Syntax		~Terrain()
	Brief		Terrain denstructor
*/
Terrain::~Terrain()
{
	if (vertexBuffer_)
	{
		vertexBuffer_->Release();
		vertexBuffer_ = 0;
	}
	if (indexBuffer_)
	{
		indexBuffer_->Release();
		indexBuffer_ = 0;
	}
	if (heightMap_)
	{
		delete [] heightMap_;
		heightMap_ = 0;
	}
}

/*
	Name		Terrain::initialise
	Syntax		Terrain::initialise(ID3D10Device* device, char* heightMapFileName)
	Param		ID3D10Device* device - Pointer to the Direct3D device
	Param		char* heightMapFileName - Name of the height map file to be loaded
	Return		bool - True if initialisation is completed successfully
	Brief		Loads the height map file and initialises the vertex and index buffers
*/
bool Terrain::initialise(ID3D10Device* device, char* heightMapFileName)
{
	d3dDevice_ = device;

	// Load the height map file
	bool result = loadHeightMapRaw(heightMapFileName);

	if (!result)
	{
		MessageBox(0, "Loading height map - Failed", "Error", MB_OK);
		return false;
	}

	// Smooth the height map
	smoothHeightMap();

	// Create the vertices and indices
	result = initialiseBuffers();
	if (!result)
	{
		MessageBox(0, "Initialising buffers - Failed", "Error", MB_OK);
		return false;
	}

	return true;
}

/*
	Name		Terrain::render
	Syntax		Terrain::render()
	Brief		Renders the terrain
*/
void Terrain::render()
{

	// Set the type of primitive to line list
	d3dDevice_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
    UINT offset = 0;
    d3dDevice_->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
	d3dDevice_->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R32_UINT, 0);
	d3dDevice_->DrawIndexed(facesNo_ * 3, 0, 0);

	return;
}

/*
	Name		Terrain::loadHeightMap
	Syntax		Terrain::loadHeightMap(char* heightMapFileName)
	Param		char* heightMapFileName - Name of the height map file to be loaded
	Brief		Loads the height map file into an array
	Details		This loads in a bitmap file
*/
bool Terrain::loadHeightMap(char* heightMapFileName)
{
	FILE* filePtr;
	int error;
	UINT count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	int imageSize;
	unsigned char* bitmapImage;

	// Open the height map file in binary
	error = fopen_s(&filePtr, heightMapFileName, "rb");
	if (error != 0)
	{
		MessageBox(0, "Opening heightmap file - Failed", "Error", MB_OK);
		return false;
	}

	// Read in the file header
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		MessageBox(0, "Reading heightmap file header - Failed", "Error", MB_OK);
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		MessageBox(0, "Reading bitmap info header - Failed", "Error", MB_OK);
		return false;
	}

	// Save the dimensions of the terrain
	width_ = bitmapInfoHeader.biWidth;
	height_ = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data
	imageSize = width_ * height_ * 3;

	// Allocate memory for the bitmap image data
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		MessageBox(0, "Allocating memory - Failed", "Error", MB_OK);
		return false;
	}

	// Move to the beginning of the bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		MessageBox(0, "Reading bitmap image data - Failed", "Error", MB_OK);
		//return false;
	}

	// Close the file
	error = fclose(filePtr);
	if (error != 0)
	{
		MessageBox(0, "Closing file - Failed", "Error", MB_OK);
		return false;
	}

	// Create the structure to hold the height map data
	heightMap_ = new D3DXVECTOR3[width_ * height_];
	if (!heightMap_)
	{
		MessageBox(0, "Creating heightMap_ - Failed", "Error", MB_OK);
		return false;
	}

	unsigned char height;
	UINT i, j, k, index;

	k = 0;

	// Read the image data into the height map
	for (j = 0; j < height_; ++j)
	{
		for (i = 0; i < width_; ++i)
		{
			height = bitmapImage[k];
			
			index = (height_ * j) + i;

			heightMap_[index].x = (float)i;
			heightMap_[index].y = (float)height;
			heightMap_[index].z = (float)j;

			k += 3;
		}
	}

	// Release the bitmap image data
	delete [] bitmapImage;
	bitmapImage = 0;

	return true;
}


/*
	Name		Terrain::loadHeightMapRaw
	Syntax		Terrain::loadHeightMapRaw(char* heightMapFileName)
	Param		char* heightMapFileName - Name of the height map file to be loaded
	Brief		Loads the height map file into an array
	Details		This loads in a raw file
*/
bool Terrain::loadHeightMapRaw(char* heightMapFileName)
{
	height_ = width_ = DIMENSIONS;
	// A height for each vertex
	std::vector<unsigned char> in(height_ * width_ );

	// Open the file
	std::ifstream inFile;
	inFile.open(heightMapFileName, std::ios_base::binary);

	if (inFile)
	{
		// Read the RAW bytes
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file
		inFile.close();
	}

	// Copy the array data into a float array and scale and offset the heights
	heightMap_ = new D3DXVECTOR3[width_ * height_];
	
	UINT i, j, index;

	// Read the image data into the height map
	for (j = 0; j < height_; ++j)
	{
		for (i = 0; i < width_; ++i)
		{	
			index = (height_ * j) + i;

			heightMap_[index].x = (float)i;
			heightMap_[index].y = (float)in[index];
			heightMap_[index].z = (float)j;
		}
	}

	return true;
}

/*
	Name		Terrain::smoothHeightMap
	Syntax		Terrain::smoothHeightMap()
	Brief		Smoothes the height map so that terrain does not look too spiky
*/
void Terrain::smoothHeightMap()
{
	for (UINT j = 0; j < height_; ++j)
	{
		for (UINT i = 0; i < width_; ++i)
		{
			heightMap_[(height_ * j) + i].y *= SMOOTHING_FACTOR;
		}
	}
}

/*
	Name		Terrain::initialiseBuffers
	Syntax		Terrain::initialiseBuffers()
	Return		bool - True once vertex and index buffers initialised
	Brief		Creates the vertices and indices for the terrain
*/
bool Terrain::initialiseBuffers()
{
	// Calculate the number of vertices in the terrain mesh
	verticesNo_ = width_ * height_;

	// Three vertices for each face
	facesNo_ = (width_-1) * (height_-1) * 2;

	Vertex* vertices;
	vertices = new Vertex[verticesNo_];
	if (!vertices)
	{
		return false;
	}

	DWORD* indices;
	indices = new DWORD[facesNo_ * 3];
	if (!indices)
	{
		return false;
	}

	// Load the vertex array with the terrain data

	for (UINT i = 0; i < (width_ * height_); ++i)
	{
		vertices[i].pos = heightMap_[i];
	}

	// Calculate texture coordinates
	float du = 1.0f / (height_ - 1);
	float dv = 1.0f / (width_ - 1);
	for (UINT i = 0; i < width_; ++i)
	{
		for (UINT j = 0; j < height_; ++j)
		{
			int index = (height_ * j) + i;
			vertices[index].texC.x = j * du;
			vertices[index].texC.y = i * dv;

		}
	}

	int k = 0;
	for (UINT i = 0; i < width_-1; ++i)
	{
		for (UINT j = 0; j < height_-1; ++j)
		{
			indices[k]   = i * height_ + j;
			indices[k+1] = (i+1) * height_ + j;
			indices[k+2] = i * height_ + j + 1;

			indices[k+3] = i * height_ + j + 1;
			indices[k+4] = (i+1) * height_ + j;
			indices[k+5] = (i+1) * height_ + j + 1;

			k += 6; // next quad
		}
	}

	calculateNormals(vertices, indices);

	D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * verticesNo_;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
    HRESULT hr = d3dDevice_->CreateBuffer(&vbd, &vinitData, &vertexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(0, "Create Box Vertex Buffer - Failed", "Error", MB_OK);
		return false;
	}

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * facesNo_ * 3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
	hr = d3dDevice_->CreateBuffer(&ibd, &iinitData, &indexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(0, "Create Box Index Buffer - Failed",
			"Error", MB_OK);
		return false;
	}

	// Release the arrays
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

/*
	Name		Terrain::calculateNormals
	Syntax		Terrain::calculateNormals(Vertex* vertices, DWORD* indices)
	Param		Vertex* vertices - Array of vertices for the terrain
	Param		DWORD* indices - Array of indices for the terrain
	Brief		Calculates the normal for each vertex
*/
void Terrain::calculateNormals(Vertex* vertices, DWORD* indices)
{
	// Estimate normals for interior nodes using central difference
	float invTwoDX = 1.0f / 2.0f;
	float invTwoDZ = 1.0f / 2.0f;
	for(UINT i = 2; i < width_ - 1; ++i)
	{
		for(UINT j = 2; j < height_ - 1; ++j)
		{
			float t = vertices[(i - 1) * height_ + j].pos.y;
			float b = vertices[(i + 1) * height_ + j].pos.y;
			float l = vertices[i * height_ + j - 1].pos.y;
			float r = vertices[i * height_ + j + 1].pos.y;

			D3DXVECTOR3 tanZ(0.0f, (t - b) * invTwoDZ, 1.0f);
			D3DXVECTOR3 tanX(1.0f, (r - l) * invTwoDX, 0.0f);

			D3DXVECTOR3 n;
			D3DXVec3Cross(&n, &tanZ, &tanX);
			D3DXVec3Normalize(&n, &n);

			vertices[i * height_ + j].normal = n;
		}
	}
}

/*
	Name		Terrain::calculateNormalsPerTriangle
	Syntax		Terrain::calculateNormalsPerTriangle(Vertex* vertices, 
													 DWORD* indices)
	Param		Vertex* vertices - Array of vertices for the terrain
	Param		DWORD* indices - Array of indices for the terrain
	Brief		Calculates the normal for each vertex
	Details		Calculates normals per triangle. This has been improved by 
				calculating the normals per vertex, taking an average of the 
				normals for the surrounding faces in the method 
				calculateNormals(Vertex* vertices, DWORD* indices)
*/
void Terrain::calculateNormalsPerTriangle(Vertex* vertices, DWORD* indices)
{
	for (DWORD i = 0; i < (facesNo_ * 3); i += 6)
	{
		D3DXVECTOR3 vertex1 = vertices[indices[i]].pos;
		D3DXVECTOR3 vertex2 = vertices[indices[i + 1]].pos;
		D3DXVECTOR3 vertex3 = vertices[indices[i + 2]].pos;
		D3DXVECTOR3 vertex4 = vertices[indices[i + 3]].pos;
		D3DXVECTOR3 vertex5 = vertices[indices[i + 4]].pos;
		D3DXVECTOR3 vertex6 = vertices[indices[i + 5]].pos;

		D3DXVECTOR3 vNormal;
		D3DXVECTOR3 vCross;
		D3DXVec3Cross(&vCross, &D3DXVECTOR3(vertex3 - vertex1), 
					  &D3DXVECTOR3(vertex2 - vertex1));
		D3DXVec3Normalize(&vNormal, &vCross);

		vertices[indices[i]].normal = D3DXVECTOR3(vNormal);
		vertices[indices[i + 1]].normal = D3DXVECTOR3(vNormal);
		vertices[indices[i + 2]].normal = D3DXVECTOR3(vNormal);

		D3DXVec3Cross(&vCross, &D3DXVECTOR3(vertex6 - vertex4), 
					  &D3DXVECTOR3(vertex5 - vertex4));
		D3DXVec3Normalize(&vNormal, &vCross);

		vertices[indices[i + 3]].normal = D3DXVECTOR3(vNormal);
		vertices[indices[i + 4]].normal = D3DXVECTOR3(vNormal);
		vertices[indices[i + 5]].normal = D3DXVECTOR3(vNormal);
	}
}

/*
	Name		Terrain::setTrans
	Syntax		Terrain::setTrans()
	Brief		Applies the Terrain's translation to its world matrix
*/
void Terrain::setTrans()
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
	Name		Terrain::increasePosX
	Syntax		Terrain::increasePosX(float x)
	Param		float x - Value to increase the x position by
	Brief		Increases the Terrain's x position
*/
void Terrain::increasePosX(float x)
{
	pos_.x += x;
}

/*
	Name		Terrain::increasePosY
	Syntax		Terrain::increasePosY(float y)
	Param		float y - Value to increase the y position by
	Brief		Increases the Terrain's y position
*/
void Terrain::increasePosY(float y)
{
	pos_.y += y;
}

/*
	Name		Terrain::increasePosZ
	Syntax		Terrain::increasePosZ(float z)
	Param		float z - Value to increase the z position by
	Brief		Increases the Terrain's z position
*/
void Terrain::increasePosZ(float z)
{
	pos_.z += z;
}

/*
	Name		Terrain::setPos
	Syntax		Terrain::setPos(D3DXVECTOR3 pos)
	Param		D3DXVECTOR3 pos - Position vector to move Terrain to
	Brief		Sets the Terrain's position
*/
void Terrain::setPos(D3DXVECTOR3 pos)
{
	pos_ = pos;
}

/*
	Name		Terrain::increaseThetaX
	Syntax		Terrain::increaseThetaX(float x)
	Param		float x - Value to increase rotation on the x axis by
	Brief		Increases the Terrain's x axis rotation
*/
void Terrain::increaseThetaX(float x)
{
	theta_.x += x;
}

/*
	Name		Terrain::increaseThetaY
	Syntax		Terrain::increaseThetaY(float y)
	Param		float y - Value to increase rotation on the y axis by
	Brief		Increases the Terrain's y axis rotation
*/
void Terrain::increaseThetaY(float y)
{
	theta_.y += y;
}

/*
	Name		Terrain::increaseThetaZ
	Syntax		Terrain::increaseThetaZ(float z)
	Param		float z - Value to increase rotation on the z axis by
	Brief		Increases the Terrain's z axis rotation
*/
void Terrain::increaseThetaZ(float z)
{
	theta_.z += z;
}

/*
	Name		Terrain::setTheta
	Syntax		Terrain::setTheta(float x, float y, float z)
	Param		float x - Value to set rotation on the x axis to
	Param		float y - Value to set rotation on the y axis to
	Param		float z - Value to set rotation on the z axis to
	Brief		Set the Terrain's rotation
*/
void Terrain::setTheta(float x, float y, float z)
{
	theta_.x = x;
	theta_.y = y;
	theta_.z = z;
}

/*
	Name		Terrain::increaseScaleX
	Syntax		Terrain::increaseScaleX(float x)
	Param		float x - Value to increase scale along the z axis by
	Brief		Increases the Terrain's x axis scaling
*/
void Terrain::increaseScaleX(float x)
{
	scale_.x += x;
}

/*
	Name		Terrain::increaseScaleY
	Syntax		Terrain::increaseScaleY(float y)
	Param		float y - Value to increase scale along the y axis by
	Brief		Increases the Terrain's z axis scaling
*/
void Terrain::increaseScaleY(float y)
{
	scale_.y += y;
}

/*
	Name		Terrain::increaseScaleZ
	Syntax		Terrain::increaseScaleZ(float z)
	Param		float z - Value to increase scale along the z axis by
	Brief		Increases the Terrain's z axis scaling
*/
void Terrain::increaseScaleZ(float z)
{
	scale_.z += z;
}

/*
	Name		Terrain::setScale
	Syntax		Terrain::setScale(float x, float y, float z)
	Param		float x - Value to set scale along the x axis to
	Param		float y - Value to set scale along the y axis to
	Param		float z - Value to set scale along the z axis to
	Brief		Set the Terrain's scale
*/
void Terrain::setScale(float x, float y, float z)
{
	scale_.x = x;
	scale_.y = y;
	scale_.z = z;
}