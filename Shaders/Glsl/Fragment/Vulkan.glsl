#version 460

struct PixelInput
{
    vec4 VertPos;
    vec2 VertUV;
    vec3 VertNormal;
};

struct PixelOutput
{
    vec4 PixelColor;
};

layout(binding = 1, std140) uniform type_Material
{
    float UseDiffuseTexture;
    vec3 DiffuseColor;
    vec3 AmbientColor;
    vec3 SpecularColor;
} Material;

uniform sampler2D SPIRV_Cross_CombinedDiffuseTextureDiffuseSampler;

layout(location = 0) in vec2 in_var_TEXCOORD;
layout(location = 1) in vec3 in_var_NORMAL;
layout(location = 0) out vec4 out_var_SV_Target;

PixelOutput src_main(PixelInput In)
{
    vec4 TexColor = texture(SPIRV_Cross_CombinedDiffuseTextureDiffuseSampler, In.VertUV);
    vec4 ActualColor = (TexColor * Material.UseDiffuseTexture) + (vec4(Material.DiffuseColor, 1.0) * (1.0 - Material.UseDiffuseTexture));
    PixelOutput Out;
    Out.PixelColor = ActualColor;
    return Out;
}

void main()
{
    PixelInput param_var_In = PixelInput(gl_FragCoord, in_var_TEXCOORD, in_var_NORMAL);
    out_var_SV_Target = src_main(param_var_In).PixelColor;
}


