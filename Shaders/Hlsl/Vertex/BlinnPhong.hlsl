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
	float3 VertTangent : TANGENT;
	float3 VertBiTangent : BINORMAL;

};

struct VertexOutput
{
	float4 VertPos : SV_Position;
	float2 VertUV : TEXCOORD;
	float3 VertNormal : NORMAL;	
	float4 VertWorld : POSITION;
	float3x3 TBN : TANGENT;
	float3 Tangent : TANGENT1;
	float3 BiTan : BINORMAL;
	
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
	
	// Construct TBN matrix
	float3 T = normalize(mul(Transform, float4(In.VertTangent, 0.0f)));
	T = normalize(T - dot(T, Out.VertNormal) * Out.VertNormal);
	
	float3 B = cross(Out.VertNormal, T);//normalize(mul(Transform, float4(In.VertBiTangent, 0.0f)));
	
	Out.Tangent = T;
	Out.BiTan = B;
	
	Out.TBN = float3x3(T, B, Out.VertNormal);
	
	return Out;
}