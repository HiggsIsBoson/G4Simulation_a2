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
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;
  void GeneratePrimaries(G4Event*) override;

  // UI: set p2 (probability of 2γ vs 3γ)
  void SetP2(G4double v) { fP2 = std::clamp(v,0.0,1.0); }

 private:
  G4ThreeVector RandomPointInSilica() const; // sample inside cylinder (r<=3cm, |z|<=5cm)
  
  std::vector<G4double> Generate3Gamma_OrePowell(G4Event* evt);
  
  // RAMBO風 3 質量ゼロ粒子生成（合計エネルギー=Ecm）
  void Rambo3(const G4double Ecm,
              std::array<G4ThreeVector,3>& pdir,
              std::array<G4double,3>& E);


  G4ParticleGun* fGun{};
  G4ParticleDefinition* fGamma{nullptr};
  G4double fP2; // default 0.5
  G4GenericMessenger* fMsg{}; // /source/
};
#endif
