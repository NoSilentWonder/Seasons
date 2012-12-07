//=============================================================================
// Leaves.fx by Elinor Townsend 2011
//
// Leaves particle system.  Particles are emitted directly in world space.
//=============================================================================


//***********************************************
// GLOBALS                                      *
//***********************************************

cbuffer cbPerFrame
{
	float4 eyePosW;
	
	// for when the emit position/direction is varying
	float4 emitPosW;
	float4 emitDirW;
	
	float sceneTime;
	float timeStep;
	float4x4 viewProj; 
};

cbuffer cbFixed
{
	// Net constant acceleration used to accerlate the particles.
	float3 accelW = {0.75f, -0.5f, -0.25f};
	
	// Texture coordinates used to stretch texture over quad 
	// when we expand point particle into a quad.
	float2 quadTexC[4] = 
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};
};
 
// Array of textures for texturing the particles.
Texture2DArray texArray;

// Random texture used to generate random numbers in shaders.
Texture1D randomTex;
 
SamplerState TriLinearSample
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};

BlendState AdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_COLOR;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

//***********************************************
// HELPER FUNCTIONS                             *
//***********************************************
float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (sceneTime + offset);
	
	// coordinates in [-1,1]
	float3 v = randomTex.SampleLevel(TriLinearSample, u, 0);
	
	// project onto unit sphere
	return normalize(v);
}

float3 RandVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (sceneTime + offset);
	
	// coordinates in [-1,1]
	float3 v = randomTex.SampleLevel(TriLinearSample, u, 0);
	
	return v;
}

float2x2 RotationMatrix(float rotation)  
{  
    float c = cos(rotation);  
    float s = sin(rotation);  
 
    return float2x2(c, -s, s ,c);  
}
 
//***********************************************
// STREAM-OUT TECH                              *
//***********************************************

#define PT_EMITTER 0
#define PT_FLARE 1
 
struct Particle
{
	float3 initialPosW : POSITION;
	float3 initialVelW : VELOCITY;
	float2 sizeW       : SIZE;
	float age          : AGE;
	uint type          : TYPE;
};
  
Particle StreamOutVS(Particle vIn)
{
	return vIn;
}

// The stream-out GS is just responsible for emitting 
// new particles and destroying old particles.  The logic
// programed here will generally vary from particle system
// to particle system, as the destroy/spawn rules will be 
// different.
[maxvertexcount(6)]
void StreamOutGS(point Particle gIn[1], 
                 inout PointStream<Particle> ptStream)
{	
	gIn[0].age += timeStep;
	
	if( gIn[0].type == PT_EMITTER )
	{	
		// Time to emit a new particle?
		if( gIn[0].age > 0.5f )
		{
			for(int i = 0; i < 3; ++i)
			{
				// give leaves a random starting velocity
				float3 velRandom = RandUnitVec3(0.0f);
			
				Particle p;
				p.initialPosW = emitPosW.xyz;
				p.initialVelW = velRandom * 5.0f;
				p.sizeW       = float2(2.5f, 2.5f);
				p.age         = 0.0f;
				p.type        = PT_FLARE;
			
				ptStream.Append(p);
			}
			
			// Reset the time to emit
			gIn[0].age = 0.0f;
		}
		
		// Always keep emitters
		ptStream.Append(gIn[0]);
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		if( gIn[0].age <= 20.0f )
			ptStream.Append(gIn[0]);
	}		
}

GeometryShader gsStreamOut = ConstructGSWithSO( 
	CompileShader( gs_4_0, StreamOutGS() ), 
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x" );
	
technique10 StreamOutTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, StreamOutVS() ) );
        SetGeometryShader( gsStreamOut );
        
        // Disable pixel shader for stream-out only
        SetPixelShader(NULL);
        
        // We must also disable the depth buffer for stream-out only
        SetDepthStencilState( DisableDepth, 0 );
    }
}

//***********************************************
// DRAW TECH                                    *
//***********************************************

struct VS_OUT
{
	float3 posW  : POSITION;
	float2 sizeW : SIZE;
	uint   type  : TYPE;
	float rotation : ROTATION;
};

VS_OUT DrawVS(Particle vIn)
{
	VS_OUT vOut;
	
	float t = vIn.age;
	
	// Constant acceleration equation
	vOut.posW = 0.5f * t * t * accelW + t * vIn.initialVelW + vIn.initialPosW;
	
	vOut.sizeW = vIn.sizeW;
	vOut.type  = vIn.type;
	
	vOut.rotation = vIn.age;
	
	return vOut;
}

struct GS_OUT
{
	float4 posH  : SV_Position;
	float2 texC  : TEXCOORD;
};

// The draw GS expands points into camera facing quads
[maxvertexcount(4)]
void DrawGS(point VS_OUT gIn[1], 
            inout TriangleStream<GS_OUT> triStream)
{	
	// do not draw emitter particles.
	if( gIn[0].type != PT_EMITTER )
	{
		// Compute world matrix so that billboard faces the camera
		float3 look = normalize(eyePosW.xyz - gIn[0].posW);
		float3 right = normalize(cross(float3(0, 1, 0), look));
		float3 up = cross(look, right);
		float4x4 w;
		
		w[0] = float4(right,       0.0f);
		w[1] = float4(up,          0.0f);
		w[2] = float4(look,        0.0f);
		w[3] = float4(gIn[0].posW, 1.0f);

		float4x4 wvp = mul(w, viewProj);
		
		// Compute 4 triangle strip vertices (quad) in local space
		// The quad faces down the +z axis in local space
		float halfWidth  = 0.5f * gIn[0].sizeW.x;
		float halfHeight = 0.5f * gIn[0].sizeW.y;
	
		float4 v[4];
		v[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f);
		v[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);
		v[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);
		v[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);
		
		GS_OUT gOut;
		[unroll]
		for(int i = 0; i < 4; ++i)
		{
			gOut.posH  = mul(v[i], wvp);
 
			float2 rotatedTexC = quadTexC[i];
			rotatedTexC -= float2(0.5, 0.5);    
			rotatedTexC = mul(rotatedTexC, RotationMatrix(gIn[0].rotation));
			rotatedTexC += float2(0.5, 0.5);  
			gOut.texC = rotatedTexC + float2(1.0f, 1.0f);
			triStream.Append(gOut);
		}	
	}
}

float4 DrawPS(GS_OUT pIn) : SV_TARGET
{
	return texArray.Sample(TriLinearSample, float3(pIn.texC, 0));
}

technique10 DrawTech
{
    pass P0
    {
        SetVertexShader(   CompileShader( vs_4_0, DrawVS() ) );
        SetGeometryShader( CompileShader( gs_4_0, DrawGS() ) );
        SetPixelShader(    CompileShader( ps_4_0, DrawPS() ) );
        
        SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetDepthStencilState( NoDepthWrites, 0 );
    }
}