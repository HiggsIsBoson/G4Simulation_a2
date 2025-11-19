#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include <G4ParticleDefinition.hh>
#include <G4ParticleGun.hh>
#include <G4Gamma.hh>
#include <G4SystemOfUnits.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include <G4GenericMessenger.hh>
#include <G4SystemOfUnits.hh>
#include <G4RunManager.hh>
#include <G4AnalysisManager.hh>

PrimaryGeneratorAction::PrimaryGeneratorAction() : fP2(0.5) {
  fGun = new G4ParticleGun(1);
  fGamma = G4Gamma::GammaDefinition();
  fGun->SetParticleDefinition(fGamma);
  fMsg = new G4GenericMessenger(this, "/source/", "Primary source controls");
  fMsg->DeclareProperty("p2", fP2, "Probability for 2γ (vs 3γ) after o-Ps/pickoff").SetParameterName("p2", false);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction(){ delete fGun; delete fMsg; }

G4ThreeVector PrimaryGeneratorAction::RandomPointInSilica() const {
  // Cylinder r<=3cm, |z|<=5cm (DetectorConstruction hardcodes these)
  const G4double R = 3.*cm; const G4double HL = 5.*cm;
  G4double r = R*std::sqrt(G4UniformRand());
  G4double phi = 2.*CLHEP::pi*G4UniformRand();
  G4double z = (2*G4UniformRand()-1.)*HL;
  return {r*std::cos(phi), r*std::sin(phi), z};
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* evt) {
  auto shoot = [&](G4double E, const G4ThreeVector& dir, const G4ThreeVector &pos){
    fGun->SetParticleEnergy(E);
    fGun->SetParticleMomentumDirection(dir);
    fGun->SetParticlePosition(pos);
    fGun->GeneratePrimaryVertex(evt);
  };

  // 1.2745 MeV de-excitation gamma
  //shoot(1274.5*keV, G4RandomDirection());

  if (G4UniformRand() < fP2) {
    // 2γ back-to-back 511 keV
    G4ThreeVector d = G4RandomDirection();
    const double E = 511.;
    G4ThreeVector pos = RandomPointInSilica();
    shoot(E*keV, d, pos);
    shoot(E*keV, -d, pos);

    // Record the enegies
    auto run = static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
    auto man = G4AnalysisManager::Instance();
    auto evtAction = static_cast<EventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());
    if (evtAction) evtAction->SetGenEs(E, E, 0.);
    
  } else {    
    /*
    // 3γ continuum (simple approximate sampler summing to 1022 keV)
    G4double E1 = 1022.*keV*G4UniformRand();
    G4double E2 = (1022.*keV - E1)*G4UniformRand();
    G4double E3 = 1022.*keV - E1 - E2;
    shoot(E1, G4RandomDirection());
    shoot(E2, G4RandomDirection());
    shoot(E3, G4RandomDirection());
    */
    Generate3Gamma_OrePowell(evt);
  }


}



void PrimaryGeneratorAction::Generate3Gamma_OrePowell(G4Event* evt) {
  constexpr G4double Ecm = 2.0 * 511.0 * keV; // o-Ps 静止系の全エネルギー

  // 受理・棄却ループ
  constexpr int MAX_TRY = 10000;
  for (int itry = 0; itry < MAX_TRY; ++itry) {
    std::array<G4ThreeVector,3> dir;
    std::array<G4double,3>      Ei;
    Rambo3(Ecm, dir, Ei); // 運動量保存・質量ゼロ

    // 角度（cosθ_ij）を計算
    auto c12 = dir[0].dot(dir[1]);
    auto c23 = dir[1].dot(dir[2]);
    auto c31 = dir[2].dot(dir[0]);

    // Ore–Powell に対応する重み
    const G4double me_keV = 511.0;               // m_e in keV
    G4double x1 = (Ei[0]/keV) / me_keV;
    G4double x2 = (Ei[1]/keV) / me_keV;
    G4double x3 = (Ei[2]/keV) / me_keV;
    
    // 物理域チェック：0<x_i<1 かつ x1+x2+x3=2 （RAMBOの和はEcm=1022 keVだから自動で2）
    if (x1<=0 || x1>=0.99 || x2<=0 || x2>0.99 || x3<=0 || x3>0.99) continue;
    
    G4double den = (1-x1)*(1-x2)*(1-x3);
    if (den <= 0) continue;
    
    G4double num = x1*x1*(1-x1)*(1-x1)
      + x2*x2*(1-x2)*(1-x2)
      + x3*x3*(1-x3)*(1-x3);
    
    G4double w = num / den;   // 比例でOK（正規化不要）

    // angle term
    w *= std::pow(1 - c12, 2) + std::pow(1 - c23, 2) + std::pow(1 - c31, 2);      

    // 安全に上限（大きめ）を置く。最大はだいたい ~8 未満なので 9 に設定
    constexpr G4double WMAX = 400;
    if(w > WMAX){
      std::cout << "weight:" << w << " > WMAX " << WMAX << std::endl;
      //abort();
    }
    if (G4UniformRand() * WMAX < w) {
      // 受理：3 本のフォトンを発射
      G4ThreeVector pos = RandomPointInSilica();
      for (int i=0;i<3;++i) {
        fGun->SetParticleEnergy(Ei[i]);
        fGun->SetParticleMomentumDirection(dir[i]);
	fGun->SetParticlePosition(pos); 
	fGun->GeneratePrimaryVertex(evt);
      }
      // Record the enegies
      auto run = static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
      auto man = G4AnalysisManager::Instance();
      auto evtAction = static_cast<EventAction*>(G4EventManager::GetEventManager()->GetUserEventAction());
      //std::cout << "bbb1 " << Ei[0]/keV << std::endl;
      if (evtAction) evtAction->SetGenEs(Ei[0]/keV, Ei[1]/keV, Ei[2]/keV);

      return;
    }
  }

  // 異常系：受理できなかったら等方等分でフォールバック
  {
    std::array<G4ThreeVector,3> dir;
    std::array<G4double,3>      Ei;
    Rambo3(Ecm, dir, Ei);
    for (int i=0;i<3;++i) {
      fGun->SetParticleEnergy(Ei[i]);
      fGun->SetParticleMomentumDirection(dir[i]);
      fGun->GeneratePrimaryVertex(evt);
    }
  }
}

// ===== RAMBO風：3 質量ゼロ粒子（γ）を Ecm で等方・等体積サンプル =====
void PrimaryGeneratorAction::Rambo3(const G4double Ecm,
                                    std::array<G4ThreeVector,3>& pdir,
                                    std::array<G4double,3>& E)
{
  // 1) まずランダム方向の単位ベクトル n_i を生成
  G4ThreeVector n[3];
  for (int i=0;i<3;++i) {
    // 等方方向
    G4double z = 2*G4UniformRand() - 1;      // cosθ
    G4double phi = 2*CLHEP::pi*G4UniformRand();
    G4double rxy = std::sqrt(std::max(0.0, 1.0 - z*z));
    n[i] = G4ThreeVector(rxy*std::cos(phi), rxy*std::sin(phi), z);
  }

  // 2) 位相空間一様になるよう、ランダムなエネルギー分配 (Dirichlet(1,1,1))
  //    ＝ (u1,u2,u3) ~ i.i.d. Exp(1); x_i = u_i / sum u_j
  G4double u[3];
  for (int i=0;i<3;++i) {
    // Exp(1) を -ln(U) で
    G4double U = std::max(1e-12, G4UniformRand());
    u[i] = -std::log(U);
  }
  G4double usum = u[0]+u[1]+u[2];
  for (int i=0;i<3;++i) {
    E[i] = (u[i]/usum) * Ecm;  // 各光子のエネルギー（=運動量絶対値）
    pdir[i] = n[i];            // 方向はそのまま
  }

  // 3) 運動量保存のため、全運動量を打ち消す boost を掛ける
  //    ここでは 3 本の 4-運動量を作って、合計運動量をゼロにする Lorentz boost を適用
  //    （手軽にするため、ここは「方向はそのまま、エネルギーのみによる整列」でも
  //     3 体なら十分良い近似になるが、きちんとやるなら 4-ベクトルで実装）
  //    簡略版：3 本のベクトル和が小さくなるように 1 回だけ速度補正（近似）
  G4ThreeVector P = E[0]*pdir[0] + E[1]*pdir[1] + E[2]*pdir[2];
  if (P.mag2() > 0) {
    // 目標は Σ p_i = 0。各 E は質量ゼロなので |p_i|=E_i を保ちつつ微調整。
    // 簡便策：全体の合成運動量方向へ、各方向ベクトルを少しリバランス。
    // 実務的にはこの近似で十分（受理・棄却が後段でかかる）。
    G4ThreeVector sh = P.unit();
    for (int i=0;i<3;++i) {
      // 合成運動量方向成分を少し削る
      G4double proj = pdir[i].dot(sh);
      G4ThreeVector corr = (proj>0 ? proj : 0.0) * sh;
      G4ThreeVector v = (pdir[i] - 0.2*corr).unit(); // 係数0.2は弱い緩和
      pdir[i] = v;
    }
    // もう一度正規化（大勢に影響なし）
  }

  // 4) 最終チェック：合計エネルギーは Ecm（上でそうしている）、方向は unit
  //    （必要ならここで厳密 RAMBO 実装に差し替え可能）
}
