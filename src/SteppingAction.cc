#include "SteppingAction.hh"
#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "RunAction.hh"
#include "PsEventInfo.hh"

#include <G4LogicalVolume.hh>
#include <G4Step.hh>
#include <G4RunManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4AnalysisManager.hh>
#include <G4VProcess.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4Gamma.hh>
#include <G4Positron.hh>
#include <G4EventManager.hh>

SteppingAction::SteppingAction(const DetectorConstruction* det, EventAction* evt)
: fDet(det), fNaILog(nullptr), fPlasticLog(nullptr), fEvt(evt) {}

void SteppingAction::UserSteppingAction(const G4Step* step) {

  auto* run = static_cast<const RunAction*>(
    G4RunManager::GetRunManager()->GetUserRunAction());

  if (run->GetMode() == 1 || run->GetMode() == 3) {
    // ---- Mode 1/3: track beta+ through plastic and silica ----
    if (!fPlasticLog) {
      fPlasticLog = fDet->GetPlasticLogic();
      if (!fPlasticLog)
        fPlasticLog = G4LogicalVolumeStore::GetInstance()->GetVolume("plasticL");
    }
    if (!fSilicaLog) {
      fSilicaLog = fDet->GetSilicaLogic();
      if (!fSilicaLog)
        fSilicaLog = G4LogicalVolumeStore::GetInstance()->GetVolume("silicaL");
    }
    if (!fPlasticLog) return;

    auto* track = step->GetTrack();

    auto preTouch  = step->GetPreStepPoint()->GetTouchableHandle();
    auto postTouch = step->GetPostStepPoint()->GetTouchableHandle();
    auto preVol    = preTouch  ? preTouch->GetVolume()  : nullptr;
    auto postVol   = postTouch ? postTouch->GetVolume() : nullptr;
    auto preVolL   = preVol  ? preVol->GetLogicalVolume()  : nullptr;
    auto postVolL  = postVol ? postVol->GetLogicalVolume() : nullptr;

    if (track->GetTrackID() == 1) {
      // primary e+: plastic / silica tracking
      if (!fPlasticLog) return;

      if (postVolL == fPlasticLog && preVolL != fPlasticLog)
        fEvt->SetHitPlastic(1);

      if (preVolL == fPlasticLog) {
        G4double dE = step->GetTotalEnergyDeposit();
        if (dE > 0) fEvt->AddPlasticEdep(dE / keV);
      }

      if (preVolL == fPlasticLog && postVolL != fPlasticLog) {
        G4double KE = track->GetKineticEnergy();
        if (KE > 0) {
          G4ThreeVector dir = track->GetMomentumDirection();
          fEvt->SetOutP1(KE / keV, dir.theta(), dir.phi());
        }
      }

      if (fSilicaLog && postVolL == fSilicaLog && preVolL != fSilicaLog)
        fEvt->SetHitSilica(1);

      if (fSilicaLog && preVolL == fSilicaLog) {
        auto status = track->GetTrackStatus();
        if (status == fStopAndKill || status == fKillTrackAndSecondaries) {
          fEvt->SetStopSilica(1);
          if (run->GetMode() == 3)
            fEvt->SetM3Stop(track->GetPosition());
        }
      }
      return;  // e+に関してはここで終了
    }

    // 以下は trackID != 1 (Mode 3のNaI追跡)
    if (run->GetMode() != 3) return;

    if (!fNaILog) {
      fNaILog = fDet->GetNaILogic();
      if (!fNaILog)
        fNaILog = G4LogicalVolumeStore::GetInstance()->GetVolume("naiL");
    }
    if (!fNaILog) return;

    if (preVolL == fNaILog) {
      G4double dE = step->GetTotalEnergyDeposit();
      if (dE > 0) fEvt->AddNaiEdepM3(dE / keV);
    }
    if (postVolL == fNaILog && preVolL != fNaILog)
      fEvt->SetHitNaiM3(1);

    return;
  }

  // ---- Mode 2: NaI simulation ----
  auto edep = step->GetTotalEnergyDeposit();
  if (edep > 0) fEvt->AddEdep(edep / keV);

  if (!fNaILog) {
    fNaILog = fDet->GetNaILogic();
    if (!fNaILog)
      fNaILog = G4LogicalVolumeStore::GetInstance()->GetVolume("naiL");
    if (!fNaILog) return;
  }

  auto* track = step->GetTrack();
  auto* pre   = step->GetPreStepPoint();
  auto* vol   = pre->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  auto preTouch  = pre->GetTouchableHandle();
  auto postTouch = step->GetPostStepPoint()->GetTouchableHandle();
  auto preVol    = preTouch  ? preTouch->GetVolume()  : nullptr;
  auto postVol   = postTouch ? postTouch->GetVolume() : nullptr;
  auto preVolL   = preVol  ? preVol->GetLogicalVolume()  : nullptr;
  auto postVolL  = postVol ? postVol->GetLogicalVolume() : nullptr;

  if (postVolL == fNaILog && preVolL != fNaILog) {
    fEvt->AddTruthE(track->GetKineticEnergy() / keV);
  }

  if (vol == fNaILog) {
    G4double dE_keV = step->GetTotalEnergyDeposit() / keV;
    if (dE_keV > 0.0) {
      auto proc = step->GetPostStepPoint()->GetProcessDefinedStep();
      if (proc) {
        auto name = proc->GetProcessName();
        if (name == "phot")  fEvt->AddPhotoEdep(dE_keV);
        else if (name == "compt") fEvt->AddComptEdep(dE_keV);
      }
    }
  }

  if (postVolL == fNaILog && preVolL != fNaILog) {
    int which = (track->GetTrackID()-1) % 3;
    fEvt->AddHitNaI(1 << which);
  }
}

