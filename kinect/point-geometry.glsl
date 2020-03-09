#version 400

// take in a point and output a triangle strip with 4 vertices (aka a "quad")
//
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 al_ProjectionMatrix;
uniform float pointSize;

in Vertex {
  vec2 tex;

}
vertex[];

out Fragment {
  vec2 tex;
  vec2 mapping;
}
fragment;

void main() {
  mat4 m = al_ProjectionMatrix;   // rename to make lines shorter
  vec4 v = gl_in[0].gl_Position;  // al_ModelViewMatrix * gl_Position

  float r = pointSize;
  //r *= 1;

  gl_Position = m * (v + vec4(-r, -r, 0.0, 0.0));
  fragment.tex = vertex[0].tex;
  fragment.mapping = vec2(-1.0, -1.0);
  EmitVertex();

  gl_Position = m * (v + vec4(r, -r, 0.0, 0.0));
  fragment.tex = vertex[0].tex;
  fragment.mapping = vec2(1.0, -1.0);
  EmitVertex();

  gl_Position = m * (v + vec4(-r, r, 0.0, 0.0));
  fragment.tex = vertex[0].tex;
  fragment.mapping = vec2(-1.0, 1.0);
  EmitVertex();

  gl_Position = m * (v + vec4(r, r, 0.0, 0.0));
  fragment.tex = vertex[0].tex;
  fragment.mapping = vec2(1.0, 1.0);
  EmitVertex();

  EndPrimitive();
}
