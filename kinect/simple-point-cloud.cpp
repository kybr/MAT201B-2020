#include "libfreenect.h"
#include "libfreenect_sync.h"

#include "al/app/al_App.hpp"
using namespace al;

#include <vector>
using std::vector;

struct MyApp : App {
  Texture rgb;
  Mesh cloud;

  void onCreate() override {
    rgb.create2D(640, 480, Texture::RGBA8, Texture::RGB, Texture::UBYTE);

    cloud.primitive(Mesh::POINTS);
    for (int i = 0; i < 480; i++) {
      for (int j = 0; j < 640; j++) {
        cloud.vertex((float)j, (float)i, 0.0f);
        cloud.texCoord(j / 639.0f, i / 479.0f);
      }
    }

    nav().pos(0, 0, 5);
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
    short minimum = 32000;
    short maximum = -32000;
    for (int i = 0; i < 480; i++) {
      for (int j = 0; j < 640; j++) {
        v[i * 640 + j].z = depth[i * 640 + j];

        short s = depth[i * 640 + j];
        if (s < minimum) minimum = s;
        if (s > maximum) maximum = s;
      }
    }

    // std::cout << minimum << "," << maximum << std::endl;

    angle += 0.9;
  }
  double angle = 0;

  void onDraw(Graphics &g) override {
    g.clear(Color(0.2));
    g.texture();  // tell AlloLib to use texture to color meshes

    rgb.bind();
    g.rotate(angle, 0, 1, 0);
    g.scale(1.0f / 640, 1.0f / 480, 1.0f / 2047);  //
    g.draw(cloud);
    rgb.unbind();
  }
};
int main() { MyApp().start(); }
