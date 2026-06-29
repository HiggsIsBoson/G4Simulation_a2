#ifndef TRACKING_ACTION_HH
#define TRACKING_ACTION_HH
#include <G4UserTrackingAction.hh>

class EventAction;
class PrimaryGeneratorAction;

// Mode 3 専用: e+ がシリカで止まったとき、
// その位置から Ps 崩壊の 2γ or 3γ をスタックに注入する
class TrackingAction : public G4UserTrackingAction {
 public:
  TrackingAction(int mode, EventAction* evt, const PrimaryGeneratorAction* pga);
  void PreUserTrackingAction(const G4Track* track) override;
  void PostUserTrackingAction(const G4Track* track) override;

 private:
  int fMode;
  EventAction* fEvt;
  const PrimaryGeneratorAction* fPGA;
};
#endif
