#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"  // gui.draw(g)

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

Vec3f rv(float scale) {
  return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
}

string slurp(string fileName);

struct AlloApp : App {
  Parameter pointSize{"/pointSize", "", 1.0, "", 0.0, 2.0};
  Parameter timeStep{"/timeStep", "", 0.1, "", 0.01, 0.6};

  ControlGUI gui;
  ShaderProgram pointShader;
  Mesh mesh;
  vector<Vec3f> velocity;
  vector<Vec3f> acceleration;
  vector<float> mass;

  void onCreate() override {
    // add more GUI here
    gui << pointSize << timeStep;
    gui.init();
    navControl().useMouse(false);

    // compile shaders
    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    // set initial conditions

    // c++11 "lambda" function
    auto rc = []() { return HSV(rnd::uniform(), 1.0f, 1.0f); };

    mesh.primitive(Mesh::POINTS);
    for (int r = 0; r < 1000; r++) {
      mesh.vertex(rv(5));
      mesh.color(rc());

      //      float m = rnd::uniform(3.0, 0.5);
      float m = 3 + rnd::normal() * 0.1;
      if (m < 0.5) m = 0.5;
      mass.push_back(m);
      mesh.texCoord(pow(m, 1.0f / 3), 0);

      // separate state arrays
      velocity.push_back(rv(0.1));
      acceleration.push_back(rv(0));
    }

    nav().pos(0, 0, 10);
  }

  bool freeze = false;
  void onAnimate(double dt) override {
    if (freeze) return;

    // ignore the real dt and set the time step;
    dt = timeStep;

    // Calculate forces

    // drag
    for (int i = 0; i < velocity.size(); i++) {
      acceleration[i] -= velocity[i] * 0.07;
    }

    // Integration
    //
    vector<Vec3f>& position(mesh.vertices());
    for (int i = 0; i < velocity.size(); i++) {
      // "backward" Euler integration
      velocity[i] += acceleration[i] / mass[i] * dt;
      position[i] += velocity[i] * dt;

      // Explicit (or "forward") Euler integration would look like this:
      // position[i] += velocity[i] * dt;
      // velocity[i] += acceleration[i] * dt;
    }

    // clear all accelerations (IMPORTANT!!)
    for (auto& a : acceleration) a.zero();
  }

  bool onKeyDown(const Keyboard& k) override {
    if (k.key() == ' ') {
      freeze = !freeze;
    }

    if (k.key() == '1') {
      // introduce some "random" forces
      for (int i = 0; i < velocity.size(); i++) {
        acceleration[i] = rv(1);
      }
    }

    return true;
  }

  void onDraw(Graphics& g) override {
    g.clear(0.01);
    g.shader(pointShader);
    g.shader().uniform("pointSize", pointSize / 100);
    g.depthTesting(true);
    g.draw(mesh);
    gui.draw(g);
  }
};

int main() { AlloApp().start(); }

string slurp(string fileName) {
  fstream file(fileName);
  string returnValue = "";
  while (file.good()) {
    string line;
    getline(file, line);
    returnValue += line + "\n";
  }
  return returnValue;
}
