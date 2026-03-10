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
#include <G4EventManager.hh>

SteppingAction::SteppingAction(const DetectorConstruction* det, EventAction* evt)
: fDet(det), fNaILog(nullptr), fEvt(evt) {}

void SteppingAction::UserSteppingAction(const G4Step* step) {

  // fNaILog の初期化（先頭で行う）
  if (!fNaILog) {
    fNaILog = fDet->GetNaILogic();
    if (!fNaILog) return;
  }

  auto* track = step->GetTrack();
  auto* pre   = step->GetPreStepPoint();
  auto* vol   = pre->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  // --- NaI 内のエネルギー堆積のみ積算 ---
  auto edep = step->GetTotalEnergyDeposit();
  if (edep > 0 && vol == fNaILog) {
    fEvt->AddEdep(edep/keV);
  }

  // --- World→NaI に入った瞬間を検出 -> truth に追加 ---
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

  // --- NaI 内でのエネルギー deposit をプロセス別に分類 ---
  if (vol == fNaILog) {
    G4double dE_keV = step->GetTotalEnergyDeposit() / keV;
    if (dE_keV > 0.0) {
      auto proc = step->GetPostStepPoint()->GetProcessDefinedStep();
      if (proc) {
        auto name = proc->GetProcessName();
        if (name == "phot") {
          fEvt->AddPhotoEdep(dE_keV);
        } else if (name == "compt") {
          fEvt->AddComptEdep(dE_keV);
        }
      }
    }
  }

  // --- NaI 突入を検出（プライマリγのみ）---
  if (postVolL == fNaILog && preVolL != fNaILog) {
    if (track->GetParentID() == 0) {  // プライマリγのみ
      int which = (track->GetTrackID()-1) % 3;
      auto evt = static_cast<EventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());
      if (evt) {
        evt->AddHitNaI(1 << which);
      }
    }
  }
}