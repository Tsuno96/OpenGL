#version 330 core

in vec2 UV;
in vec3 fragmentColor;
in vec3 normal;
in vec3 Normal_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 Position_worldspace;;
in vec3 EyeDirection_cameraspace;;

out vec4 color;

uniform sampler2D myTextureSampler;
uniform vec3 uLightColor;
uniform float uLightPower;
uniform vec3 LightPosition_worldspace;
uniform float opacity;


void main(){
	
	// DIFFUSE
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );
	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );

	// Cosine of the angle between the normal and the light direction,
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );

	// Material properties
	//vec3 MaterialDiffuseColor =  texture( myTextureSampler, UV ).rgb;
	vec3 MaterialDiffuseColor =  fragmentColor;

	// Light properties
	vec3 LightColor = uLightColor;
	float LightPower = uLightPower;
	// Compute distance between vertice and light
	float distance = length(Position_worldspace - LightPosition_worldspace);

	// AMBIANT 
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;

	// SPECULAR
	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );

	//color = texture( myTextureSampler, UV ).rgb+fragmentColor;
	//color = MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance);
	// ComputeColor
	color.xyz =  
	// Ambiant component
	MaterialAmbientColor +
	// Diffuse component
	MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance) +
	// Specular component
	MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance * distance)
	;
	color.a =opacity;
}