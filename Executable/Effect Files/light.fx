struct SurfaceInfo
{
	float3 position;
    float3 normal;
    float4 diffuse;
    float4 specular;
};

float3 ParallelLight(SurfaceInfo v, float3 direction, float4 ambient, float4 diffuse, float4 specular, float3 eyePos, float shadowFactor)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -direction;
	
	// Add the ambient term.
	litColor += v.diffuse * ambient;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, v.normal);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specularPower  = max(v.specular.a, 1.0f);
		float3 toEye     = normalize(eyePos - v.position);
		float3 R         = reflect(-lightVec, v.normal);
		float specularFactor = pow(max(dot(R, toEye), 0.0f), specularPower);
					
		// diffuse and specular terms
		litColor += shadowFactor * diffuseFactor * v.diffuse * diffuse;
		litColor += shadowFactor * specularFactor * v.specular * specular;
	}
	
	return litColor;
}

float3 PointLight(SurfaceInfo v, float3 position, float range, float4 ambient, float4 diffuse, float4 specular, float3 attenuation, float3 eyePos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	
	// The vector from the surface to the light.
	float3 lightVec = position - v.position;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	if( d > range )
		return float3(0.0f, 0.0f, 0.0f);
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Add the ambient light term.
	litColor += v.diffuse * ambient;	
	
	// Add diffuse and specularular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, v.normal);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specularPower  = max(v.specular.a, 1.0f);
		float3 toEye     = normalize(eyePos - v.position);
		float3 R         = reflect(-lightVec, v.normal);
		float specularFactor = pow(max(dot(R, toEye), 0.0f), specularPower);
	
		// diffuse and specularular terms
		litColor += diffuseFactor * v.diffuse * diffuse;
		litColor += specularFactor * v.specular * specular;
	}
	
	// attenuationenuate
	return litColor / dot(attenuation, float3(1.0f, d, d*d));
}

float3 Spotlight(SurfaceInfo v, float3 position, float range, float4 ambient, float4 diffuse, float4 specular, float3 attenuation, float3 direction, float spotFactor, float3 eyePos)
{
	float3 litColor = PointLight(v, position, range, ambient, diffuse, specular, attenuation, eyePos);
	
	// The vector from the surface to the light.
	float3 lightVec = normalize(position - v.position);
	
	float s = pow(max(dot(-lightVec, direction), 0.0f), spotFactor);
	
	// Scale color by spotlight factor.
	return litColor*s;
}