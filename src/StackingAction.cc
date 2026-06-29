#include "StackingAction.hh"
#include "EventAction.hh"

#include <G4Track.hh>
#include <G4Gamma.hh>
#include <G4Positron.hh>
#include <G4SystemOfUnits.hh>
#include <cmath>

StackingAction::StackingAction(int mode, EventAction* evt)
: fMode(mode), fEvt(evt) {}

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track) {
  if (fMode != 3) return fUrgent;

  // e+ がシリカで止まった直後に Geant4 が生成する自動対消滅 511 keV ガンマを殺す
  // （TrackingAction::PostUserTrackingAction が代わりに Ps 崩壊ガンマを注入する）
  if (fEvt->GetM3KillAnnihil()
      && track->GetParentID() == 1          // primary e+ の子
      && track->GetDefinition() == G4Gamma::Gamma()
      && std::abs(track->GetKineticEnergy()/keV - 511.) < 2.) {
    return fKill;
  }
  return fUrgent;
}
