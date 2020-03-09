#version 400

in Fragment {
  vec2 tex;
  vec2 mapping;
}
fragment;

layout(location = 0) out vec4 fragmentColor;

uniform sampler2D imageTexure;

void main() {
  vec4 color = texture(imageTexure, fragment.tex);
  float r = dot(fragment.mapping, fragment.mapping);
  if (r > 1) discard;
  fragmentColor = vec4(color.rgb, 1 - r * r);
}
