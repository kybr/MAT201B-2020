#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"  // gui.draw(g)

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

Vec3f rv(float scale = 1.0f) {
  return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
}

string slurp(string fileName);  // forward declaration

struct AlloApp : App {
  // add more GUI here
  Parameter moveRate{"/moveRate", "", 1.0, "", 0.0, 2.0};
  Parameter turnRate{"/turnRate", "", 1.0, "", 0.0, 2.0};
  Parameter localRadius{"/localRadius", "", 0.4, "", 0.01, 0.9};
  Parameter size{"/size", "", 1.0, "", 0.0, 2.0};
  Parameter ratio{"/ratio", "", 1.0, "", 0.0, 2.0};
  ControlGUI gui;

  ShaderProgram shader;
  Mesh mesh;

  vector<Pose> agent;

  void onCreate() override {
    // add more GUI here
    gui << moveRate << turnRate << localRadius << size << ratio;
    gui.init();
    navControl().useMouse(false);

    // compile shaders
    shader.compile(slurp("../tetrahedron-vertex.glsl"),
                   slurp("../tetrahedron-fragment.glsl"),
                   slurp("../tetrahedron-geometry.glsl"));

    mesh.primitive(Mesh::POINTS);

    for (int _ = 0; _ < 1000; _++) {
      Pose p;
      p.pos(rv());
      p.faceToward(rv());
      agent.push_back(p);

      mesh.vertex(p.pos());
      mesh.normal(p.uf());
      Vec3f up(p.uu());
      mesh.color(up.x, up.y, up.z);
    }

    nav().pos(0, 0, 10);
  }

  void onAnimate(double dt) override {
    // suggested data structure
    //
    struct Data {
      Vec3f heading, center;
      unsigned neighborCount{0};
      float distance{0};
    };
    vector<Data> data;
    data.resize(agent.size());

    // for each pair of agents
    //
    int N = agent.size();
    for (unsigned i = 0; i < N; i++)
      for (unsigned j = 1 + i; j < N; j++) {
        float distance = (agent[j].pos() - agent[i].pos()).mag();
        if (distance < localRadius) {
          // put code here
        }
      }

    //
    //
    for (unsigned i = 0; i < N; i++) {
      agent[i].faceToward(rv(), 0.03 * turnRate);
      agent[i].pos() += agent[i].uf() * moveRate * 0.002;

      if (agent[i].pos().mag() > 1.1) {
        agent[i].pos(rv());
        agent[i].faceToward(rv());
      }
    }

    // visualize the agents
    //
    vector<Vec3f>& m(mesh.vertices());
    vector<Vec3f>& n(mesh.normals());
    vector<Color>& c(mesh.colors());
    for (unsigned i = 0; i < N; i++) {
      m[i] = agent[i].pos();
      n[i] = agent[i].uf();
      const Vec3d& up(agent[i].uu());
      c[i].set(up.x, up.y, up.z);
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(0.1, 0.1, 0.1);
    // gl::depthTesting(true); // or g.depthTesting(true);
    // gl::blending(true); // or g.blending(true);
    // gl::blendTrans(); // or g.blendModeTrans();
    g.shader(shader);
    g.shader().uniform("size", size * 0.03);
    g.shader().uniform("ratio", ratio * 0.2);
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
