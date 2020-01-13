#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"  // al::addSphere
#include "al/math/al_Random.hpp"      // al::rnd::uniform
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

  Vec3f rando(float scale) {
    return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
  };

  void onAnimate(double dt) override {
    // a "lambda" is a c++11 feature
    // a way to define a function
    // has syntax:
    // return-type variable-name = [capture-block](parameter-list){code-block}
    // -> return-type
    auto rv = [](float scale) -> Vec3f {
      return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
    };
    m.vertices()[10] += rv(0.001);
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
