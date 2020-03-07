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
const int M = 3;

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

  vector<Vec3f> velocity;
  vector<Vec3f> acceleration;
  vector<float> mass;

  void onCreate() override {
    auto rc = []() { return HSV(rnd::uniform(), 1.0f, 1.0f); };

    mesh.primitive(Mesh::POINTS);
    for (int i = 0; i < N; i++) {
      mesh.vertex(rv(5));
      mesh.color(rc());

      // float m = rnd::uniform(3.0, 0.5);
      float m = 3 + rnd::normal() / 2;
      if (m < 0.5) m = 0.5;
      mass.push_back(m);

      // using a simplified volume/size relationship
      mesh.texCoord(pow(m, 1.0f / 3), 0);  // s, t

      // separate state arrays
      velocity.push_back(rv(0.1));
      acceleration.push_back(rv(1));
    }

    // compile shaders
    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    trailShader.compile(slurp("../trail-vertex.glsl"),
                        slurp("../trail-fragment.glsl"),
                        slurp("../trail-geometry.glsl"));

    history.primitive(Mesh::LINE_STRIP);
    vector<Vec3f>& m(mesh.vertices());
    for (int i = 0; i < N * M; i++) {
      history.vertex(m[i / M]);
      history.color(rc());
      history.texCoord(rnd::uniform(0.5, 0.05), 0.0);
    }

    nav().pos(0, 0, 10);
  }

  double t = 0;
  void onAnimate(double dt) override {
    t += dt;
    if (t > 3) {
      t -= 3;
      for (int i = 0; i < velocity.size(); i++) {
        acceleration[i] += rv(40);
      }
    }

    // drag
    for (int i = 0; i < velocity.size(); i++) {
      acceleration[i] -= velocity[i] * 0.5;
    }

    {
      static int n = 0;
      if (n >= M) n = 0;

      vector<Vec3f>& m(mesh.vertices());
      vector<Vec3f>& h(history.vertices());
      for (int i = 0; i < N; i++) {
        h[i * M + n] = m[i];
      }

      n++;
    }

    // Integration
    //
    vector<Vec3f>& position(mesh.vertices());
    for (int i = 0; i < velocity.size(); i++) {
      velocity[i] += acceleration[i] / mass[i] * dt;
      position[i] += velocity[i] * dt;
    }

    for (auto& a : acceleration) a.zero();
  }

  void onDraw(Graphics& g) override {
    g.clear(Color(0.3));
    gl::blending(true);
    gl::blendTrans();
    gl::depthTesting(true);

    //    g.shader(pointShader);
    //    g.shader().uniform("pointSize", 1.0f / 100);
    //    g.draw(mesh);

    g.shader(trailShader);
    g.draw(history);
  }
};

int main() { AlloApp().start(); }
