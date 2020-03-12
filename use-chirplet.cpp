#include "Gamma/Oscillator.h"
#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
using namespace al;
using namespace gam;

class Chirpy : public Chirplet<> {
 public:
  double freq2() { return mFreq2; }
};

const int N = 200;
struct MyApp : App {
  Chirpy chirplet[N];  // a Gaussian enveloped sine

  void onCreate() override {
    for (int i = 0; i < N; i++) {
      chirplet[i].amp(rnd::uniform(0.3, 0.9));
      chirplet[i].length(rnd::uniform(0.1, 0.8));
      chirplet[i].freq(rnd::uniform(220, 880), rnd::uniform(220, 880));
    }
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      for (int i = 0; i < N; i++) {
        if (chirplet[i].done()) {
          chirplet[i].amp(rnd::uniform(0.3, 0.9));
          chirplet[i].length(rnd::uniform(0.1, 0.8));
          chirplet[i].freq(chirplet[i].freq2(), rnd::uniform(220, 880));
        }
      }

      float s = 0;
      for (int i = 0; i < N; i++) {
        s += chirplet[i]().r;
      }
      s /= N;
      io.out(0) = io.out(1) = s;
    }
  }
};

int main() {
  MyApp app;
  app.configureAudio(44100, 2048, 2, 0);
  app.start();
}
