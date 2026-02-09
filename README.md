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

#### 3. “g4”という名前の環境を作る
```
conda create -n g4 cmake
```

#### 4. 環境“ML”に入る
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
`build/`の中で
```
./nai_spectrum ../macros/batch.mac --p2 1.0 --out  ../analysis/nai_p2_100.root
```
を実行。無事に終了すれば`../analysis/nai_p2_100.root`というファイルが生成される。
ここで`100`は全体に占める2γイベントの割合を表していて、 `100`は100% 2γという意味である。
出力ファイル名は`--out`から指定できる。

3γだけを作りたい場合は :
```
./nai_spectrum ../macros/batch.mac --p2 0.0 --out  ../analysis/nai_p2_000.root
```
とすればよい。

またGUIを使って検出器のビジュアルを描画させることもできる。これは :
```
./nai_spectrum ../macros/vis.mac
```
のように別の設定ファイルを用意したので, それを呼んであげればよい。

<img width="500" height="450" alt="Screenshot 2026-02-09 at 1 03 25" src="https://github.com/user-attachments/assets/37b50789-06ed-4420-a605-ed17a8101fa8" />

<br>


## Geant4では何をやっているか？

#### 大まかな構造
- 検出器を設計 (`src/DetectorConstruction.cc`) : 位置・寸法・素材を指定している。
- 粒子の生成 (`src/PrimaryGeneratorAction.cc`) : 原点で2γもしくは3γを生成 ("イベント")。
  * 2γ : 511keVをback-to-backに等方的に生成
  * 3γ : Ore-Powel分布に従ったエネルギーの組み合わせで, エネルギー・運動量保存を満たしながら等方的に生成。
- Step 単位の処理の設定 (`src/StepAction.cc`) : 粒子と検出器の相互作用はstep by stepで行われる（散乱, シャワー発展）。ステップ自体のシミュレーションはGeant4がやってくれるが, その各ステップの始まり・終わりに行う処理はユーザーが指定できる。ここでは, そのstepでNaIに落とされたエネルギーを加算したり, そのstepでの相互作用の種類 (光電効果・コンプトン散乱・対生成 etc.) の情報を取ってきて,後段のeventレベルの処理に送っている。
- Event 単位の処理の設定 (`src/EventAction.cc`) : 一連のstepを束ねたものがeventである。Stepと同様, Eventの最初と最後にやる処理はユーザーで定義できる。ここではγ線がNaIに落としたエネルギーの総和の計算や, γ線と検出器の相互作用の種類の判定をしている。
- Run 単位の処理の設定 (`src/RunAction.cc`) : Eventを束ねたものがRunである。ここではファイルに書き出す変数などの定義を行っている。
- メイン関数 (`main.cc`) : 実行ファイルで実際に実行しているもの。コマンドで指定したオプションや設定ファイルの読み込み, 上で言及した個別要素の呼び出しなどを行っている。

#### 検出器の設定 (`src/DetectorConstruction.cc`)
- `World`という, 検出器の外の真空の領域を定義 (50cm×50cm×50cm)
- NaI (直方体) とシリカ (円筒型)を配置している。
  * 寸法などのパラメータはコードの上部にある`fSilicaR(3.*cm), fSilicaHL(5.*cm)`などで定義している (今は実装していないが, 設定ファイルから設定するようにすることも可能である)。

#### 設定ファイル
`macros`に入ってる設定ファイルで, プログラムで使用する外部情報を定義することができる。
現状検出器分解能と, 生成するイベント数くらいしか定義していないが, 原理的には検出器の大きさとか位置などもここで指定できる。
こういうことでプログラムのソースコードにいちいちベタ書きしなくても, 設定ファイルを分けることで効果的に管理することができる（ミスの防止にもつながる）。

GUIを使用する場合と, そうでない場合 ("batch"と呼ぶ) で設定ファイルを分けている。
- vis.mac : GUIありのときに呼ぶやつ
- batch.mac : GUIなしのときに呼ぶやつ

`batch.mac`の方にある以下行は検出器分解能についての設定である:
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
面倒なことにGeant4の環境は基本的にROOTと共存ができないので, 生成されたROOTファイルを開けて解析するには先に作ってもらったROOTの環境を使う必要があります。
今いる`g4`の環境をdeactivateしてROOT環境をactivateするか, もうひとつプロンプトのタブ/ウィンドウを開いてそっちはROOT環境を起動しましょう。  

`G4Simulation_a2-main/`にいる想定でファイルを開けるには
```
root -l analysis/nai_p2_100.root
```

##### 中身のブランチ
<img width="479" height="604" alt="Screenshot 2026-02-08 at 13 52 14" src="https://github.com/user-attachments/assets/d4622dea-2d59-4fc7-9d06-73552518626c" />

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
