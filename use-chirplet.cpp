#include "Gamma/Oscillator.h"
#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
using namespace al;
using namespace gam;

struct MyApp : App {
  Chirplet<> chirplet;  // a Gaussian enveloped sine

  void onCreate() override {
    chirplet.length(0.1);
    chirplet.freq(rnd::uniform(220, 880));
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      if (chirplet.done()) {
        chirplet.length(0.1);
        chirplet.freq(rnd::uniform(220, 880));
      }

      float s = chirplet().r;
      io.out(0) = io.out(1) = s;
    }
  }
};

int main() { MyApp().start(); }
