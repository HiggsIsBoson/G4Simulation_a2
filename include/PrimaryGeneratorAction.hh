#ifndef PRIMARY_GENERATOR_ACTION_HH
#define PRIMARY_GENERATOR_ACTION_HH
#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ParticleDefinition.hh>
#include <G4ThreeVector.hh>
class G4ParticleGun;
class G4Event;
class G4GenericMessenger;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
 public:
  explicit PrimaryGeneratorAction(int mode = 2);
  ~PrimaryGeneratorAction() override;
  void GeneratePrimaries(G4Event*) override;

  void SetP2(G4double v) { fP2 = std::clamp(v,0.0,1.0); }
  G4double GetP2() const { return fP2; }

  // 3γ generation (also used by TrackingAction in Mode 3)
  static void Rambo3Static(G4double Ecm,
                           std::array<G4ThreeVector,3>& pdir,
                           std::array<G4double,3>& E);
  static bool SampleOrePowell3(std::array<G4ThreeVector,3>& dirs,
                                std::array<G4double,3>& energies);

 private:
  // Mode 2
  G4ThreeVector RandomPointInSilica() const;
  std::vector<G4double> Generate3Gamma_OrePowell(G4Event* evt);
  void Rambo3(const G4double Ecm,
              std::array<G4ThreeVector,3>& pdir,
              std::array<G4double,3>& E);
  static void Rambo3Impl(G4double Ecm,
                         std::array<G4ThreeVector,3>& pdir,
                         std::array<G4double,3>& E);

  // Mode 1: Na22 beta+ spectrum sampling (endpoint 545.4 keV, rejection sampling)
  G4double SampleNa22BetaEnergy() const;

  int fMode;

  G4ParticleGun* fGun{};
  G4ParticleDefinition* fGamma{nullptr};
  G4ParticleDefinition* fPositron{nullptr};
  G4double fP2;
  G4GenericMessenger* fMsg{};

  // Mode 1 geometry (must match DetectorConstruction)
  G4double fNa22Z{-20.};    // Na22 source z position [mm], = -2 cm
  G4double fPlasticZ{0.};   // plastic center z [mm]
};
#endif
