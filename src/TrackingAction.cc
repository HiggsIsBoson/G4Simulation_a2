#include "TrackingAction.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "PsTrackInfo.hh"

#include <G4Track.hh>
#include <G4Gamma.hh>
#include <G4Positron.hh>
#include <G4EventManager.hh>
#include <G4StackManager.hh>
#include <G4DynamicParticle.hh>
#include <G4SystemOfUnits.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>

TrackingAction::TrackingAction(int mode, EventAction* evt, const PrimaryGeneratorAction* pga)
: fMode(mode), fEvt(evt), fPGA(pga) {}

void TrackingAction::PreUserTrackingAction(const G4Track* track) {
  if (fMode != 3) return;

  bool isPsOrigin = false;

  // 自分がPsTrackInfoを持つ（TrackingActionが直接注入したPs崩壊ガンマ）
  if (track->GetUserInformation() &&
      dynamic_cast<PsTrackInfo*>(track->GetUserInformation())) {
    isPsOrigin = true;
  }
  // 親がPs由来trackIDに含まれる（光電子・Compton電子など二次粒子）
  else if (fEvt->IsPsTrack(track->GetParentID())) {
    const_cast<G4Track*>(track)->SetUserInformation(new PsTrackInfo());
    isPsOrigin = true;
  }

  if (isPsOrigin) {
    fEvt->AddPsTrackID(track->GetTrackID());
  }
}

void TrackingAction::PostUserTrackingAction(const G4Track* track) {
  if (fMode != 3) return;
  // primary e+ (trackID==1) だけを処理
  if (track->GetTrackID() != 1) return;
  if (track->GetDefinition() != G4Positron::Positron()) return;

  // SteppingAction が SetM3Stop() を呼んでいれば、シリカで止まっている
  if (!fEvt->GetM3KillAnnihil()) return;

  G4ThreeVector pos  = fEvt->GetM3StopPos();
  G4double      t0   = track->GetGlobalTime();

  auto* stackMan = G4EventManager::GetEventManager()->GetStackManager();

  G4double p2 = fPGA->GetP2();
  auto pushPsGamma = [&](const G4ThreeVector& dir, G4double E) {
    auto* trk = new G4Track(new G4DynamicParticle(G4Gamma::Gamma(), dir, E), t0, pos);
    trk->SetUserInformation(new PsTrackInfo());  // Ps由来とタグ付け
    stackMan->PushOneTrack(trk);
  };

  if (G4UniformRand() < p2) {
    // 2γ: back-to-back 511 keV
    G4ThreeVector d = G4RandomDirection();
    pushPsGamma( d, 511.*keV);
    pushPsGamma(-d, 511.*keV);
    fEvt->SetM3NGamma(2);
  } else {
    // 3γ: Ore-Powell distribution
    std::array<G4ThreeVector,3> dirs;
    std::array<G4double,3>      energies;
    PrimaryGeneratorAction::SampleOrePowell3(dirs, energies);
    for (int i=0; i<3; ++i) pushPsGamma(dirs[i], energies[i]);
    fEvt->SetM3NGamma(3);
  }
}
