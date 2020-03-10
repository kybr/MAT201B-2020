#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/math/al_Random.hpp"
using namespace al;

struct AlloApp : App {
  Mesh mesh;

  void onCreate() override {
    addCube(mesh);
    mesh.primitive(Mesh::POINTS);
    nav().pos(0, 0, 10);
  }

  void onAnimate(double dt) override {
    mesh.indices().clear();
    mesh.index(rnd::uniform(8));
  }

  void onDraw(Graphics& g) override {
    g.clear(0.1, 0.1, 0.1);
    g.draw(mesh);
  }
};

int main() { AlloApp().start(); }
