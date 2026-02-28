#include "RunAction.hh"
#include <G4AnalysisManager.hh>
#include <G4GenericMessenger.hh>
#include <G4SystemOfUnits.hh>


RunAction::RunAction() : fa(2.5), fb(0.01), fc(0.0), fOutName("nai_spectrum") {
  fMsg = new G4GenericMessenger(this, "/analysis/", "Analysis controls");
  fMsg->DeclareProperty("a", fa, "Resolution param a (sqrt(keV))");
  fMsg->DeclareProperty("b", fb, "Resolution param b (fraction)");
  fMsg->DeclareProperty("c", fc, "Resolution param c (keV)");
  fMsg->DeclareProperty("out", fOutName, "Output base filename (no extension)");
  std::cout << "aaa2 " << fOutName << std::endl;
}

RunAction::~RunAction(){ delete fMsg; }

void RunAction::Book(){

  auto man = G4AnalysisManager::Instance();
  man->OpenFile(fOutName);
  man->SetVerboseLevel(1);
  man->SetDefaultFileType("root");         // ★ ROOT形式に
  //man->SetDefaultFileType("csv"); // write CSV by default
  
  // ★ Ntuple (event-by-event)
  man->CreateNtuple("nai", "NaI energy deposit per event");

  fNtColEdep   = man->CreateNtupleDColumn("rawE"); // raw
  fNtColEsm    = man->CreateNtupleDColumn("smE"); // smeared
  fNtColOrigin = man->CreateNtupleIColumn("Origin");     // 0/1/2 etc
  fNtColE1     = man->CreateNtupleDColumn("trE1");
  fNtColE2     = man->CreateNtupleDColumn("trE2");
  fNtColE3     = man->CreateNtupleDColumn("trE3");
  fNtColHitNaI = man->CreateNtupleIColumn("HitNaI");     // 0..7 のビット

  man->FinishNtuple();
}

void RunAction::BeginOfRunAction(const G4Run*){ Book(); }

void RunAction::EndOfRunAction(const G4Run*){
  auto man = G4AnalysisManager::Instance();
  man->AddNtupleRow();
  man->Write();
  man->CloseFile();
}
