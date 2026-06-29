#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH
#include <G4VUserDetectorConstruction.hh>
#include <G4ThreeVector.hh>
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;

class DetectorConstruction : public G4VUserDetectorConstruction {
 public:
  // silicaZ_mm: Mode 1/3 でのシリカ中心z座標 [mm]。デフォルト50mm (gap≈30mm)
  explicit DetectorConstruction(int mode = 2, G4double silicaZ_mm = 50.);
  ~DetectorConstruction() override = default;
  G4VPhysicalVolume* Construct() override;

  G4LogicalVolume* GetNaILogic()     const { return fNaILogic; }
  G4LogicalVolume* GetPlasticLogic() const { return fPlasticLogic; }
  G4LogicalVolume* GetSilicaLogic()  const { return fSilicaLogic; }
  G4double GetNa22Z()    const { return fNa22Z; }
  G4double GetPlasticZ() const { return fPlasticZ; }
  G4double GetSilicaZ()  const { return fSilicaZ; }

 private:
  // Parameters
  G4double fSilicaR;    // 3 cm
  G4double fSilicaHL;   // 5 cm half-length (total 10 cm)
  G4double fNaISX;      // 2.5 cm half-length (5 cm)
  G4double fNaISY;      // 2.5 cm half-length (5 cm)
  G4double fNaISZ;      // 10 cm half-length (20 cm)
  G4double fGap;        // 1 mm gap between cylinder and NaI

  // Materials
  G4Material* fMatAir{};
  G4Material* fMatSiO2{};
  G4Material* fMatNaI{};
  G4Material* fMatPlastic{};

  int fMode;

  // Mode 1 parameters (all in Geant4 units = mm)
  G4double fPlasticHL;     // half-thickness of plastic (0.15 mm)
  G4double fPlasticR;      // radius of plastic disk (15 mm)
  G4double fNa22Z;         // z of Na22 source (0 mm)
  G4double fPlasticZ;      // z of plastic center (10 mm)
  G4double fSilicaHLBox;   // half-side of silica cube (10 mm → 2cm cube)
  G4double fSilicaZ;       // z of silica center (50 mm)

  // Logical volume handles
  G4LogicalVolume* fNaILogic{};
  G4LogicalVolume* fPlasticLogic{};
  G4LogicalVolume* fSilicaLogic{};
};
#endif
