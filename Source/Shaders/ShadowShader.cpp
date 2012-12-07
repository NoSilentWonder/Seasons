/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Shadow Shader
	Brief		Definition of Shadow Shader Class inherited from Shader
*/

#include "Shaders/ShadowShader.hpp"
#include "Scene/Scene.hpp"

/*
	Name		ShadowShader::initialise
	Syntax		ShadowShader::initialise()
	Return		bool - Returns true once initialised
	Brief		Initialises the shader
*/
bool ShadowShader::initialise()
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
	
	hr = D3DX10CreateEffectFromFile("Effect Files/ShadowMap.fx", 0, 0, "fx_4_0", 
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

	technique_ = fx_->GetTechniqueByName("BuildShadowMapTech");
	
	lightWvpVar_	= fx_->GetVariableByName("lightWvp")->AsMatrix();
	diffuseMapVar_	= fx_->GetVariableByName("diffuseMap")->AsShaderResource();


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

	if (FAILED(hr))
	{
		MessageBoxA(0, "Creating input layout - Failed", "Error", MB_OK);
		return false;
	}

	return true;
}

/*
	Name		ShadowShader::deinitialise
	Syntax		ShadowShader::deinitialise()
	Brief		Deinitialises the shader
*/
void ShadowShader::deinitialise()
{
}

/*
	Name		ShadowShader::setLightWvp
	Syntax		ShadowShader::setLightWvp(D3DXMATRIX lightWvp)
	Param		D3DXMATRIX lightWvp - The light WVP matrix of the model
	Brief		Sets the effect file's light WVP
*/
void ShadowShader::setLightWvp(D3DXMATRIX lightWvp)
{
	lightWvpVar_->SetMatrix((float*)&lightWvp);
}

/*
	Name		ShadowShader::setTechniqueDesc
	Syntax		ShadowShader::setTechniqueDesc(D3D10_TECHNIQUE_DESC* techDesc)
	Param		D3D10_TECHNIQUE_DESC* techDesc - The technique description
				for the model shader technique
	Brief		Retrieves the technique description
*/
void ShadowShader::setTechniqueDesc(D3D10_TECHNIQUE_DESC* techDesc)
{
	technique_->GetDesc(techDesc);
}

/*
	Name		ShadowShader::setDiffuseRV
	Syntax		ShadowShader::setDiffuseRV(ID3D10ShaderResourceView* diffuseMapRV)
	Param		ID3D10ShaderResourceView* diffuseMapRV - The diffuse map 
				resource
	Brief		Prepares the shader for rendering by setting the diffuse resource
*/
void ShadowShader::setDiffuseRV(ID3D10ShaderResourceView* diffuseMapRV)
{
	diffuseMapVar_->SetResource(diffuseMapRV);
}

/*
	Name		ShadowShader::applyPassState
	Syntax		ShadowShader::applyPassState(UINT pass)
	Param		UNIT pass - Pass number
	Brief		Sets the state contained in the pass to the device
*/
void ShadowShader::applyPassState(UINT pass)
{
	technique_->GetPassByIndex(pass)->Apply(0);
}
