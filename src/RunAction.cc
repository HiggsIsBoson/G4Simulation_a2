#include "RunAction.hh"
#include <G4AnalysisManager.hh>
#include <G4GenericMessenger.hh>
#include <G4SystemOfUnits.hh>


RunAction::RunAction(int mode)
: fMode(mode), fa(2.5), fb(0.01), fc(0.0),
  fOutName(mode==1 ? "plastic_sim" : (mode==3 ? "full_chain" : "nai_spectrum")) {
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
  man->SetDefaultFileType("root");

  if (fMode == 1) {
    // Mode 1: beta+ through plastic
    man->CreateNtuple("plastic", "beta+ transmission through plastic per event");
    fNtP1GenE       = man->CreateNtupleDColumn("gen_E");       // initial KE [keV]
    fNtP1GenTheta   = man->CreateNtupleDColumn("gen_theta");   // initial polar angle [rad]
    fNtP1GenPhi     = man->CreateNtupleDColumn("gen_phi");     // initial azimuth [rad]
    fNtP1HitPlastic = man->CreateNtupleIColumn("hit_plastic"); // 1 if e+ entered plastic
    fNtP1HitSilica  = man->CreateNtupleIColumn("hit_silica");  // 1 if e+ entered silica
    fNtP1StopSilica = man->CreateNtupleIColumn("stop_silica"); // 1 if e+ stopped in silica
    fNtP1Edep       = man->CreateNtupleDColumn("edep");        // energy deposit in plastic [keV]
    fNtP1OutE       = man->CreateNtupleDColumn("out_E");       // outgoing KE [keV], 0 if stopped
    fNtP1OutTheta   = man->CreateNtupleDColumn("out_theta");   // outgoing polar angle [rad]
    fNtP1OutPhi     = man->CreateNtupleDColumn("out_phi");     // outgoing azimuth [rad]
    man->FinishNtuple();
  } else if (fMode == 3) {
    // Mode 3: full-chain ntuple
    man->CreateNtuple("chain", "full chain: Na22 → plastic → silica(Ps) → NaI");
    fNtM3GenE        = man->CreateNtupleDColumn("gen_E");        // e+ initial KE [keV]
    fNtM3PlasticEdep = man->CreateNtupleDColumn("plastic_edep"); // energy deposit in plastic [keV]
    fNtM3HitSilica   = man->CreateNtupleIColumn("hit_silica");   // 1 if e+ entered silica
    fNtM3StopSilica  = man->CreateNtupleIColumn("stop_silica");  // 1 if e+ stopped in silica (Ps formed)
    fNtM3NGamma      = man->CreateNtupleIColumn("n_gamma");      // 2 or 3 (valid if stop_silica==1)
    fNtM3NaiEdep     = man->CreateNtupleDColumn("nai_edep");      // NaI total energy deposit [keV]
    fNtM3NaiEdepSm   = man->CreateNtupleDColumn("nai_edep_sm");    // NaI edep with detector smearing [keV]
    fNtM3NaiEdepPs   = man->CreateNtupleDColumn("nai_edep_ps");   // Ps由来のNaI edep (raw) [keV]
    fNtM3NaiEdepPsSm = man->CreateNtupleDColumn("nai_edep_ps_sm");// Ps由来のNaI edep (smeared) [keV]
    fNtM3HitNai      = man->CreateNtupleIColumn("hit_nai");        // 1 if any gamma hit NaI
    man->FinishNtuple();
  } else {
    // Mode 2: NaI energy deposit
    man->CreateNtuple("nai", "NaI energy deposit per event");
    fNtColEdep   = man->CreateNtupleDColumn("rawE");
    fNtColEsm    = man->CreateNtupleDColumn("smE");
    fNtColOrigin = man->CreateNtupleIColumn("Origin");
    fNtColE1     = man->CreateNtupleDColumn("trE1");
    fNtColE2     = man->CreateNtupleDColumn("trE2");
    fNtColE3     = man->CreateNtupleDColumn("trE3");
    fNtColHitNaI = man->CreateNtupleIColumn("HitNaI");
    man->FinishNtuple();
  }
}

void RunAction::BeginOfRunAction(const G4Run*){ Book(); }

void RunAction::EndOfRunAction(const G4Run*){
  auto man = G4AnalysisManager::Instance();
  man->Write();
  man->CloseFile();

  if (fMode == 3 && fM3NTotal > 0) {
    G4cout << "\n========== Mode 3 Summary ==========" << G4endl;
    G4cout << "  Total events        : " << fM3NTotal << G4endl;
    G4cout << "  Stopped in silica   : " << fM3NStopSilica
           << "  (" << 100.*fM3NStopSilica/fM3NTotal << " %)" << G4endl;
    G4cout << "  NaI hit             : " << fM3NHitNai
           << "  (" << 100.*fM3NHitNai/fM3NTotal << " %)" << G4endl;
    G4cout << "====================================" << G4endl;
  }
  if (fMode == 1 && fP1NTotal > 0) {
    G4double mean = fP1EdepSum / fP1NTotal;
    G4double rms  = std::sqrt(std::max(0., fP1EdepSum2/fP1NTotal - mean*mean));
    G4cout << "\n========== Mode 1 Summary ==========" << G4endl;
    G4cout << "  Total events        : " << fP1NTotal << G4endl;
    G4cout << "  Transmitted (out_E>0): " << fP1NTransmitted
           << "  (" << 100.*fP1NTransmitted/fP1NTotal << " %)" << G4endl;
    G4cout << "  Stopped in plastic  : " << fP1NTotal - fP1NTransmitted
           << "  (" << 100.*(fP1NTotal-fP1NTransmitted)/fP1NTotal << " %)" << G4endl;
    G4cout << "  plastic edep mean   : " << mean << " keV  (RMS=" << rms << " keV)" << G4endl;
    G4cout << "====================================" << G4endl;
  }
}
