//==========================================================================================
// 
// Terrain.fx by Elinor Townsend
// Based on tex.fx and the terrain.fx examples by Frank Luna (C) 2008 All Rights Reserved.
//
//==========================================================================================
 
cbuffer cbPerFrame
{	
	float3 eyePosW;
	float3 sunDirection;
	float3 fogColor;
};

cbuffer cbPerObject
{
	float4x4 world;
	float4x4 wvp; 
	float4x4 texMatrix;
};

cbuffer cbFixed
{
	float fogStart = 10.0f;
	float fogRange = 650.0f; //anything beyond 5 will be affected by fog (saturates)	
};


// Nonnumeric values cannot be added to a cbuffer.
Texture2D layer0;
Texture2D layer1;
Texture2D layer2;
Texture2D blendMap;
Texture2D specularMap;

SamplerState TriLinearSample
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float2 texC    : TEXCOORD;
};

struct VS_OUT
{
	float4 posH			: SV_POSITION;
	float shade			: SHADE;
    float4 posW			: POSITION;
    float4 normalW		: NORMAL;
    float2 tiledUV      : TEXCOORD0;
    float2 stretchedUV  : TEXCOORD1; 
    float  fogLerp		: FOG;

};
 
VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
		// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), world);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), world);
	
	// Fog
	float d   = distance(vOut.posW, eyePosW);
	vOut.fogLerp = saturate((d - fogStart) / fogRange);
		
	vOut.shade = saturate(max(dot(vOut.normalW, sunDirection), 0.7f) );
		
	// Transform to homogeneous clip space.
	vOut.posH = mul(float4(vIn.posL, 1.0f), wvp);
		
	vOut.tiledUV     = 50*vIn.texC;
	vOut.stretchedUV = vIn.texC;

	
	return vOut;
}

float4 PS(VS_OUT pIn) : SV_Target
{
	// Get materials from texture maps for diffuse col.	
	float4 c0 = layer0.Sample( TriLinearSample, pIn.tiledUV );
	float4 c1 = layer1.Sample( TriLinearSample, pIn.tiledUV );
	float4 c2 = layer2.Sample( TriLinearSample, pIn.tiledUV );
	
	float4 t = blendMap.Sample( TriLinearSample, pIn.stretchedUV ); 

	// Find the inverse of all the blend weights so that we can  scale the total color to the range [0, 1].
    float totalInverse = 1.0f / (t.r + t.g + t.b);
    
    // Scale the colors by each layer by its corresponding weight
    // stored in the blendmap.  
    c0 *= t.r * totalInverse;
    c1 *= t.g * totalInverse;
	c2 *= t.b * totalInverse;


  // Blend the fog color and the shade * diffuse component
	float3 foggedColor = lerp((pIn.shade * (c0+c1+c2)), fogColor, pIn.fogLerp);
	
	return float4(foggedColor, 1.0f);
}

technique10 TexTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
