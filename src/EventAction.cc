#include "EventAction.hh"
#include "RunAction.hh"
#include "PsEventInfo.hh"
#include <G4AnalysisManager.hh>
#include <G4RunManager.hh>
#include <G4Event.hh>
#include <Randomize.hh>

EventAction::EventAction(RunAction* run): fRun(run){}

void EventAction::BeginOfEventAction(const G4Event* evt) {

  fEdep_keV         = 0.0;
  fEdep_smeared_keV = 0.0;
  fTruthE_keV       = 0.0;
  fE1 = fE2 = fE3   = 0.0;
  fHitNaI           = 0;
  fEdepPhoto_keV    = 0.0;
  fEdepCompt_keV    = 0.0;

  fP1GenE       = 0.0;
  fP1GenTheta   = 0.0;
  fP1GenPhi     = 0.0;
  fP1HitPlastic = 0;
  fP1HitSilica  = 0;
  fP1StopSilica = 0;
  fP1Edep       = 0.0;
  fP1OutE       = 0.0;
  fP1OutTheta   = 0.0;
  fP1OutPhi     = 0.0;

  fM3KillAnnihil = false;
  fM3StopPos     = G4ThreeVector(0,0,0);
  fM3NGamma      = 0;
  fM3NaiEdep     = 0.0;
  fM3NaiEdepPs   = 0.0;
  fM3HitNai      = 0;
  fPsTrackIDs.clear();

  if (!evt->GetUserInformation()) {
    auto* info = new PsEventInfo();
    const_cast<G4Event*>(evt)->SetUserInformation(info);
  }

}

void EventAction::EndOfEventAction(const G4Event* evt) {

  auto man = G4AnalysisManager::Instance();
  auto run = static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());

  if (run->GetMode() == 1) {
    // gen_E は PsEventInfo 経由で取得（GeneratePrimaries が書き込む）
    if (auto* info = static_cast<PsEventInfo*>(evt->GetUserInformation())) {
      fP1GenE     = info->GetBetaE();
      fP1GenTheta = info->GetBetaTheta();
      fP1GenPhi   = info->GetBetaPhi();
    }
    // Mode 1: fill plastic ntuple
    man->FillNtupleDColumn(run->GetNtP1GenE(),       fP1GenE);
    man->FillNtupleDColumn(run->GetNtP1GenTheta(),   fP1GenTheta);
    man->FillNtupleDColumn(run->GetNtP1GenPhi(),     fP1GenPhi);
    man->FillNtupleIColumn(run->GetNtP1HitPlastic(), fP1HitPlastic);
    man->FillNtupleIColumn(run->GetNtP1HitSilica(),  fP1HitSilica);
    man->FillNtupleIColumn(run->GetNtP1StopSilica(), fP1StopSilica);
    man->FillNtupleDColumn(run->GetNtP1Edep(),       fP1Edep);
    man->FillNtupleDColumn(run->GetNtP1OutE(),       fP1OutE);
    man->FillNtupleDColumn(run->GetNtP1OutTheta(),   fP1OutTheta);
    man->FillNtupleDColumn(run->GetNtP1OutPhi(),     fP1OutPhi);
    man->AddNtupleRow();
    const_cast<RunAction*>(run)->AccumP1(fP1Edep, fP1OutE > 0);
  } else if (run->GetMode() == 3) {
    // Mode 3: full-chain ntuple
    G4double genE = 0;
    if (auto* info = static_cast<PsEventInfo*>(evt->GetUserInformation()))
      genE = info->GetBetaE();
    man->FillNtupleDColumn(run->GetNtM3GenE(),       genE);
    man->FillNtupleDColumn(run->GetNtM3PlasticEdep(),fP1Edep);
    man->FillNtupleIColumn(run->GetNtM3HitSilica(),  fP1HitSilica);
    man->FillNtupleIColumn(run->GetNtM3StopSilica(), fP1StopSilica);
    man->FillNtupleIColumn(run->GetNtM3NGamma(),     fM3NGamma);
    man->FillNtupleDColumn(run->GetNtM3NaiEdep(),    fM3NaiEdep);
    {
      auto smear = [&](G4double E) {
        G4double sigma = std::sqrt(fRun->GetA()*fRun->GetA()*E +
                                   fRun->GetB()*fRun->GetB()*E*E +
                                   fRun->GetC()*fRun->GetC());
        return G4RandGauss::shoot(E, sigma);
      };
      man->FillNtupleDColumn(run->GetNtM3NaiEdepSm(),   smear(fM3NaiEdep));
      man->FillNtupleDColumn(run->GetNtM3NaiEdepPs(),   fM3NaiEdepPs);
      man->FillNtupleDColumn(run->GetNtM3NaiEdepPsSm(), smear(fM3NaiEdepPs));
    }
    man->FillNtupleIColumn(run->GetNtM3HitNai(),     fM3HitNai);
    man->AddNtupleRow();
    const_cast<RunAction*>(run)->AccumM3(fP1StopSilica, fM3HitNai>0);
  } else {
    // Mode 2: fill NaI ntuple
    int originCode = 0;
    if (fEdepPhoto_keV > fEdepCompt_keV && fEdepPhoto_keV > 0.0) originCode = 1;
    else if (fEdepCompt_keV > 0.0) originCode = 2;

    if (auto* info = static_cast<PsEventInfo*>(evt->GetUserInformation())) {
      if (originCode == 1)      info->SetOrigin(GammaOrigin::Photo);
      else if (originCode == 2) info->SetOrigin(GammaOrigin::Compton);
      else                      info->SetOrigin(GammaOrigin::None);

      auto Egamma = info->GetGenGammaEnergies();
      if(Egamma.size()>=1) fE1 = Egamma[0];
      if(Egamma.size()>=2) fE2 = Egamma[1];
      if(Egamma.size()>=3) fE3 = Egamma[2];
    }

    const double E = fEdep_keV;
    G4double sigma = std::sqrt(fRun->GetA()*fRun->GetA()*E +
                               fRun->GetB()*fRun->GetB()*E*E +
                               fRun->GetC()*fRun->GetC());
    fEdep_smeared_keV = G4RandGauss::shoot(E, sigma);

    man->FillNtupleDColumn(run->GetNtColEdep(),   fEdep_keV);
    man->FillNtupleDColumn(run->GetNtColEsm(),    fEdep_smeared_keV);
    man->FillNtupleIColumn(run->GetNtColOrigin(), originCode);
    man->FillNtupleDColumn(run->GetNtColE1(), fE1);
    man->FillNtupleDColumn(run->GetNtColE2(), fE2);
    man->FillNtupleDColumn(run->GetNtColE3(), fE3);
    man->FillNtupleIColumn(run->GetNtColHitNaI(), fHitNaI);
    man->AddNtupleRow();
  }

  G4int evtID = evt->GetEventID();
  if ((evtID+1) % 10000 == 0) {
    G4cout << ">>> Event " << std::setw(8) << evtID+1 << " finished" << G4endl;
  }

}
