/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Terrain Shader
	Brief		Definition of Terrain Shader Class inherited from Shader
*/

#ifndef TERRAINSHADER_H
#define TERRAINSHADER_H

#include "Shaders/Shader.hpp"

class TerrainShader : public Shader
{
public:
    bool initialise();
	void setupRender(D3D10_TECHNIQUE_DESC* techDesc, D3DXVECTOR3* cameraPos, 
					 D3DXVECTOR3* sunDir, D3DXVECTOR3* fogColour, ID3D10ShaderResourceView* layerMapRVs[3], 
					 ID3D10ShaderResourceView* blendMapRV, ID3D10ShaderResourceView* specMapRV);
	void applyPassState(UINT pass);
	ID3D10InputLayout* getLayout() const { return vertexLayout_; };
	void deinitialise();

private:
	ID3D10EffectMatrixVariable* worldVar_;
	ID3D10EffectVariable* cameraPositionVar_;
	ID3D10EffectVariable* sunDirectionVar_;
	ID3D10EffectVariable* fogColorVar_;
	ID3D10EffectShaderResourceVariable* layerMap0Var_;
	ID3D10EffectShaderResourceVariable* layerMap1Var_;
	ID3D10EffectShaderResourceVariable* layerMap2Var_;
	ID3D10EffectShaderResourceVariable* blendMapVar_;
	ID3D10EffectShaderResourceVariable* specularMapVar_;
	ID3D10EffectMatrixVariable* texMatrixVar_;
};

#endif