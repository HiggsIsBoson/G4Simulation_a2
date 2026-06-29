#ifndef PS_EVENT_INFO_HH
#define PS_EVENT_INFO_HH
#include <G4VUserEventInformation.hh>
#include <globals.hh>

enum class GammaOrigin { None, Photo, Compton };

class PsEventInfo : public G4VUserEventInformation {
 public:
  void SetOrigin(GammaOrigin a) { fOrigin = a; }
  GammaOrigin GetOrigin() const { return fOrigin; }

  void SetGenGammaEnergies(const std::vector<G4double>& es) { fGenEs = es; }
  const std::vector<G4double>& GetGenGammaEnergies() const { return fGenEs; }

  // Mode 1: initial beta+ kinematics
  void SetGenBeta(G4double E_keV, G4double theta, G4double phi) {
    fBetaE = E_keV; fBetaTheta = theta; fBetaPhi = phi;
  }
  G4double GetBetaE()     const { return fBetaE; }
  G4double GetBetaTheta() const { return fBetaTheta; }
  G4double GetBetaPhi()   const { return fBetaPhi; }

  void Print() const override {}

 private:
  GammaOrigin fOrigin{GammaOrigin::None};
  std::vector<G4double> fGenEs;
  G4double fBetaE{0}, fBetaTheta{0}, fBetaPhi{0};

};
#endif
