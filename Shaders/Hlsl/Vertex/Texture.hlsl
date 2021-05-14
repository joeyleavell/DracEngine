cbuffer Scene : register(b0, space0)
{
	float4x4 ViewProjection;
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
	VertexOutput Res;
	Res.VertUV = In.VertUV;
	Res.VertColor = In.VertColor;
	
	// Transform the position by the model view projection matrix.
	Res.VertPos =  mul(ViewProjection, float4(In.VertPos, 1.0));
	
	return Res;
}