SamplerState BatchTextureSampler : register(t0, space2);
Texture2D <float4> BatchTexture : register(t0, space2);

struct PixelInput
{
	float4 VertPos : SV_Position;
	float2 VertUV : TEXCOORD;
	float4 VertColor : COLOR;	
};

struct PixelOutput
{
	float4 PixelColor : SV_Target;
};

PixelOutput main(PixelInput In)
{
	PixelOutput Res;

	float4 TextureColor = BatchTexture.Sample(BatchTextureSampler, In.VertUV);
				
	Res.PixelColor = In.VertColor * float4(TextureColor.r, TextureColor.g, TextureColor.b, TextureColor.a);
	//Res.PixelColor = float4(TextureColor.a, TextureColor.a, TextureColor.a, 1.0f);

	return Res;
}