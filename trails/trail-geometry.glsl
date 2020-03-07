#version 400

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 al_ProjectionMatrix;

in Vertex {
  vec4 color;
  float size;
  int id;
}
vertex[];

out Fragment {
  vec4 color;
}
fragment;

uniform int ID;
uniform int M;

void main() {

  // skip certain connections. not totally correct, but meh...
  if ((vertex[0].id % M) == ID)
    return;
  if ((vertex[0].id % M) == M - 1)
    return;

  mat4 m = al_ProjectionMatrix;  // rename to make lines shorter
  vec4 a = gl_in[0].gl_Position;
  vec4 b = gl_in[1].gl_Position;

  const float r = 0.03;

  // does vec3(0.0, 0.0, 1.0) point at the eye in the coordinate system?
  // we hope that this billboards; it seems to, but really?
  // XXX i think that this is broken; it's just not quite right
  vec4 d = vec4(normalize(cross(b.xyz - a.xyz, vec3(0.0, 0.0, 1.0))), 0.0) * r;

  gl_Position = m * (a + d * vertex[0].size);
  fragment.color = vertex[0].color;
  EmitVertex();

  gl_Position = m * (a - d * vertex[0].size);
  fragment.color = vertex[0].color;
  EmitVertex();

  gl_Position = m * (b + d * vertex[1].size);
  fragment.color = vertex[1].color;
  EmitVertex();

  gl_Position = m * (b - d * vertex[1].size);
  fragment.color = vertex[1].color;
  EmitVertex();

  EndPrimitive();
}
