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
	float4 VertWorld : POSITION;
};

VertexOutput main(VertexInput In)
{
	VertexOutput Out;
	Out.VertWorld = mul(Transform, float4(In.VertPos, 1.0f));
	Out.VertPos = mul(ViewProjection, Out.VertWorld);
	//Out.VertPos = float4(In.VertPos, 1.0f);
	Out.VertUV = In.VertUV;
	
	// Transform the surface normal by the model's matrix
	Out.VertNormal = normalize(mul(Transform, float4(In.VertNormal, 0.0f)));
	
	return Out;
}