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

struct AlloApp : App {
  ShaderProgram pointShader;
  ShaderProgram lineShader;
  Texture pointTexture;
  Texture lineTexture;
  Mesh mesh;

  FBO renderTarget;
  Texture rendered;

  void updateFBO(int w, int h) {
    rendered.create2D(w, h);
    renderTarget.bind();
    renderTarget.attachTexture2D(rendered);
    renderTarget.unbind();
  }

  void onCreate() override {
    // use a texture to control the alpha channel of each particle
    //
    pointTexture.create2D(256, 256, Texture::R8, Texture::RED, Texture::SHORT);
    int Nx = pointTexture.width();
    int Ny = pointTexture.height();
    std::vector<short> alpha;
    alpha.resize(Nx * Ny);
    for (int j = 0; j < Ny; ++j) {
      float y = float(j) / (Ny - 1) * 2 - 1;
      for (int i = 0; i < Nx; ++i) {
        float x = float(i) / (Nx - 1) * 2 - 1;
        float m = exp(-13 * (x * x + y * y));
        m *= pow(2, 15) - 1;  // scale by the largest positive short int
        alpha[j * Nx + i] = m;
      }
    }
    pointTexture.submit(&alpha[0]);

    lineTexture.create1D(256, Texture::R8, Texture::RED, Texture::SHORT);
    std::vector<short> beta;
    beta.resize(lineTexture.width());
    for (int i = 0; i < beta.size(); ++i) {
      beta[i] = alpha[128 * beta.size() + i];
    }
    lineTexture.submit(&beta[0]);

    // compile and link the shaders
    //
    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));
    lineShader.compile(slurp("../line-vertex.glsl"),
                       slurp("../line-fragment.glsl"),
                       slurp("../line-geometry.glsl"));

    // create a mesh of points scattered randomly with random colors
    //
    auto rv = []() {
      return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS());
    };

    mesh.primitive(Mesh::LINES);
    for (int i = 0; i < 1000; i++) {
      mesh.vertex(rv() * 5);
      mesh.color(HSV(rnd::uniform(), 1.0, 1.0));
      mesh.texCoord(rnd::uniform(0.5, 0.05), 0.0);
    }

    updateFBO(width(), height());
  }

  void onAnimate(double dt) override {
    if (feature) return;

    timer += dt;
    if (timer > 7) {
      timer -= 7;
      which++;
      if (which >= mesh.vertices().size()) which = 0;
    }

    nav().faceToward(mesh.vertices()[which], Vec3d(0, 1, 0), 0.01);
    nav().nudgeF(0.007);
  }
  int which = 0;
  double timer = 0;

  void onResize() {
    updateFBO(width(), height());
    //
  }

  bool feature{false};
  bool onKeyDown(const Keyboard& k) override {
    //
    if (k.key() == ' ') feature = !feature;
    return true;
  }

  void onDraw(Graphics& g) override {
    static int Width;
    static int Height;
    if (Width != width() || Height != height()) {
      Width = width();
      Height = height();
      onResize();
    }
    renderTarget.bind();  ///////////////////////////////////////////////
    g.clear(0.37);

    g.blending(true);
    g.blendModeAdd();

    lineTexture.bind();
    g.shader(lineShader);
    mesh.primitive(Mesh::LINES);
    g.draw(mesh);
    lineTexture.unbind();

    pointTexture.bind();
    g.shader(pointShader);
    mesh.primitive(Mesh::POINTS);
    g.draw(mesh);
    pointTexture.unbind();

    renderTarget.unbind();  /////////////////////////////////////////////

    g.blending(false);
    // throw the rendering onto the screen
    // g.clear(1);
    g.quadViewport(rendered);
  }
};

int main() { AlloApp().start(); }