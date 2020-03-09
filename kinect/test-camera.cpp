#include "libfreenect.h"
#include "libfreenect_sync.h"

#include "al/app/al_App.hpp"
using namespace al;

struct MyApp : App {
  Texture rgb;

  void onCreate() override {
    rgb.create2D(640, 480, Texture::RGBA8, Texture::RGB, Texture::UBYTE);
  }

  void onAnimate(double dt) override {
    void *data;
    uint32_t ts;
    if (freenect_sync_get_video(&data, &ts, 0, FREENECT_VIDEO_RGB) < 0)  //
      exit(1);
    rgb.submit(data);
  }

  void onDraw(Graphics &g) override {
    // map the whole window to the texture
    g.quadViewport(rgb);
  }
};
int main() { MyApp().start(); }
