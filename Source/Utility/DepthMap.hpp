/*
	Created 	Elinor Townsend 2011
*/

/*	
	Name		Depth Map
	Brief		Definition of Depth Map Class
*/

#ifndef _DEPTHMAP_H
#define _DEPTHMAP_H

#include <d3dx10.h>

class DepthMap
{
public:
	DepthMap();
	~DepthMap();

	void initialise(ID3D10Device* device, UINT width, UINT height);
	ID3D10ShaderResourceView* depthMap();
	void begin();
	void end();
private:
	DepthMap(const DepthMap& rhs);
	DepthMap& operator=(const DepthMap& rhs);

	void buildDepthMap();

	UINT width_;
	UINT height_;

	ID3D10Device* d3dDevice_;

	ID3D10ShaderResourceView* depthMapSRV_;
	ID3D10DepthStencilView* depthMapDSV_;

	D3D10_VIEWPORT viewport_;
};

#endif // _DEPTHMAP_H