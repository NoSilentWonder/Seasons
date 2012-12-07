/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Sky Map Shader
	Brief		Definition of Sky Map Shader Class inherited from Shader
*/

#ifndef SKYMAPSHADER_H
#define SKYMAPSHADER_H

#include "Shaders/Shader.hpp"

class SkyMapShader : public Shader
{
public:
    bool initialise();
	void setupRender(D3D10_TECHNIQUE_DESC * techDesc, ID3D10ShaderResourceView * resourceView);
	void applyPassState(UINT pass);
	ID3D10InputLayout *  getLayout() const { return vertexLayout_; };
	void deinitialise();

private:
	ID3D10EffectShaderResourceVariable * skyMapVar_;
};

#endif