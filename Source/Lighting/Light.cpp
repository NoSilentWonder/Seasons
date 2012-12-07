/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Light
	Brief		Definition of DirectX Light Class
*/

#include "Lighting/Light.hpp"

/*
	Name		Light::Light
	Syntax		Light()
	Brief		Light constructor initialises member variables
*/
Light::Light()
: ambient_(0, 0, 0, 1), diffuse_(0, 0, 0, 1), specular_(0, 0, 0, 1),
  position_(0, 0, 0), direction_(0, 0, 0), attenuation_(0, 0, 0),
  range_(0), spotFactor_(1)
{

}

/*
	Name		Light::~Light
	Syntax		~Light()
	Brief		Light destructor 
*/
Light::~Light()
{
}

/*
	Name		Light::setAmbient
	Syntax		Light::setAmbient(D3DXCOLOR ambient)
	Param		D3DXCOLOR ambient - The value to which the light's ambience should be set
	Brief		Sets the ambient value of the light
*/
void Light::setAmbient(D3DXCOLOR ambient)
{
	ambient_ = ambient;
}

/*
	Name		Light::setDiffuse
	Syntax		Light::setDiffuse(D3DXCOLOR diffuse)
	Param		D3DXCOLOR diffuse - The value to which the light's diffuse value should be set
	Brief		Sets the diffuse value of the light
*/
void Light::setDiffuse(D3DXCOLOR diffuse)
{
	diffuse_ = diffuse;
}

/*
	Name		Light::setSpecular
	Syntax		Light::setSpecular(D3DXCOLOR specular)
	Param		D3DXCOLOR specular - The value to which the light's specular value should be set
	Brief		Sets the specular value of the light
*/
void Light::setSpecular(D3DXCOLOR specular)
{
	specular_ = specular;
}

/*
	Name		Light::setPosition
	Syntax		Light::setPosition(D3DXVECTOR3 position)
	Param		D3DXVECTOR3 position - The position to set for the light
	Brief		Sets the light's position
*/
void Light::setPosition(D3DXVECTOR3 position)
{
	position_ = position;
}

/*
	Name		Light::setDirection
	Syntax		Light::setDirection(D3DXVECTOR3 direction)
	Param		D3DXVECTOR3 direction - The direction to set for the light
	Brief		Sets the light's direction
*/
void Light::setDirection(D3DXVECTOR3 direction)
{
	direction_ = direction;
}

/*
	Name		Light::setAttenuation
	Syntax		Light::setAttenuation(D3DXVECTOR3 attenuation)
	Param		D3DXVECTOR3 attenuation - The attentuation to set for the light
	Brief		Sets the light's attenuation
*/
void Light::setAttenuation(D3DXVECTOR3 attenuation)
{
	attenuation_ = attenuation;
}

/*
	Name		Light::setRange
	Syntax		Light::setRange(float range)
	Param		float range - The range to set for the light
	Brief		Sets the light's range
*/
void Light::setRange(float range)
{
	range_ = range;
}

/*
	Name		Light::setSpotlightFactor
	Syntax		Light::setSpotlightFactor(float spotFactor)
	Param		float spotFactor - The spotlight factor to set for the light
	Brief		Sets the light's spotlight factor
*/
void Light::setSpotlightFactor(float spotFactor)
{
	spotFactor_ = spotFactor;
}