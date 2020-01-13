#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"  // al::addSphere
using namespace al;

struct MyApp : App {
  Mesh m;

  void onCreate() override {
    //
    addSphere(m, 1, 16, 16);
    m.primitive(Mesh::POINTS);
  }

  void onAnimate(double dt) override {
    //
  }

  void onDraw(Graphics& g) override {
    g.clear(0.0);
    g.draw(m);
    //
  }
};

int main() {
  MyApp app;
  app.start();
}
