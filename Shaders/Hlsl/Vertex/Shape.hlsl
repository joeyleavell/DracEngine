cbuffer Scene : register(b0, space0)
{
	float4x4 ViewProjection;
}

struct VertexInput
{
	float3 Position : SV_Position;
	float4 Color : COLOR;
};

struct VertexOutput
{
	float4 VertPos : SV_Position;
	float4 VertColor : COLOR;
};

VertexOutput main(VertexInput In)
{
	VertexOutput Out;
	Out.VertColor = In.Color;

	// Transform the position by the model view projection matrix.
	Out.VertPos = mul(ViewProjection, float4(In.Position, 1.0));
	
	return Out;	
}