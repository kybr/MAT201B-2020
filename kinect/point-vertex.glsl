#version 400

layout(location = 0) in vec3 vertexPosition;

layout(location = 2) in vec2 vertexTex;


uniform mat4 al_ModelViewMatrix;
uniform mat4 al_ProjectionMatrix;

out Vertex {
  vec2 tex;
}
vertex;

void main() {
  gl_Position = al_ModelViewMatrix * vec4(vertexPosition, 1.0);
  vertex.tex = vertexTex;
}
