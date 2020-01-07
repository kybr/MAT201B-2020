#include "al/app/al_App.hpp"
using namespace al;

struct MyApp : App {
  float value;
  float frequency;

  void onCreate() override {
    //

    value = 0;
  }

  void onAnimate(double dt) override {
    //
  }

  void onDraw(Graphics& g) override {
    g.clear(value);
    //
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      value += frequency / 44100;
      if (value > 1)  //
        value -= 1;

      io.out(0) = io.out(1) = value;
    }
  }

  bool onKeyDown(const Keyboard& k) override {
    //
    printf("got here\n");
    return false;
  }

  bool onMouseMove(const Mouse& m) override {
    //
    printf("%d,%d\n", m.x(), m.y());

    frequency = 880 * m.y() / height();
    return false;
  }
};

int main() {
  MyApp app;
  app.start();
}
