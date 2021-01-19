#version 330 core

in vec2 UV;
in vec3 fragmentColor;
in vec3 normal;
in vec3 Normal_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Position_worldspace;;

out vec3 color;

uniform sampler2D myTextureSampler;
uniform vec3 uLightColor;
uniform float uLightPower;
uniform vec3 LightPosition_worldspace;


void main(){
	
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );
	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );

	// Cosine of the angle between the normal and the light direction,
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );
	
	//Material properties
	vec3 MaterialDiffuseColor =  texture( myTextureSampler, UV ).rgb;

	//Light properties
	vec3 LightColor = uLightColor;
	float LightPower = uLightPower;
	float distance = length(Position_worldspace - LightPosition_worldspace);


	//color = texture( myTextureSampler, UV ).rgb+fragmentColor;
	//color = MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance);
	color = MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance);
}