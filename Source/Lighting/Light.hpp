/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Light
	Brief		Definition of DirectX Light Class
*/

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <d3dx10.h>

class Light
{
public:
	Light();
	~Light();
	void setAmbient(D3DXCOLOR ambient);
	void setDiffuse(D3DXCOLOR diffuse);
	void setSpecular(D3DXCOLOR specular);
	void setPosition(D3DXVECTOR3 position);
	void setDirection(D3DXVECTOR3 direction);
	void setAttenuation(D3DXVECTOR3 attenuation);
	void setRange(float range);
	void setSpotlightFactor(float spotFactor);

	D3DXCOLOR getAmbient() const { return ambient_; };
	D3DXCOLOR getDiffuse() const { return diffuse_; };
	D3DXCOLOR getSpecular() const { return specular_; };
	D3DXVECTOR3 getPosition() const { return position_; };
	D3DXVECTOR3 getDirection() const { return direction_; };
	D3DXVECTOR3 getAttenuation() const { return attenuation_; };
	float getRange() const { return range_; };
	float getSpotlightFactor() const { return spotFactor_; };


private:	
	D3DXCOLOR ambient_;
	D3DXCOLOR diffuse_;
	D3DXCOLOR specular_;
	D3DXVECTOR3 position_;
	D3DXVECTOR3 direction_;
	D3DXVECTOR3 attenuation_;
	float range_;
	float spotFactor_;
};

#endif