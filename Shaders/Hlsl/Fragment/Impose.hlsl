// Sampler for the scene texture
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
	PixelOutput Out;

	// Sample scene texture
	float4 Pixel = BatchTexture.Sample(BatchTextureSampler, In.VertUV);
	Pixel = 1 - Pixel;

	// Simply set the output to the scene texture's pixel value
	Out.PixelColor = float4(Pixel.r, Pixel.g, Pixel.b, 1.0);	
	//Out.PixelColor = float4(1.0, 0.0, 1.0, 1.0);
	
	return Out;
}