#version 460

struct PixelInput
{
    vec2 VertTexCoord;
    vec4 VertColor;
};

struct PixelOutput
{
    vec4 PixelColor;
};

uniform sampler2D SPIRV_Cross_CombinedFontTextureFontSampler;

layout(location = 0) in vec2 in_var_TEXCOORD;
layout(location = 1) in vec4 in_var_COLOR;
layout(location = 0) out vec4 out_var_SV_Target;

PixelOutput src_main(PixelInput In)
{
    PixelOutput Out;
    Out.PixelColor = In.VertColor * texture(SPIRV_Cross_CombinedFontTextureFontSampler, In.VertTexCoord).x;
    if (Out.PixelColor.x <= 0.00999999977648258209228515625)
    {
        discard;
    }
    return Out;
}

void main()
{
    PixelInput param_var_In = PixelInput(in_var_TEXCOORD, in_var_COLOR);
    PixelOutput _31 = src_main(param_var_In);
    out_var_SV_Target = _31.PixelColor;
}


