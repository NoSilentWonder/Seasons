/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Summer
	Brief		Definition of Summer Class inherited from State
*/

#include "States/Summer.hpp"
#include "States/Autumn.hpp"
#include "Scene/Scene.hpp"

#include "Shaders/TerrainShader.hpp"
#include "Shaders/SkyMapShader.hpp"

/*
	Name		Summer::Summer
	Syntax		Summer()
	Brief		Summer constructor initialises member variables
*/
Summer::Summer()
: d3dDevice_(0), terrainShader_(0), skyMapShader_(0), terrainBlendMapRV_(0), 
  terrainSpecMap_(0), skyMapRV_(0), moveX_(0), moveZ_(0), yaw_(0), pitch_(0), 
  sunDirection_(-300.0f, 100.0f, -100.0f), fogColor_(0.7f, 0.65f, 0.55f), MOVESPEED(50), 
  ROTATESPEED(1.5), noCullRS_(0) 
{
	terrainLayerMapRVs_[0] = 0;
	terrainLayerMapRVs_[1] = 0;
	terrainLayerMapRVs_[2] = 0;
}

/*
	Name		Summer::~Summer
	Syntax		~Summer()
	Brief		Summer destructor
*/
Summer::~Summer()
{
}

/*
	Name		Summer::getNextState
	Syntax		Summer::getNextState()
	Return		State* - A pointer to a next state type object
	Brief		Creates a new object of the next state type
*/
State* Summer::getNextState()
{
    return new Autumn;
}

/*
	Name		Summer::initialise
	Syntax		Summer::initialise()
	Return		bool - Returns true once initialised
	Brief		Initialises the state
*/
bool Summer::initialise()
{
	
	camera_ = new Camera;
	input_ = new DirectInput;

	d3dDevice_ = Scene::instance()->getDevice();
	if (!d3dDevice_)
	{
		MessageBox(0, "Retrieving device - Failed",
			"Error", MB_OK);
		return false;
	}

	D3D10_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D10_RASTERIZER_DESC));
    rsDesc.FillMode = D3D10_FILL_SOLID;
    rsDesc.CullMode = D3D10_CULL_NONE;
    rsDesc.FrontCounterClockwise = false;

	HRESULT hr = d3dDevice_->CreateRasterizerState(&rsDesc, &noCullRS_);
	if (FAILED(hr))
	{
		MessageBox(0, "Creating rasteriser state - Failed", "Error", MB_OK);
		return false;
	}

	initialiseShaders();
	initialiseGeometry();
	createResources();

	light_.setDirection(D3DXVECTOR3(0.6f, -0.97f, 0.25f));
	light_.setAmbient(D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f));
	light_.setDiffuse(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	light_.setSpecular(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	light_.setPosition(D3DXVECTOR3(-10.0f, 100.0f, 350.0f));

	D3DXMATRIX lightView, lightVolume;

	D3DXMatrixLookAtLH(&lightView, &light_.getPosition(),
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	D3DXMatrixOrthoLH(&lightVolume, 200.0f, 200.0f, -35.0f, 100.0f);
	
	lightViewProj_ = lightView * lightVolume;

	moveX_ = 0.0f;
	moveZ_ = 0.0f;
	yaw_ = 0.0f;
	pitch_ = 0.0f;

    return true;
}

/*
	Name		Summer::deinitialise
	Syntax		Summer::deinitialise()
	Return		bool - Returns true once deinitialised
	Brief		Deinitialises the state
*/
bool Summer::deinitialise()
{
	delete camera_;
	delete input_;
	delete terrainShader_;
	delete skyMapShader_;
    return true;
}

/*
	Name		Summer::update
	Syntax		Summer::update(float dt)
	Param		float dt - Time since last frame
	Return		bool - True if the state is to be changed
	Brief		Updates the state
*/
bool Summer::update(float dt)
{
	input_->update();

	if (input_->isKeyDown(DIK_1)) return true;

	// Rotate camera
	if (input_->isKeyDown(DIK_LEFT))	yaw_   -= ROTATESPEED*dt;
	if (input_->isKeyDown(DIK_RIGHT))	yaw_   += ROTATESPEED*dt;
	if (input_->isKeyDown(DIK_UP))		pitch_ += ROTATESPEED*dt;
	if (input_->isKeyDown(DIK_DOWN))	pitch_ -= ROTATESPEED*dt;

	if (input_->getMouseLeftDown())
	{
		if (input_->getMouseX() < 0) yaw_	-= ROTATESPEED*10*dt;
		if (input_->getMouseX() > 0) yaw_	+= ROTATESPEED*10*dt;
		if (input_->getMouseY() > 0) pitch_	+= ROTATESPEED*10*dt;
		if (input_->getMouseY() < 0) pitch_	-= ROTATESPEED*10*dt;
	}

	camera_->rotate(yaw_, pitch_, 0);
	
	// Move the camera
	if (input_->isKeyDown(DIK_D)) moveX_ = MOVESPEED*dt;
	if (input_->isKeyDown(DIK_A)) moveX_ = -MOVESPEED*dt;
	if (input_->isKeyDown(DIK_W)) moveZ_ = MOVESPEED*dt;
	if (input_->isKeyDown(DIK_S)) moveZ_ = -MOVESPEED*dt;
	
	camera_->move(moveX_, moveZ_);

	moveX_ = 0.0f;
	moveZ_ = 0.0f;

	// Update camera - updates the View and Projection matrices
	camera_->update();

	skySphere_.setPos(camera_->getPosition());
	skySphere_.setTrans();

	tree_.update(lightViewProj_);
    return false;
}

/*
	Name		Summer::render
	Syntax		Summer::render()
	Brief		Updates the state
*/
void Summer::render()
{
	// Reset the depth stencil state and blend state 
	d3dDevice_->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	d3dDevice_->OMSetBlendState(0, blendFactors, 0xffffffff);

	// Render tree
	// Disable culling of back faces so that tree foliage is rendered correctly
	d3dDevice_->RSSetState(noCullRS_);
	// Build the shadow map for the tree model
	tree_.renderShadow();
	// Draw tree
	tree_.render(&camera_->getPosition(), &light_, &fogColor_);
	// Reenable back face culling once tree is rendered
	d3dDevice_->RSSetState(0);

	// Render terrain
	d3dDevice_->IASetInputLayout(terrainShader_->getLayout());  
	// Set world and wvp transformation matrices
	Scene::instance()->setWorld(terrain_.getWorld());
	Scene::instance()->setWVP();	
	// Create a new technique description for our terrain technique
    D3D10_TECHNIQUE_DESC terrainTechDesc;
	terrainShader_->setupRender(&terrainTechDesc, &camera_->getPosition(), 
								&sunDirection_, &fogColor_, terrainLayerMapRVs_, 
								terrainBlendMapRV_, terrainSpecMap_);
	// Draw the terrain
	for ( UINT p = 0; p < terrainTechDesc.Passes; ++p )
	{
		terrainShader_->applyPassState(p);
		terrain_.render();
	}
	
	// Render sky map
	d3dDevice_->IASetInputLayout( skyMapShader_->getLayout() );  
	// Set world and wvp transformation matrices
	Scene::instance()->setWorld(skySphere_.getWorld());
	Scene::instance()->setWVP();
	// Create a new technique description for our skymap technique
    D3D10_TECHNIQUE_DESC skymaptechDesc;
	skyMapShader_->setupRender(&skymaptechDesc, skyMapRV_);
	// Draw the sky map
	for (UINT p = 0; p < skymaptechDesc.Passes; ++p)
	{
		skyMapShader_->applyPassState(p);
		skySphere_.render();
	}
}

/*
	Name		Summer::initialiseShaders
	Syntax		Summer::initialiseShaders()
	Brief		Initialises the shaders used in the state
*/
void Summer::initialiseShaders()
{
	// Create and initialise terrain shader
	terrainShader_ = new TerrainShader;
	terrainShader_->initialise();

	// Create and initialise sky map shader
	skyMapShader_ = new SkyMapShader;
	skyMapShader_->initialise();
}

/*
	Name		Summer::initialiseGeometry
	Syntax		Summer::initialiseGeometry()
	Brief		Initialises the geometry for the state
*/
void Summer::initialiseGeometry()
{
	// Initialise the terrain
	terrain_.initialise(d3dDevice_, "Assets/heightmap3.raw");
	terrain_.setScale(5.0f, 5.0f, 5.0f);
	terrain_.setPos(D3DXVECTOR3(-600.0f, -150.0f, -600.0f));
	terrain_.setTrans();

	// Create sky sphere and scale it so that it does not clip with the camera
	skySphere_.initialise(d3dDevice_);
	skySphere_.setScale(50.0f, 50.0f, 50.0f);
	skySphere_.setTrans();

	// Initialise the tree
	tree_.initialise(d3dDevice_, L"Assets/Tree/tree.m3d");
	tree_.setScale(25.0f, 25.0f, 25.0f);
	tree_.setPos(D3DXVECTOR3(0.0f, -40.0f, 350.0f));
	tree_.setTrans();
}

/*
	Name		Summer::createResources
	Syntax		Summer::createResources()
	Brief		Creates the resources used in the state
*/
void Summer::createResources()
{
	// Load resources for terrain
	HRESULT hr = D3DX10CreateShaderResourceViewFromFile(d3dDevice_, 
											"Assets/2D Textures/grass0.dds", 
											0, 0, &terrainLayerMapRVs_[0], 0 );
	if (FAILED(hr))
	{
		MessageBox(0, "Loading grass0 - Failed", "Error", MB_OK);
		return;
	}
		
	hr = D3DX10CreateShaderResourceViewFromFile(d3dDevice_, 
											"Assets/2D Textures/dark_grass.dds", 
											0, 0, &terrainLayerMapRVs_[1], 0 );
	if (FAILED(hr))
	{
		MessageBox(0, "Loading dark grass - Failed", "Error", MB_OK);
		return;
	}

	hr = D3DX10CreateShaderResourceViewFromFile(d3dDevice_, 
											"Assets/2D Textures/grass.dds", 
											0, 0, &terrainLayerMapRVs_[2], 0 );
	if (FAILED(hr))
	{
		MessageBox(0, "Loading grass - Failed", "Error", MB_OK);
		return;
	}

	hr = D3DX10CreateShaderResourceViewFromFile(d3dDevice_, 
											"Assets/2D Textures/blendSummer.jpg", 
											0, 0, &terrainBlendMapRV_, 0 );
	if (FAILED(hr))
	{
		MessageBox(0, "Loading blendmap for terrain - Failed", "Error", MB_OK);
		return;
	}

	hr = D3DX10CreateShaderResourceViewFromFile(d3dDevice_, 
											"Assets/2D Textures/defaultspec.dds", 
											0, 0, &terrainSpecMap_, 0 );
	if (FAILED(hr))
	{
		MessageBox(0, "Load terrain specular map - Failed", "Error", MB_OK);
		return;
	}

	// Load in the skymap texture
	D3DX10_IMAGE_LOAD_INFO loadSMInfo;
	loadSMInfo.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

	ID3D10Texture2D* SMTexture = 0;
    hr = D3DX10CreateTextureFromFile(d3dDevice_, 
									 "Assets/Skymap/SummerSkymap.dds", 
									 &loadSMInfo, NULL, 
									 (ID3D10Resource**)&SMTexture, NULL);
	if (FAILED(hr))
	{
		MessageBox(0, "Load cube texture - Failed", "Error", MB_OK);
		return;
	}

	// Create texture description
	D3D10_TEXTURE2D_DESC SMTextureDesc;
	SMTexture->GetDesc(&SMTextureDesc);

	D3D10_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	SMViewDesc.Format = SMTextureDesc.Format;
	SMViewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;

	// Create shader resource view
	d3dDevice_->CreateShaderResourceView(SMTexture, &SMViewDesc, &skyMapRV_);
	SMTexture->Release();
}