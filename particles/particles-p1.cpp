#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"  // gui.draw(g)

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

string slurp(string fileName);  // forward declaration

struct AlloApp : App {
  Parameter pointSize{"/pointSize", "", 1.0, "", 0.0, 2.0};
  Parameter timeStep{"/timeStep", "", 0.1, "", 0.01, 0.6};
  Parameter gravConst{"/gravConst", "", 0.1, "", 0.01, 0.99};
  Parameter dragFactor{"/dragFactor", "", 0.1, "", 0.01, 0.99};
  Parameter maxAccel{"/maxAccel", "", 1.0, "", 0.01, 7.0};  // ??
  // add more GUI here
  ControlGUI gui;

  ShaderProgram pointShader;
  Mesh mesh;

  // simulation state
  vector<Vec3f> velocity;
  vector<Vec3f> acceleration;
  vector<float> mass;

  void onCreate() override {
    gui << pointSize << timeStep << gravConst << dragFactor << maxAccel;
    // add more GUI here
    gui.init();
    navControl().useMouse(false);

    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    mesh.primitive(Mesh::POINTS);

    reset();

    nav().pos(0, 0, 10);
  }

  void reset() {
    // empty all containers
    mesh.reset();
    velocity.clear();
    acceleration.clear();

    // seed random number generators to maintain determinism
    rnd::Random<> rng;
    rng.seed(42);
    auto rc = [&]() { return HSV(rng.uniform(), 1.0f, 1.0f); };
    auto rv = [&](float scale) -> Vec3f {
      return Vec3f(rng.uniformS(), rng.uniformS(), rng.uniformS()) * scale;
    };

    for (int _ = 0; _ < 2; _++) {
      mesh.vertex(rv(5));
      mesh.color(rc());

      // float m = rnd::uniform(3.0, 0.5);
      float m = 3 + rnd::normal() / 2;
      if (m < 0.5) m = 0.5;
      mass.push_back(m);

      // using a simplified volume/size relationship
      mesh.texCoord(pow(m, 1.0f / 3), 0);  // s, t

      // separate state arrays
      // XXX use the cross product to make an initial velocity that "tends to
      // spin around the y axis"
      velocity.push_back(rv(0.1));
      acceleration.push_back(rv(1));
    }
  }

  float biggestEver{0};

  bool freeze = false;
  void onAnimate(double dt) override {
    if (freeze) return;

    // ignore the real dt and set the time step;
    dt = timeStep;

    {
      const vector<Vec3f>& position(mesh.vertices());
      for (int i = 0; i < position.size(); i++) {
        for (int j = 1 + i; j < position.size(); j++) {
          Vec3f r = position[j] - position[i];
          Vec3f F = (float)gravConst * r / pow(r.mag(), 3);
          acceleration[i] += F * mass[j];
          acceleration[j] -= F * mass[i];
        }
      }
    }

    // drag
    for (int i = 0; i < velocity.size(); i++) {
      acceleration[i] -= velocity[i] * dragFactor;
    }

    // measure
    float recentMaximum = 0;
    for (int i = 0; i < acceleration.size(); i++) {
      if (acceleration[i].mag() > recentMaximum)  //
        recentMaximum = acceleration[i].mag();
    }
    if (recentMaximum > biggestEver) {
      biggestEver = recentMaximum;
      cout << "Biggest Ever Acceleration Magnitude:" << biggestEver << endl;
    }

    for (int i = 0; i < acceleration.size(); i++) {
      float m = acceleration[i].mag();
      if (m > maxAccel) {
        acceleration[i].normalize(maxAccel);
        cout << "Limiting Acceleration: " << m << " -> " << (float)maxAccel
             << endl;
      }
    }

    // Integration
    //
    vector<Vec3f>& position(mesh.vertices());
    for (int i = 0; i < velocity.size(); i++) {
      // "backward" Euler integration
      velocity[i] += acceleration[i] / mass[i] * dt;
      position[i] += velocity[i] * dt;
    }

    // clear all accelerations (IMPORTANT!!)
    for (auto& a : acceleration) a.zero();
  }

  bool onKeyDown(const Keyboard& k) override {
    if (k.key() == ' ') {
      freeze = !freeze;
    }

    if (k.key() == 'r') {
      //
      reset();
    }

    return true;
  }

  void onDraw(Graphics& g) override {
    g.clear(0.3);
    g.shader(pointShader);
    g.shader().uniform("pointSize", pointSize / 100);
    g.blending(true);
    g.blendModeTrans();
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
