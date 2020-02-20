#include "al/app/al_DistributedApp.hpp"
#include "al_ext/statedistribution/al_CuttleboneStateSimulationDomain.hpp"
using namespace al;

#include <iostream>
using namespace std;

struct SharedState {};

class AlloApp : public DistributedAppWithState<SharedState> {
  std::shared_ptr<CuttleboneStateSimulationDomain<SharedState>>
      cuttleboneDomain;

  void onCreate() override {
    cout << "i am " << name() << endl;

    cuttleboneDomain =
        CuttleboneStateSimulationDomain<SharedState>::enableCuttlebone(this);
    if (!cuttleboneDomain) {
      std::cerr << "ERROR: Could not start Cuttlebone. Quitting." << std::endl;
      quit();
    }
  }

  bool onKeyDown(Keyboard const &k) override {
    if (k.key() == ' ') {
      // The space bar will turn off omni rendering
      if (omniRendering) {
        omniRendering->drawOmni = !omniRendering->drawOmni;
      } else {
        std::cout << "Not doing omni rendering" << std::endl;
      }
    }
    return true;
  }
};

int main() {
  AlloApp app;
  app.start();
}
