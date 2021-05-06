struct DirectionalLight
{
	float3 Direction;
	float3 Color;
};

struct PointLight
{
	float3 Position;
	float3 Color;
};

// SCENE RESOURCES
cbuffer SceneLighting : register(b2, space2)
{
	float3 ViewPosition;

	float NumPointLights;
	PointLight PointLights[10];
}

// MATERIAL RESOURCES

cbuffer Material : register(b3, space3)
{
	// Whether to use the actual maps
	float1 UseAlbedoMap;//    : packoffset(c0.x);
	float1 UseNormalMap;//    : packoffset(c0.y);
	float1 UseRoughnessMap;// : packoffset(c0.z);
	float1 UseMetallicMap;//  : packoffset(c0.w);
	float1 UseAOMap;//        : packoffset(c1.x);

	// Constant values
	float1 Roughness;//       : packoffset(c2.x);
	float1 Metalness;//       : packoffset(c2.y);
	float3 Albedo;//         : packoffset(c1.y);
	
}

SamplerState AlbedoMapSampler : register(t0, space3);
Texture2D <float4> AlbedoMap : register(t0, space3);

struct PixelInput
{
	float4 VertPos : SV_Position;
	float2 VertUV : TEXCOORD;
	float3 VertNormal : NORMAL;	
	float4 VertWorld : POSITION;
};

struct PixelOutput
{
	float4 PixelColor : SV_Target;
};

#define PI 3.14159265

// Trowbridge-Reitz GGX normal distribution function
// Approximates the amount the surface's microfacets are aligned to the halfway vector
float DistributionGGX(float3 Normal, float3 Halfway, float Roughness)
{
    float Roughness2 = Roughness * Roughness;
    float NdotH      = max(dot(Normal, Halfway), 0.0);
    float NdotH2     = NdotH * NdotH;
	
    float Num        = Roughness2;
    float Denom      = (NdotH2 * (Roughness2 - 1.0) + 1.0);
    Denom            = PI * Denom * Denom;
	
    return Num / Denom;
}

// Schlick-GGX geometry function
// Approximates the self-shadowing nature of microfacets
float GeometrySchlickGGX(float3 Normal, float3 View, float K)
{
	float Dot   = max(dot(Normal, View), 0.0f);
	float Num   = Dot;
	float Denom = Dot * (1 - K) + K;
	
	return Num / Denom;
}

// Account for both viewing shadowing and light direction shadowing
float GeometrySmith(float3 Normal, float3 View, float3 Light, float K)
{
	float Factor1 = GeometrySchlickGGX(Normal, View, K);
	float Factor2 = GeometrySchlickGGX(Normal, Light, K);
	
	return Factor1 * Factor2;
}

float3 FresnelSchlick(float3 Halfway, float3 View, float3 Albedo, float Metalness)
{
	// Interp for metalness
	float3 F0 = float3(0.04, 0.04, 0.04);
	F0        = lerp(F0, Albedo, Metalness);
	
	float Diff = float3(1.0f, 1.0f, 1.0f) - F0;
	float HdotV = max(dot(Halfway, View), 0.0);
		
	return F0 + Diff * pow(1 - HdotV, 5);
}

float3 CookTorranceBRDF(float3 SurfaceNormal, float3 ViewDir, float3 HalfwayDir, float3 LightDir, float3 Albedo, float Roughness, float Metalness)
{
	// Fr = Kd * Flambert + Ks * Fcook-torrance

	// Calculate K based on roughness and direct lighting mode
	float K = (Roughness + 1) * (Roughness + 1) / 8;

	float Distribution = DistributionGGX(SurfaceNormal, HalfwayDir, Roughness);
	float Geometry = GeometrySmith(SurfaceNormal, ViewDir, LightDir, K);
	float3 Fresnel = FresnelSchlick(HalfwayDir, ViewDir, Albedo, 1.0);

	float3 Ks = Fresnel;	
	float3 Kd = float3(1.0, 1.0, 1.0) - Ks;
	Kd *= 1.0 - Metalness; // Metallic materials do not refract light

	// The lambert portion of the diffuse factor
	float3 CookTorranceLambert = Albedo / PI;
	
	float3 CookTorranceSpecNum   = Distribution * Geometry * Fresnel;
	float  CookTorranceSpecDenom = 4.0 * max(dot(SurfaceNormal, ViewDir), 0.0) * max(dot(SurfaceNormal, LightDir), 0.0);
	float  CookTorranceSpec      = CookTorranceSpecNum / max(CookTorranceSpecDenom, 0.001); // Prevent divide by zero
	
	return Kd * CookTorranceLambert + CookTorranceSpec; // Ks already accounted for in fresnel factor
}

float3 CalcPointLightPBR(PointLight InLight, float3 FragPos, float3 SurfaceNormal, float3 Albedo)
{
	float3 LightDir = -normalize(InLight.Position);//normalize(InLight.Position - FragPos);
	float3 ViewDir = normalize(ViewPosition - FragPos);
	float3 HalfwayDir = normalize(LightDir + ViewDir);
	
	// This will use the roughness and metallness from the material
	float3 BRDF = CookTorranceBRDF(SurfaceNormal, ViewDir, HalfwayDir, LightDir, Albedo, Roughness, Metalness);
	float NdotL = max(dot(LightDir, SurfaceNormal), 0.0);
	
	// Calculate the radiance of the point light
	float3 Radiance = InLight.Color;
	
	float3 Result = BRDF * Radiance * NdotL;
	
	return Result;
}

// Calculate effects of light
float3 CalcPointLightPhong(PointLight InLight, float3 FragPos, float3 SurfaceNormal, float3 Albedo)
{
	float3 LightDir = -normalize(InLight.Position);//normalize(InLight.Position - FragPos);
	float3 ViewDir = normalize(ViewPosition - FragPos);
	float3 HalfwayDir = normalize(LightDir + ViewDir);
	
	float Shiny = 32.0f;
	
	float Ambient = 0.03 * float3(1.0f, 1.0f, 1.0f);
	float Diffuse = max(dot(SurfaceNormal, LightDir), 0.0);
	float Specular = pow(max(dot(SurfaceNormal, HalfwayDir), 0.0), Shiny);
	
	float Roughness = 0.8f;
	float K = (Roughness + 1) * (Roughness + 1) / 8;
	float Distribution = DistributionGGX(SurfaceNormal, HalfwayDir, Roughness);
	float Geometry = GeometrySmith(SurfaceNormal, ViewDir, LightDir, K);
	float3 Fresnel = FresnelSchlick(HalfwayDir, ViewDir, Albedo, 1.0);
	
	return Diffuse * Albedo * InLight.Color + Specular * InLight.Color;
}

PixelOutput main(PixelInput In)
{
	PixelOutput Out;
	
	float Dist = distance(ViewPosition, In.VertWorld.xyz);
	float Scale = max(1.0f - Dist / 50.0f, 0.05f);
	
	// Select between using constant diffuse color and texture diffuse color
	float4 AlbedoMapValue = AlbedoMap.Sample(AlbedoMapSampler, In.VertUV);
	float4 AlbedoColor = mul(UseAlbedoMap, AlbedoMapValue) + mul(1.0f - UseAlbedoMap, float4(Albedo, 1.0f));
	
	float3 OutRadiance = float3(0.0f, 0.0f, 0.0f);
	int PointLightsFloored = floor(NumPointLights);
	for(int PointLightIndex = 0; PointLightIndex < PointLightsFloored; PointLightIndex++)
	{
		PointLight TheLight = PointLights[PointLightIndex];

		OutRadiance += CalcPointLightPBR(TheLight, In.VertWorld, In.VertNormal, AlbedoColor);		
		//OutRadiance += CalcPointLightPhong(TheLight, In.VertWorld, In.VertNormal, AlbedoColor);
	}
	
	// Contribute ambient occlusion to final result
	float AmbientOcclusion = 0.0f;
	float3 Ambient = float3(0.03, 0.03, 0.03) * AlbedoColor * AmbientOcclusion;
	OutRadiance += float4(Ambient, 0.0f);
		
	// Tone mapping/HDR
	OutRadiance = OutRadiance / (OutRadiance + float3(1.0, 1.0, 1.0));
	//OutRadiance = pow(OutRadiance, float3(1.0/2.2, 1.0/2.2, 1.0/2.2)); 

	OutRadiance.r = min(OutRadiance.r, 1.0);
	OutRadiance.g = min(OutRadiance.g, 1.0);
	OutRadiance.b = min(OutRadiance.b, 1.0);
	
	//OutRadiance = AlbedoMapValue;
	
	Out.PixelColor = float4(OutRadiance, 1.0f);
			
	return Out;
}