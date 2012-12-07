/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Depth Map
	Brief		Definition of Depth Map Class - Used to create a 2d depth-only
				texture
*/


#include "Utility/DepthMap.hpp"
#include "Scene/Scene.hpp"

/*
	Name		DepthMap::DepthMap
	Syntax		DepthMap()
	Brief		DepthMap constructor initialises member variables
*/
DepthMap::DepthMap()
: width_(0), height_(0),  d3dDevice_(0), depthMapSRV_(0), depthMapDSV_(0)
{
	ZeroMemory(&viewport_, sizeof(D3D10_VIEWPORT));
}

/*
	Name		DepthMap::~DepthMap
	Syntax		~DepthMap()
	Brief		DepthMap destructor
*/
DepthMap::~DepthMap()
{
	if (depthMapSRV_)
	{
		depthMapSRV_->Release();
		depthMapSRV_ = 0;
	}
	
	if (depthMapDSV_)
	{
		depthMapDSV_->Release();
		depthMapDSV_ = 0;
	}
}

/*
	Name		DepthMap::initialise
	Syntax		DepthMap::initialise(ID3D10Device* device, UINT width, UINT height)
	Param		ID3D10Device* device - Pointer to the Direct3D device
	Param		UINT width - Width of the depth map
	Param		UINT height - Height of the depth map
	Brief		Initialises the Depth Map
*/
void DepthMap::initialise(ID3D10Device* device, UINT width, UINT height)
{
	width_  = width;
	height_ = height;

	d3dDevice_ = device;

	buildDepthMap();
 
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.Width    = width;
	viewport_.Height   = height;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

/*
	Name		DepthMap::depthMap
	Syntax		DepthMap::depthMap()
	Return		ID3D10ShaderResourceView* - the depth map shader RV
	Brief		Initialises the Depth Map class
*/
ID3D10ShaderResourceView* DepthMap::depthMap()
{
	return depthMapSRV_;
}

/*
	Name		DepthMap::begin
	Syntax		DepthMap::begin()
	Brief		Changes the render target, viewport and depth/stencil buffer
				so that the scene can be rendered to the depth map texture
*/
void DepthMap::begin()
{
	ID3D10RenderTargetView* renderTargets[1] = {0};	
	d3dDevice_->OMSetRenderTargets(1, renderTargets, depthMapDSV_);
	d3dDevice_->RSSetViewports(1, &viewport_);
	d3dDevice_->ClearDepthStencilView(depthMapDSV_, D3D10_CLEAR_DEPTH, 1.0f, 0);
}

/*
	Name		DepthMap::end
	Syntax		DepthMap::end()
	Brief		Resets the OM target and viewport
*/
void DepthMap::end()
{
	Scene::instance()->resetOMTargetsAndViewport();
}

/*
	Name		DepthMap::buildDepthMap
	Syntax		DepthMap::buildDepthMap()
	Brief		Builds the depth map
*/
void DepthMap::buildDepthMap()
{
	ID3D10Texture2D* depthMap = 0;

	D3D10_TEXTURE2D_DESC texDesc;
	
	texDesc.Width     = width_;
	texDesc.Height    = height_;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format    = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count   = 1;  
	texDesc.SampleDesc.Quality = 0;  
	texDesc.Usage          = D3D10_USAGE_DEFAULT;
	texDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0; 
	texDesc.MiscFlags      = 0;

	HRESULT hr = d3dDevice_->CreateTexture2D(&texDesc, 0, &depthMap);
	if (FAILED(hr))
	{
		MessageBox(0, "Create 2d texture - Failed", "Error", MB_OK);
	}

	D3D10_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = d3dDevice_->CreateDepthStencilView(depthMap, &dsvDesc, &depthMapDSV_);
	if (FAILED(hr))
	{
		MessageBox(0, "Create depth map DSV - Failed", "Error", MB_OK);
	}

	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = d3dDevice_->CreateShaderResourceView(depthMap, &srvDesc, &depthMapSRV_);
	if (FAILED(hr))
	{
		MessageBox(0, "Create depth map SRV - Failed", "Error", MB_OK);
	}
	depthMap->Release();
}