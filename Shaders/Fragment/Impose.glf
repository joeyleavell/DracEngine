#version 330 core

uniform sampler2DMS InterfaceSampler;
uniform float ScreenWidth;
uniform float ScreenHeight;
uniform int USamples;

out vec4 OutColor;

in vec2 VTexCoord;

vec4 SampleTexture(sampler2DMS Sampler, vec2 ScreenSize, vec2 Coord)
{
    vec4 Color = vec4(0.0);

    for (int Sample = 0; Sample < USamples; Sample++)
        Color += texelFetch(Sampler, ivec2(Coord.x * ScreenWidth, Coord.y * ScreenHeight), Sample);

    return Color / float(USamples);
}

void main()
{
	vec4 InterfaceColor = SampleTexture(InterfaceSampler, vec2(ScreenWidth, ScreenHeight), VTexCoord);
	
	OutColor = InterfaceColor;
}