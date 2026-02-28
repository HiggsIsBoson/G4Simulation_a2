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

  // エネルギー集計のために必要です
  G4LogicalVolume* GetNaILogic() const { return fNaILogic; }

 private:
  // --- パラメータ (ノートの計測値に基づきます) ---
  G4double fSilicaR;    // ガラス管の半径 (1.5 cm)
  G4double fSilicaHL;   // ガラス管の半分の長さ
  G4double fNaI4R;      // NaI 4 の半径 (3.0 cm)
  G4double fNaI4HL;     // NaI 4 の半分の長さ (8.75 cm)

  // --- マテリアル ---
  G4Material* fMatAir{};
  G4Material* fMatSiO2{};
  G4Material* fMatNaI{};
  G4Material* fMatTable; // 追加
  G4Material* fMatLead;  // 追加

  // --- 論理ボリューム (エネルギー計測対象) ---
  G4LogicalVolume* fNaILogic{};
};

#endif