#ifndef EVENT_ACTION_HH
#define EVENT_ACTION_HH
#include <G4UserEventAction.hh>
#include <G4ThreeVector.hh>
#include <globals.hh>
#include <set>
#include "PsEventInfo.hh"

class RunAction;
class EventAction : public G4UserEventAction {
 public:
  explicit EventAction(RunAction* run);
  void BeginOfEventAction(const G4Event* evt) override;
  void EndOfEventAction(const G4Event* evt) override;

  // Mode 2
  void AddEdep(G4double dE_keV) { fEdep_keV += dE_keV; }
  void AddTruthE(G4double e) { fTruthE_keV += e; }
  G4double GetTruthE() const { return fTruthE_keV; }
  void AddHitNaI(int bit) { fHitNaI |= bit; }
  int  GetHitNaI() const { return fHitNaI; }
  void AddPhotoEdep(G4double dE_keV) { fEdepPhoto_keV += dE_keV; }
  void AddComptEdep(G4double dE_keV) { fEdepCompt_keV += dE_keV; }

  // Mode 1
  void SetGenP1(G4double E_keV, G4double theta, G4double phi) {
    fP1GenE = E_keV; fP1GenTheta = theta; fP1GenPhi = phi;
  }
  void SetHitPlastic(int v)  { fP1HitPlastic = v; }
  void SetHitSilica(int v)   { fP1HitSilica  = v; }
  void SetStopSilica(int v)  { fP1StopSilica = v; }
  void AddPlasticEdep(G4double dE_keV) { fP1Edep += dE_keV; }
  void SetOutP1(G4double E_keV, G4double theta, G4double phi) {
    fP1OutE = E_keV; fP1OutTheta = theta; fP1OutPhi = phi;
  }

  // Mode 3
  // SteppingAction が e+ 停止を検出したらここを呼ぶ
  void SetM3Stop(const G4ThreeVector& pos) {
    fM3StopPos = pos; fM3KillAnnihil = true;
  }
  bool GetM3KillAnnihil() const { return fM3KillAnnihil; }
  G4ThreeVector GetM3StopPos() const { return fM3StopPos; }
  void SetM3NGamma(int n) { fM3NGamma = n; }
  void AddNaiEdepM3(G4double dE_keV) { fM3NaiEdep += dE_keV; }
  void AddNaiEdepPs(G4double dE_keV) { fM3NaiEdepPs += dE_keV; }
  void SetHitNaiM3(int v) { fM3HitNai |= v; }
  G4double GetM3NaiEdep() const { return fM3NaiEdep; }

  // Ps由来trackID管理（子孫へのタグ伝搬用）
  void  AddPsTrackID(G4int id)         { fPsTrackIDs.insert(id); }
  bool  IsPsTrack(G4int parentID) const { return fPsTrackIDs.count(parentID) > 0; }

 private:
  RunAction* fRun{};

  // Mode 2 fields
  G4double fEdep_keV{0.0};
  G4double fEdep_smeared_keV{0.0};
  G4double fTruthE_keV{0.0};
  G4double fE1{0}, fE2{0}, fE3{0};
  int fHitNaI{0};
  G4double fEdepPhoto_keV{0.0};
  G4double fEdepCompt_keV{0.0};

  // Mode 1 fields
  G4double fP1GenE{0};
  G4double fP1GenTheta{0};
  G4double fP1GenPhi{0};
  int      fP1HitPlastic{0};
  int      fP1HitSilica{0};
  int      fP1StopSilica{0};
  G4double fP1Edep{0};
  G4double fP1OutE{0};
  G4double fP1OutTheta{0};
  G4double fP1OutPhi{0};

  // Mode 3 fields
  bool          fM3KillAnnihil{false};
  G4ThreeVector fM3StopPos{};
  int           fM3NGamma{0};
  G4double      fM3NaiEdep{0};
  G4double      fM3NaiEdepPs{0};   // Ps由来のみのNaI edep
  int           fM3HitNai{0};
  std::set<G4int> fPsTrackIDs{};   // Ps由来trackIDの集合（子孫へのタグ伝搬用）
};
#endif
