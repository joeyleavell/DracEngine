// Sampler/texture combo for font texture
SamplerState FontSampler : register(t0, space1);
Texture2D <float4> FontTexture : register(t0, space1);

struct PixelOutput
{
	float4 PixelColor : SV_Target;
};

struct PixelInput
{
	float2 VertTexCoord : TEXCOORD;
	float4 VertColor : COLOR;
};

PixelOutput main(PixelInput In)
{
	PixelOutput Out;

	float Value = FontTexture.Sample(FontSampler, In.VertTexCoord).r;
	Out.PixelColor = In.VertColor * Value;
	Out.PixelColor.a = Value;
		
	// Todo: do we want an if statement here?
	if(Value <= .01)
		discard;
			
	return Out;
}