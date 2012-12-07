/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Sky Map Shader
	Brief		Definition of Sky Map Shader Class inherited from Shader
*/

#include "Shaders/SkyMapShader.hpp"
#include "Scene/Scene.hpp"

/*
	Name		SkyMapShader::initialise
	Syntax		SkyMapShader::initialise()
	Return		bool - Returns true once initialised
	Brief		Initialises the shader
*/
bool SkyMapShader::initialise()
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

	hr = D3DX10CreateEffectFromFile("Effect Files/SkyMap.fx", 0, 0, "fx_4_0", 
									D3D10_SHADER_ENABLE_STRICTNESS, 0, d3dDevice_, 
									0, 0, &fx_, &compilationErrors, 0);
	if (FAILED(hr))
	{
		MessageBoxA(0, "Creating effect from file - Failed", "Error", MB_OK);
		return false;
	}

	wvpVar_		= fx_->GetVariableByName("wvp")->AsMatrix();
	technique_	= fx_->GetTechniqueByName("SkyTech");
	skyMapVar_	= fx_->GetVariableByName("skyMap")->AsShaderResource();

	// Build vertex layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
    D3D10_PASS_DESC passDesc;
    technique_->GetPassByIndex(0)->GetDesc(&passDesc);
    hr = d3dDevice_->CreateInputLayout(layout, 3, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &vertexLayout_);

	if (FAILED(hr))
	{
		MessageBoxA(0, "Creating input layout - Failed", "Error", MB_OK);
		return false;
	}
	
	return true;
}

/*
	Name		SkyMapShader::deinitialise
	Syntax		SkyMapShader::deinitialise()
	Brief		Deinitialises the shader
*/
void SkyMapShader::deinitialise()
{
}

/*
	Name		SkyMapShader::setupRender
	Syntax		SkyMapShader::setupRender(D3D10_TECHNIQUE_DESC* techDesc, ID3D10ShaderResourceView* resourceView)
	Param		D3D10_TECHNIQUE_DESC* techDesc - The technique description for the sky map technique
	Param		ID3D10ShaderResourceView* resourceView - Texture resource to be used for the sky map
	Brief		Prepares the shader for rendering
*/
void SkyMapShader::setupRender(D3D10_TECHNIQUE_DESC* techDesc, ID3D10ShaderResourceView* resourceView)
{
	wvpVar_->SetMatrix((float*)&Scene::instance()->getWVP());

	// Get the technique description for our skymap technique
	technique_->GetDesc(techDesc);

	// Set the sky map texture
	skyMapVar_->SetResource(resourceView);
}

/*
	Name		SkyMapShader::applyPassState
	Syntax		SkyMapShader::applyPassState(UINT pass)
	Param		UNIT pass - Pass number
	Brief		Sets the state contained in the pass to the device
*/
void SkyMapShader::applyPassState(UINT pass)
{
	technique_->GetPassByIndex(pass)->Apply(0);
}

