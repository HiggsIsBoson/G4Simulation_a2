#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH
#include <G4VUserDetectorConstruction.hh>
#include <G4ThreeVector.hh>
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;

class DetectorConstruction : public G4VUserDetectorConstruction {
 public:
  DetectorConstruction();
  ~DetectorConstruction() override = default;
  G4VPhysicalVolume* Construct() override;

  G4LogicalVolume* GetNaILogic() const { return fNaILogic; }

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

  // Keep a handle to NaI logical
  G4LogicalVolume* fNaILogic{};
};
#endif
