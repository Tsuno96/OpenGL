#version 330 core

in vec2 UV;
in vec3 fragmentColor;

out vec3 color;

uniform sampler2D myTextureSampler;

void main(){
  color = texture( myTextureSampler, UV ).rgb *0.8 + fragmentColor*0.2;

}