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

  if (!evt->GetUserInformation()) {
    auto* info = new PsEventInfo();
    const_cast<G4Event*>(evt)->SetUserInformation(info);
  }

}

void EventAction::EndOfEventAction(const G4Event* evt) {

  auto man = G4AnalysisManager::Instance();
  auto run = static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());

  int originCode = 0; // 0=None, 1=photo, 2=compt
  if (fEdepPhoto_keV > fEdepCompt_keV && fEdepPhoto_keV > 0.0) {
    originCode = 1;
  } else if (fEdepCompt_keV > 0.0) {
    originCode = 2;
  }

  // PsEventInfo
  if (auto* info = static_cast<PsEventInfo*>(evt->GetUserInformation())) {
    if (originCode == 1)      info->SetOrigin(GammaOrigin::Photo);
    else if (originCode == 2) info->SetOrigin(GammaOrigin::Compton);
    else                      info->SetOrigin(GammaOrigin::None);

    auto Egamma = info->GetGenGammaEnergies();
    //std::cout << "ggg1 " << Egamma.size() << std::endl; 
    if(Egamma.size()>=1) fE1 = Egamma[0];
    if(Egamma.size()>=2) fE2 = Egamma[1];
    if(Egamma.size()>=3) fE3 = Egamma[2];
  }

  
  /*
  auto* info = static_cast<PsEventInfo*>(evt->GetUserInformation());
  int originCode = 0;

  if (info) {
    if (info->GetOrigin()==GammaOrigin::Photo) originCode = 1;
    else if (info->GetOrigin()==GammaOrigin::Compton) originCode = 2;
  }
  */

  const double E = fEdep_keV;
  G4double sigma = std::sqrt(fRun->GetA()*fRun->GetA()*E +
			     fRun->GetB()*fRun->GetB()*E*E +
			     fRun->GetC()*fRun->GetC());
  fEdep_smeared_keV = G4RandGauss::shoot(E, sigma);

  // Fill
  man->FillNtupleDColumn(run->GetNtColEdep(),   fEdep_keV);
  man->FillNtupleDColumn(run->GetNtColEsm(),    fEdep_smeared_keV);
  man->FillNtupleIColumn(run->GetNtColOrigin(), originCode);
  man->FillNtupleDColumn(run->GetNtColE1(), fE1);
  man->FillNtupleDColumn(run->GetNtColE2(), fE2);
  man->FillNtupleDColumn(run->GetNtColE3(), fE3);
  man->FillNtupleIColumn(run->GetNtColHitNaI(), fHitNaI);

  man->AddNtupleRow(); 

  // Reset
  fEdep_keV = 0.0;
  fEdep_smeared_keV = 0.0;
  fTruthE_keV = 0.0;
  fHitNaI = 0;
  fE1 = fE2 = fE3 = 0.0;

  // Event counter
  G4int evtID = evt->GetEventID(); 
  if ((evtID+1) % 10000 == 0) {
    G4cout << ">>> Event " << std::setw(8) << evtID+1 << " finished" << G4endl;
  }

}
