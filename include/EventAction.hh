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
 private:
  RunAction* fRun{};
  G4double fEdep_keV{0.0};
  G4double fEdep_smeared_keV{0.0};
  G4double fTruthE_keV{0.0};  
};
#endif
