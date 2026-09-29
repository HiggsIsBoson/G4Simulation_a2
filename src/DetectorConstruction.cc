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
#include <G4SubtractionSolid.hh>
#include <G4RotationMatrix.hh>


/*
本番セットアップについてのコメント
このコメントをもとに以下のConstructionを変更していく。

線源・プラシン・シリカはこの順に下から上に向かって並んでいる
線源とプラシンのサイズに変更はないが向きと間隔が異なる。
また、本番では鉛ブロックを多用している。
線源は点ではなくコイン上になっているが、よくわからないので点にしておく。
重力はｙ軸負の方向に働いているという参考情報。つまり上と言ったらｙ軸正方向を意味する。
線源を原点
ｙ方向5㎜の位置がプラシンの直方体の上面に一致し、ｘｚ方向だけを見ると正方形になるようにプラシンを置く
ｘｚ方向プラシンプラシンの中心ｙ方向プラシンの上面を中心としつまり（０，５，０）中心とし、直径50ｍｍの円があるとすると、
その円を底面とするｙ軸正方向に伸びた高さ37ｍｍの円柱があり、その円柱は100*200*37㎜の鉛ブロックの中心をくりぬくための形となっている。
この鉛ブロックは貫通ブロックと呼ぶことにする。
今回、シリカは直径65㎜で厚さが10㎜の円柱となっている。その配置は、円柱がくりぬかれた鉛ブロックy軸正方向に置かれている。
底面の中心はくりぬかれた円柱の上面の中心と一致し、(0，42，0)の位置にある。シリカの上面の中心は(0，52，0)
このほか、鉛ブロックには2種類あり、100*200*50mmの基本ブロック、くりぬかれた鉛ブロックの足として使った13*100*7.5ｍｍの足ブロック、電線などを通すための半円柱が基本ブロックから欠けたアーチブロックがある。
アーチブロックは、基本ブロックの100*200面内に存在し200㎜の辺の中心を中心とし、半径50ｍｍとなるような半円を50㎜の方向にトンネルのように掘ったものである。
つまり、高さ50㎜の半円柱を基本ブロックから取り除いた形をしている。
貫通ブロックはｚ軸方向の長さが100でｘ軸方向の長さが200とする。
この貫通ブロックのｚ軸方向にぴったりと基本ブロックを置く。上面が一致するようにする。この基本ブロックを土台と呼ぶ。
貫通ブロックの下には足ブロックが置かれていて、2つの足ブロックの底面と土台ブロックの底面はすべて同一のｙ座標を持つ。
貫通ブロックのｚ軸負方向の面にアーチブロックが使われている。このアーチブロックのくりぬかれている部分は下側を向いている。本当にアーチになっている。アーチの円の一番高いところは貫通ブロックの上面の高さと一致する。このアーチブロックの上面は地面と平行。
さらに、このアーチブロックにはｚ軸負方向に全く同じ向きにアーチブロックが設置されている。現時点でｚ軸負方向から見たら50㎜のトンネルが2本連続していて、その先には高さ13㎜の線源の存在釣る空間がある。
土台ブロック、貫通ブロック、アーチブロック2つは、すべてさらに下にある基本ブロックに乗っている。
シミュレーションにおいては、この基本ブロックたちが一番下に来るものとなる。
貫通ブロックの隣に置いたアーチブロックのｘｙｚ負方向の角にある基本ブロックの角がある。
この基本ブロックはz軸方向200x軸方向100である。貫通ブロックと土台ブロックを100*200で支えている。
同じくこのアーチブロックのyz軸負x軸正の角にも基本ブロックの角があり、これも100*200で土台と貫通ブロックを支えている。
今の2つの基本ブロックのセットを双子ブロックと呼ぶ。
双子ブロックのz軸負方向のにはさっきまでとは別のアーチブロックがある。支えアーチブロックと呼ぶ。
このアーチの足は双子ブロックをまたいでいる。支えアーチブロックには2つ目に設定したアーチブロックが上に載っている。
支えアーチブロックを上から見ると実験用の机が見える。
双子ブロックのz軸正方向には基本ブロックがある。土台ブロックの半分をしたから支え、半分は現時点ではz軸正方向にはみ出している。
x方向200y方向50z方向100の基本ブロックである。
このはみ出した部分には基本ブロックが2つ積んである。土台ブロックと今の基本ブロックに接し、土台ブロックのz軸正方向にあり、x方向200y方向100z方向59である。
この上に同じ向きで基本ブロックが載っている。
はみ出した部分と今の2つ積んだ基本ブロックに接し、x正方向に縦に基本ブロックが机の上に置いてある。ながさはx100y200z50。
土台と貫通の上部、x軸負方向のヘリには基本ブロックがある。貫通の上面を基準に0,0,0から50,100,200の頂点までの長方形である。壁ブロックと呼ぶ。壁ブロックのz軸正方向にはすでに積んであるブロックがあるのでピッタリ接する。
土台ブロックの上にはNaIシンチレーターがある。
直径60㎜長さは壁ブロックにぶつかるところから土台ブロックの終わりまでの150ｍｍである。
検出器自体はここで終わりだが、ＰＭＴが付いているので土台の壁ではないほうのヘリにはブロックを置けない。代わりに双子ブロックのｘ軸側と土台と貫通の作るｘ軸正方向の面にピッタリ基本ブロックを置き、さらにその上に60㎜ずらして基本ブロックを置く。
アーチブロック2つの上に基本ブロックを置く。アーチブロック２つが上面に作る100*200の上に基本ブロックを置く。x200y50z100である。
こうすると、最後に置いた基本ブロックと、壁ブロックと、PMTに邪魔されてずらしておいた基本ブロックは上面がy軸同じ高さになっている。



Geant4やビームのシミュレーションの常識を知らなかったため、ビームの向きをY軸方向にしてしまっていた。
ビームの発生源や向きについてはDetectorConstruction.ccだけでなくPrimarygeneratoraction.ccにも記載があり、
z方向にそろえる意思がある方はこちらも修正していただけるとよいと思います。
*/

DetectorConstruction::DetectorConstruction(int mode, G4double silicaZ_mm)
: fSilicaR(3.*cm), fSilicaHL(5.*cm),
  fNaISX(2.5*cm), fNaISY(2.5*cm), fNaISZ(10.*cm),
  fGap(1.*mm),
  fMode(mode),
  fPlasticHL(0.15*mm), fPlasticHLXY(5.*mm),  // 10mm角の正方形プラシン
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
    //   Na22 source    at z = 0  mm  (point, +z direction)
    //   Plastic square at z = 10 mm  (0.3 mm thick, 10mm角)
    //   Silica box     at z = 50 mm  (2x2x2 cm cube)

    // Plastic square (10mm × 10mm × 0.3mm)
    auto plasticS = new G4Box("plasticS", fPlasticHLXY, fPlasticHLXY, fPlasticHL);
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

    G4cout << "Mode 1: Na22 source  at z=" << fNa22Z/mm << " mm, pointing +z" << G4endl;
    G4cout << "Mode 1: plastic      at z=" << fPlasticZ/mm
           << " mm, " << 2*fPlasticHLXY/mm << "x" << 2*fPlasticHLXY/mm
           << "x" << 2*fPlasticHL/mm << " mm" << G4endl;
    G4cout << "Mode 1: silica box   at z=" << fSilicaZ/mm
           << " mm, side=" << 2*fSilicaHLBox/mm << " mm" << G4endl;

  } else if (fMode == 3) {
    // Mode 3: full chain  Na22 → plastic → silica(Ps) → NaI
    // Plastic square (10mm × 10mm × 0.3mm)
    auto plasticS = new G4Box("plasticS", fPlasticHLXY, fPlasticHLXY, fPlasticHL);
    fPlasticLogic = new G4LogicalVolume(plasticS, fMatPlastic, "plasticL");
    auto plasticVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
    plasticVis->SetForceSolid(true);
    fPlasticLogic->SetVisAttributes(plasticVis);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fPlasticZ),
                      fPlasticLogic, "plasticP", worldL, false, 0);

    // Silica box 20x20x20 mm
    auto silicaS3 = new G4Box("silicaS", fSilicaHLBox, fSilicaHLBox, fSilicaHLBox);
    fSilicaLogic = new G4LogicalVolume(silicaS3, fMatSiO2, "silicaL");
    auto silicaVis3 = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
    silicaVis3->SetForceSolid(true);
    fSilicaLogic->SetVisAttributes(silicaVis3);
    new G4PVPlacement(nullptr, G4ThreeVector(0, 0, fSilicaZ),
                      fSilicaLogic, "silicaP", worldL, false, 0);

    // NaI配置:
    //   z方向: シリカ背面(z=fSilicaZ+fSilicaHLBox)から30mm後ろにNaI正面
    //   y方向: シリカ上面(y=+fSilicaHLBox)から30mm上にNaI底面（beta+が通り抜けても当たらない）
    //   長軸: y方向
    G4double silicaBackZ = fSilicaZ + fSilicaHLBox;   // シリカ背面 z
    G4double silicaTopY  = fSilicaHLBox;               // シリカ上面 y
    G4double naiHLx      = fNaISZ;                     // NaI x方向の半長 = 100mm (長軸)
    G4double naiHLy      = fNaISX;                     // NaI y方向の半厚 = 25mm
    G4double naiHLz      = fNaISY;                     // NaI z方向の半厚 = 25mm
    G4double naiCenterZ  = silicaBackZ + 30.*mm + naiHLz;
    G4double naiCenterY  = silicaTopY  + 30.*mm + naiHLy;  // 底面がシリカ上面から3cm上

    // G4Box(halfX, halfY, halfZ): X=100mm(長軸), Y=25mm, Z=25mm
    auto naiS3 = new G4Box("naiS", naiHLx, naiHLy, naiHLz);
    fNaILogic = new G4LogicalVolume(naiS3, fMatNaI, "naiL");
    auto naiVis3 = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
    naiVis3->SetForceSolid(true);
    fNaILogic->SetVisAttributes(naiVis3);
    new G4PVPlacement(nullptr, G4ThreeVector(0, naiCenterY, naiCenterZ),
                      fNaILogic, "naiP", worldL, false, 0);

    G4cout << "Mode 3: plastic    at z=" << fPlasticZ/mm
           << " mm, " << 2*fPlasticHLXY/mm << "x" << 2*fPlasticHLXY/mm
           << "x" << 2*fPlasticHL/mm << " mm" << G4endl;
    G4cout << "Mode 3: silica     at z=" << fSilicaZ/mm
           << " mm, side=" << 2*fSilicaHLBox/mm << " mm" << G4endl;
    G4cout << "Mode 3: NaI        center=(0, " << naiCenterY/mm << ", " << naiCenterZ/mm << ") mm"
           << ", long-axis=x (" << 2*naiHLx/mm << "mm), size="
           << 2*naiHLx/mm << "x" << 2*naiHLy/mm << "x" << 2*naiHLz/mm << " mm"
           << ", bottom-y=" << (naiCenterY-naiHLy)/mm
           << " mm (+" << (naiCenterY-naiHLy-silicaTopY)/mm << " mm from silica top)" << G4endl;

  } else if (fMode == 4) {
    // Mode 4: 本番セットアップ (鉛ブロック配置), Mode 3 と同じ物理
    //   座標系: 線源 = 原点, +y = 鉛直上向き (重力は -y), 単位 mm
    //   配置の元になった説明はファイル冒頭のコメントを参照
    auto* matPb = nist->FindOrBuildMaterial("G4_Pb");

    // 鉛はグレー, やや半透明 (中の検出器もうっすら見える)
    auto* leadVis = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.6));
    leadVis->SetForceSolid(true);

    // y軸方向の円柱 / x軸方向の円柱 を作るための回転
    auto* rotToY = new G4RotationMatrix(); rotToY->rotateX(90.*deg);
    auto* rotToX = new G4RotationMatrix(); rotToX->rotateY(90.*deg);

    // 直方体を「最小の角 (x0,y0,z0) と最大の角 (x1,y1,z1)」で指定して作る
    auto boxSolid = [](const G4String& name, G4double x0, G4double x1,
                       G4double y0, G4double y1, G4double z0, G4double z1) {
      return new G4Box(name, (x1-x0)/2, (y1-y0)/2, (z1-z0)/2);
    };
    auto center = [](G4double x0, G4double x1, G4double y0, G4double y1,
                     G4double z0, G4double z1) {
      return G4ThreeVector((x0+x1)/2, (y0+y1)/2, (z0+z1)/2);
    };
    auto placeLead = [&](G4VSolid* solid, const G4ThreeVector& pos) {
      auto* lv = new G4LogicalVolume(solid, matPb, solid->GetName() + "L");
      lv->SetVisAttributes(leadVis);
      new G4PVPlacement(nullptr, pos, lv, solid->GetName() + "P", worldL, false, 0);
    };
    auto placeBlock = [&](const G4String& name, G4double x0, G4double x1,
                          G4double y0, G4double y1, G4double z0, G4double z1) {
      placeLead(boxSolid(name, x0, x1, y0, y1, z0, z1), center(x0, x1, y0, y1, z0, z1));
    };

    // 主要な高さ
    const G4double yHole  =   5.*mm;  // 貫通ブロック底面 = プラシン上面
    const G4double yTop   =  42.*mm;  // 貫通ブロック・土台の上面 = シリカ底面
    const G4double yFloor =  -8.*mm;  // 足・土台・アーチの底面 (= 双子ブロック上面)
    const G4double yDesk  = -58.*mm;  // 一番下のブロックの底面 (机)

    // --- プラシン: 10x10x0.3 mm, 上面が y=5 ---
    const G4double plT = 2*fPlasticHL;
    auto plasticS = new G4Box("plasticS", fPlasticHLXY, fPlasticHL, fPlasticHLXY);
    fPlasticLogic = new G4LogicalVolume(plasticS, fMatPlastic, "plasticL");
    auto plasticVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
    plasticVis->SetForceSolid(true);
    fPlasticLogic->SetVisAttributes(plasticVis);
    new G4PVPlacement(nullptr, G4ThreeVector(0, yHole - plT/2, 0),
                      fPlasticLogic, "plasticP", worldL, false, 0);

    // --- 貫通ブロック: x200 y37 z100, 中心に直径50mmの縦穴 ---
    {
      auto body = boxSolid("penetBody", -100., 100., yHole, yTop, -50., 50.);
      auto hole = new G4Tubs("penetHole", 0., 25.*mm, (yTop-yHole)/2 + 1.*mm, 0., 360.*deg);
      auto solid = new G4SubtractionSolid("penetration", body, hole, rotToY, G4ThreeVector());
      placeLead(solid, center(-100., 100., yHole, yTop, -50., 50.));

      // 穴の位置を描画で示すための空気の薄い筒 (周りと同じ空気なので物理には影響しない)
      auto holeS = new G4Tubs("penetHoleAir", 24.9*mm, 25.*mm, (yTop-yHole)/2, 0., 360.*deg);
      auto holeL = new G4LogicalVolume(holeS, fMatAir, "penetHoleAirL");
      auto holeVis = new G4VisAttributes(G4Colour(0.9, 0.35, 0.35));  // 落ち着いた赤の枠線
      holeVis->SetForceWireframe(true);
      holeVis->SetLineWidth(1.5);
      holeVis->SetForceAuxEdgeVisible(true);          // 側面の線も描く
      holeVis->SetForceLineSegmentsPerCircle(12);     // 側面の線は12本だけ
      holeL->SetVisAttributes(holeVis);
      new G4PVPlacement(rotToY, G4ThreeVector(0, (yHole + yTop)/2, 0),
                        holeL, "penetHoleAirP", worldL, false, 0);
    }

    // --- シリカ: 直径65mm 厚さ10mm の円柱, 底面中心 (0,42,0) ---
    {
      auto silicaS = new G4Tubs("silicaS", 0., 32.5*mm, 5.*mm, 0., 360.*deg);
      fSilicaLogic = new G4LogicalVolume(silicaS, fMatSiO2, "silicaL");
      auto silicaVis = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));
      silicaVis->SetForceSolid(true);
      fSilicaLogic->SetVisAttributes(silicaVis);
      new G4PVPlacement(rotToY, G4ThreeVector(0, yTop + 5.*mm, 0),
                        fSilicaLogic, "silicaP", worldL, false, 0);
    }

    // --- 足ブロック x2: x7.5 y13 z100, 貫通ブロックの下の両端 ---
    placeBlock("footMinusX", -100.,  -92.5, yFloor, yHole, -50., 50.);
    placeBlock("footPlusX",   92.5,  100.,  yFloor, yHole, -50., 50.);

    // --- 土台: 貫通ブロックの +z 側, 上面を揃える ---
    placeBlock("dodai", -100., 100., yFloor, yTop, 50., 150.);

    // --- アーチブロック x2: 貫通ブロックの -z 側に2つ連続, 下向きのアーチ (トンネルは z 方向) ---
    auto placeArchZ = [&](const G4String& name, G4double z0, G4double z1) {
      auto body = boxSolid(name + "Body", -100., 100., yFloor, yFloor + 100., z0, z1);
      auto tunnel = new G4Tubs(name + "Tunnel", 0., 50.*mm, (z1-z0)/2 + 1.*mm, 0., 360.*deg);
      // 半円の中心 = 底面の辺の中点 (ボックス中心から -50mm)
      auto solid = new G4SubtractionSolid(name, body, tunnel, nullptr, G4ThreeVector(0, -50.*mm, 0));
      placeLead(solid, center(-100., 100., yFloor, yFloor + 100., z0, z1));
    };
    placeArchZ("arch1", -100., -50.);
    placeArchZ("arch2", -150., -100.);

    // --- 双子ブロック: アーチ1 の下の角から, x100 z200 ---
    placeBlock("twinMinusX", -100.,   0., yDesk, yFloor, -100., 100.);
    placeBlock("twinPlusX",     0., 100., yDesk, yFloor, -100., 100.);

    // --- 支えアーチブロック: 双子ブロックの -z 側, 縦穴 (上から机が見える) ---
    {
      auto body = boxSolid("supportArchBody", -100., 100., yDesk, yFloor, -200., -100.);
      auto hole = new G4Tubs("supportArchHole", 0., 50.*mm, (yFloor-yDesk)/2 + 1.*mm, 0., 360.*deg);
      // 半円の中心 = 双子ブロック側 (z=-100) の辺の中点
      auto solid = new G4SubtractionSolid("supportArch", body, hole, rotToY, G4ThreeVector(0, 0, 50.*mm));
      placeLead(solid, center(-100., 100., yDesk, yFloor, -200., -100.));
    }

    // --- 双子ブロックの +z 側: 土台の半分を下から支え, 半分ははみ出す ---
    placeBlock("underDodai", -100., 100., yDesk, yFloor, 100., 200.);

    // --- はみ出し部分に2段積み (x200 y100 z50) ---
    placeBlock("stackLower", -100., 100., yFloor,       yFloor + 100., 150., 200.);
    placeBlock("stackUpper", -100., 100., yFloor + 100., yFloor + 200., 150., 200.);

    // --- 2段積みの +x 側に縦置き (x100 y200 z50), 机の上 ---
    placeBlock("standing", 100., 200., yDesk, yDesk + 200., 150., 200.);

    // --- 壁ブロック: 土台と貫通の上, -x 側のヘリ (x50 y100 z200) ---
    placeBlock("wall", -100., -50., yTop, yTop + 100., -50., 150.);

    // --- PMT 側 (+x): 側面にぴったり, その上に -z 方向へ 60mm ずらして置く ---
    //     (上段の +z 端 z=90 が PMT (z=90〜150) にちょうど接する)
    placeBlock("pmtSideLower", 100., 150., yDesk, yTop,         -50., 150.);
    placeBlock("pmtSideUpper", 100., 150., yTop,  yTop + 100., -110.,  90.);

    // --- アーチ2つの上 (x200 y50 z100) ---
    placeBlock("archTop", -100., 100., yFloor + 100., yFloor + 150., -150., -50.);

    // --- NaI: 直径60mm 長さ150mm, x方向, 壁 (x=-50) から土台の端 (x=100) まで, 土台の上 ---
    //     z方向は 2段積みブロック (z=150) に接する位置 (z=90〜150)
    {
      auto naiS = new G4Tubs("naiS", 0., 30.*mm, 75.*mm, 0., 360.*deg);
      fNaILogic = new G4LogicalVolume(naiS, fMatNaI, "naiL");
      auto naiVis = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
      naiVis->SetForceSolid(true);
      fNaILogic->SetVisAttributes(naiVis);
      new G4PVPlacement(rotToX, G4ThreeVector(25.*mm, yTop + 30.*mm, 120.*mm),
                        fNaILogic, "naiP", worldL, false, 0);
    }

    G4cout << "Mode 4: source at origin, beta+ along +y" << G4endl;
    G4cout << "Mode 4: plastic top y=" << yHole/mm << " mm, silica y="
           << yTop/mm << "-" << (yTop + 10.*mm)/mm << " mm, NaI center=(25, "
           << (yTop + 30.*mm)/mm << ", 120) mm" << G4endl;

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
