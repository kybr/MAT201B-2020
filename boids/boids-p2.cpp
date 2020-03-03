#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/spatial/al_HashSpace.hpp"
#include "al/ui/al_ControlGUI.hpp"  // gui.draw(g)

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

Vec3f rv() { return Vec3f(rnd::uniform(), rnd::uniform(), rnd::uniform()); }

string slurp(string fileName);  // forward declaration

#define N (3000)
HashSpace space(6, N);

struct Agent : Pose {
  Vec3f heading, center;
  unsigned flockCount{1};
};

struct AlloApp : App {
  Parameter moveRate{"/moveRate", "", 1.0, "", 0.0, 2.0};
  Parameter turnRate{"/turnRate", "", 1.0, "", 0.0, 2.0};
  Parameter localRadius{"/localRadius", "", 0.4, "", 0.01, 0.9};
  ParameterInt k{"/k", "", 5, "", 1, 15};
  Parameter size{"/size", "", 1.0, "", 0.0, 2.0};
  Parameter ratio{"/ratio", "", 1.0, "", 0.0, 2.0};
  ControlGUI gui;

  ShaderProgram shader;
  Mesh mesh;

  // vector<DrawableAgent> agent;
  vector<Agent> agent;

  void onCreate() override {
    gui << moveRate << turnRate << localRadius << k << size << ratio;
    gui.init();
    navControl().useMouse(false);

    // compile shaders
    shader.compile(slurp("../tetrahedron-vertex.glsl"),
                   slurp("../tetrahedron-fragment.glsl"),
                   slurp("../tetrahedron-geometry.glsl"));

    mesh.primitive(Mesh::POINTS);

    for (int _ = 0; _ < N; _++) {
      Agent a;
      a.pos(rv());
      space.move(_, a.pos() * space.dim());
      a.faceToward(rv());
      agent.push_back(a);
      //
      mesh.vertex(a.pos());
      mesh.normal(a.uf());
      const Vec3f& up(a.uu());
      mesh.color(up.x, up.y, up.z);
    }

    nav().pos(0, 0, 10);
  }

  float t = 0;
  int frameCount = 0;
  void onAnimate(double dt) override {
    t += dt;
    frameCount++;
    if (t > 1) {
      t -= 1;
      cout << frameCount << "fps ";
      frameCount = 0;
    }

    for (unsigned i = 0; i < N; i++) {
      agent[i].center = agent[i].pos();
      agent[i].heading = agent[i].uf();
      agent[i].flockCount = 1;
    }

    float sum = 0;
    for (int i = 0; i < N; i++) {
      HashSpace::Query query(k);
      int results = query(space, agent[i].pos() * space.dim(),
                          space.maxRadius() * localRadius);
      for (int j = 0; j < results; j++) {
        int id = query[j]->id;
        agent[i].heading += agent[id].uf();
        agent[i].center += agent[id].pos();
        agent[i].flockCount++;
      }
      sum += agent[i].flockCount;
    }

    if (frameCount == 0)  //
      cout << sum / N << "nn" << endl;

    for (unsigned i = 0; i < N; i++) {
      if (agent[i].flockCount < 1) {
        printf("shit is f-cked\n");
        fflush(stdout);
        exit(1);
      }

      if (agent[i].flockCount == 1) {
        agent[i].faceToward(Vec3f(0, 0, 0), 0.003 * turnRate);
        continue;
      }

      // make averages
      agent[i].center /= agent[i].flockCount;
      agent[i].heading /= agent[i].flockCount;

      float distance = (agent[i].pos() - agent[i].center).mag();

      // alignment: steer towards the average heading of local flockmates
      //
      agent[i].faceToward(agent[i].pos() + agent[i].heading, 0.003 * turnRate);
      agent[i].faceToward(agent[i].center, 0.003 * turnRate);
      agent[i].faceToward(agent[i].pos() - agent[i].center, 0.003 * turnRate);
    }

    // move the agents along (KEEP THIS CODE)
    //
    for (int i = 0; i < N; i++) {
      agent[i].pos() += agent[i].uf() * moveRate * 0.02;
    }

    for (unsigned i = 0; i < N; i++) {
      Vec3d p = agent[i].pos();

      // WRAP! (make a toroidal space)
      if (p.x > 1) p.x -= 1;
      if (p.y > 1) p.y -= 1;
      if (p.z > 1) p.z -= 1;
      if (p.x < 0) p.x += 1;
      if (p.y < 0) p.y += 1;
      if (p.z < 0) p.z += 1;

      agent[i].pos(p);
      space.move(i, agent[i].pos() * space.dim());
    }

    // visualize the agents
    //
    vector<Vec3f>& v(mesh.vertices());
    vector<Vec3f>& n(mesh.normals());
    vector<Color>& c(mesh.colors());
    for (unsigned i = 0; i < N; i++) {
      v[i] = agent[i].pos();
      n[i] = agent[i].uf();
      const Vec3d& up(agent[i].uu());
      c[i].set(up.x, up.y, up.z);
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(0.1, 0.1, 0.1);
    gl::depthTesting(true);  // or g.depthTesting(true);
    gl::blending(true);      // or g.blending(true);
    gl::blendTrans();        // or g.blendModeTrans();
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
