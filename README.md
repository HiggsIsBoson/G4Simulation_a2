## Anacondaで環境構築
もう済んでる場合はスキップしてOK.

#### 1. Anacondaをインストール
[ここ](https://www.anaconda.com/docs/getting-started/anaconda/install#macos-linux-installation:how-do-i-verify-my-installers-integrity)からダウンロード  
<br>

#### 2. コマンドプロンプトを開けてanacondaを起動
- macOS : ターミナル
- Windows : Miniconda3 or Anaconda (なかったらダウンロード)
```
(base) [cshion@MacBook-Air-6 kaggle_higgs-boson]$
```
みたいに (base) がプロンプトの前についてたら成功。  
<br>

#### 3. "g4"という名前の環境を作る
```
conda create -n g4 cmake
```

#### 4. 環境"g4"に入る
```
conda activate g4
```

#### 5. 追加で必要なパッケージをinstallする
```
conda install -c conda-forge geant4
```
 * `geant4`をインストール
 * `conda-forge`という別のrepositoryを参照する必要がある
 <br>

#### 環境から抜けたくなったら
```
conda deactivate 
```

#### 今ある環境の一覧を表示
```
conda info -e
```
目的に応じて環境を使い分けた方がよい（じゃないと一回インストールしたらバージョン管理が不可能になる）  
<br><br>


## Geant4を走らせる

#### 1. このrepositoryをダウンロード

<img width="400" height="300" alt="Screenshot 2026-02-08 at 13 02 38" src="https://github.com/user-attachments/assets/bf3786c8-8f4a-454f-8662-5a06d7286bc7" />

`G4Simulation_a2-main`というフォルダがダウンロードされるので, 適当な場所に移動しておく。
<br>

#### 2. コマンドプロンプトを開けてanacondaを起動
- macOS : ターミナル
- Windows : Miniconda3 or Anaconda (なかったらダウンロード)  
<br>
  
#### 3. 環境を起動
```
conda activate g4
```

#### 4. CMakeを実行
`G4Simulation_a2-main`の中で`build/`というフォルダを作り, 中でCMakeを実行する:
```
cmake -DCMAKE_PREFIX_PATH="$CONDA_PREFIX" -DZLIB_ROOT="$CONDA_PREFIX" ../
```
これはコンパイルのスクリプト (`Makefile`) を生成するコマンドである。  
<br>

#### 5. コンパイルを実行 
```
make -j 8 
```
8は同時に実行するスレッドの数（早くなる）  
うまくいっていれば`nai_spectrum`という実行ファイルが生成されているはず。
<br>

#### 6. 設定ファイルを指定して実行

現在 **Mode 1**, **Mode 2**, **Mode 3** の3つのセットアップがある（`--mode`で切り替え）。

---

**Mode 1: Na22のbeta+がプラシンを透過するシミュレーション**

```
./nai_spectrum --mode 1 --out ../analysis/plastic_sim.root ../macros/batch_mode1.mac
```

セットアップは以下の通り:
```
z=0 mm       z=10 mm              z=50 mm, x=40mm
Na22線源 →→  [プラシン 0.3mm厚]   [シリカゲル 20×20×20 mm]
(+z方向)     (半径15mm円盤)        (中心)
```
Na22線源から+z方向に単一のbeta+を1本射出し, プラシンでの損失エネルギーと通過後のbeta+のエネルギー・方向を記録する。

---

**Mode 2（デフォルト）: シリカ中心でのγ線生成 → NaI検出**

`build/`の中で
```
./nai_spectrum --mode 2 --p2 1.0 --out ../analysis/nai_p2_100.root ../macros/batch_mode2.mac
```
を実行。無事に終了すれば`../analysis/nai_p2_100.root`というファイルが生成される。
ここで`--p2`は全イベントに占める2γイベントの割合を指定するオプションで, `1.0`は100% 2γという意味である。

3γだけを作りたい場合は :
```
./nai_spectrum --mode 2 --p2 0.0 --out ../analysis/nai_p2_000.root ../macros/batch_mode2.mac
```
とすればよい。

---

**Mode 3: フルチェーンシミュレーション（Na22 → プラシン → シリカ(Ps形成) → NaI）**

```
./nai_spectrum --mode 3 --p2 0.5 --out ../analysis/full_chain.root ../macros/batch_mode3.mac
```

ここで `--p2` は **pick-off による2γ崩壊の確率**（残り `1-p2` がo-Psの3γ自己崩壊）を指定するオプションである（Mode 2 と同じ意味）。

セットアップは以下の通り:
```
z=0 mm      z=10 mm              z=50 mm, x=40mm                   z=115 mm
Na22線源 →→ [プラシン 0.3mm厚]   [シリカゲル 20×20×20 mm]   ↑y方向
(+z方向)    (10×10mm角)          (中心)                     [NaI 200×50×50 mm]
                                                             (底面: y=40 mm, シリカ上面+30 mm)
                                                             (前面: z=90 mm, シリカ背面+30 mm)
                                                             (長軸: x方向)
```

Na22から beta+ と 1274.5 keV γ線（等方的）を同時放出し、beta+ がシリカで停止するとポジトロニウム（Ps）が形成されたとみなして2γまたは3γを射出する。1274.5 keV γ線はGeant4の通常のEM物理でNaIまで伝搬される。

---

**GUIモード（検出器の3D描画）**
GUIモードでは作ったモデルを目視できるので、ジオメトリの確認に便利である。
```
./nai_spectrum --mode 1 --ui ../macros/vis.mac   # Mode 1のジオメトリを表示
./nai_spectrum --mode 2 --ui ../macros/vis.mac   # Mode 2のジオメトリを表示
./nai_spectrum --mode 3 --ui ../macros/vis.mac   # Mode 3のジオメトリを表示
```

<img width="500" height="450" alt="Screenshot 2026-02-09 at 1 03 25" src="https://github.com/user-attachments/assets/37b50789-06ed-4420-a605-ed17a8101fa8" />

<br>


## Geant4では何をやっているか？

#### 大まかな構造
- 検出器を設計 (`src/DetectorConstruction.cc`) : 位置・寸法・素材を指定している。
- 粒子の生成 (`src/PrimaryGeneratorAction.cc`) : モードに応じた一次粒子を生成する ("イベント")。
  * Mode 1 : Na22のbeta+スペクトル（endpoint 545.4 keV）でサンプリングしたエネルギーのe+を+z方向に1本生成
  * Mode 2 : シリカ中心で2γもしくは3γを生成
    - 2γ : 511keVをback-to-backに等方的に生成
    - 3γ : Ore-Powell分布 ([この論文](https://arxiv.org/pdf/hep-ph/0311002)の式16) に従ったエネルギーの組み合わせで, エネルギー・運動量保存を満たしながら等方的に生成
  * Mode 3 : Na22のbeta+（Mode 1と同様）と1274.5 keV γ線を同時に放出。beta+/gamma は各々GeantにE&M物理でシミュレートされる。
- Step 単位の処理の設定 (`src/SteppingAction.cc`) : 粒子と検出器の相互作用はstep by stepで行われる（散乱, シャワー発展）。ステップ自体のシミュレーションはGeant4がやってくれるが, その各ステップの始まり・終わりに行う処理はユーザーが指定できる。
  * Mode 1 : プラシン内でe+が落とすエネルギーを積算し, プラシンを抜けた瞬間のe+のエネルギーと方向を記録する。
  * Mode 2 : そのstepでNaIに落とされたエネルギーを加算したり, 相互作用の種類 (光電効果・コンプトン散乱 etc.) の情報を取ってきてeventレベルの処理に送っている。
  * Mode 3 : Mode 1 と同様にe+の停止を検出し、シリカ停止時にGeant4の対消滅γ線をキャンセルして代わりにPs由来の2γ/3γを生成する。NaIに落とされたエネルギーを積算する（Ps由来かどうかも区別して積算）。
- Event 単位の処理の設定 (`src/EventAction.cc`) : 一連のstepを束ねたものがeventである。Stepと同様, Eventの最初と最後にやる処理はユーザーで定義できる。ここでは各モードで必要な物理量をまとめてファイルに書き出している。
- Run 単位の処理の設定 (`src/RunAction.cc`) : Eventを束ねたものがRunである。ここではファイルに書き出す変数などの定義を行っている。Mode 1とMode 2でntupleの構造が異なる。
- メイン関数 (`src/main.cc`) : 実行ファイルで実際に実行しているもの。コマンドで指定したオプションや設定ファイルの読み込み, 上で言及した個別要素の呼び出しなどを行っている。

#### 検出器の設定 (`src/DetectorConstruction.cc`)
- `World`という, 検出器の外の空気の領域を定義 (50cm×50cm×50cm)
- モードによって配置するジオメトリが変わる:
  * **Mode 1** : プラシン正方形（10×10mm, 厚さ0.3mm, z=10mm）とシリカ直方体（2×2×2cm, z=50mm）を配置
  * **Mode 2** : シリカ円筒（半径3cm, 長さ10cm）とNaI直方体（5×5×20cm）を配置
  * **Mode 3** : Mode 1と同じプラシン・シリカに加え, NaI直方体（50×50×200mm, 長軸x方向）をシリカ背面から30mm後方かつシリカ上面から30mm上に配置
  * 寸法などのパラメータはコードの上部で定義している。

#### 設定ファイル
`macros`に入ってる設定ファイルで, プログラムで使用する外部情報を定義することができる。
こういうことでプログラムのソースコードにいちいちベタ書きしなくても, 設定ファイルを分けることで効果的に管理することができる（ミスの防止にもつながる）。

GUIを使用する場合と, そうでない場合 ("batch"と呼ぶ) で設定ファイルを分けている。
- `vis.mac` : GUIありのときに呼ぶやつ（Mode 1/2/3 共通）
- `batch_mode1.mac` : Mode 1のバッチ実行用
- `batch_mode2.mac` : Mode 2のバッチ実行用
- `batch_mode3.mac` : Mode 3のバッチ実行用

`batch_mode2.mac`の方にある以下行は検出器分解能についての設定である（Mode 2のみ有効）:
```
/analysis/a 0.52
/analysis/b 0.0
/analysis/c 0.0
```
`a`はstocastic term, `b`はnoise term, `c`はconstant termと呼ばれていて, それぞれシンチレーション光子計数の統計的なふらつき, ノイズの寄与, 検出器の不均一性や不感領域に由来するものである。
分解能はこれらの係数を使って $\sigma/E = \frac{a}{\sqrt{E~[\mathrm{keV}]}} \oplus \frac{b}{E~[\mathrm{keV}]} \oplus c$ と表されたものを使用している（$\oplus$ は二乗和である）。   

生成するイベント数は`beamOn`コマンドのオプションから指定できる。
```
/run/beamOn 1000
```
<br>

## 出力ファイルの見方

##### ファイルを開ける
面倒なことにGeant4の環境は基本的にROOTと共存ができないので, 生成されたROOTファイルを開けて解析するには別のROOTの環境を使う必要があります。
今いる`g4`の環境をdeactivateしてROOT環境をactivateするか, もうひとつプロンプトのタブ/ウィンドウを開いてそっちはROOT環境を起動しましょう。  

---

**Mode 2の出力** (`analysis/nai_p2_***.root`)

`G4Simulation_a2-main/`にいる想定でファイルを開けるには
```
root -l analysis/mode2_nai_p2_100.root
```

##### 中身のブランチ（ntupleの名前は`nai`）

エネルギーの単位は全てkeVである。
- `rawE` : イベントあたりにNaI検出器に落とした真の総エネルギー。「真の」というのは検出器分解能によるなまりがなかった場合の、という意味である。無限の分解能があったときの測定結果と言ってもよい。0になっているのはγ線がNaIに当たらなかった or 相互作用しなかったイベント。
- `smE` : rawEを検出器分解能でなまらせた（"smear"された）もの。現実で観測する量に相当。
- `Origin` : 相互作用の種類。最後のstepが光電吸収だったら1, そうじゃなかったら0。コンプトン散乱した後に同じNaI内で光電吸収されたものも含む。
- `trE1`,`trE2`,`trE3` : 生成したγ線の真のエネルギー。エネルギーが高い順にソートしてある。
- `HitNaI` : NaIの領域を通ったら1, そうでなければ0。NaIの領域を通ったのに透過したイベントを見分けるのに使える。

##### 検出したエネルギー分布の描画
```
nai->Draw("smE","smE>0")
```

##### 生成したγ線のエネルギー分布の描画
```
nai->Draw("trE1")
```

##### 2γと3γのエネルギー分布を重ねて書く (`analysis/`にいるとき)
```
root -l plot.cc
```

---

**Mode 1の出力** (`analysis/plastic_sim.root`)

```
root -l analysis/plastic_sim.root
```

##### 中身のブランチ（ntupleの名前は`plastic`）

エネルギーの単位は全てkeV, 角度はradianである。
- `gen_E` : 生成したbeta+の運動エネルギー [keV]。Na22のbeta+スペクトルに従ってサンプリングされる (endpoint 545.4 keV)。
- `gen_theta`, `gen_phi` : 生成時の運動方向の極角・方位角 [rad]（現状は常に+z方向なのでそれぞれ0, 0）。
- `hit_plastic` : beta+がプラシンに入射したら1, そうでなければ0。
- `edep` : プラシン内に落としたエネルギー [keV]。止まった場合は`gen_E`に近い値になる。
- `out_E` : プラシンを抜けた後のbeta+の運動エネルギー [keV]。プラシン内で止まった場合は0。
- `out_theta`, `out_phi` : プラシンを抜けた後の運動方向の極角・方位角 [rad]（`out_E > 0`のイベントのみ有意）。

##### プラシンでのエネルギー損失分布の描画
```
plastic->Draw("edep")
```

##### 透過したbeta+のエネルギー分布の描画
```
plastic->Draw("out_E","out_E>0")
```

##### 透過率の計算
```
plastic->GetEntries("out_E>0") / plastic->GetEntries()
```

---

**Mode 3の出力** (`analysis/full_chain.root`)

```
root -l analysis/full_chain.root
```

##### 中身のブランチ（ntupleの名前は`chain`）

エネルギーの単位は全てkeVである。
- `gen_E` : 生成したbeta+の運動エネルギー [keV]。
- `plastic_edep` : プラシン内に落としたエネルギー [keV]。
- `hit_silica` : beta+がシリカに入射したら1。
- `stop_silica` : beta+がシリカ内で停止したら1（= Ps形成とみなす）。
- `n_gamma` : Psから生成したγ線の本数（2または3）。`stop_silica==1`のイベントのみ有意。
- `nai_edep` : NaIに落としたエネルギーの合計 [keV]（真の値）。
- `nai_edep_sm` : `nai_edep`を検出器分解能でなまらせたもの [keV]。
- `nai_edep_ps` : そのうちPs由来のγ線が落としたエネルギー [keV]（1274.5 keV γ線の寄与を除いた量）。
- `nai_edep_ps_sm` : `nai_edep_ps`を分解能でなまらせたもの [keV]。
- `hit_nai` : NaIに何かが当たったら1。

##### シリカで止まったイベントのNaIエネルギー分布
```
chain->Draw("nai_edep_sm","stop_silica==1 && hit_nai==1")
```

##### Ps由来成分 vs 1274keV γ線成分の比較
```
chain->Draw("nai_edep_ps_sm","stop_silica==1 && hit_nai==1","")
chain->Draw("nai_edep_sm-nai_edep_ps_sm","stop_silica==1 && hit_nai==1","same")
```
