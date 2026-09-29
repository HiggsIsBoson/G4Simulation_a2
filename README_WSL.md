## WSLの場合：ソースからビルドして環境構築

WSL2 (Ubuntu 24.04) で動作確認済み。
WSLでは **condaのGeant4を使わず, Geant4をソースからビルドする** のがおすすめ。

<details>
<summary>なぜcondaのGeant4はお勧めできないのか</summary>

- Linux版のconda-forge Geant4はQtなしでビルドされているので, マウスで操作できるGUIが出ない。
- WSLの画面表示の仕組みではOpenGLの古い方式 (GLX) が動かない。そのためconda版の`/vis/open OGL`は
  `can't create a glX context with direct rendering` / `X Error ... BadValue (X_GLXCreateContext)` で落ちる。
</details>

2026前期に実際に行った方法を書いています。
次にMacでなくWindowsで解析する人は以下の方法にとらわれずぜひネイティブにインストールしてみてほしい。
MacやLinuxとGeant4/ROOTと相性の良さ、それからWindowsのやばさが分かると思う。

<br>

### 1. 必要なパッケージをインストール
```
sudo apt update
sudo apt install build-essential cmake curl qt6-base-dev qt6-wayland libxerces-c-dev libexpat1-dev libxmu-dev libmotif-dev
```
<br>

### 2. Geant4をソースからビルド (1時間ほどかかる)
`~/geant4/` 以下にインストールする例。
バージョンは適宜変えてよい。
ビルドにはかなりのマシンパワーと時間がかかるので、この間に実験のセットアップを[src/DetectorConstruction.ccのコメント](src/DetectorConstruction.cc)のようにできるだけ正確に文字に起こしておいた。
これを生成AIに投げてDetectorConstruction.ccを編集する。
```
mkdir -p ~/geant4/src && cd ~/geant4/src
curl -L -o geant4-v11.4.2.tar.gz https://github.com/Geant4/geant4/archive/refs/tags/v11.4.2.tar.gz
tar xzf geant4-v11.4.2.tar.gz

mkdir -p ~/geant4/build && cd ~/geant4/build
cmake ../src/geant4-11.4.2 \
  -DCMAKE_INSTALL_PREFIX=$HOME/geant4/install \
  -DCMAKE_BUILD_TYPE=Release \
  -DGEANT4_USE_QT=ON -DGEANT4_USE_QT_QT6=ON \
  -DGEANT4_USE_OPENGL_X11=ON \
  -DGEANT4_USE_GDML=ON \
  -DGEANT4_INSTALL_DATA=ON
make -j$(nproc)
make install
```
- **condaの環境に入った状態 (`(base)` などが付いた状態) でやらないこと。** 先に`conda deactivate`しておく。condaのライブラリが混ざるとビルドやリンクで失敗する。
- `-DGEANT4_INSTALL_DATA=ON`にすると, 物理データ (約2GB) を自動でダウンロードする。
- ビルドが終われば`~/geant4/build`と`~/geant4/src`は消してよい。
<br>

### 3. Geant4の環境を読み込む (端末を開くたびに)
```
source ~/geant4/install/bin/geant4.sh
```
- ライブラリと物理データのパスが設定される。**これを忘れると`nai_spectrum`が起動しない。**
- 毎回打つのが面倒なら`~/.bashrc`の最後に書いておく。
- ROOTの環境 (`thisroot.sh`) と同じ端末で読み込んでも問題ない (conda版と違って共存できる)。
  - これはClaudeが言っていることなので未検証。というかそもそもROOTとGeant4の共存の問題の再現ができていないので何とも言えない。
<br>

### 4. このプロジェクトをビルド
```
cd G4Simulation_a2
mkdir build && cd build
cmake ..
make -j$(nproc)
```
- conda版と違い, `-DCMAKE_PREFIX_PATH`などのオプションは不要。
- **`cmake`の前に必ず手順3の`source`をしておく。** 忘れると`Geant4_DIR-NOTFOUND`で失敗し, `Makefile`が作られない (その後の`make`は`No targets specified and no makefile found`になる)。
- 元のREADMEのconda用の`cmake`コマンドや`compile.sh`はconda前提なので, WSLでは上のコマンドを使う。
<br>

### 5. 実行
バッチ実行は元の[README](README.md)と同じ。
```
./nai_spectrum --mode 3 --p2 0.5 --out ../analysis/full_chain.root ../macros/batch_mode3.mac
```

GUIも元のREADMEと同じ`vis.mac`で動く。
```
./nai_spectrum --mode 3 --ui ../macros/vis.mac
```
- Qtのウィンドウが開き, マウスで回転・拡大ができる。
- コマンド (`/run/beamOn 20`など) はウィンドウ下部の入力欄に打つ。
- Mode 4 (本番セットアップ) 用の表示マクロ (飛跡の色分け・鉛の表示切り替え) : `../macros/vis_mode4.mac`
<br>

### トラブルシューティング
| 症状 | 対処 |
|---|---|
| `error while loading shared libraries: libG4...so` | `source ~/geant4/install/bin/geant4.sh`を忘れている |
| `can't create a glX context` / `X Error ... BadValue` | condaのGeant4でビルドした`nai_spectrum`を使っている。`build/`を消して手順4からやり直す |
| `libEGL warning: DRI3 error: Could not get DRI3 device` | 無視してよい (GPUによる描画の高速化が効かないという警告) |
| QtのGUIがどうしても出ない | 使うマクロの`/vis/open OGL`を`/vis/open TSGZB` (OpenGLを使わないソフトウェア描画) に変える。マウス操作はできないが, 定点カメラでジオメトリの確認はできる |
