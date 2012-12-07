/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Autumn
	Brief		Definition of Autumn Class inherited from State
*/

#ifndef AUTUMN_H
#define AUTUMN_H

#include "States/State.hpp"
#include "Lighting/Light.hpp"
#include "Geometry/Terrain.hpp"
#include "Geometry/SkySphere.hpp"
#include "Geometry/Model.hpp"

class TerrainShader;
class SkyMapShader;
class ParticleSystem;

class Autumn : public State
{
public:
	Autumn();
	~Autumn();
    virtual State* getNextState();
	virtual bool initialise();
	virtual bool deinitialise();
	virtual bool update(float dt);
	virtual void render();

private:
	void initialiseShaders();
	void initialiseGeometry();
	void initialiseParticleSystems();
	void createResources();

	ID3D10Device* d3dDevice_;

	ID3D10RasterizerState* noCullRS_;

	TerrainShader* terrainShader_;
	SkyMapShader* skyMapShader_;

	// Terrain resources
	ID3D10ShaderResourceView* terrainLayerMapRVs_[3];
	ID3D10ShaderResourceView* terrainBlendMapRV_;
	ID3D10ShaderResourceView* terrainSpecMap_;

	// Sky map resource
	ID3D10ShaderResourceView* skyMapRV_;

	// Leaves particle system resource
	ID3D10ShaderResourceView* leavesArrayRV_;

	Light light_;

	Terrain terrain_;
	SkySphere skySphere_;
	Model tree_;

	ParticleSystem* leaves_;

	float moveX_, moveZ_, yaw_, pitch_; // Camera movement
	D3DXVECTOR3 sunDirection_;
	D3DXVECTOR3 fogColor_;
	D3DXMATRIX lightViewProj_;

	// Constants
	const int MOVESPEED;
	const float ROTATESPEED;
};

#endif // AUTUMN_H