#version 330 core

uniform sampler2D USampler;

out vec4 OutColor;

in vec2 VTexCoord;
in vec4 VColor;

void main()
{	
	vec4 TexColor = texture(USampler, VTexCoord);
	if(TexColor.r <= .01)
		discard;
		
	OutColor = vec4(VColor.rgb, TexColor.r);
}