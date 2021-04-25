struct PixelOutput
{
	float4 PixelColor : SV_Target;
};

struct PixelInput
{
	float4 VertColor : COLOR;
};

PixelOutput main(PixelInput In)
{
	PixelOutput Out;
	Out.PixelColor = In.VertColor;
	//Out.PixelColor = float4(1.0, 0.0, 0.0, 1.0);
	
	return Out;
}