#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"  // gui.draw(g)

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

string slurp(string fileName);  // forward declaration

struct AlloApp : App {
  ShaderProgram shader;
  Mesh mesh;

  void onCreate() override {
    shader.compile(slurp("../tetrahedron-vertex.glsl"),
                   slurp("../tetrahedron-fragment.glsl"),
                   slurp("../tetrahedron-geometry.glsl"));

    mesh.primitive(Mesh::POINTS);

    mesh.vertex(0, 0, 0);
    mesh.normal(1, 0, 0);
    mesh.color(0, 1, 0);

    nav().pos(0, 0, 5);
  }

  double t = 0;
  void onAnimate(double dt) override {
    t += dt;
    mesh.colors()[0].g = sin(t);  // y
    mesh.colors()[0].b = cos(t);  // z
  }

  void onDraw(Graphics& g) override {
    g.clear(0.1, 0.1, 0.1);
    g.rotate(t * 66, 0, 1, 0);
    gl::blending(true);
    gl::depthTesting(true);
    g.shader(shader);
    g.shader().uniform("size", 1.0);
    g.shader().uniform("ratio", 0.2);
    g.draw(mesh);
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
