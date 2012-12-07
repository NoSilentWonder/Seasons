/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Terrain Shader
	Brief		Definition of Terrain Shader Class inherited from Shader
*/

#include "Shaders/TerrainShader.hpp"
#include "Scene/Scene.hpp"

/*
	Name		TerrainShader::initialise
	Syntax		TerrainShader::initialise()
	Return		bool - Returns true once initialised
	Brief		Initialises the shader
*/
bool TerrainShader::initialise()
{
	d3dDevice_ = Scene::instance()->getDevice();
	if (!d3dDevice_)
	{
		MessageBox(0, "Retrieving device - Failed",
			"Error", MB_OK);
		return false;
	}

	// Build FX
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	
	hr = D3DX10CreateEffectFromFile("Effect Files/Terrain.fx", 0, 0, 
		"fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, d3dDevice_, 0, 0, &fx_, &compilationErrors, 0);
	if (FAILED(hr))
	{
		if ( compilationErrors )
		{
			MessageBoxA(0, "Creating effect from file - Failed",
			"Error", MB_OK);
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			compilationErrors->Release();
			return false;
		}
	} 

	technique_			= fx_->GetTechniqueByName("TexTech");
	
	wvpVar_				= fx_->GetVariableByName("wvp")->AsMatrix();
	worldVar_			= fx_->GetVariableByName("world")->AsMatrix();
	cameraPositionVar_	= fx_->GetVariableByName("eyePosW");
	sunDirectionVar_	= fx_->GetVariableByName("sunDirection");
	fogColorVar_		= fx_->GetVariableByName("fogColor");
	layerMap0Var_		= fx_->GetVariableByName("layer0")->AsShaderResource();
	layerMap1Var_		= fx_->GetVariableByName("layer1")->AsShaderResource();
	layerMap2Var_		= fx_->GetVariableByName("layer2")->AsShaderResource();
	blendMapVar_		= fx_->GetVariableByName("blendMap")->AsShaderResource();
	specularMapVar_		= fx_->GetVariableByName("specularMap")->AsShaderResource();
	texMatrixVar_		= fx_->GetVariableByName("texMatrix")->AsMatrix();

	// Build vertex layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    technique_->GetPassByIndex(0)->GetDesc(&PassDesc);
    hr = d3dDevice_->CreateInputLayout(layout, 3, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &vertexLayout_);

	if (FAILED(hr))
	{
		MessageBoxA(0, "Creating input layout - Failed", "Error", MB_OK);
		return false;
	}

	return true;
}

/*
	Name		TerrainShader::deinitialise
	Syntax		TerrainShader::deinitialise()
	Brief		Deinitialises the shader
*/
void TerrainShader::deinitialise()
{
}

/*
	Name		TerrainShader::setupRender
	Syntax		TerrainShader::setupRender( D3D10_TECHNIQUE_DESC* techDesc, D3DXVECTOR3* cameraPos, 
											D3DXVECTOR3* sunDir, D3DXVECTOR3* fogColor, ID3D10ShaderResourceView* layerMapRVs[3], 
											ID3D10ShaderResourceView* blendMapRV, ID3D10ShaderResourceView* specMapRV)
	Param		D3D10_TECHNIQUE_DESC* techDesc - The technique description for the practice shader technique
	Param		D3DXVECTOR3* cameraPos - The position of the camera
	Param		D3DXVECTOR3* sunDir - The direction of the sunlight
	Param		D3DXVECTOR3* fogColor - The colour of the fog effect
	Param		ID3D10ShaderResourceView* layerMapRVs[3] - Array of the diffuse map resources
	Param		ID3D10ShaderResourceView* blendMapRV - The blend map resource
	Param		ID3D10ShaderResourceView* specMapRV - The specular map resource
	Brief		Prepares the shader for rendering
*/
void TerrainShader::setupRender(D3D10_TECHNIQUE_DESC* techDesc, D3DXVECTOR3* cameraPos, 
					 D3DXVECTOR3* sunDir, D3DXVECTOR3* fogColor, ID3D10ShaderResourceView* layerMapRVs[3], 
					 ID3D10ShaderResourceView* blendMapRV, ID3D10ShaderResourceView* specMapRV)
{
	// Set constants
	cameraPositionVar_->SetRawValue(cameraPos, 0, sizeof(D3DXVECTOR3));
	sunDirectionVar_->SetRawValue(sunDir, 0, sizeof(D3DXVECTOR3));
	fogColorVar_->SetRawValue(fogColor, 0, sizeof(D3DXVECTOR3));
	wvpVar_->SetMatrix((float*)&Scene::instance()->getWVP());
	worldVar_->SetMatrix((float*)&Scene::instance()->getWorld());
	layerMap0Var_->SetResource(layerMapRVs[0]);
	layerMap1Var_->SetResource(layerMapRVs[1]);
	layerMap2Var_->SetResource(layerMapRVs[2]);
	blendMapVar_->SetResource(blendMapRV);
	specularMapVar_->SetResource(specMapRV);
 
	// Don't transform texture coordinates, so just use identity transformation
	D3DXMATRIX texMtx;
	D3DXMatrixIdentity(&texMtx);
	texMatrixVar_->SetMatrix((float*)&texMtx);

    technique_->GetDesc( techDesc );

}

/*
	Name		TerrainShader::applyPassState
	Syntax		TerrainShader::applyPassState(UINT pass)
	Param		UNIT pass - Pass number
	Brief		Sets the state contained in the pass to the device
*/
void TerrainShader::applyPassState(UINT pass)
{
	technique_->GetPassByIndex(pass)->Apply(0);
}