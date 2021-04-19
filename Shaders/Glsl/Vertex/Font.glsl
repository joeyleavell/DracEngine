#version 460

out gl_PerVertex
{
    vec4 gl_Position;
};

struct VertexInput
{
    vec3 Position;
    vec2 TexCoord;
    vec4 Color;
};

struct VertexOutput
{
    vec4 VertPos;
    vec2 VertTexCoord;
    vec4 VertColor;
};

layout(binding = 0, std140) uniform type_Scene
{
    mat4 ViewProjection;
} Scene;

layout(location = 0) in vec3 in_var_SV_Position;
layout(location = 1) in vec2 in_var_TEXCOORD;
layout(location = 2) in vec4 in_var_COLOR;
layout(location = 0) out vec2 out_var_TEXCOORD;
layout(location = 1) out vec4 out_var_COLOR;

VertexOutput src_main(VertexInput In)
{
    VertexOutput Out;
    Out.VertTexCoord = In.TexCoord;
    Out.VertColor = In.Color;
    Out.VertPos = vec4(In.Position, 1.0) * Scene.ViewProjection;
    return Out;
}

void main()
{
    VertexInput param_var_In = VertexInput(in_var_SV_Position, in_var_TEXCOORD, in_var_COLOR);
    VertexOutput _37 = src_main(param_var_In);
    gl_Position = _37.VertPos;
    out_var_TEXCOORD = _37.VertTexCoord;
    out_var_COLOR = _37.VertColor;
}


