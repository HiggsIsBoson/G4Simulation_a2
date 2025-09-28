#ifndef RUN_ACTION_HH
#define RUN_ACTION_HH
#include <G4UserRunAction.hh>
#include <G4String.hh>
class G4Run;
class G4GenericMessenger;

class RunAction : public G4UserRunAction {
 public:
  RunAction();
  ~RunAction() override;
  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;

  G4double GetA() const { return fa; }
  G4double GetB() const { return fb; }
  G4double GetC() const { return fc; }

 private:
  void Book();

  // Resolution model sigma(E) = sqrt(a^2 E + b^2 E^2 + c^2)  [E in keV]
  G4double fa, fb, fc; // tunable via UI
  G4GenericMessenger* fMsg{}; // /analysis/

  // Handles
  G4String fOutName; // base filename
};
#endif
