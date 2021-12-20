// Sampler for the scene texture
SamplerState SceneTextureSampler : register(t0, space0);
Texture2D <float4> SceneTexture : register(t0, space0);

struct PixelInput
{
	float4 VertPos : SV_Position;
	float2 VertUV : TEXCOORD;
	float3 VertNormal : NORMAL;	
};

struct PixelOutput
{
	float4 PixelColor : SV_Target;
};

PixelOutput main(PixelInput In)
{
	PixelOutput Out;

	// Sample scene texture
	float4 Pixel = SceneTextureSampler.Sample(SceneTexture, In.VertUV);

	// Simply set the output to the scene texture's pixel value
	Out.PixelColor = Pixel;	
	
	return Out;
}