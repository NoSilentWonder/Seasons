/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Camera
	Brief		Definition of Camera class
*/

#include "Camera/Camera.hpp"
#include "Scene/Scene.hpp"

/*
	Name		Camera::Camera
	Syntax		Camera()
	Brief		Camera constructor initialises member variables
*/
Camera::Camera()
: position_(0, 0, 0), target_(0, 0, 1), up_(0, 1, 0), front_(0, 0, 1), 
  right_(1, 0, 0), DEFAULT_FRONT(0, 0, 1), DEFAULT_RIGHT(1, 0, 0), zoomFactor_(1)
{
	update();
}

/*
	Name		Camera::~Camera
	Syntax		~Camera()
	Brief		Camera destructor
*/
Camera::~Camera()
{
	// Nothing to clean up
}

/*
	Name		Camera::update
	Syntax		Camera::update()
	Brief		Updates the camera
*/
void Camera::update()
{
	setCameraViewMatrix();
	setCameraProjectionMatrix();
}

/*
	Name		Camera::move
	Syntax		Camera::move(float moveX, float moveZ)
	Param		float moveX - Distance to move the camera along its x-axis
	Param		float moveZ - Distance to move the camera along its z-axis
	Brief		Moves the camera
*/
void Camera::move(float moveX, float moveZ)
{
	position_ += moveX * right_;
	position_ += moveZ * front_;

	target_ += position_ + target_;
}

/*
	Name		Camera::rotate
	Syntax		Camera::rotate(float yaw, float pitch, float roll)
	Param		float yaw - Rotation around the y-axis
	Param		float pitch - Rotation around the x-axis
	Param		float roll - Rotation around the z-axis
	Brief		Adjusts the rotation of the camera
	Details	roll should be 0 to restrict camera to rotating left/right and up/down
*/
void Camera::rotate(float yaw, float pitch, float roll)
{
	// Update camera rotation matrix and find new Target and Up vectors
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix_, yaw, pitch, roll);
	D3DXVec3TransformCoord(&target_, &DEFAULT_FRONT, &rotationMatrix_);

	// Find new Right and Front vectors - Moving only on the x and z axes
	D3DXMATRIX rotateY;
	D3DXMatrixRotationY(&rotateY, yaw);
	D3DXVec3TransformNormal(&right_, &DEFAULT_RIGHT, &rotateY);
	D3DXVec3TransformNormal(&up_, &up_, &rotateY);
	D3DXVec3TransformNormal(&front_, &DEFAULT_FRONT, &rotationMatrix_);
}

/*
	Name		Camera::zoom
	Syntax		Camera::zoom(float direction)
	Param		float direction - A positive or negative float
	Brief		Adjusts the zoom factor of the camera
	Details	direction is taken from the mouse wheel and will be positive if zoming out or negative if zooming in
*/
void Camera::zoom(float direction)
{
	if ((zoomFactor_ < 2.0f) && (direction < 0))
	{
		zoomFactor_ += 0.1f;
	}
	if ((zoomFactor_ > 0.1f) && (direction > 0))
	{
		zoomFactor_ -= 0.1f;
	}
}

/*
	Name		Camera::setCameraViewMatrix
	Syntax		Camera::setCameraViewMatrix()
	Brief		Sets the camera view matrix
*/
void Camera::setCameraViewMatrix()
{
	// Create the view matrix
	D3DXMATRIX view;
	D3DXMatrixLookAtLH(&view, &position_, &target_, &up_);

	// Set the view matrix
	Scene::instance()->setView(view);
}

/*
	Name		Camera::setCameraProjectionMatrix
	Syntax		Camera::setCameraProjectionMatrix()
	Brief		Sets the camera projection matrix
*/
void Camera::setCameraProjectionMatrix()
{
	// Create the projection matrix
	D3DXMATRIX projection;
	D3DXMatrixPerspectiveFovLH(&projection, (float)D3DX_PI * 0.25f, Scene::instance()->getAspect(), 1.0f, 5000.0f * zoomFactor_);

	// Set the projection matrix
	Scene::instance()->setProjection(projection);
}