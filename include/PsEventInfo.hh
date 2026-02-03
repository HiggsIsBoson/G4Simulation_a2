#ifndef PS_EVENT_INFO_HH
#define PS_EVENT_INFO_HH
#include <G4VUserEventInformation.hh>
#include <globals.hh>

enum class GammaOrigin { None, Photo, Compton };

class PsEventInfo : public G4VUserEventInformation {
 public:
  void SetOrigin(GammaOrigin a) { fOrigin = a; }
  GammaOrigin GetOrigin() const { return fOrigin; }

  void SetGenGammaEnergies(const std::vector<G4double>& es) {    fGenEs = es;  }
  const std::vector<G4double>& GetGenGammaEnergies() const {    return fGenEs;  }

  // 必須: 抽象クラスの純粋仮想関数を実装
  void Print() const override {}

 private:
  GammaOrigin fOrigin{GammaOrigin::None};
  std::vector<G4double> fGenEs;

};
#endif
