cd build

# =====================================================================
#  Mode 1: Na22のbeta+がプラシンを通過するシミュレーション
#          線源(z=0)→プラシン(z=10mm, 厚さ0.3mm)→シリカ(z=50mm, 2cm角)
# =====================================================================

# バッチ実行 (--out にはフルパスか .root 拡張子つきで指定)
#./nai_spectrum --mode 1 --out ../analysis/mode1_plastic_sim.root ../macros/batch_mode1.mac

# シリカ位置を変えてgap=5mmに設定 (silicaZ = plasticz_back + gap + silicaHalf = 10.15 + 5 + 10 = 25.15mm)
#./nai_spectrum --mode 1 --silica-z 25 --out ../analysis/mode1_plastic_sim_gap5.root ../macros/batch_mode1.mac


# =====================================================================
#  Mode 2 (デフォルト): シリカ中心で2γ/3γを生成してNaIのエネルギー分布を調べる
# =====================================================================

# 2γのみ (p2=1.0): 511keV back-to-back
#./nai_spectrum --mode 2 --p2 1.0 --out ../analysis/mode2_nai_p2_100.root ../macros/batch_mode2.mac

# 3γのみ (p2=0.0): Ore-Powell分布
#./nai_spectrum --mode 2 --p2 0.0 --out ../analysis/mode2_nai_p2_000.root ../macros/batch_mode2.mac

# 50/50混合 (p2=0.5)
#./nai_spectrum --mode 2 --p2 0.5 --out ../analysis/mode2_nai_p2_050.root ../macros/batch_mode2.mac


# =====================================================================
#  Mode 3 (フルチェーン): Na22 → beta+ → プラシン → シリカでPs形成 → 2γ/3γ → NaI
#          線源からの1274.5 keVガンマも同時に出す
#          ntuple "chain": gen_E, plastic_edep, hit_silica, stop_silica, n_gamma, nai_edep, hit_nai
# =====================================================================

./nai_spectrum --mode 3 --p2 0.0 --out ../analysis/mode3_p2_000.root ../macros/batch_mode3.mac
./nai_spectrum --mode 3 --p2 1.0 --out ../analysis/mode3_p2_100.root ../macros/batch_mode3.mac

# シリカ位置を変える場合 (例: gap=5mm)
#./nai_spectrum --mode 3 --p2 0.5 --silica-z 25 --out ../analysis/full_chain_gap5.root ../macros/batch_mode3.mac


# =====================================================================
#  GUIモード (検出器の3D描画・インタラクティブ操作)
# =====================================================================

# Mode 3 でGUI起動
#./nai_spectrum --mode 3 --ui ../macros/vis.mac

# Mode 2 (シリカ+NaI) でGUI起動
#./nai_spectrum --mode 2 --ui ../macros/vis.mac

# Mode 1 (プラシン+シリカ) でGUI起動
#./nai_spectrum --mode 1 --ui ../macros/vis.mac

cd ..
