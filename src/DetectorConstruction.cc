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

DetectorConstruction::DetectorConstruction()
: fSilicaR(3.*cm), fSilicaHL(5.*cm),
  fNaISX(2.5*cm), fNaISY(2.5*cm), fNaISZ(10.*cm),
  fGap(1.*mm) {}

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

  // Silica cylinder (axis along z)
  auto silicaS = new G4Tubs("silicaS", 0., fSilicaR, fSilicaHL, 0., 360*deg);
  auto silicaL = new G4LogicalVolume(silicaS, fMatSiO2, "silicaL");
  new G4PVPlacement(nullptr, {}, silicaL, "silicaP", worldL, false, 0);

  // NaI block 5×5×20 cm^3, center at +x right next to cylinder (+ small gap)
  auto naiS = new G4Box("naiS", fNaISX, fNaISY, fNaISZ);
  fNaILogic = new G4LogicalVolume(naiS, fMatNaI, "naiL");

  // NaI visualization
  auto vis = new G4VisAttributes(G4Colour(0.0,0.0,1.0)); // 青
  vis->SetForceSolid(true);
  fNaILogic->SetVisAttributes(vis);

  // NaI position
  G4double x = fSilicaR + fGap + fNaISX; // touching with gap
  new G4PVPlacement(nullptr, G4ThreeVector(x,0,0), fNaILogic, "naiP", worldL, false, 0);

  G4cout << "NaI placed at x=" << x/cm << " cm, half-length="
	 << fNaISZ/cm << " cm" << G4endl;

  return worldP;
}
