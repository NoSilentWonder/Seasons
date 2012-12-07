/*
	Created 	Frank Luna (c) 2008 All Rights Reserved.

	Name		SkyMap
	Brief		SkyMap Shader - Used to project a cube map onto geometry rendered to the far plane
*/

cbuffer cbPerFrame
{
	float4x4 wvp;
};

TextureCube skyMap;

SamplerState TriLinearSample
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct SKYMAP_VS_IN
{
	float3 posL : POSITION;
};

struct SKYMAP_VS_OUT
{
    float4 posH : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

// Vertex Shader
SKYMAP_VS_OUT SKYMAP_VS(SKYMAP_VS_IN vIn)
{
	SKYMAP_VS_OUT vOut;
	
	// Set z = w so that z/w = 1 (i.e., skydome always on far plane)
	vOut.posH = mul(float4(vIn.posL, 1.0f), wvp).xyww;
	
	// Use local vertex position as cube map lookup vector
	vOut.texCoord = vIn.posL;
	
	return vOut;
}

// Pixel Shader
float4 SKYMAP_PS(SKYMAP_VS_OUT pIn) : SV_Target
{
	return skyMap.Sample(TriLinearSample, pIn.texCoord);
}

RasterizerState NoCull
{
	CullMode = None;
};

DepthStencilState LessEqualDSS
{
	// Make sure the depth function is LESS_EQUAL and not just LESS
	// Otherwise, the normalised depth values at z = 1 (NDC) will
	// fail the depth tet if the depth buffer was cleared to 1
	DepthFunc = LESS_EQUAL;
};

technique10 SkyTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, SKYMAP_VS()));
		SetGeometryShader(0);
		SetPixelShader(CompileShader(ps_4_0, SKYMAP_PS()));
		
		SetRasterizerState(NoCull);
		SetDepthStencilState(LessEqualDSS, 0);
	}
}