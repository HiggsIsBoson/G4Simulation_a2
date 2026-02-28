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

  // --- NaI 内の γ の相互作用で分類 ---
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
  
  // ★ NaI 内でのエネルギー deposit をプロセス別に分類
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

  // NaI突入を検出
  if (postVolL == fNaILog && preVolL != fNaILog) {
    // このγが何番目かを識別するために parentID を使う
    //int id = track->GetParentID(); // 0ならprimary, >0はsecondary

    // gammaを発射した順番を PrimaryGeneratorAction で track->SetUserInformation に書くか、
    // あるいは直接 trackID %3 でタグ付けするなど工夫が必要
    // ここでは簡略化して「track->GetTrackID() % 3」で識別
    int which = (track->GetTrackID()-1) % 3; // 0,1,2 に対応

    auto run = static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
    auto man = G4AnalysisManager::Instance();

    // 今の値を読み出してビットを立てる
    auto evt = static_cast<EventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());
    if (evt) {
      evt->AddHitNaI(1 << which);
    }

  }


}

