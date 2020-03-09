#include "libfreenect.h"
#include "libfreenect_sync.h"

#include "al/app/al_App.hpp"
using namespace al;

#include <fstream>
#include <string>
#include <vector>
using std::fstream;
using std::string;
using std::vector;

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

struct MyApp : App {
  Texture rgb;
  Mesh cloud;
  ShaderProgram pointShader;

  void onCreate() override {
    rgb.create2D(640, 480, Texture::RGBA8, Texture::RGB, Texture::UBYTE);

    cloud.primitive(Mesh::POINTS);
    for (int i = 0; i < 480; i++) {
      for (int j = 0; j < 640; j++) {
        cloud.vertex((float)j, (float)i, 0.0f);
        cloud.texCoord(j / 639.0f, i / 479.0f);
      }
    }
    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    nav().pos(0.5, 0.5, 4);
  }

  void onAnimate(double dt) override {
    uint32_t ts;

    // grab the RGB camera data and send it to the GPU as a texture
    void *data;
    if (freenect_sync_get_video(&data, &ts, 0, FREENECT_VIDEO_RGB) < 0)  //
      exit(1);
    rgb.submit(data);

    // get the depth data and (re)set the z of each element of the mesh
    short *depth;
    if (freenect_sync_get_depth((void **)&depth, &ts, 0, FREENECT_DEPTH_11BIT) <
        0)  //
      exit(1);

    vector<Vec3f> &v(cloud.vertices());
    for (int i = 0; i < 480; i++)    //
      for (int j = 0; j < 640; j++)  //
        v[i * 640 + j].z = depth[i * 640 + j];
  }

  void onDraw(Graphics &g) override {
    g.clear(Color(0.2));

    g.shader(pointShader);
    g.shader().uniform("pointSize", 1.0f / 1000);

    rgb.bind();
    //    g.scale(1.0f / 640, 1.0f / 480, 1.0f / 2047);  //
    g.draw(cloud);
    rgb.unbind();
  }
};

int main() { MyApp().start(); }
