#ifndef STACKING_ACTION_HH
#define STACKING_ACTION_HH
#include <G4UserStackingAction.hh>

class EventAction;

// Mode 3 専用: e+ がシリカで自動対消滅した511 keV ガンマを殺す
// （代わりに TrackingAction が Ps 崩壊ガンマを注入する）
class StackingAction : public G4UserStackingAction {
 public:
  StackingAction(int mode, EventAction* evt);
  G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* track) override;

 private:
  int fMode;
  EventAction* fEvt;
};
#endif
