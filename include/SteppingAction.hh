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
  const DetectorConstruction* fDet{};
  G4LogicalVolume* fNaILog{};
  G4LogicalVolume* fPlasticLog{};
  G4LogicalVolume* fSilicaLog{};
  EventAction* fEvt{};
};

#endif
