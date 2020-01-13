#include "al/app/al_App.hpp"
using namespace al;

int main() {
    // Color, RGB, HSV, Luv, CIE
  Color c(HSV(0.707, 0.9, 0.7));
  for (int i = 0; i < 4; i++ ) {
      printf("%f ", c.components[i]);
  }
  printf("\n");
}
