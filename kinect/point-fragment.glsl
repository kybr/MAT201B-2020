#version 400

in Fragment {
  vec2 tex;
  vec2 mapping;
}
fragment;

layout(location = 0) out vec4 fragmentColor;

uniform sampler2D imageTexure;

// https://github.com/OpenKinect/libfreenect/blob/7398a410400198f58fae2801cd6347ac3c867e00/examples/glpclview.c#L71
mat4 rgbMatrix() {

  // XXX i don't know where i would apply this!
  return mat4(
     5.34866271e+02,   3.89654806e+00,   0.00000000e+00,   1.74704200e-02,
    -4.70724694e+00,  -5.28843603e+02,   0.00000000e+00,  -1.22753400e-02,
    -3.19670762e+02,  -2.60999685e+02,   0.00000000e+00,  -9.99772000e-01,
    -6.98445586e+00,   3.31139785e+00,   0.00000000e+00,   1.09167360e-02
  );
}


void main() {
  vec4 color = texture(imageTexure, fragment.tex);
  float r = dot(fragment.mapping, fragment.mapping);
  if (r > 1) discard;
  fragmentColor = vec4(color.rgb, 1 - r * r);
}
