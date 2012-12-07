/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Scene
	Brief		DirectX 3D Scene - maintains the Direct3D device and the FSM  
*/

#ifndef _SCENE_H_
#define _SCENE_H_

#include <d3dx10.h>
#include "GameTimer/GameTimer.h"

class State;

class Scene
{
public:
	Scene();
	~Scene();

	static Scene* instance();

	void initialise();
	void onResize();
	bool runFrame();	
	void deinitialise();
	void resetOMTargetsAndViewport();

	void setWidth(int width);
	void setHeight(int height);
	void setPaused(bool paused);
	void setMinimised(bool min);
	void setMaximised(bool max);
	void setResizing(bool resizing);

	void setWorld(D3DXMATRIX world);
	void setView(D3DXMATRIX view);
	void setProjection(D3DXMATRIX projection);
	void setWVP();

	bool isPaused() const { return paused_; };
	bool isMinimised() const { return minimised_; };
	bool isMaximised() const { return maximised_; };
	bool isResizing() const { return resizing_; };

	float getAspect() const { return aspect_; };
	ID3D10Device * getDevice() const { return d3dDevice_; };
	D3DXMATRIX getWorld() const { return world_; };
	D3DXMATRIX getView() const { return view_; };
	D3DXMATRIX getProjection() const { return projection_; };
	D3DXMATRIX getWVP() const { return wvp_; };
	GameTimer* getTimer() { return &timer_; };

private:
	void startFrame();
	void endFrame();

	void changeState();

	static Scene* instance_;

	ID3D10Device* d3dDevice_;
	IDXGISwapChain* swapChain_;
	ID3D10Texture2D* depthStencilBuffer_;
	ID3D10RenderTargetView* renderTargetView_;
	ID3D10DepthStencilView* depthStencilView_;

	GameTimer timer_;

	int width_;
	int height_;
	float aspect_;

	bool paused_;
	bool minimised_;
	bool maximised_;
	bool resizing_;

	bool initialised_;

	D3DXMATRIX world_; 
	D3DXMATRIX view_;
	D3DXMATRIX projection_; 
	D3DXMATRIX wvp_;

	State* currentState_;
};

#endif