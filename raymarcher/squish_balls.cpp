// Mert Toka / 2020
// example raymarcher in AlloLib
//
#include "al/app/al_App.hpp"
#include "al/ui/al_ControlGUI.hpp"
using namespace al;

#include <fstream>
#include <vector>
using namespace std;

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

struct AlloApp : App {
  Parameter count{"/count", "", 10.0, "", 1.0, 50.0};
  Parameter morph{"/morph", "", 0.8, "", 0.0, 1.0};
  ShaderProgram raymarcher;
  ControlGUI gui;
  Mesh mesh;

  void onCreate() override {  //
    gui << count << morph;
    gui.init();

    // disable all controls
    navControl().disable();
    navControl().useMouse(false);

    // plane that covers the whole viewport
    mesh.primitive(Mesh::TRIANGLE_STRIP);
    mesh.vertex(-1, 1);
    mesh.vertex(-1, -1);
    mesh.vertex(1, 1);
    mesh.vertex(1, -1);

    // compile shaders
    raymarcher.compile(slurp("../vertex.glsl"), slurp("../fragment.glsl"));

    nav().pos(0, 0, 0.2);
  }

  double time = 0;
  void onAnimate(double dt) override { time += dt; }

  void onDraw(Graphics& g) override {
    g.clear(0.0);
    g.shader(raymarcher);
    g.shader().uniform("time", (float)time);
    g.shader().uniform("res", Vec2f(width(), height()));
    g.shader().uniform("count", count);
    g.shader().uniform("morph", morph);
    g.draw(mesh);

    gui.draw(g);
  }
};

int main() { AlloApp().start(); }
