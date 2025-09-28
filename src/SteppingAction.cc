#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "PsEventInfo.hh"

#include <G4LogicalVolume.hh>
#include <G4Step.hh>
#include <G4RunManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4AnalysisManager.hh>
#include <G4VProcess.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4Gamma.hh>

SteppingAction::SteppingAction(const DetectorConstruction* det, EventAction* evt)
: fDet(det), fNaILog(nullptr), fEvt(evt) {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
  
  // --- deposit を EventAction に蓄積 ---
  auto edep = step->GetTotalEnergyDeposit();
  if (edep > 0) {
    fEvt->AddEdep(edep/keV);
  }

  if (!fNaILog) {
    fNaILog = fDet->GetNaILogic();
    if (!fNaILog) {
      fNaILog = G4LogicalVolumeStore::GetInstance()->GetVolume("naiL");
    }
    if (!fNaILog) return;
  }

  // --- NaI 内の γ の最初の相互作用で分類 ---
  auto* track = step->GetTrack();
  auto* pre   = step->GetPreStepPoint();
  auto* vol   = pre->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  // World→NaI に入った瞬間を検出->add to truth
  auto preTouch  = pre->GetTouchableHandle();
  auto postTouch = step->GetPostStepPoint()->GetTouchableHandle();  
  auto preVol  = preTouch ? preTouch->GetVolume() : nullptr;
  auto postVol = postTouch ? postTouch->GetVolume() : nullptr;
  auto preVolL  = preVol ? preVol->GetLogicalVolume() : nullptr;
  auto postVolL = postVol ? postVol->GetLogicalVolume() : nullptr;  
  if (postVolL == fNaILog && preVolL != fNaILog) {
    G4double eGamma = track->GetKineticEnergy()/keV;
    fEvt->AddTruthE(eGamma);
  }
  

  if (vol == fNaILog && track->GetDefinition()==G4Gamma::GammaDefinition()) {
    auto proc = step->GetPostStepPoint()->GetProcessDefinedStep();
    if (proc) {
      auto* info = static_cast<PsEventInfo*>(
          G4RunManager::GetRunManager()->GetCurrentEvent()->GetUserInformation());
      if (info && info->GetOrigin()==GammaOrigin::None) {
        if (proc->GetProcessName()=="phot") {
          info->SetOrigin(GammaOrigin::Photo);
        } else if (proc->GetProcessName()=="compt") {
          info->SetOrigin(GammaOrigin::Compton);
        }
      }
    }
  }


}

