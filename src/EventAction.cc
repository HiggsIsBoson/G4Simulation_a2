#include "EventAction.hh"
#include "RunAction.hh"
#include "PsEventInfo.hh"
#include <G4AnalysisManager.hh>
#include <G4RunManager.hh>
#include <G4Event.hh>
#include <Randomize.hh>

EventAction::EventAction(RunAction* run): fRun(run){}

void EventAction::BeginOfEventAction(const G4Event* evt) {
  fEdep_keV = 0.0;
  auto* info = new PsEventInfo();
  const_cast<G4Event*>(evt)->SetUserInformation(info);

}

void EventAction::EndOfEventAction(const G4Event* evt) {

  auto man = G4AnalysisManager::Instance();
  auto run = static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
  auto* info = static_cast<PsEventInfo*>(evt->GetUserInformation());
  int originCode = 0;
  /*
  if (info) {
    if (info->GetOrigin()==GammaOrigin::Photo) originCode = 1;
    else if (info->GetOrigin()==GammaOrigin::Compton) originCode = 2;
  }
  */

  if(fabs(fEdep_keV-511.)<1. || fabs(fEdep_keV-2*511.)<1.){
    originCode = int(GammaOrigin::Photo);
  }else{
    originCode = int(GammaOrigin::Compton);
  }

  const double E = fEdep_keV;
  G4double sigma = std::sqrt(fRun->GetA()*fRun->GetA()*E +
			     fRun->GetB()*fRun->GetB()*E*E +
			     fRun->GetC()*fRun->GetC());
  fEdep_smeared_keV = G4RandGauss::shoot(E, sigma);

  // Fill
  man->FillNtupleDColumn(run->GetNtColEdep(),   fEdep_keV);
  man->FillNtupleDColumn(run->GetNtColEsm(),    fEdep_smeared_keV);
  man->FillNtupleIColumn(run->GetNtColOrigin(), originCode);
  //std::cout << "aaa1 " << fE1 << std::endl;
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
