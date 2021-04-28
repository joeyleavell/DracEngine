cbuffer Light : register(b2, space2)
{
	float Intensity : packoffset(c0.x);
	float3 Color : packoffset(c1.x);
	float3 Direction : packoffset(c2.x);
}

cbuffer Material : register(b3, space3)
{
	float UseDiffuseTexture : packoffset(c0.x);
	float3 DiffuseColor : packoffset(c1.x);
	float3 AmbientColor : packoffset(c2.x);
	float3 SpecularColor : packoffset(c3.x);
}

SamplerState DiffuseSampler : register(t0, space3);
Texture2D <float4> DiffuseTexture : register(t0, space3);

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
	
	float3 norm = normalize(Direction);
	float diffuse = dot(In.VertNormal, -norm);
	
	// Select between using constant diffuse color and texture diffuse color
	float4 TexColor = DiffuseTexture.Sample(DiffuseSampler, In.VertUV);
	float4 ActualColor = mul(UseDiffuseTexture, TexColor) + mul(1.0f - UseDiffuseTexture, float4(DiffuseColor, 1.0f));
	
	Out.PixelColor = float4(ActualColor.rgb * diffuse, ActualColor.a);
	//Out.PixelColor = float4(ActualColor.rgb, ActualColor.a);
	
	//Out.PixelColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	//Out.PixelColor = DiffuseTexture.Sample(DiffuseSampler, In.VertUV);
	
	return Out;
}