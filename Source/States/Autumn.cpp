/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Autumn
	Brief		Definition of Autumn Class inherited from State
*/

#include "States/Autumn.hpp"
#include "States/Winter.hpp"
#include "Scene/Scene.hpp"

#include "Shaders/TerrainShader.hpp"
#include "Shaders/SkyMapShader.hpp"

#include "ParticleSystem/ParticleSystem.hpp"
#include "ParticleSystem/Particle.hpp"

/*
	Name		Autumn::Autumn
	Syntax		Autumn()
	Brief		Autumn constructor initialises member variables
*/
Autumn::Autumn()
: d3dDevice_(0), terrainShader_(0), skyMapShader_(0), terrainBlendMapRV_(0), 
  terrainSpecMap_(0), skyMapRV_(0), moveX_(0), moveZ_(0), yaw_(0), pitch_(0), 
  sunDirection_(-300.0f, 250.0f, -200.0f), fogColor_(0.4f, 0.4f, 0.25f), MOVESPEED(50), 
  ROTATESPEED(1.5), noCullRS_(0), leavesArrayRV_(0), leaves_(0) 
{
	terrainLayerMapRVs_[0] = 0;
	terrainLayerMapRVs_[1] = 0;
	terrainLayerMapRVs_[2] = 0;
}

/*
	Name		Autumn::~Autumn
	Syntax		~Autumn()
	Brief		Autumn destructor
*/
Autumn::~Autumn()
{
}

/*
	Name		Autumn::getNextState
	Syntax		Autumn::getNextState()
	Return		State* - A pointer to a next state type object
	Brief		Creates a new object of the next state type
*/
State* Autumn::getNextState()
{
    return new Winter;
}

/*
	Name		Autumn::initialise
	Syntax		Autumn::initialise()
	Return		bool - Returns true once initialised
	Brief		Initialises the state
*/
bool Autumn::initialise()
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
	initialiseParticleSystems();
	createResources();

	light_.setDirection(D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f));
	light_.setAmbient(D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f));
	light_.setDiffuse(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	light_.setSpecular(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f));
	light_.setPosition(D3DXVECTOR3(-95.0f, 200.0f, 350.0f));

	D3DXMATRIX lightView, lightVolume;

	D3DXMatrixLookAtLH(&lightView, &light_.getPosition(),
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	D3DXMatrixOrthoLH(&lightVolume, 400.0f, 400.0f, -100.0f, 100.0f);
	
	lightViewProj_ = lightView * lightVolume;


	moveX_ = 0.0f;
	moveZ_ = 0.0f;
	yaw_ = 0.0f;
	pitch_ = 0.0f;

    return true;
}

/*
	Name		Autumn::deinitialise
	Syntax		Autumn::deinitialise()
	Return		bool - Returns true once deinitialised
	Brief		deinitialises the state
*/
bool Autumn::deinitialise()
{
	delete camera_;
	delete input_;
	delete terrainShader_;
	delete skyMapShader_;
	delete leaves_;
    return true;
}

/*
	Name		Autumn::update
	Syntax		Autumn::update(float dt)
	Param		float dt - Time since last frame
	Return		bool - True if the state is to be changed
	Brief		Updates the state
*/
bool Autumn::update(float dt)
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

	leaves_->update(dt, Scene::instance()->getTimer()->getGameTime());

	tree_.update(lightViewProj_);
    return false;
}

/*
	Name		Autumn::render
	Syntax		Autumn::render()
	Brief		Updates the state
*/
void Autumn::render()
{
	// Reset the depth stencil state and blend state 
	d3dDevice_->OMSetDepthStencilState(0, 0);
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	d3dDevice_->OMSetBlendState(0, blendFactor, 0xffffffff);

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

	// Render leaf particles
	d3dDevice_->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	leaves_->setEyePos(camera_->getPosition());
	leaves_->setEmitPos(D3DXVECTOR3(0.0f, 50.0f, 350.0f));
	leaves_->render();
}

/*
	Name		Autumn::initialiseShaders
	Syntax		Autumn::initialiseShaders()
	Brief		Initialises the shaders used in the state
*/
void Autumn::initialiseShaders()
{
	// Create and initialise terrain shader
	terrainShader_ = new TerrainShader;
	terrainShader_->initialise();

	// Create and initialise sky map shader
	skyMapShader_ = new SkyMapShader;
	skyMapShader_->initialise();
}

/*
	Name		Autumn::initialiseGeometry
	Syntax		Autumn::initialiseGeometry()
	Brief		Initialises the geometry for the state
*/
void Autumn::initialiseGeometry()
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
	tree_.initialise(d3dDevice_, L"Assets/Tree/tree_autumn.m3d");
	tree_.setScale(25.0f, 25.0f, 25.0f);
	tree_.setPos(D3DXVECTOR3(0.0f, -40.0f, 350.0f));
	tree_.setTrans();
}

/*
	Name		Autumn::initialiseParticleSystems
	Syntax		Autumn::initialiseParticleSystems()
	Brief		Initialises the particle systems used in the state
*/
void Autumn::initialiseParticleSystems()
{
	HRESULT hr = 0;

	std::vector<std::string> tumbling_leaves;
	tumbling_leaves.push_back("Assets/2D Textures/tumbling_leaf.dds");

	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to 
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	UINT arraySize = (UINT)tumbling_leaves.size();

	std::vector<ID3D10Texture2D*> srcTex(arraySize, 0);
	for (UINT i = 0; i < arraySize; ++i)
	{
		D3DX10_IMAGE_LOAD_INFO loadInfo;

        loadInfo.Width  = D3DX10_FROM_FILE;
        loadInfo.Height = D3DX10_FROM_FILE;
        loadInfo.Depth  = D3DX10_FROM_FILE;
        loadInfo.FirstMipLevel = 0;
        loadInfo.MipLevels = D3DX10_FROM_FILE;
        loadInfo.Usage = D3D10_USAGE_STAGING;
        loadInfo.BindFlags = 0;
        loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
        loadInfo.MiscFlags = 0;
        loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        loadInfo.Filter = D3DX10_FILTER_NONE;
        loadInfo.MipFilter = D3DX10_FILTER_NONE;
		loadInfo.pSrcInfo  = 0;

        hr = D3DX10CreateTextureFromFile(d3dDevice_, tumbling_leaves[i].c_str(), 
			&loadInfo, 0, (ID3D10Resource**)&srcTex[i], 0);

		if (FAILED(hr))
		{
			MessageBox(0, "Create tumbling leaves texture - Failed", "Error", MB_OK);
			return;
		}
	}

	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	D3D10_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D10_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width              = texElementDesc.Width;
	texArrayDesc.Height             = texElementDesc.Height;
	texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	texArrayDesc.ArraySize          = arraySize;
	texArrayDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	texArrayDesc.SampleDesc.Count   = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage              = D3D10_USAGE_DEFAULT;
	texArrayDesc.BindFlags          = D3D10_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags     = 0;
	texArrayDesc.MiscFlags          = 0;

	ID3D10Texture2D* texArray = 0;
	hr = d3dDevice_->CreateTexture2D( &texArrayDesc, 0, &texArray);
	if (FAILED(hr))
	{
		MessageBox(0, "Create tumbling_leaves array - Failed", "Error", MB_OK);
		return;
	}

	// Copy individual texture elements into texture array.

	// for each texture element...
	for (UINT i = 0; i < arraySize; ++i)
	{
		// for each mipmap level...
		for (UINT j = 0; j < texElementDesc.MipLevels; ++j)
		{
			D3D10_MAPPED_TEXTURE2D mappedTex2D;
			srcTex[i]->Map(j, D3D10_MAP_READ, 0, &mappedTex2D);
                    
            d3dDevice_->UpdateSubresource(texArray, 
				D3D10CalcSubresource(j, i, texElementDesc.MipLevels),
                0, mappedTex2D.pData, mappedTex2D.RowPitch, 0);

            srcTex[i]->Unmap(j);
		}
	}	

	// Create a resource view to the texture array.
	D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = arraySize;

	hr = d3dDevice_->CreateShaderResourceView(texArray, &viewDesc, &leavesArrayRV_);
	if (FAILED(hr))
	{
		MessageBox(0, "Create tumbling_leaves RV - Failed", "Error", MB_OK);
		return;
	}

	// Cleanup - we only need the resource view.
	texArray->Release();

	for (UINT i = 0; i < arraySize; ++i)
	{
		srcTex[i]->Release(); 
	}

	leaves_ = new ParticleSystem(PARTICLE_LEAVES);
	leaves_->initialise(d3dDevice_, leavesArrayRV_, 1000);
}

/*
	Name		Autumn::createResources
	Syntax		Autumn::createResources()
	Brief		Creates the resources used in the state
*/
void Autumn::createResources()
{
	// Load resources for terrain
	HRESULT hr = D3DX10CreateShaderResourceViewFromFile(d3dDevice_, 
											"Assets/2D Textures/leaves.dds", 
											0, 0, &terrainLayerMapRVs_[0], 0 );
	if (FAILED(hr))
	{
		MessageBox(0, "Loading leaves - Failed", "Error", MB_OK);
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
											"Assets/2D Textures/blendAutumn.jpg", 
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
									 "Assets/Skymap/AutumnSkymap.dds", 
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