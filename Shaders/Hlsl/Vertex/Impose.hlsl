cbuffer Scene : register(b0, space0)
{
	float4x4 ViewProjection;
}

cbuffer Group : register(b1, space1)
{
	float FlipTexture;
}

struct VertexInput
{
	float3 VertPos : SV_Position;
	float2 VertUV : TEXCOORD;
	float4 VertColor : COLOR;
};

struct VertexOutput
{
	float4 VertPos : SV_Position;
	float2 VertUV : TEXCOORD;
	float4 VertColor : COLOR;	
};

VertexOutput main(VertexInput In)
{
	// Pass data straight through
	VertexOutput Out;
	Out.VertPos = float4(In.VertPos, 1.0);
	// If FlipTexture is 1, this will flip the UV's. Otherwise, they stay the same. This is needed because some rendering APIs (OpenGL) use a different texture origin
	Out.VertUV = float2(In.VertUV.x, FlipTexture * (1.0 - In.VertUV.y) + (1.0 - FlipTexture) * In.VertUV.y);
	Out.VertColor = In.VertColor;
	
	return Out;
}