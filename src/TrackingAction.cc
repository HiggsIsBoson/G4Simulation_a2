#include "TrackingAction.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"

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
  if (G4UniformRand() < p2) {
    // 2γ: back-to-back 511 keV
    G4ThreeVector d = G4RandomDirection();
    auto* dp1 = new G4DynamicParticle(G4Gamma::Gamma(), d,  511.*keV);
    auto* dp2 = new G4DynamicParticle(G4Gamma::Gamma(), -d, 511.*keV);
    stackMan->PushOneTrack(new G4Track(dp1, t0, pos));
    stackMan->PushOneTrack(new G4Track(dp2, t0, pos));
    fEvt->SetM3NGamma(2);
  } else {
    // 3γ: Ore-Powell distribution
    std::array<G4ThreeVector,3> dirs;
    std::array<G4double,3>      energies;
    PrimaryGeneratorAction::SampleOrePowell3(dirs, energies);
    for (int i=0; i<3; ++i) {
      auto* dp = new G4DynamicParticle(G4Gamma::Gamma(), dirs[i], energies[i]);
      stackMan->PushOneTrack(new G4Track(dp, t0, pos));
    }
    fEvt->SetM3NGamma(3);
  }
}
