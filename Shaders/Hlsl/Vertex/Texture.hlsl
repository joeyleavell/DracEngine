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
	float FlipTexture = 0.0;
	VertexOutput Res;
	Res.VertUV = float2(In.VertUV.x, FlipTexture * (1.0 - In.VertUV.y) + (1.0 - FlipTexture) * In.VertUV.y);
	Res.VertColor = In.VertColor;
	
	// Transform the position by the model view projection matrix.
	Res.VertPos =  mul(ViewProjection, float4(In.VertPos, 1.0));
	
	return Res;
}