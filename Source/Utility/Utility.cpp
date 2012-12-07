/*
	Created 	Frank Luna (C) 2008 All Rights Reserved.
*/

/*	
	Name		Utility
	Brief		Abridged version of d3dUtil.h/.cpp and TextureMgr.h/.cpp 
				by Frank Luna
*/

#include "Utility/Utility.hpp"
#include "Scene/Scene.hpp"

/*
	Name		createRandomTexture
	Syntax		createRandomTexture(ID3D10ShaderResourceView* _texRV)
	Param		ID3D10ShaderResourceView* _texRV - The texture resource view
	Brief		Builds a random 1D texture used for generating
				random values in effect files
*/
ID3D10ShaderResourceView* createRandomTexture()
{
	// Create random data
	D3DXVECTOR4 randomValues[1024];

	for (int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = randFloat(-1.0f, 1.0f);
		randomValues[i].y = randFloat(-1.0f, 1.0f);
		randomValues[i].z = randFloat(-1.0f, 1.0f);
		randomValues[i].w = randFloat(-1.0f, 1.0f);
	}

	D3D10_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024 * sizeof(D3DXVECTOR4);
	initData.SysMemSlicePitch = 1024 * sizeof(D3DXVECTOR4);

	// Create the texture
	D3D10_TEXTURE1D_DESC texDesc;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D10_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D10Texture1D* randomTex = 0;
	ID3D10Device* d3dDevice = Scene::instance()->getDevice();
	HRESULT hr = d3dDevice->CreateTexture1D(&texDesc, &initData, &randomTex);
	if (FAILED(hr))
	{
		MessageBox(0, "Create random texture - Failed", "Error", MB_OK);
		return 0;
	}

	// Create the resource view
	D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	ID3D10ShaderResourceView* texRV;
	hr = d3dDevice->CreateShaderResourceView(randomTex, &viewDesc, &texRV);

	if (FAILED(hr))
	{
		MessageBox(0, "Create random tex RV - Failed", "Error", MB_OK);
		return 0;
	}

	randomTex->Release();
	randomTex = 0;

	return texRV;
}

/*
	Name		wStringtoString
	Syntax		wStringtoString(const std::wstring &wstr)
	Param		const std::wstring &wstr - The wstring 
	Brief		Converts a wstring to string
*/
std::string wStringtoString(const std::wstring &wstr)
{
    // Convert a Unicode string to an ASCII string
    std::string stringTo;
    char *sizeTo = new char[wstr.length() + 1];
    sizeTo[wstr.size()] = '\0';
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, sizeTo, (int)wstr.length(), 
						NULL, NULL);
    stringTo = sizeTo;
    delete[] sizeTo;
    return stringTo;
}
