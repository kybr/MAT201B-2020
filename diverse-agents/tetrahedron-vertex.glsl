#version 400

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color; // a not used
layout(location = 2) in vec2 tex; 
layout(location = 3) in vec3 normal;

out Vertex {
  vec3 position;
  vec3 forward;
  vec3 up;
  int faceCount;
  float spikiness;
}
vertex;

void main() {
  vertex.position = position;
  vertex.forward = normal;
  vertex.up = color.rgb;
  vertex.faceCount = int(tex.x);
  vertex.spikiness = tex.y;
}
