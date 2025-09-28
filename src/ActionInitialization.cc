#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include <G4RunManager.hh>

void ActionInitialization::Build() const {
  auto* runMan = G4RunManager::GetRunManager();

  const auto* det = static_cast<const DetectorConstruction*>(runMan->GetUserDetectorConstruction());

  auto* run = new RunAction();
  SetUserAction(run);

  auto* evt = new EventAction(run);
  SetUserAction(evt);

  SetUserAction(new PrimaryGeneratorAction());
  SetUserAction(new SteppingAction(det, evt));
}

void ActionInitialization::BuildForMaster() const {
  SetUserAction(new RunAction());   // マスター専用 RunAction
}
