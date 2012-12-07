/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Model
	Brief		Definition of Model Class
*/

#include <fstream>
#include <istream>
#include <tchar.h>
#include "Geometry/Model.hpp"
#include "Vertex/Vertex.hpp"
#include "Utility/Utility.hpp"
#include "Lighting/Light.hpp"
#include "Shaders/ModelShader.hpp"
#include "Shaders/ShadowShader.hpp"
#include "Scene/Scene.hpp"


// Overloads the binary >> operator to take D3DXVECTOR3
std::wistream& operator>>(std::wistream& is, D3DXVECTOR3& v)
{
	is >> v.x >> v.y >> v.z;
	return is;
}

// Overloads the binary >> operator to take D3DXVECTOR2
std::wistream& operator>>(std::wistream& is, D3DXVECTOR2& v)
{
	is >> v.x >> v.y;
	return is;
}

/*
	Name		Model::Model
	Syntax		Model()
	Brief		Model constructor
*/
Model::Model() 
: verticesNo_(0), facesNo_(0), d3dDevice_(0), scale_(1,1,1), theta_(0,0,0), 
  pos_(0,0,0)
{

}

/*
	Name		Model::~Model
	Syntax		~Model()
	Brief		Model denstructor
*/
Model::~Model()
{
	if (meshData_)
		meshData_->Release();

	delete modelShader_;
	delete shadowShader_;
}

/*
	Name		Model::initialise
	Syntax		Model::initialise(ID3D10Device* device, std::wstring modelName)
	Param		ID3D10Device* device - Pointer to the Direct3D device
	Param		std::wstring modelName - Name of the model file to be loaded
	Return		bool - True if initialisation is completed successfully
	Brief		Loads the model file and initialises the vertex and index 
				buffers
*/
bool Model::initialise(ID3D10Device* device, std::wstring modelName)
{
	d3dDevice_ = device;

	modelShader_ = new ModelShader;
	modelShader_->initialise();

	shadowShader_ = new ShadowShader;
	shadowShader_->initialise();

	shadowMap_.initialise(d3dDevice_, 1024, 1024);

	// Load the model file
	bool result = loadModel(modelName);

	if (!result)
	{
		MessageBox(0, "Loading model - Failed", "Error", MB_OK);
		return false;
	}
	return true;
}

/*
	Name		Model::render
	Syntax		Model::render(D3DXVECTOR3* cameraPos, Light* light, 
				D3DXVECTOR3* fogColor)
	Param		D3DXVECTOR3* cameraPos - The position of the camera
	Param		Light* light - The light used in the scene
	Param		D3DXVECTOR3* fogColor - The fog color
	Brief		Renders the model
*/
void Model::render(D3DXVECTOR3* cameraPos, Light* light, D3DXVECTOR3* fogColor)
{
	d3dDevice_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dDevice_->IASetInputLayout(modelShader_->getLayout());

	setTrans();
	Scene::instance()->setWorld(world_);
	Scene::instance()->setWVP();

	modelShader_->setWorldandWvp();

	modelShader_->setConstants(cameraPos, light, fogColor);

	modelShader_->setShadowMap(shadowMap_.depthMap());

    D3D10_TECHNIQUE_DESC techDesc;
    modelShader_->setTechniqueDesc(&techDesc);

    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
		for(UINT subsetID = 0; subsetID < subsetsNo_; ++subsetID)
		{
			modelShader_->setupRender(&reflectMaterials_[subsetID], 
									  diffuseTextures_[subsetID], 
									  specTextures_[subsetID], 
									  normalTextures_[subsetID]);
			modelShader_->applyPassState(i);
			meshData_->DrawSubset(subsetID);
		}
	}

	modelShader_->unbindShadowMap();
}

/*
	Name		Model::renderShadow
	Syntax		Model::renderShadow()
	Brief		Renders the model's shadow map
*/
void Model::renderShadow()
{
	d3dDevice_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dDevice_->IASetInputLayout(shadowShader_->getLayout());

	// Render model to shadow map
	shadowMap_.begin();

	D3D10_TECHNIQUE_DESC techDesc;
	shadowShader_->setTechniqueDesc(&techDesc);

    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
		// We only need diffuse map for drawing into shadow map
		for(UINT subsetID = 0; subsetID < subsetsNo_; ++subsetID)
		{
			shadowShader_->setDiffuseRV(diffuseTextures_[subsetID]);
			shadowShader_->applyPassState(i);
			meshData_->DrawSubset(subsetID);
		}
	}

	shadowMap_.end();
}

/*
	Name		Model::loadModel
	Syntax		Model::loadModel(std::wstring modelName)
	Param		std::wstring modelName - Name of the model file to be loaded
	Brief		Loads the model file 
*/
bool Model::loadModel(std::wstring modelName)
{
	std::wifstream inFile(modelName.c_str());
 
	verticesNo_  = 0;
	facesNo_ = 0;
	subsetsNo_ = 0;
	std::wstring skipString;

	if( inFile )
	{
		inFile >> skipString; // file header text
		inFile >> skipString; // #Subsets
		inFile >> subsetsNo_;
		inFile >> skipString; // #Vertices
		inFile >> verticesNo_;
		inFile >> skipString; // #Triangles
		inFile >> facesNo_;
	
		// Create mesh of correct size for model
		D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
			 D3D10_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, 
			 D3D10_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, 
			 D3D10_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, 
			 D3D10_INPUT_PER_VERTEX_DATA, 0},
		};
		HRESULT hr = D3DX10CreateMesh(d3dDevice_, vertexDesc, 4, 
			vertexDesc[0].SemanticName, verticesNo_, 
			facesNo_, D3DX10_MESH_32_BIT, &meshData_);

		if (FAILED(hr))
		{
			MessageBox(0, "Create Mesh - Failed", "Error", MB_OK);
			return false;
		}

		// Load textures and materials data
		inFile >> skipString; // Subsets header text
		for(UINT i = 0; i < subsetsNo_; ++i)
		{
			std::wstring diffuseMapName;
			ID3D10ShaderResourceView* diffuseMapResourceView;

			std::wstring specMapName;
			ID3D10ShaderResourceView* specMapResourceView;

			std::wstring normalMapName;
			ID3D10ShaderResourceView* normalMapResourceView;

			inFile >> diffuseMapName;
			inFile >> specMapName;
			inFile >> normalMapName;
			
			D3DXVECTOR3 reflectivity;
			inFile >> skipString; 
			inFile >> reflectivity;

			reflectMaterials_.push_back(reflectivity);

			hr = D3DX10CreateShaderResourceViewFromFile(d3dDevice_,
										wStringtoString(diffuseMapName).c_str(),
										0, 0, &diffuseMapResourceView, 0 );
			if (FAILED(hr))
			{
				MessageBox(0, "Create diffuse RV - Failed", "Error", MB_OK);
				return false;
			}
			else
				diffuseTextures_.push_back(diffuseMapResourceView);

			hr = D3DX10CreateShaderResourceViewFromFile(d3dDevice_,
										wStringtoString(specMapName).c_str(), 
										0, 0, &specMapResourceView, 0 );
			if (FAILED(hr))
			{
				MessageBox(0, "Create spec RV - Failed", "Error", MB_OK);
				return false;
			}
			else
				specTextures_.push_back(specMapResourceView);

			hr = D3DX10CreateShaderResourceViewFromFile(d3dDevice_,
										wStringtoString(normalMapName).c_str(), 
										0, 0, &normalMapResourceView, 0 );
			if (FAILED(hr))
			{
				MessageBox(0, "Create normal RV - Failed", "Error", MB_OK);
				return false;
			}
			else
				normalTextures_.push_back(normalMapResourceView);
		}

		// Load vertex data for model to mesh
		MeshVertex* vertices = new MeshVertex[verticesNo_];
		inFile >> skipString; // vertices header text
		for(UINT i = 0; i < verticesNo_; ++i)
		{
			inFile >> skipString; // Position:
			inFile >> vertices[i].pos;

			inFile >> skipString; // Tangent:
			inFile >> vertices[i].tangent;

			inFile >> skipString; // Normal:
			inFile >> vertices[i].normal;

			inFile >> skipString; // Tex-Coords:
			inFile >> vertices[i].texC;
		}
		hr = meshData_->SetVertexData(0, vertices);
		if (FAILED(hr))
		{
			MessageBox(0, "Setting mesh vertex data - Failed", "Error", MB_OK);
			return false;
		}
		delete[] vertices;

		// Load index data for model to mesh
		DWORD* indices = new DWORD[facesNo_ * 3];
		UINT* attributeBuffer = new UINT[facesNo_];
		inFile >> skipString; // triangles header text
		for(UINT i = 0; i < facesNo_; ++i)
		{
			inFile >> indices[i * 3 + 0];
			inFile >> indices[i * 3 + 1];
			inFile >> indices[i * 3 + 2];
			inFile >> attributeBuffer[i];
		}
		hr = meshData_->SetIndexData(indices, facesNo_*3);
		if (FAILED(hr))
		{
			MessageBox(0, "Setting mesh index data - Failed", "Error", MB_OK);
			return false;
		}
		hr = meshData_->SetAttributeData(attributeBuffer);
		if (FAILED(hr))
		{
			MessageBox(0, "Setting mesh attribute data - Failed", "Error", 
					   MB_OK);
			return false;
		}

		delete[] indices;
		delete[] attributeBuffer;

		// Optimise and commit mesh
		hr = meshData_->GenerateAdjacencyAndPointReps(0.001f);
		hr = meshData_->Optimize(D3DX10_MESHOPT_ATTR_SORT | 
								 D3DX10_MESHOPT_VERTEX_CACHE, 0, 0);
		hr = meshData_->CommitToDevice();
		if (FAILED(hr))
		{
			return false;
		}
	}
	return true;
}

/*
	Name		Model::update
	Syntax		Model::update(D3DXMATRIX lightViewProj)
	Param		D3DXMATRIX lightViewProj - light view projection matrix
	Brief		Updates the wvp matrix for the lights affecting the model
*/
void Model::update(D3DXMATRIX lightViewProj)
{
	D3DXMATRIX lightWvp = world_ * lightViewProj;
	modelShader_->setLightWvp(lightWvp);
	shadowShader_->setLightWvp(lightWvp);
}

/*
	Name		Model::setTrans
	Syntax		Model::setTrans()
	Brief		Applies the Model's translation to its world matrix
*/
void Model::setTrans()
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
	Name		Model::increasePosX
	Syntax		Model::increasePosX(float x)
	Param		float x - Value to increase the x position by
	Brief		Increases the Model's x position
*/
void Model::increasePosX(float x)
{
	pos_.x += x;
}

/*
	Name		Model::increasePosY
	Syntax		Model::increasePosY(float y)
	Param		float y - Value to increase the y position by
	Brief		Increases the Model's y position
*/
void Model::increasePosY(float y)
{
	pos_.y += y;
}

/*
	Name		Model::increasePosZ
	Syntax		Model::increasePosZ(float z)
	Param		float z - Value to increase the z position by
	Brief		Increases the Model's z position
*/
void Model::increasePosZ(float z)
{
	pos_.z += z;
}

/*
	Name		Model::setPos
	Syntax		Model::setPos(D3DXVECTOR3 pos)
	Param		D3DXVECTOR3 pos - Position vector to move Model to
	Brief		Sets the Model's position
*/
void Model::setPos(D3DXVECTOR3 pos)
{
	pos_ = pos;
}

/*
	Name		Model::increaseThetaX
	Syntax		Model::increaseThetaX(float x)
	Param		float x - Value to increase rotation on the x axis by
	Brief		Increases the Model's x axis rotation
*/
void Model::increaseThetaX(float x)
{
	theta_.x += x;
}

/*
	Name		Model::increaseThetaY
	Syntax		Model::increaseThetaY(float y)
	Param		float y - Value to increase rotation on the y axis by
	Brief		Increases the Model's y axis rotation
*/
void Model::increaseThetaY(float y)
{
	theta_.y += y;
}

/*
	Name		Model::increaseThetaZ
	Syntax		Model::increaseThetaZ(float z)
	Param		float z - Value to increase rotation on the z axis by
	Brief		Increases the Model's z axis rotation
*/
void Model::increaseThetaZ(float z)
{
	theta_.z += z;
}

/*
	Name		Model::setTheta
	Syntax		Model::setTheta(float x, float y, float z)
	Param		float x - Value to set rotation on the x axis to
	Param		float y - Value to set rotation on the y axis to
	Param		float z - Value to set rotation on the z axis to
	Brief		Set the Model's rotation
*/
void Model::setTheta(float x, float y, float z)
{
	theta_.x = x;
	theta_.y = y;
	theta_.z = z;
}

/*
	Name		Model::increaseScaleX
	Syntax		Model::increaseScaleX(float x)
	Param		float x - Value to increase scale along the z axis by
	Brief		Increases the Model's x axis scaling
*/
void Model::increaseScaleX(float x)
{
	scale_.x += x;
}

/*
	Name		Model::increaseScaleY
	Syntax		Model::increaseScaleY(float y)
	Param		float y - Value to increase scale along the y axis by
	Brief		Increases the Model's z axis scaling
*/
void Model::increaseScaleY(float y)
{
	scale_.y += y;
}

/*
	Name		Model::increaseScaleZ
	Syntax		Model::increaseScaleZ(float z)
	Param		float z - Value to increase scale along the z axis by
	Brief		Increases the Model's z axis scaling
*/
void Model::increaseScaleZ(float z)
{
	scale_.z += z;
}

/*
	Name		Model::setScale
	Syntax		Model::setScale(float x, float y, float z)
	Param		float x - Value to set scale along the x axis to
	Param		float y - Value to set scale along the y axis to
	Param		float z - Value to set scale along the z axis to
	Brief		Set the Model's scale
*/
void Model::setScale(float x, float y, float z)
{
	scale_.x = x;
	scale_.y = y;
	scale_.z = z;
}