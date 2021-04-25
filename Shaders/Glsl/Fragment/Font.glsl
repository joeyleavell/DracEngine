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
    float Value = texture(SPIRV_Cross_CombinedFontTextureFontSampler, In.VertTexCoord).x;
    PixelOutput Out;
    Out.PixelColor = In.VertColor * Value;
    Out.PixelColor.w = Value;
    if (Value <= 0.00999999977648258209228515625)
    {
        discard;
    }
    return Out;
}

void main()
{
    PixelInput param_var_In = PixelInput(in_var_TEXCOORD, in_var_COLOR);
    PixelOutput _34 = src_main(param_var_In);
    out_var_SV_Target = _34.PixelColor;
}


