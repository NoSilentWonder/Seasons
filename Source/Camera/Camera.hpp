/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Camera
	Brief		Definition of Camera class 
*/

#ifndef CAMERA_H
#define CAMERA_H

#include <d3dx10.h>

class Camera
{
public:
	Camera();		// Initialises mCameraPos (0, 0, 6) and mCameraView (0, 0, -6) (looking at the origin)
	~Camera();	
	void update();
	void move(float moveX, float moveZ);
	void rotate(float yaw, float pitch, float roll);
	void zoom(float direction);
	D3DXVECTOR3 getPosition() const { return position_; };
	
private:
	void setCameraViewMatrix();
	void setCameraProjectionMatrix();
	D3DXVECTOR3 position_;
	D3DXVECTOR3 target_;	
	D3DXVECTOR3 up_;
	D3DXVECTOR3 front_;
	D3DXVECTOR3 right_;
	const D3DXVECTOR3 DEFAULT_FRONT;
	const D3DXVECTOR3 DEFAULT_RIGHT;

	D3DXMATRIX rotationMatrix_;

	float zoomFactor_;
};

#endif