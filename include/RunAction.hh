#ifndef RUN_ACTION_HH
#define RUN_ACTION_HH
#include <G4UserRunAction.hh>
#include <G4String.hh>
class G4Run;
class G4GenericMessenger;

class RunAction : public G4UserRunAction {
 public:
  explicit RunAction(int mode = 2);
  ~RunAction() override;
  void BeginOfRunAction(const G4Run*) override;
  void EndOfRunAction(const G4Run*) override;

  int GetMode() const { return fMode; }

  // Mode 2 resolution model
  G4double GetA() const { return fa; }
  G4double GetB() const { return fb; }
  G4double GetC() const { return fc; }

  // Mode 2 ntuple columns
  G4int GetNtColEdep()   const { return fNtColEdep; }
  G4int GetNtColEsm()    const { return fNtColEsm; }
  G4int GetNtColOrigin() const { return fNtColOrigin; }
  G4int GetNtColE1()     const { return fNtColE1; }
  G4int GetNtColE2()     const { return fNtColE2; }
  G4int GetNtColE3()     const { return fNtColE3; }
  G4int GetNtColHitNaI() const { return fNtColHitNaI; }

  // Mode 1 ntuple columns
  G4int GetNtP1GenE()      const { return fNtP1GenE; }
  G4int GetNtP1GenTheta()  const { return fNtP1GenTheta; }
  G4int GetNtP1GenPhi()    const { return fNtP1GenPhi; }
  G4int GetNtP1HitPlastic() const { return fNtP1HitPlastic; }
  G4int GetNtP1HitSilica()  const { return fNtP1HitSilica; }
  G4int GetNtP1StopSilica() const { return fNtP1StopSilica; }
  G4int GetNtP1Edep()      const { return fNtP1Edep; }
  G4int GetNtP1OutE()      const { return fNtP1OutE; }
  G4int GetNtP1OutTheta()  const { return fNtP1OutTheta; }
  G4int GetNtP1OutPhi()    const { return fNtP1OutPhi; }

 private:
  void Book();

  int fMode;

  // Resolution model sigma(E) = sqrt(a^2 E + b^2 E^2 + c^2)  [E in keV]
  G4double fa, fb, fc;
  G4GenericMessenger* fMsg{};

  G4String fOutName;

  // Mode 2 handles
  G4int fNtColEdep{-1};
  G4int fNtColEsm{-1};
  G4int fNtColOrigin{-1};
  G4int fNtColE1{-1};
  G4int fNtColE2{-1};
  G4int fNtColE3{-1};
  G4int fNtColHitNaI{-1};

  // Mode 1 counters (for end-of-run summary)
  G4long fP1NTotal{0};
  G4long fP1NTransmitted{0};
  G4double fP1EdepSum{0};
  G4double fP1EdepSum2{0};

 public:
  // Mode 3 ntuple columns
  G4int GetNtM3GenE()        const { return fNtM3GenE; }
  G4int GetNtM3PlasticEdep() const { return fNtM3PlasticEdep; }
  G4int GetNtM3HitSilica()   const { return fNtM3HitSilica; }
  G4int GetNtM3StopSilica()  const { return fNtM3StopSilica; }
  G4int GetNtM3NGamma()      const { return fNtM3NGamma; }
  G4int GetNtM3NaiEdep()     const { return fNtM3NaiEdep; }
  G4int GetNtM3NaiEdepSm()   const { return fNtM3NaiEdepSm; }
  G4int GetNtM3NaiEdepPs()   const { return fNtM3NaiEdepPs; }
  G4int GetNtM3NaiEdepPsSm() const { return fNtM3NaiEdepPsSm; }
  G4int GetNtM3HitNai()      const { return fNtM3HitNai; }

  void AccumP1(G4double edep, G4bool transmitted) {
    ++fP1NTotal;
    fP1EdepSum  += edep;
    fP1EdepSum2 += edep * edep;
    if (transmitted) ++fP1NTransmitted;
  }
  void AccumM3(G4bool stoppedInSilica, G4bool hitNai) {
    ++fM3NTotal;
    if (stoppedInSilica) ++fM3NStopSilica;
    if (hitNai)          ++fM3NHitNai;
  }

 private:
  // Mode 1 handles
  G4int fNtP1GenE{-1};
  G4int fNtP1GenTheta{-1};
  G4int fNtP1GenPhi{-1};
  G4int fNtP1HitPlastic{-1};
  G4int fNtP1HitSilica{-1};
  G4int fNtP1StopSilica{-1};
  G4int fNtP1Edep{-1};
  G4int fNtP1OutE{-1};
  G4int fNtP1OutTheta{-1};
  G4int fNtP1OutPhi{-1};

  // Mode 3 handles
  G4int fNtM3GenE{-1};
  G4int fNtM3PlasticEdep{-1};
  G4int fNtM3HitSilica{-1};
  G4int fNtM3StopSilica{-1};
  G4int fNtM3NGamma{-1};
  G4int fNtM3NaiEdep{-1};
  G4int fNtM3NaiEdepSm{-1};
  G4int fNtM3NaiEdepPs{-1};
  G4int fNtM3NaiEdepPsSm{-1};
  G4int fNtM3HitNai{-1};

  // Mode 3 counters
  G4long   fM3NTotal{0};
  G4long   fM3NStopSilica{0};
  G4long   fM3NHitNai{0};
};
#endif
