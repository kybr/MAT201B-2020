#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
using namespace al;

#include <fstream>
#include <vector>
using namespace std;

Vec3f rv(float scale) {
  return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
}

const int N = 13;
const int M = 123;

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
  ShaderProgram pointShader;
  ShaderProgram trailShader;

  Mesh mesh;
  Mesh history;

  vector<Vec3f> normal;

  void onCreate() override {
    auto rc = []() { return HSV(rnd::uniform(), 1.0f, 1.0f); };

    mesh.primitive(Mesh::POINTS);
    for (int i = 0; i < N; i++) {
      normal.push_back(rv(1).normalize());

      mesh.vertex(rv(2));
      mesh.color(rc());
      mesh.texCoord(rnd::uniform(0.2, 0.7), 0);
    }

    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    trailShader.compile(slurp("../trail-vertex.glsl"),
                        slurp("../trail-fragment.glsl"),
                        slurp("../trail-geometry.glsl"));

    history.primitive(Mesh::LINE_LOOP);
    vector<Vec3f>& m(mesh.vertices());
    for (int i = 0; i < N * M; i++) {
      history.vertex(m[i / M]);
      history.color(rc());
      history.texCoord(0.3, 0.0);
    }

    nav().pos(0, 0, 10);
  }

  int index = 0;
  void onAnimate(double dt) override {
    index++;
    if (index == M)  //
      index = 0;

    vector<Vec3f>& m(mesh.vertices());

    for (int i = 0; i < N; i++) {
      rotate(m[i], normal[i], dt * 1.3);
    }

    vector<Vec3f>& h(history.vertices());
    for (int i = 0; i < N; i++) {
      h[i * M + index] = m[i];
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(Color(0.3));
    // gl::blending(true);
    // gl::blendTrans();
    // gl::depthTesting(true);

    g.shader(trailShader);
    g.shader().uniform("ID", index);
    g.shader().uniform("M", M);
    g.draw(history);

    g.shader(pointShader);
    g.shader().uniform("pointSize", 1.0f / 33);
    g.draw(mesh);
  }
};

int main() { AlloApp().start(); }
