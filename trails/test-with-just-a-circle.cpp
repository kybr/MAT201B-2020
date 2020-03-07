#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
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

const int M = 10;

struct AlloApp : App {
  ShaderProgram pointShader;
  ShaderProgram trailShader;

  Mesh mesh;
  Mesh point;

  void onCreate() override {
    point.primitive(Mesh::POINTS);
    point.vertex(0, 0, 0);
    point.color(Color(1));
    point.texCoord(0.5, 0);

    mesh.primitive(Mesh::LINE_LOOP);

    for (int i = 0; i < M; i++) {
      mesh.vertex(sin(2 * M_PI * i / M), cos(2 * M_PI * i / M), 0);
      mesh.color(HSV(1.0f * i / M, 1.0f, 1.0f));
      mesh.texCoord(0.3, 0.0);
    }

    for (int i = 0; i < M; i++) {
      mesh.vertex(sin(2 * M_PI * i / M), cos(2 * M_PI * i / M), -1);
      mesh.color(HSV(1.0f * i / M, 1.0f, 1.0f));
      mesh.texCoord(0.3, 0.0);
    }

    for (int i = 0; i < M; i++) {
      mesh.vertex(sin(2 * M_PI * i / M), cos(2 * M_PI * i / M), -2);
      mesh.color(HSV(1.0f * i / M, 1.0f, 1.0f));
      mesh.texCoord(0.3, 0.0);
    }

    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    trailShader.compile(slurp("../trail-vertex.glsl"),
                        slurp("../trail-fragment.glsl"),
                        slurp("../trail-geometry.glsl"));

    nav().pos(0, 0, 10);
  }

  int id = 0;
  void onAnimate(double dt) override {
    static double t = 0;
    t += dt;
    if (t > 0.5) {
      t -= 0.5;
      id++;
      if (id == M)  //
        id = 0;

      point.vertices()[0] = mesh.vertices()[id];
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(Color(0.2));

    g.shader(trailShader);
    g.shader().uniform("ID", id);
    g.shader().uniform("M", M);
    g.draw(mesh);

    g.shader(pointShader);
    g.shader().uniform("pointSize", 1.0f / 23);
    g.draw(point);
  }
};

int main() { AlloApp().start(); }
