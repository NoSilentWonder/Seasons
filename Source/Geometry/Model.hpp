/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Model
	Brief		Definition of Model Class
*/

#ifndef MODEL_H
#define MODEL_H

#include <d3dx10.h>
#include <vector>
#include <string>
#include "Utility/DepthMap.hpp"

class ModelShader;
class ShadowShader;
class Light;

class Model
{
public:
	Model();
	~Model();
	bool initialise(ID3D10Device* device, std::wstring modelName);
	void render(D3DXVECTOR3* cameraPos, Light* light, D3DXVECTOR3* fogColor); 
	void renderShadow();
	void update(D3DXMATRIX lightViewProj);
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
	bool loadModel(std::wstring modelName);

	ID3DX10Mesh* meshData_;

	DepthMap shadowMap_;

	D3DXMATRIX world_;

	D3DXVECTOR3 pos_, theta_, scale_;

	DWORD verticesNo_;
	DWORD facesNo_;

	DWORD subsetsNo_;
	std::vector<D3DXVECTOR3> reflectMaterials_;
	std::vector<ID3D10ShaderResourceView*> diffuseTextures_;
	std::vector<ID3D10ShaderResourceView*> specTextures_;
	std::vector<ID3D10ShaderResourceView*> normalTextures_;

	ID3D10Device* d3dDevice_;

	ModelShader* modelShader_;
	ShadowShader* shadowShader_;
};

#endif // MODEL_H