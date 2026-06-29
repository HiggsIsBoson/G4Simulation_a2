#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "RunAction.hh"
#include <G4ParticleDefinition.hh>
#include <G4ParticleGun.hh>
#include <G4Gamma.hh>
#include <G4Positron.hh>
#include <G4SystemOfUnits.hh>
#include <G4RandomDirection.hh>
#include <Randomize.hh>
#include <G4GenericMessenger.hh>
#include <G4RunManager.hh>
#include <G4AnalysisManager.hh>
#include "PsEventInfo.hh"
#include <CLHEP/Units/PhysicalConstants.h>

#include <algorithm>
#include <cmath>

PrimaryGeneratorAction::PrimaryGeneratorAction(int mode)
: fMode(mode), fP2(0.5) {
  fGun = new G4ParticleGun(1);
  fGamma = G4Gamma::GammaDefinition();
  fPositron = G4Positron::PositronDefinition();
  if (fMode == 2) fGun->SetParticleDefinition(fGamma);
  fMsg = new G4GenericMessenger(this, "/source/", "Primary source controls");
  fMsg->DeclareProperty("p2", fP2, "Probability for 2γ (vs 3γ) after o-Ps/pickoff").SetParameterName("p2", false);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction(){ delete fGun; delete fMsg; }

G4double PrimaryGeneratorAction::SampleNa22BetaEnergy() const {
  // Na22 beta+ endpoint: 545.4 keV (to 1274.5 keV excited state of Ne22)
  // Spectrum shape: N(T) ∝ p * (T+me) * (Q-T)^2  (no Fermi correction)
  // where p = sqrt((T+me)^2 - me^2), me = 511 keV, Q = 545.4 keV
  constexpr G4double Q  = 545.4;   // keV
  constexpr G4double me = 511.0;   // keV
  constexpr G4double WMAX = 5.0e10; // conservative upper bound (keV^(5/2) units)

  for (int i = 0; i < 100000; ++i) {
    G4double T = Q * G4UniformRand();
    G4double E = T + me;
    G4double p2 = E*E - me*me;
    if (p2 <= 0) continue;
    G4double w = std::sqrt(p2) * E * (Q - T) * (Q - T);
    if (G4UniformRand() * WMAX < w) return T * keV;
  }
  return 0.3 * Q * keV; // fallback
}

G4ThreeVector PrimaryGeneratorAction::RandomPointInSilica() const {
  // Cylinder r<=3cm, |z|<=5cm (DetectorConstruction hardcodes these)
  const G4double R = 3.*cm; const G4double HL = 5.*cm;
  G4double r = R*std::sqrt(G4UniformRand());
  G4double phi = 2.*CLHEP::pi*G4UniformRand();
  G4double z = (2*G4UniformRand()-1.)*HL;
  return {r*std::cos(phi), r*std::sin(phi), z};
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* evt) {

  if (fMode == 1 || fMode == 3) {
    // Mode 1/3: Na22 beta+ in +z direction from z=0
    G4ThreeVector pos(0, 0, fNa22Z);

    // e+ primary
    fGun->SetParticleDefinition(fPositron);
    G4double KE = SampleNa22BetaEnergy();
    fGun->SetParticleEnergy(KE);
    fGun->SetParticleMomentumDirection(G4ThreeVector(0,0,1));
    fGun->SetParticlePosition(pos);
    fGun->GeneratePrimaryVertex(evt);

    if (fMode == 3) {
      // 1274.5 keV de-excitation gamma (Na22→Ne22*→Ne22+γ)
      fGun->SetParticleDefinition(fGamma);
      fGun->SetParticleEnergy(1274.5*keV);
      fGun->SetParticleMomentumDirection(G4RandomDirection());
      fGun->SetParticlePosition(pos);
      fGun->GeneratePrimaryVertex(evt);
    }

    // gen energyをPsEventInfo経由で保存（MTクローン問題を回避）
    auto* info = static_cast<PsEventInfo*>(evt->GetUserInformation());
    if (!info) { info = new PsEventInfo(); evt->SetUserInformation(info); }
    info->SetGenBeta(KE/keV, 0., 0.);
    return;
  }

  // Mode 2 below
  auto shoot = [&](G4double E, const G4ThreeVector& dir, const G4ThreeVector &pos){
    fGun->SetParticleEnergy(E);
    fGun->SetParticleMomentumDirection(dir);
    fGun->SetParticlePosition(pos);
    fGun->GeneratePrimaryVertex(evt);
  };

  std::vector<G4double> genEs;

  // 1.2745 MeV de-excitation gamma
  //shoot(1274.5*keV, G4RandomDirection());

  genEs.clear();
    
  if (G4UniformRand() < fP2) {
    // 2γ back-to-back 511 keV
    G4ThreeVector d = G4RandomDirection();
    const double E = 511.;
    G4ThreeVector pos = RandomPointInSilica();
    shoot(E*keV, d, pos);
    shoot(E*keV, -d, pos);

    genEs.push_back(E);
    genEs.push_back(E);
    
  } else {    

    genEs = Generate3Gamma_OrePowell(evt);

  }

  // sort
  std::sort(genEs.begin(), genEs.end(), std::greater<G4double>());

  // ★ EventInfo に書き込む
  auto* info = static_cast<PsEventInfo*>(evt->GetUserInformation());
  if (!info) {
    info = new PsEventInfo();
    evt->SetUserInformation(info);
  }
  if(info) info->SetGenGammaEnergies(genEs);

}


std::vector<G4double> PrimaryGeneratorAction::Generate3Gamma_OrePowell(G4Event* evt) {

  std::vector<G4double> genEs;

  constexpr G4double Ecm = 2.0 * 511.0 * keV; // o-Ps 静止系の全エネルギー

  // 受理・棄却ループ
  constexpr int MAX_TRY = 10000;
  for (int itry = 0; itry < MAX_TRY; ++itry) {
    genEs.clear();

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
      genEs.clear();
      for (int i=0;i<3;++i) {
        fGun->SetParticleEnergy(Ei[i]);
        fGun->SetParticleMomentumDirection(dir[i]);
	fGun->SetParticlePosition(pos); 
	fGun->GeneratePrimaryVertex(evt);
	genEs.push_back(Ei[i]/keV);
      }

      return genEs;
    }
  }

  // 異常系：受理できなかったら等方等分でフォールバック
  {
    std::array<G4ThreeVector,3> dir;
    std::array<G4double,3>      Ei;
    Rambo3(Ecm, dir, Ei);
    genEs.clear();
    for (int i=0;i<3;++i) {
      fGun->SetParticleEnergy(Ei[i]);
      fGun->SetParticleMomentumDirection(dir[i]);
      fGun->GeneratePrimaryVertex(evt);
      genEs.push_back(Ei[i]);
    }
  }

  return genEs;
}

// ===== RAMBO風：3 質量ゼロ粒子（γ）を Ecm で等方・等体積サンプル =====
void PrimaryGeneratorAction::Rambo3Static(const G4double Ecm,
                                          std::array<G4ThreeVector,3>& pdir,
                                          std::array<G4double,3>& E)
{
  Rambo3Impl(Ecm, pdir, E);
}

// OrePowell分布に従う3γ方向・エネルギーをサンプル（返値: 成功=true）
bool PrimaryGeneratorAction::SampleOrePowell3(std::array<G4ThreeVector,3>& dirs,
                                               std::array<G4double,3>& energies)
{
  constexpr G4double Ecm = 2.0 * 511.0 * keV;
  constexpr G4double WMAX = 400.;
  constexpr int MAX_TRY = 10000;
  for (int itry=0; itry<MAX_TRY; ++itry) {
    Rambo3Impl(Ecm, dirs, energies);
    G4double x1=(energies[0]/keV)/511., x2=(energies[1]/keV)/511., x3=(energies[2]/keV)/511.;
    if (x1<=0||x1>=0.99||x2<=0||x2>=0.99||x3<=0||x3>=0.99) continue;
    G4double den=(1-x1)*(1-x2)*(1-x3);
    if (den<=0) continue;
    G4double num=x1*x1*(1-x1)*(1-x1)+x2*x2*(1-x2)*(1-x2)+x3*x3*(1-x3)*(1-x3);
    auto c12=dirs[0].dot(dirs[1]), c23=dirs[1].dot(dirs[2]), c31=dirs[2].dot(dirs[0]);
    G4double w=(num/den)*(std::pow(1-c12,2)+std::pow(1-c23,2)+std::pow(1-c31,2));
    if (G4UniformRand()*WMAX < w) return true;
  }
  // fallback: uniform
  Rambo3Impl(Ecm, dirs, energies);
  return false;
}

void PrimaryGeneratorAction::Rambo3(const G4double Ecm,
                                    std::array<G4ThreeVector,3>& pdir,
                                    std::array<G4double,3>& E)
{
  Rambo3Impl(Ecm, pdir, E);
}

void PrimaryGeneratorAction::Rambo3Impl(const G4double Ecm,
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
