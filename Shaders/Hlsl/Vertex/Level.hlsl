cbuffer Scene : register(b0, space0)
{
	float4x4 ViewProjection;
}

cbuffer Model : register(b1, space1)
{
	float4x4 Transform;
}

struct VertexInput
{
	float3 VertPos : SV_Position;
	float2 VertUV : TEXCOORD;
	float3 VertNormal : NORMAL;
};

struct VertexOutput
{
	float4 VertPos : SV_Position;
	float2 VertUV : TEXCOORD;
	float3 VertNormal : NORMAL;	
};

VertexOutput main(VertexInput In)
{
	VertexOutput Out;
	Out.VertPos = mul(ViewProjection, mul(Transform, float4(In.VertPos, 1.0f)));
	//Out.VertPos = float4(In.VertPos, 1.0f);
	Out.VertUV = In.VertUV;
	Out.VertNormal = In.VertNormal;
	
	return Out;
}