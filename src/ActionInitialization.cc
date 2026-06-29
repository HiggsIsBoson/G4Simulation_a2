#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"
#include "StackingAction.hh"
#include <G4RunManager.hh>

void ActionInitialization::Build() const {
  auto* runMan = G4RunManager::GetRunManager();
  const auto* det = static_cast<const DetectorConstruction*>(runMan->GetUserDetectorConstruction());

  auto* run = new RunAction(fMode);
  SetUserAction(run);

  auto* evt = new EventAction(run);
  SetUserAction(evt);

  auto* pga = new PrimaryGeneratorAction(fMode);
  SetUserAction(pga);
  SetUserAction(new SteppingAction(det, evt));

  if (fMode == 3) {
    SetUserAction(new TrackingAction(fMode, evt, pga));
    SetUserAction(new StackingAction(fMode, evt));
  }
}

void ActionInitialization::BuildForMaster() const {
  SetUserAction(new RunAction(fMode));
}
