/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Model Shader
	Brief		Definition of Model Shader Class inherited from Shader
*/

#include "Shaders/ModelShader.hpp"
#include "Scene/Scene.hpp"
#include "Lighting/Light.hpp"

/*
	Name		ModelShader::initialise
	Syntax		ModelShader::initialise()
	Return		bool - Returns true once initialised
	Brief		Initialises the shader
*/
bool ModelShader::initialise()
{
	d3dDevice_ = Scene::instance()->getDevice();
	if (!d3dDevice_)
	{
		MessageBox(0, "Retrieving device - Failed", "Error", MB_OK);
		return false;
	}

	// Build FX
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	
	hr = D3DX10CreateEffectFromFile("Effect Files/Mesh.fx", 0, 0, "fx_4_0", 
									D3D10_SHADER_ENABLE_STRICTNESS, 0, d3dDevice_, 
									0, 0, &fx_, &compilationErrors, 0);
	if (FAILED(hr))
	{
		if (compilationErrors)
		{
			MessageBoxA(0, "Creating effect from file - Failed", "Error", MB_OK);
			compilationErrors->Release();
			return false;
		}
	} 

	technique_ = fx_->GetTechniqueByName("MeshTech");

	lightPosVar_		= fx_->GetVariableByName("position");
	lightDirVar_		= fx_->GetVariableByName("direction");
	lightAmbientVar_	= fx_->GetVariableByName("ambient");
	lightDiffuseVar_	= fx_->GetVariableByName("diffuse");
	lightSpecVar_		= fx_->GetVariableByName("specular");
	lightAttVar_		= fx_->GetVariableByName("attenuation");
	lightSpotFactorVar_	= fx_->GetVariableByName("spotFactor")->AsScalar();
	lightRangeVar_		= fx_->GetVariableByName("range")->AsScalar();
	cameraPositionVar_  = fx_->GetVariableByName("cameraPos");
	lightWvpVar_	    = fx_->GetVariableByName("lightWvp")->AsMatrix();
	fogColorVar_		= fx_->GetVariableByName("fogColor");
	wvpVar_				= fx_->GetVariableByName("wvp")->AsMatrix();
	worldVar_			= fx_->GetVariableByName("world")->AsMatrix();
	reflectMtrlVar_		= fx_->GetVariableByName("reflectMaterial")->AsVector();
	diffuseMapVar_		= fx_->GetVariableByName("diffuseMap")->AsShaderResource();
	specMapVar_			= fx_->GetVariableByName("specMap")->AsShaderResource();
	normalMapVar_		= fx_->GetVariableByName("normalMap")->AsShaderResource();
	shadowMapVar_		= fx_->GetVariableByName("shadowMap")->AsShaderResource();

	// Build vertex layout
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
    D3D10_PASS_DESC passDesc;
    technique_->GetPassByIndex(0)->GetDesc(&passDesc);
    hr = d3dDevice_->CreateInputLayout(vertexDesc, 4, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &vertexLayout_);
	return true;
}

/*
	Name		ModelShader::deinitialise
	Syntax		ModelShader::deinitialise()
	Brief		Deinitialises the shader
*/
void ModelShader::deinitialise()
{
}

/*
	Name		ModelShader::setLightWvp
	Syntax		ModelShader::setLightWvp(D3DXMATRIX lightWvp)
	Param		D3DXMATRIX lightWvp - The light WVP matrix of the model
	Brief		Sets the effect file's light WVP
*/
void ModelShader::setLightWvp(D3DXMATRIX lightWvp)
{
	lightWvpVar_->SetMatrix((float*)&lightWvp);
}

/*
	Name		ModelShader::setTechniqueDesc
	Syntax		ModelShader::setTechniqueDesc(D3D10_TECHNIQUE_DESC* techDesc)
	Param		D3D10_TECHNIQUE_DESC* techDesc - The technique description
				for the model shader technique
	Brief		Sets the effect file's light WVP
*/
void ModelShader::setTechniqueDesc(D3D10_TECHNIQUE_DESC* techDesc)
{
	technique_->GetDesc(techDesc);
}

/*
	Name		ModelShader::setWorldandWvp
	Syntax		ModelShader::setWorldandWvp()
	Brief		Sets the effect file's world and WVP
*/
void ModelShader::setWorldandWvp()
{
	worldVar_->SetMatrix((float*)&Scene::instance()->getWorld());
	wvpVar_->SetMatrix((float*)&Scene::instance()->getWVP());
}

/*
	Name		ModelShader::setConstants
	Syntax		ModelShader::setConstants(D3DXVECTOR3* cameraPos, Light* light, 
				D3DXVECTOR3* fogColor)
	Param		D3DXVECTOR3* cameraPos - The position of the camera
	Param		Light* light - The light used in the scene
	Param		D3DXVECTOR3* fogColor - The fog color
	Brief		Sets the effect file's light WVP
*/
void ModelShader::setConstants(D3DXVECTOR3* cameraPos, Light* light, 
							   D3DXVECTOR3* fogColor)
{
	// Set camera position
	cameraPositionVar_->SetRawValue(cameraPos, 0, sizeof(D3DXVECTOR3));

	// Set light variables
	lightPosVar_->SetRawValue(light->getPosition(), 0, sizeof(D3DXVECTOR3));
	lightDirVar_->SetRawValue(light->getDirection(), 0, sizeof(D3DXVECTOR3));
	lightAmbientVar_->SetRawValue(light->getAmbient(), 0, sizeof(D3DXCOLOR));
	lightDiffuseVar_->SetRawValue(light->getDiffuse(), 0, sizeof(D3DXCOLOR));
	lightSpecVar_->SetRawValue(light->getSpecular(), 0, sizeof(D3DXCOLOR));
	lightAttVar_->SetRawValue(light->getAttenuation(), 0, sizeof(D3DXVECTOR3));
	lightSpotFactorVar_->SetFloat(light->getSpotlightFactor());
	lightRangeVar_->SetFloat(light->getRange());
	fogColorVar_->SetRawValue(fogColor, 0, sizeof(D3DXVECTOR3));
}

/*
	Name		ModelShader::setShadowMap
	Syntax		ModelShader::setShadowMap(ID3D10ShaderResourceView* shadowMap)
	Param		ID3D10ShaderResourceView* shadowMap - shadow map
	Brief		Sets the effect file's shadow map variable
*/
void ModelShader::setShadowMap(ID3D10ShaderResourceView* shadowMap)
{
	shadowMapVar_->SetResource(shadowMap);
}

/*
	Name		ModelShader::unbindShadowMap
	Syntax		ModelShader::unbindShadowMap()
	Brief		Unbinds the shadow map
*/
void ModelShader::unbindShadowMap()
{
	shadowMapVar_->SetResource(0);

	D3D10_TECHNIQUE_DESC techDesc;
    technique_->GetDesc(&techDesc);

    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
        ID3D10EffectPass* pass = technique_->GetPassByIndex(i);
		pass->Apply(0);
	}
}

/*
	Name		ModelShader::setupRender
	Syntax		ModelShader::setupRender(D3DXVECTOR3* reflectMaterial, 
								 ID3D10ShaderResourceView* diffuseMapRV, 
								 ID3D10ShaderResourceView* specMapRV,
								 ID3D10ShaderResourceView* normalMapRV)
	Param		D3DXVECTOR3* reflectMaterial - The reflect material
	Param		ID3D10ShaderResourceView* diffuseMapRV - The diffuse map 
				resource
	Param		ID3D10ShaderResourceView* specMapRV - The specular map resource
	Param		ID3D10ShaderResourceView* normalMapRV - The normal map resource
	Brief		Prepares the shader for rendering
*/
void ModelShader::setupRender(D3DXVECTOR3* reflectMaterial, 
								 ID3D10ShaderResourceView* diffuseMapRV, 
								 ID3D10ShaderResourceView* specMapRV,
								 ID3D10ShaderResourceView* normalMapRV)
{
	
	reflectMtrlVar_->SetRawValue((void*)&reflectMaterial, 
										   0, sizeof(D3DXVECTOR3));
	diffuseMapVar_->SetResource(diffuseMapRV);
	specMapVar_->SetResource(specMapRV);
	normalMapVar_->SetResource(normalMapRV);
}

/*
	Name		ModelShader::applyPassState
	Syntax		ModelShader::applyPassState(UINT pass)
	Param		UNIT pass - Pass number
	Brief		Sets the state contained in the pass to the device
*/
void ModelShader::applyPassState(UINT pass)
{
	technique_->GetPassByIndex(pass)->Apply(0);
}
