#include "al/app/al_App.hpp"
#include "al/graphics/al_Image.hpp"  // al::Image
using namespace al;

struct MyApp : App {
  Mesh mesh;

  void onCreate() override {
    mesh.primitive(Mesh::POINTS);

    const char* filename = "../Drake-Equation.jpg";
    auto imageData = Image(filename);
    if (imageData.array().size() == 0) {
      std::cout << "failed to load image" << std::endl;
    }

    Image::RGBAPix pixel;
    for (int c = 0; c < imageData.width(); c++)
      for (int r = 0; r < imageData.height(); r++) {
        imageData.read(pixel, c, r);
        mesh.vertex(c * 0.1, r * 0.1, 0);
        mesh.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
      }

    nav().pos(0, 0, 5);
  }

  void onAnimate(double dt) override {}

  void onDraw(Graphics& g) override {
    g.clear(0.1);
    g.meshColor();
    g.draw(mesh);
  }
};

int main() {
  MyApp app;
  app.start();
}
