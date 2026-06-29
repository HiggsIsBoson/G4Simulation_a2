#include <string>
#include <iostream>

// Geant4 core
#include <G4RunManager.hh>
#include <G4UImanager.hh>

// UI / Visualization
#include <G4UIExecutive.hh>
#include <G4VisExecutive.hh>

// Physics list
#include <FTFP_BERT.hh>

// Your classes
#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv) {

  // --- parse command line options ---
  std::string p2_value;
  std::string out_name;
  std::string macro_file;
  G4double silicaZ_mm = 50.;  // Mode 1/3: silica center z [mm]
  bool useUI = false;
  int simMode = 2;

  for (int i=1; i<argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--p2" && i+1 < argc) {
      p2_value = argv[++i];
    } else if (arg == "--out" && i+1 < argc) {
      out_name = argv[++i];
    } else if (arg == "--mode" && i+1 < argc) {
      simMode = std::stoi(argv[++i]);
    } else if (arg == "--silica-z" && i+1 < argc) {
      silicaZ_mm = std::stod(argv[++i]);
    } else if (arg == "-ui" || arg == "--ui") {
      useUI = true;
    } else if (arg[0] != '-') {
      macro_file = arg;
    }
  }

  auto* runManager = new G4RunManager();

  runManager->SetUserInitialization(new DetectorConstruction(simMode, silicaZ_mm));
  runManager->SetUserInitialization(new FTFP_BERT());
  runManager->SetUserInitialization(new ActionInitialization(simMode));

  auto* UIman = G4UImanager::GetUIpointer();

  if (simMode == 2 || simMode == 3) {
    if (!p2_value.empty()){
      UIman->ApplyCommand("/source/p2 " + p2_value);
    } else if (!useUI) {
      std::cout << "Mode " << simMode << ": Specify --p2 from the command line. Abort." << std::endl;
      return 1;
    }
  }

  if (!out_name.empty()){
    UIman->ApplyCommand("/analysis/out " + out_name);
  }
  else if(!useUI && (simMode == 2 || simMode == 3)){
    std::cout << "Specify the output file from the command line. Abort." << std::endl;
    return 1;
  }


  if (useUI) {
    auto* ui = new G4UIExecutive(argc, argv, "qt");

    auto* visManager = new G4VisExecutive();
    visManager->Initialize();

    if (!macro_file.empty()) {
      UIman->ApplyCommand("/control/execute " + macro_file);
    }

    ui->SessionStart();

    delete visManager;
    delete ui;
  }
  else {
    // batch mode
    if (!macro_file.empty()) {
      UIman->ApplyCommand("/control/execute " + macro_file);
    }

  }

  delete runManager;
  return 0;
}

