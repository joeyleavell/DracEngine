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
	// Pass data straight through
	VertexOutput Out;
	Out.VertPos = float4(In.VertPos, 1.0);
	Out.VertUV = In.VertUV;
	Out.VertNormal = In.VertNormal;
	
	return Out;
}