/*
	@Created 	Elinor Townsend 2011
*/

/*	
	@Name		Base State
	@Brief		Definition of abstract base class
*/

#ifndef BASESTATE_H
#define BASESTATE_H

#include "Camera/Camera.hpp"
#include "Input/DirectInput.hpp"

class BaseState
{
public:
	// Pure virtual function
    virtual BaseState * getNextState() = 0;
	virtual bool initialise() = 0;
	virtual bool destroy() = 0;
	virtual bool update() = 0;
	virtual void render() = 0;

protected:
	Camera * mCamera;
	DirectInput * mInput;
};

#endif