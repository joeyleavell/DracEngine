#version 330 core

uniform sampler2D DiffuseTexture;
uniform vec3 diffuse_color;
uniform int UseTexture;

in vec2 VTexCoord;

void main()
{
	// Sample the diffuse texture

	vec4 DiffuseColor = vec4(diffuse_color, 1.0);

	if(UseTexture == 1)
	{
		DiffuseColor *= texture2D(DiffuseTexture, VTexCoord);
	}

	gl_FragColor = DiffuseColor;
}