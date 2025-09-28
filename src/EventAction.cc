#include "EventAction.hh"
#include "RunAction.hh"
#include "PsEventInfo.hh"
#include <G4AnalysisManager.hh>
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

  man->FillNtupleDColumn(0, fEdep_keV);  // Edep
  man->FillNtupleDColumn(1, fEdep_smeared_keV);  // Edep (smeared)
  man->FillNtupleDColumn(2, fTruthE_keV); // truth sum
  man->FillNtupleIColumn(3, originCode); // Origin分類
  man->AddNtupleRow();                   // ★ 1イベントごとに呼ぶ

  // Reset for the next event
  fEdep_keV = 0.0; 
  fEdep_smeared_keV = 0.0;
  fTruthE_keV = 0.0;
}
