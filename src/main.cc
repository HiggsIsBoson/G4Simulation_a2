#include <G4RunManager.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>

#include <FTFP_BERT.hh> 

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv) {

  // --- parse command line options ---
  std::string p2_value;
  std::string out_name;
  std::string macro_file =  "../macros/vis.mac"; // default
  bool useUI = false;

  for (int i=1; i<argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--p2" && i+1 < argc) {
      p2_value = argv[++i];
    } else if (arg == "--out" && i+1 < argc) {
      out_name = argv[++i];
    } else if (arg == "-ui") {
      useUI = true;
    } else if (arg[0] != '-') {
      // マクロファイル指定
      macro_file = arg;
    }
  }

  // Run manager
  auto* runManager = new G4RunManager();

  // Detector, action init
  runManager->SetUserInitialization(new DetectorConstruction());
  runManager->SetUserInitialization(new FTFP_BERT());    
  runManager->SetUserInitialization(new ActionInitialization());

  // Visualization
  auto* visManager = new G4VisExecutive();
  visManager->Initialize();

  auto* UIman = G4UImanager::GetUIpointer();

  // === 実行オプションの解釈 ===
  //  ./nai_spectrum               → バッチ、終了
  //  ./nai_spectrum macros/run.mac → バッチでマクロ実行
  //  ./nai_spectrum -ui            → インタラクティブUI起動


  // --- apply options before running ---
  if (!p2_value.empty()) {
    UIman->ApplyCommand("/source/p2 " + p2_value);
  }
  if (!out_name.empty()) {
    UIman->ApplyCommand("/analysis/out " + out_name);
  }

  // === execution control ===
  std::cout << "macro_file: " << macro_file << std::endl;
  if (useUI) {
    auto* ui = new G4UIExecutive(argc, argv);
    std::cout << "Loading macro_file: " << macro_file << std::endl;
    if (!macro_file.empty()) {
      UIman->ApplyCommand("/control/execute ../macros/vis.mac");
    }
    ui->SessionStart();
    delete ui;
  } else if (!macro_file.empty()) {
    std::cout << "Loading macro_file: " << macro_file << std::endl;
    UIman->ApplyCommand("/control/execute " + macro_file);
  }

  delete visManager;
  delete runManager;
  return 0;
}
