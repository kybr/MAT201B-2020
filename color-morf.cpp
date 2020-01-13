#include "al/app/al_App.hpp"
using namespace al;

struct MyApp : App {
  float value;

  void onCreate() override {
    //
    value = 0;
  }

  void onAnimate(double dt) override {
    value += dt / 10;
    if (value > 1)  //
      value -= 1;
  }

  void onDraw(Graphics& g) override {
    // g.clear(RGB(value, 1 - value, 1 - value * value));
    g.clear(HSV(value, 1.0, 1.0));
  }
};

int main() {
  MyApp app;
  app.start();
}
