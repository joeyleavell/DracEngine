#version 460

out gl_PerVertex
{
    vec4 gl_Position;
};

struct VertexInput
{
    vec3 VertPos;
    vec2 VertUV;
    vec3 VertNormal;
};

struct VertexOutput
{
    vec4 VertPos;
    vec2 VertUV;
    vec3 VertNormal;
};

layout(binding = 0, std140) uniform type_Scene
{
    mat4 ViewProjection;
} Scene;

layout(location = 0) in vec3 in_var_SV_Position;
layout(location = 1) in vec2 in_var_TEXCOORD;
layout(location = 2) in vec3 in_var_NORMAL;
layout(location = 0) out vec2 out_var_TEXCOORD;
layout(location = 1) out vec3 out_var_NORMAL;

VertexOutput src_main(VertexInput In)
{
    VertexOutput Out;
    Out.VertPos = vec4(In.VertPos, 1.0) * Scene.ViewProjection;
    Out.VertUV = In.VertUV;
    Out.VertNormal = In.VertNormal;
    return Out;
}

void main()
{
    VertexInput param_var_In = VertexInput(in_var_SV_Position, in_var_TEXCOORD, in_var_NORMAL);
    VertexOutput _37 = src_main(param_var_In);
    gl_Position = _37.VertPos;
    out_var_TEXCOORD = _37.VertUV;
    out_var_NORMAL = _37.VertNormal;
}


