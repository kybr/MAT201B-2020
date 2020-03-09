#version 400

layout(location = 0) in vec3 vertexPosition;

layout(location = 2) in vec2 vertexTex;


uniform mat4 al_ModelViewMatrix;
uniform mat4 al_ProjectionMatrix;

out Vertex {
  vec2 tex;
}
vertex;

// https://github.com/OpenKinect/libfreenect/blob/7398a410400198f58fae2801cd6347ac3c867e00/examples/glpclview.c#L51
mat4 vertexMatrix() {
  float fx = 594.21f;
  float fy = 591.04f;
  float a = -0.0030711f;
  float b = 3.3309495f;
  float cx = 339.5f;
  float cy = 242.7f;
  return mat4(
    1/fx,       0,  0, 0,
    0,      -1/fy,  0, 0,
    0,          0,  0, a,
    -cx/fx, cy/fy, -1, b
  );
}

void main() {
  gl_Position = al_ModelViewMatrix * vertexMatrix() * vec4(vertexPosition, 1.0);
  vertex.tex = vertexTex;
}
