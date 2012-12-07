/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Shadow Shader
	Brief		Definition of Shadow Shader Class inherited from Shader
*/

#ifndef _SHADOWSHADER_H
#define _SHADOWSHADER_H

#include "Shaders/Shader.hpp"

class ShadowShader : public Shader
{
public:
    bool initialise();
	void setLightWvp(D3DXMATRIX lightWvp);
	void setTechniqueDesc(D3D10_TECHNIQUE_DESC* techDesc);
	void setDiffuseRV(ID3D10ShaderResourceView* diffuseMapRV);
	void applyPassState(UINT pass);
	ID3D10InputLayout *  getLayout() const { return vertexLayout_; };
	void deinitialise();

private:
	ID3D10EffectMatrixVariable* lightWvpVar_;
	ID3D10EffectShaderResourceVariable* diffuseMapVar_;
};

#endif // _SHADOWSHADER_H