#include "DetectorConstruction.hh"
#include <G4Box.hh>
#include <G4NistManager.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4Tubs.hh>
#include <G4Trd.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4RotationMatrix.hh>

DetectorConstruction::DetectorConstruction()
: fNaI4R(3.0*cm), 
  fNaI4HL(7.75*cm), 
  fNaILogic(nullptr)
{}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    auto* nist = G4NistManager::Instance();
    G4bool checkOverlaps = true;

    // --- 1. 素材定義 ---
    auto* fMatAir    = nist->FindOrBuildMaterial("G4_AIR");
    auto* fMatNaI    = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");
    auto* fMatLead   = nist->FindOrBuildMaterial("G4_Pb");
    auto* matVinyl   = nist->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE");
    auto* fMatSilica = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    auto* fMatTable  = nist->FindOrBuildMaterial("G4_WOOD_ST");
    if (!fMatTable) {
        fMatTable = new G4Material("Wood", 0.7*g/cm3, 1);
        fMatTable->AddElement(nist->FindOrBuildElement("C"), 1);
    }

    // --- 2. World ---
    auto worldS = new G4Box("worldS", 50*cm, 50*cm, 50*cm);
    auto worldL = new G4LogicalVolume(worldS, fMatAir, "worldL");
    auto worldP = new G4PVPlacement(nullptr, G4ThreeVector(), worldL, "worldP", nullptr, false, 0);
    worldL->SetVisAttributes(G4VisAttributes::GetInvisible());

    // --- 3. 机 (Table) ---
    auto tableS = new G4Box("tableS", 30*cm, 1.0*cm, 30*cm);
    auto tableL = new G4LogicalVolume(tableS, fMatTable, "tableL");
    tableL->SetVisAttributes(new G4VisAttributes(G4Colour(0.45, 0.25, 0.0))); 
    new G4PVPlacement(nullptr, G4ThreeVector(0, -1.0*cm, 0), tableL, "tableP", worldL, false, 0, checkOverlaps);

    // --- 座標計算の基準 (ここを整理しました) ---
    G4double Z_Pb_front = 1.0*cm; 
    G4double tapeR = 3.3*cm;
    G4double z_Tape   = Z_Pb_front - tapeR;     
    G4double z_NaI    = Z_Pb_front - fNaI4R - 0.1*mm;    
    G4double z_Silica = Z_Pb_front - 1.0*cm - 1.0*cm; 
    G4double z_Pb2_front = (z_Tape - tapeR) - 3.0*cm; // 背面壁の位置

    // --- 4. ビニールテープ (Tape) ---
    G4double tapeW = 2.0*cm; 
    auto tapeS = new G4Tubs("tapeS", 0., tapeR, tapeW/2, 0., 360.*deg);
    auto tapeL = new G4LogicalVolume(tapeS, matVinyl, "tapeL");
    tapeL->SetVisAttributes(new G4VisAttributes(G4Colour(0.5, 0.0, 0.5, 0.7))); 
    G4RotationMatrix* rotTape = new G4RotationMatrix();
    rotTape->rotateX(90.*deg); 
    new G4PVPlacement(rotTape, G4ThreeVector(0, 1.0*cm, z_Tape), tapeL, "tapeP", worldL, false, 0, checkOverlaps);

    // --- 5. 鉛の支え (修正版：壁の間に収まる長さ) ---
    G4double supportX = 2.0*cm;
    G4double supportY = 5.0*cm;
    // 前面壁(1.0)と背面壁(z_Pb2_front)の間の距離に収まるように設定
    G4double supportZ = (Z_Pb_front - z_Pb2_front) - 0.1*cm; 
    auto supportS = new G4Box("supportS", supportX/2, supportY/2, supportZ/2);
    auto supportL = new G4LogicalVolume(supportS, fMatLead, "supportL");
    supportL->SetVisAttributes(new G4VisAttributes(G4Colour(0.3, 0.3, 0.3, 0.9)));

    G4double z_SupportCenter = (Z_Pb_front + z_Pb2_front) / 2.0;
    G4double x_Support = tapeR + 1.0*cm + supportX/2;

    new G4PVPlacement(nullptr, G4ThreeVector(-x_Support, supportY/2, z_SupportCenter), supportL, "supportP_L", worldL, false, 0, checkOverlaps);
    new G4PVPlacement(nullptr, G4ThreeVector(x_Support, supportY/2, z_SupportCenter), supportL, "supportP_R", worldL, false, 0, checkOverlaps);

    // --- 6. シリカ (Silica) ---
    auto silicaS = new G4Trd("silicaS", 1.0*cm, 1.0*cm, 1.0*cm, 0.75*cm, 1.0*cm);
    auto silicaL = new G4LogicalVolume(silicaS, fMatSilica, "silicaL");
    silicaL->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 1.0)));
    G4RotationMatrix* rotSilica = new G4RotationMatrix();
    rotSilica->rotateY(180.*deg); 
    new G4PVPlacement(rotSilica, G4ThreeVector(0, 3.0*cm, z_Silica), silicaL, "silicaP", worldL, false, 0, checkOverlaps);

    // --- 7. NaI 4 検出器 ---
    auto nai4S = new G4Tubs("nai4S", 0., fNaI4R, fNaI4HL, 0., 360.*deg);
    fNaILogic = new G4LogicalVolume(nai4S, fMatNaI, "nai4L");
    fNaILogic->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 0.6)));
    G4RotationMatrix* rotNaI = new G4RotationMatrix();
    rotNaI->rotateY(90.*deg); 
    new G4PVPlacement(rotNaI, G4ThreeVector(0, 8.0*cm, z_NaI), fNaILogic, "nai4P", worldL, false, 0, checkOverlaps);

    // --- 8. 鉛の遮蔽壁 (色の追加と描画のチラつき防止) ---
    G4double shieldX = 40*cm; 
    G4double shieldY = 20*cm; 
    G4double shieldT = 5.0*cm; 
    
    // 共通の見た目（半透明のグレー）
    auto* leadVis = new G4VisAttributes(G4Colour(0.2, 0.2, 0.2, 0.4));

    auto shieldS = new G4Box("shieldS", shieldX/2, shieldY/2, shieldT/2);
    auto shieldL = new G4LogicalVolume(shieldS, fMatLead, "shieldL");
    shieldL->SetVisAttributes(leadVis); // 正面・背面
    
    // 正面
    new G4PVPlacement(nullptr, G4ThreeVector(0, shieldY/2, Z_Pb_front + shieldT/2), shieldL, "shieldP1", worldL, false, 0, checkOverlaps);
    // 背面
    new G4PVPlacement(nullptr, G4ThreeVector(0, shieldY/2, z_Pb2_front - shieldT/2), shieldL, "shieldP2", worldL, false, 0, checkOverlaps);

    // 右側
    G4double sideLength = Z_Pb_front - z_Pb2_front; 
    auto sideS = new G4Box("sideS", shieldT/2, shieldY/2, sideLength/2);
    auto sideLV = new G4LogicalVolume(sideS, fMatLead, "sideLV");
    sideLV->SetVisAttributes(leadVis); // 【追加】右壁にも色を設定
    new G4PVPlacement(nullptr, G4ThreeVector(shieldX/2, shieldY/2, z_SupportCenter), sideLV, "shieldP_Right", worldL, false, 0, checkOverlaps);

    // 天井
    G4double topLength = (Z_Pb_front + shieldT) - (z_Pb2_front - shieldT); 
    auto topS = new G4Box("topS", shieldX/2, shieldT/2, topLength/2);
    auto topL = new G4LogicalVolume(topS, fMatLead, "topL");
    topL->SetVisAttributes(leadVis); // 【追加】天井にも色を設定

    // 【修正】チラつき防止のため、y座標を 0.1mm だけ上にずらす
    new G4PVPlacement(nullptr, G4ThreeVector(0, shieldY + shieldT/2 + 0.1*mm, z_SupportCenter), topL, "shieldP_Top", worldL, false, 0, checkOverlaps);
    
    return worldP;
}