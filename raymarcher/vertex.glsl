#version 400

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 vertexTexture;

uniform mat4 al_ModelViewMatrix;
uniform mat4 al_ProjectionMatrix;

void main() {
  gl_Position = al_ProjectionMatrix * al_ModelViewMatrix * vec4(vertexPosition, 1.0);
}