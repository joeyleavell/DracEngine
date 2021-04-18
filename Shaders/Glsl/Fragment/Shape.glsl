#version 460

struct PixelInput
{
    vec4 VertColor;
};

struct PixelOutput
{
    vec4 PixelColor;
};

layout(location = 0) in vec4 in_var_COLOR;
layout(location = 0) out vec4 out_var_SV_Target;

PixelOutput src_main(PixelInput In)
{
    PixelOutput Out;
    Out.PixelColor = In.VertColor;
    return Out;
}

void main()
{
    PixelInput param_var_In = PixelInput(in_var_COLOR);
    out_var_SV_Target = src_main(param_var_In).PixelColor;
}


