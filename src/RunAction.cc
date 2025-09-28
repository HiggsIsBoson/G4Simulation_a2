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
  man->CreateNtupleDColumn("rawE");   // double カラム
  man->CreateNtupleDColumn("smE");   // double カラム
  man->CreateNtupleDColumn("trE");
  man->CreateNtupleIColumn("Origin");     // int カラム (0=None,1=phot,2=compt)
  man->FinishNtuple();
}

void RunAction::BeginOfRunAction(const G4Run*){ Book(); }

void RunAction::EndOfRunAction(const G4Run*){
  auto man = G4AnalysisManager::Instance();
  man->Write();
  man->CloseFile();
}
