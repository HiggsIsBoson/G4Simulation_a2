#ifndef STEPPING_ACTION_HH
#define STEPPING_ACTION_HH
#include <G4UserSteppingAction.hh>
#include "RunAction.hh"

class DetectorConstruction; class EventAction; class G4LogicalVolume; class G4Step;

class SteppingAction : public G4UserSteppingAction {
 public:
  SteppingAction(const DetectorConstruction* det, EventAction* evt);
  void UserSteppingAction(const G4Step*) override;
 private:
  const DetectorConstruction* fDet{};   // ← DetectorConstruction を保持
  G4LogicalVolume* fNaILog{};           // ← 最初は nullptr、あとで遅延取得
  EventAction* fEvt{};
};

#endif
