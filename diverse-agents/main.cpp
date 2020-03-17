#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"  // gui.draw(g)

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

string slurp(string fileName);  // forward declaration

struct AlloApp : App {
  Parameter size{"/size", "", 1.0, "", 0.0, 2.0};
  Parameter ratio{"/ratio", "", 1.0, "", 0.0, 2.0};
  ParameterInt faceCount{"/faceCount", "", 3, "", 3, 10};
  Parameter spikiness{"/spikiness", "", 0.0, "", 0.0, 1.0};
  ControlGUI gui;

  ShaderProgram shader;
  Mesh mesh;

  Pose agent;

  void onCreate() override {
    gui << size << ratio << faceCount << spikiness;
    gui.init();
    navControl().useMouse(false);

    shader.compile(slurp("../tetrahedron-vertex.glsl"),
                   slurp("../tetrahedron-fragment.glsl"),
                   slurp("../tetrahedron-geometry.glsl"));

    mesh.primitive(Mesh::POINTS);

    agent.pos(Vec3f(0, 0, 0));
    agent.faceToward(Vec3f(1, 1, 1));
    mesh.vertex(agent.pos());
    mesh.normal(agent.uf());
    const Vec3f& up(agent.uu());
    mesh.color(up.x, up.y, up.z);
    mesh.texCoord(faceCount, spikiness);

    nav().pos(0, 0, 4);
  }

  void onAnimate(double dt) override {
#if 0
    vector<Vec3f>& v(mesh.vertices());
    vector<Vec3f>& n(mesh.normals());
    vector<Color>& c(mesh.colors());
    for (unsigned i = 0; i < N; i++) {
      v[i] = agent[i].pos();
      n[i] = agent[i].uf();
      const Vec3d& up(agent[i].uu());
      c[i].set(up.x, up.y, up.z);
    }
#endif

    mesh.texCoord2s()[0].x = faceCount;
    mesh.texCoord2s()[0].y = spikiness;
    angle += 1;
  }

  double angle = 0;

  void onDraw(Graphics& g) override {
    g.clear(Color(0.1));
    gl::depthTesting(true);
    gl::blending(true);
    gl::blendTrans();
    g.rotate(angle, 0, 1, 0);
    g.shader(shader);
    g.shader().uniform("size", size * 0.3);
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
