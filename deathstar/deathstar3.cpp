#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
using namespace al;

struct MyApp : App {
  Mesh m;
  Light light;
  Mesh mesh;

  void onCreate() override {
    //
    addSphere(m, 1, 16, 16);
    m.primitive(Mesh::POINTS);

    light.pos(5, 5, 5);

    addSphere(mesh, 0.01, 100, 100);
    mesh.generateNormals();
  }

  void onAnimate(double dt) override {
    //
  }

  void onDraw(Graphics& g) override {
    g.clear(0.1);
    g.depthTesting(true);
    g.lighting(true);
    g.light(light);

    for (int i = 0; i < m.vertices().size(); i++) {
      g.pushMatrix();
      g.translate(m.vertices()[i]);
      g.draw(mesh);
      g.popMatrix();
    }
  }
};

int main() {
  MyApp app;
  app.start();
}
