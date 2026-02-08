#ifndef EVENT_ACTION_HH
#define EVENT_ACTION_HH
#include <G4UserEventAction.hh>
#include <globals.hh> 
#include "PsEventInfo.hh"

class RunAction;
class EventAction : public G4UserEventAction {
 public:
  explicit EventAction(RunAction* run);
  void BeginOfEventAction(const G4Event* evt) override;
  void EndOfEventAction(const G4Event* evt) override;
  void AddEdep(G4double dE_keV) { fEdep_keV += dE_keV; }
  void AddTruthE(G4double e) { fTruthE_keV += e; }
  G4double GetTruthE() const { return fTruthE_keV; }

  void AddHitNaI(int bit) { fHitNaI |= bit; }
  int  GetHitNaI() const { return fHitNaI; }

  void AddPhotoEdep(G4double dE_keV) { fEdepPhoto_keV += dE_keV; }
  void AddComptEdep(G4double dE_keV) { fEdepCompt_keV += dE_keV; }

 private:
  RunAction* fRun{};
  G4double fEdep_keV{0.0};
  G4double fEdep_smeared_keV{0.0};
  G4double fTruthE_keV{0.0};  
  G4double fE1{0}, fE2{0}, fE3{0};
  int fHitNaI{0};

  G4double fEdepPhoto_keV{0.0};
  G4double fEdepCompt_keV{0.0};
};
#endif
