#include "al/app/al_DistributedApp.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al_ext/statedistribution/al_CuttleboneStateSimulationDomain.hpp"

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

Vec3f rv(float scale) {
  return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
}

string slurp(string fileName);  // forward declaration

#define N (1000)

struct SharedState {
  Vec3f position[N];
  Pose camera;
  float background;
  float pointSize;
};

struct AlloApp : public DistributedAppWithState<SharedState> {
  Parameter pointSize{"/pointSize", "", 0.7, "", 0.0, 1.0};
  Parameter background{"/background", "", 0.1, "", 0.0, 1.0};
  Parameter timeStep{"/timeStep", "", 0.1, "", 0.01, 0.6};
  ControlGUI gui;

  ShaderProgram pointShader;
  Mesh mesh;

  vector<Vec3f> velocity;
  vector<Vec3f> acceleration;
  vector<float> mass;

  std::shared_ptr<CuttleboneStateSimulationDomain<SharedState>>
      cuttleboneDomain;

  void onCreate() override {
    cuttleboneDomain =
        CuttleboneStateSimulationDomain<SharedState>::enableCuttlebone(this);
    if (!cuttleboneDomain) {
      std::cerr << "ERROR: Could not start Cuttlebone. Quitting." << std::endl;
      quit();
    }

    gui << pointSize << background << timeStep;
    gui.init();
    navControl().useMouse(false);

    // compile shaders
    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    // c++11 "lambda" function
    auto rc = []() { return HSV(rnd::uniform(), 1.0f, 1.0f); };

    mesh.primitive(Mesh::POINTS);
    for (int i = 0; i < N; i++) {
      mesh.vertex(rv(5));
      mesh.color(rc());

      float m = 3 + rnd::normal() / 2;
      if (m < 0.5) m = 0.5;
      mass.push_back(m);

      mesh.texCoord(pow(m, 1.0f / 3), 0);  // s, t

      velocity.push_back(rv(0.1));
      acceleration.push_back(rv(1));
    }

    nav().pos(0, 0, 10);
  }

  void onAnimate(double dt) override {
    if (cuttleboneDomain->isSender()) {
      // ignore the real dt and set the time step;
      dt = timeStep;

      // drag
      for (int i = 0; i < velocity.size(); i++) {
        acceleration[i] -= velocity[i] * 0.1;
      }

      // Integration
      //
      vector<Vec3f>& position(mesh.vertices());
      for (int i = 0; i < velocity.size(); i++) {
        // "backward" Euler integration
        velocity[i] += acceleration[i] / mass[i] * dt;
        position[i] += velocity[i] * dt;
        state().position[i] = position[i];
      }

      state().background = background;
      state().pointSize = pointSize;
      state().camera = nav();

      // clear all accelerations (IMPORTANT!!)
      for (auto& a : acceleration) a.zero();

      navControl().active(!isImguiUsingInput());
    } else {
      nav().set(state().camera);
      vector<Vec3f>& position(mesh.vertices());
      for (int i = 0; i < velocity.size(); i++) {
        position[i] = state().position[i];
      }
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(Color(state().background));
    g.shader(pointShader);
    g.shader().uniform("pointSize", state().pointSize / 20);
    gl::blending(true);
    gl::blendTrans();
    gl::depthTesting(true);
    g.draw(mesh);
    gui.draw(g);
  }

  bool onKeyDown(const Keyboard& k) override {
    if (k.key() == '1') {
      for (int i = 0; i < velocity.size(); i++) {
        acceleration[i] = rv(1) / mass[i];
      }
    }

    return true;
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
