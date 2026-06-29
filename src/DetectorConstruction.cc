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

DetectorConstruction::DetectorConstruction(int mode, G4double silicaZ_mm)
: fSilicaR(3.*cm), fSilicaHL(5.*cm),
  fNaISX(2.5*cm), fNaISY(2.5*cm), fNaISZ(10.*cm),
  fGap(1.*mm),
  fMode(mode),
  fPlasticHL(0.15*mm), fPlasticR(15.*mm),
  fNa22Z(0.*mm),
  fPlasticZ(10.*mm),
  fSilicaHLBox(10.*mm), fSilicaZ(silicaZ_mm*mm) {}

G4VPhysicalVolume* DetectorConstruction::Construct() {
  auto* nist = G4NistManager::Instance();

  // Materials
  fMatAir = nist->FindOrBuildMaterial("G4_AIR");
  fMatNaI = nist->FindOrBuildMaterial("G4_SODIUM_IODIDE");

  // Powdered silica: make custom density (e.g., 0.5 g/cm3)
  auto* Si = nist->FindOrBuildElement("Si");
  auto* O  = nist->FindOrBuildElement("O");
  G4double rho = 0.5*g/cm3; // editable later
  fMatSiO2 = new G4Material("SiO2_powder", rho, 2);
  fMatSiO2->AddElement(Si,1);
  fMatSiO2->AddElement(O,2);

  fMatPlastic = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

  // World
  auto worldS = new G4Box("worldS", 50*cm, 50*cm, 50*cm);
  auto worldL = new G4LogicalVolume(worldS, fMatAir, "worldL");
  auto worldP = new G4PVPlacement(nullptr, {}, worldL, "worldP", nullptr, false, 0);

  // World の可視化属性を透明にする
  auto worldVis = new G4VisAttributes();
  worldVis->SetVisibility(false);  // 完全に非表示
  worldL->SetVisAttributes(worldVis);

  if (fMode == 1) {
    // Mode 1:
    //   Na22 source  at z = 0  mm  (point, +z direction)
    //   Plastic disk at z = 10 mm  (0.3 mm thick, r=15 mm)
    //   Silica box   at z = 50 mm  (2x2x2 cm cube, 3 cm gap after plastic back face)

    // Plastic disk
    auto plasticS = new G4Tubs("plasticS", 0., fPlasticR, fPlasticHL, 0., 360*deg);
    fPlasticLogic = new G4LogicalVolume(plasticS, fMatPlastic, "plasticL");
    auto plasticVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
    plasticVis->SetForceSolid(true);
    fPlasticLogic->SetVisAttributes(plasticVis);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fPlasticZ),
                      fPlasticLogic, "plasticP", worldL, false, 0);

    // Silica box 20x20x20 mm
    auto silicaS = new G4Box("silicaS", fSilicaHLBox, fSilicaHLBox, fSilicaHLBox);
    fSilicaLogic = new G4LogicalVolume(silicaS, fMatSiO2, "silicaL");
    auto silicaVis = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    silicaVis->SetForceSolid(true);
    fSilicaLogic->SetVisAttributes(silicaVis);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fSilicaZ),
                      fSilicaLogic, "silicaP", worldL, false, 0);

    G4cout << "Mode 1: Na22 source at z=" << fNa22Z/mm << " mm, pointing +z" << G4endl;
    G4cout << "Mode 1: plastic disk   at z=" << fPlasticZ/mm
           << " mm, thickness=" << 2*fPlasticHL/mm << " mm, R=" << fPlasticR/mm << " mm" << G4endl;
    G4cout << "Mode 1: silica box     at z=" << fSilicaZ/mm
           << " mm, side=" << 2*fSilicaHLBox/mm << " mm" << G4endl;

  } else if (fMode == 3) {
    // Mode 3: full chain  Na22 → plastic → silica(Ps) → NaI
    // plastic
    auto plasticS = new G4Tubs("plasticS", 0., fPlasticR, fPlasticHL, 0., 360*deg);
    fPlasticLogic = new G4LogicalVolume(plasticS, fMatPlastic, "plasticL");
    auto plasticVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
    plasticVis->SetForceSolid(true);
    fPlasticLogic->SetVisAttributes(plasticVis);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fPlasticZ),
                      fPlasticLogic, "plasticP", worldL, false, 0);

    // silica box
    auto silicaS3 = new G4Box("silicaS", fSilicaHLBox, fSilicaHLBox, fSilicaHLBox);
    fSilicaLogic = new G4LogicalVolume(silicaS3, fMatSiO2, "silicaL");
    auto silicaVis3 = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    silicaVis3->SetForceSolid(true);
    fSilicaLogic->SetVisAttributes(silicaVis3);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fSilicaZ),
                      fSilicaLogic, "silicaP", worldL, false, 0);

    // NaI (+x side of silica)
    auto naiS3 = new G4Box("naiS", fNaISX, fNaISY, fNaISZ);
    fNaILogic = new G4LogicalVolume(naiS3, fMatNaI, "naiL");
    auto naiVis3 = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
    naiVis3->SetForceSolid(true);
    fNaILogic->SetVisAttributes(naiVis3);
    G4double xNaI = fSilicaHLBox + fGap + fNaISX;
    new G4PVPlacement(nullptr, G4ThreeVector(xNaI, 0, fSilicaZ),
                      fNaILogic, "naiP", worldL, false, 0);

    G4cout << "Mode 3: plastic    at z=" << fPlasticZ/mm
           << " mm, thickness=" << 2*fPlasticHL/mm << " mm" << G4endl;
    G4cout << "Mode 3: silica     at z=" << fSilicaZ/mm
           << " mm, side=" << 2*fSilicaHLBox/mm << " mm" << G4endl;
    G4cout << "Mode 3: NaI        at x=" << xNaI/mm
           << " mm, z=" << fSilicaZ/mm << " mm" << G4endl;

  } else {
    // Mode 2 (default): silica + NaI for Ps lifetime study
    auto silicaS = new G4Tubs("silicaS", 0., fSilicaR, fSilicaHL, 0., 360*deg);
    auto silicaL = new G4LogicalVolume(silicaS, fMatSiO2, "silicaL");
    new G4PVPlacement(nullptr, {}, silicaL, "silicaP", worldL, false, 0);

    // NaI block 5×5×20 cm^3, center at +x right next to cylinder (+ small gap)
    auto naiS = new G4Box("naiS", fNaISX, fNaISY, fNaISZ);
    fNaILogic = new G4LogicalVolume(naiS, fMatNaI, "naiL");

    auto vis = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
    vis->SetForceSolid(true);
    fNaILogic->SetVisAttributes(vis);

    G4double x = fSilicaR + fGap + fNaISX;
    new G4PVPlacement(nullptr, G4ThreeVector(x,0,0), fNaILogic, "naiP", worldL, false, 0);

    G4cout << "Mode 2: NaI placed at x=" << x/cm << " cm, half-length="
           << fNaISZ/cm << " cm" << G4endl;
  }

  return worldP;
}
