#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec3 vertexNormal_modelspace;
layout(location = 4) in vec3 vertexTangent_modelspace;
layout(location = 5) in vec3 vertexBitangent_modelspace;

out vec2 UV; 
out vec3 fragmentColor;
out vec3 normal;
out vec3 Position_worldspace;

out vec3 LightDirection_cameraspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 Tangent_cameraspace;
out vec3 Bitangent_cameraspace;

out vec3 LightDirection_tangentspace;
out vec3 EyeDirection_tangentspace;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 LightPosition_worldspace;

uniform mat3 MV3x3;
uniform sampler2D NormalTextureSampler;

void main(){

	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	// Position of the vertex, in worldspace : M * position
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

	// Normal of the the vertex, in camera space
	//Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz;
	Normal_cameraspace = MV3x3 * normalize(vertexNormal_modelspace);
	Tangent_cameraspace = MV3x3 * normalize(vertexTangent_modelspace);
	Bitangent_cameraspace = MV3x3 * normalize(vertexBitangent_modelspace);
	mat3 TBN = transpose(mat3(
        Tangent_cameraspace,
        Bitangent_cameraspace,
        Normal_cameraspace
    ));
	LightDirection_tangentspace = TBN * LightDirection_cameraspace;
	EyeDirection_tangentspace =  TBN * EyeDirection_cameraspace;



	fragmentColor = vertexColor;
	UV = vertexUV;
}

