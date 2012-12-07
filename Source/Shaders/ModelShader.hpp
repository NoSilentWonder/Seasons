/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Model Shader
	Brief		Definition of Model Shader Class inherited from Shader
*/

#ifndef _MODEL_SHADER_H
#define _MODEL_SHADER_H

#include "Shaders/Shader.hpp"

class Light;

class ModelShader : public Shader
{
public:
    bool initialise();
	void setLightWvp(D3DXMATRIX lightWvp);
	void setTechniqueDesc(D3D10_TECHNIQUE_DESC* techDesc);
	void setWorldandWvp();
	void setConstants(D3DXVECTOR3* cameraPos, Light* light,
						D3DXVECTOR3* fogColor);
	void setShadowMap(ID3D10ShaderResourceView* shadowMap);
	void unbindShadowMap();
	void setupRender(D3DXVECTOR3* reflectMaterial, 
								 ID3D10ShaderResourceView* diffuseMapRV, 
								 ID3D10ShaderResourceView* specMapRV,
								 ID3D10ShaderResourceView* normalMapRV);
	void applyPassState(UINT pass);
	ID3D10InputLayout*  getLayout() const { return vertexLayout_; };
	void deinitialise();

private:
	ID3D10EffectMatrixVariable* worldVar_;
	ID3D10EffectVariable* cameraPositionVar_;
	ID3D10EffectVariable* lightPosVar_;
	ID3D10EffectVariable* lightDirVar_;
	ID3D10EffectVariable* lightAmbientVar_;
	ID3D10EffectVariable* lightDiffuseVar_;
	ID3D10EffectVariable* lightSpecVar_;
	ID3D10EffectVariable* lightAttVar_;
	ID3D10EffectScalarVariable* lightSpotFactorVar_;
	ID3D10EffectScalarVariable* lightRangeVar_;
	ID3D10EffectMatrixVariable* lightWvpVar_;
	ID3D10EffectVariable* fogColorVar_;
	ID3D10EffectVectorVariable* reflectMtrlVar_;
	ID3D10EffectShaderResourceVariable* diffuseMapVar_;
	ID3D10EffectShaderResourceVariable* specMapVar_;
	ID3D10EffectShaderResourceVariable* normalMapVar_;
	ID3D10EffectShaderResourceVariable* shadowMapVar_;
	ID3D10EffectTechnique*  shadowTech_;
};

#endif // _MODEL_SHADER_H