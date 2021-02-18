#version 330 core

// Directional light parameters
uniform vec3 LightDirection;
uniform vec3 LightColor;
uniform float LightIntensity;

// Material uniform parameters
uniform sampler2D DiffuseTexture;
uniform int HasDiffuseTexture;
uniform vec3 BaseDiffuseColor;

// Shader inputs
in vec2 VTexCoord;
in vec3 VNormal;

out vec4 FragColor;

void main()
{
	// Lighting diffuse component
	float DiffuseComponent = max(0.0, min(1.0, dot(VNormal, normalize(-LightDirection))));

	// Sample the diffuse material
	vec4 DiffuseColor = vec4(LightColor, 1.0) * DiffuseComponent * LightIntensity;
	if(HasDiffuseTexture == 1)
	{
		DiffuseColor *= texture2D(DiffuseTexture, VTexCoord);
	}
	else
	{
		DiffuseColor *= vec4(BaseDiffuseColor, 1.0);
	}

	// Calculate final color
	FragColor = DiffuseColor;
}