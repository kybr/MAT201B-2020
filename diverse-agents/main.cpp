#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"  // gui.draw(g)

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

string slurp(string fileName);  // forward declaration

struct AlloApp : App {
  Parameter size{"/size", "", 0.3, "", 0.0, 1.0};
  Parameter ratio{"/ratio", "", 2.0, "", 0.0, 4.0};
  Parameter fade{"/fade", "", 1.0, "", 0.0, 1.0};
  ControlGUI gui;

  ShaderProgram shader;
  Mesh mesh;

  void onCreate() override {
    gui << size << ratio << fade;
    gui.init();
    navControl().useMouse(false);

    shader.compile(slurp("../tetrahedron-vertex.glsl"),
                   slurp("../tetrahedron-fragment.glsl"),
                   slurp("../tetrahedron-geometry.glsl"));

    mesh.primitive(Mesh::POINTS);

    auto rv = []() {
      return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS());
    };
    auto r = []() { return rnd::uniform(); };

    for (int i = 0; i < 255; i++) {
      mesh.vertex(rv());
      mesh.normal(rv().normalize());
      Vec3f up = rv().normalize();
      mesh.color(up.x, up.y, up.z);
      float v = r();
      mesh.texCoord(v * v * 17 + 3, r());
    }

    nav().pos(0, 0, 4);
  }

  void onDraw(Graphics& g) override {
    g.clear(Color(0.1));
    gl::depthTesting(true);
    gl::blending(true);
    gl::blendTrans();
    g.shader(shader);
    g.shader().uniform("size", size * 0.3);
    g.shader().uniform("fade", fade);
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
