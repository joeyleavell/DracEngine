#version 430 core

layout(location=2) uniform sampler2D u_sampler;

out vec4 frag_color;

in vec2 tex_coord;
in vec4 vert_color;

void main()
{
	vec4 Color = texture(u_sampler, tex_coord);
		
	frag_color = vert_color * vec4(Color.r, Color.g, Color.b, Color.a);
}