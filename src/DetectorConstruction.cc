#include "DetectorConstruction.hh"
#include <G4Box.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4Tubs.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4RotationMatrix.hh>

DetectorConstruction::DetectorConstruction()
: fSilicaR(1.5*cm), fSilicaHL(5.0*cm), 
  fNaI4R(3.0*cm), 
  fNaI4HL(7.75*cm), 
  fNaILogic(nullptr)
{}

G4VPhysicalVolume* DetectorConstruction::Construct() {
  auto* nist = G4NistManager::Instance();

  // --- 1. 素材定義 ---
  fMatAir   = nist->FindOrBuildMaterial("G4_AIR");
  fMatNaI   = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");
  auto* matGlass = nist->FindOrBuildMaterial("G4_Pyrex_Glass");
  fMatLead  = nist->FindOrBuildMaterial("G4_Pb"); 
  auto* matBase = nist->FindOrBuildMaterial("G4_LUCITE"); // 土台（プラスチック）

  fMatTable = nist->FindOrBuildMaterial("G4_WOOD_ST");
  if (!fMatTable) {
      fMatTable = new G4Material("Wood", 0.7*g/cm3, 1);
      fMatTable->AddElement(nist->FindOrBuildElement("C"), 1);
  }

  auto* Si = nist->FindOrBuildElement("Si");
  auto* O  = nist->FindOrBuildElement("O");
  fMatSiO2 = new G4Material("SiO2_powder", 0.5*g/cm3, 2);
  fMatSiO2->AddElement(Si, 1);
  fMatSiO2->AddElement(O, 2);

  // 重なりチェックを有効にするフラグ
  G4bool checkOverlaps = true;

  // --- 2. World ---
  auto worldS = new G4Box("worldS", 60*cm, 60*cm, 60*cm);
  auto worldL = new G4LogicalVolume(worldS, fMatAir, "worldL");
  auto worldP = new G4PVPlacement(nullptr, {}, worldL, "worldP", nullptr, false, 0);
  worldL->SetVisAttributes(G4VisAttributes::GetInvisible());

  // --- 3. 机と土台 (下から積み上げ) ---
  // 机の天面を y = -4.0cm に設定
  auto tableS = new G4Box("tableS", 40*cm, 2.5*cm, 40*cm);
  auto tableL = new G4LogicalVolume(tableS, fMatTable, "tableL");
  tableL->SetVisAttributes(new G4VisAttributes(G4Colour(0.45, 0.25, 0.0))); 
  new G4PVPlacement(nullptr, G4ThreeVector(0, -6.5*cm, 0), tableL, "tableP", worldL, false, 0, checkOverlaps);

  // 土台の天面を y = -2.0cm に設定
  auto baseS = new G4Box("baseS", 3.0*cm, 1.0*cm, 2.5*cm); 
  auto baseL = new G4LogicalVolume(baseS, matBase, "baseL");
  baseL->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 0.5, 1.0, 0.5)));
  new G4PVPlacement(nullptr, G4ThreeVector(0, -3.0*cm, 0), baseL, "baseP", worldL, false, 0, checkOverlaps);

  // --- 4. ガラス管と粉末 (Z軸方向配置) ---
  // ガラス管の底を土台の天面(-2.0cm)に合わせる -> 中心 y = -2.0 + 1.5 = -0.5cm
  G4double glassY = -0.5*cm;
  auto glassS = new G4Tubs("glassS", 0., fSilicaR, fSilicaHL, 0., 360.*deg);
  auto glassL = new G4LogicalVolume(glassS, matGlass, "glassL");
  glassL->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1.0, 1.0, 0.1)));
  new G4PVPlacement(nullptr, G4ThreeVector(0, glassY, 0), glassL, "glassP", worldL, false, 0, checkOverlaps);

  // シリカ粉末：ガラス管内の下半分 (180度〜360度)
  auto powderS = new G4Tubs("powderS", 0., fSilicaR, fSilicaHL, 180.*deg, 180.*deg);
  auto powderL = new G4LogicalVolume(powderS, fMatSiO2, "powderL");
  powderL->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1.0, 0.0)));
  new G4PVPlacement(nullptr, {}, powderL, "powderP", glassL, false, 0, checkOverlaps);

  // --- 5. NaI 4 検出器 (X軸方向配置) ---
  // ガラス管の天面位置は glassY(-0.5) + fSilicaR(1.5) = +1.0cm
  // 検出器が接するための中心 y は、天面(1.0) + 検出器の半径(3.0) = 4.0cm
  // Overlap回避のため、0.1mm だけ上に逃がします
  G4double naiY = 4.01*cm; 
  
  auto nai4S = new G4Tubs("nai4S", 0., fNaI4R, fNaI4HL, 0., 360.*deg);
  fNaILogic = new G4LogicalVolume(nai4S, fMatNaI, "nai4L");
  fNaILogic->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 0.5)));

  G4RotationMatrix* rotNaI = new G4RotationMatrix();
  rotNaI->rotateY(90.*deg); 
  new G4PVPlacement(rotNaI, G4ThreeVector(0, naiY, 0), fNaILogic, "nai4P", worldL, false, 0, checkOverlaps);

  // --- 6. 鉛のシールド (積み上げ配置) ---
  auto leadS = new G4Box("leadS", 10*cm, 2.5*cm, 5*cm); 
  auto leadL = new G4LogicalVolume(leadS, fMatLead, "leadL");
  leadL->SetVisAttributes(new G4VisAttributes(G4Colour(0.3, 0.3, 0.3, 0.4))); 

  // 壁をきれいに積み上げるための Y座標リスト
  // y1: 机の直上(-1.5), y2: その上(3.5), y3: さらにその上(8.5)
  for(auto y : {-1.5*cm, 3.5*cm, 8.5*cm}) {
    new G4PVPlacement(nullptr, G4ThreeVector(0, y, -10.0*cm), leadL, "leadZ", worldL, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(0, y,  10.0*cm), leadL, "leadZ", worldL, false, 0, checkOverlaps);
  }

  G4RotationMatrix* rotLead = new G4RotationMatrix();
  rotLead->rotateY(90.*deg);
  for(auto y : {-1.5*cm, 3.5*cm, 8.5*cm}) {
    new G4PVPlacement(rotLead, G4ThreeVector(13.75*cm, y, 0), leadL, "leadX_R", worldL, false, 0, checkOverlaps);
    new G4PVPlacement(rotLead, G4ThreeVector(-13.75*cm, y, 0), leadL, "leadX_L", worldL, false, 0, checkOverlaps);
  }

  // 天井 (y = 11.0 + 2.5 = 13.5cm)
  auto lidS = new G4Box("lidS", 25*cm, 2.5*cm, 20*cm);
  auto lidL = new G4LogicalVolume(lidS, fMatLead, "lidL");
  lidL->SetVisAttributes(new G4VisAttributes(G4Colour(0.3, 0.3, 0.3, 0.2))); 
  new G4PVPlacement(nullptr, G4ThreeVector(0, 13.5*cm, 0), lidL, "lidP", worldL, false, 0, checkOverlaps);

  return worldP;
}