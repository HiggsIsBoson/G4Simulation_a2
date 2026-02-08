## Anacondaで環境構築

##### Anacondaをダウンロード・インストール
https://www.anaconda.com/docs/getting-started/anaconda/install#macos-linux-installation:how-do-i-verify-my-installers-integrity

##### ターミナルを開けてanacondaを起動
```
(base) [cshion@MacBook-Air-6 kaggle_higgs-boson]$
```
みたいに (base) がプロンプトの前についてたら成功。  
<br>

##### “g4”という名前の環境を作る
```
conda create -n g4
```

##### 環境“ML”に入る
```
conda activate g4
```

##### 追加で必要なパッケージをinstallする
```
conda install -c conda-forge geant4
```
 * `geant4`をインストール
 * `conda-forge`という別のrepositoryを参照する必要がある
 <br>

##### 環境から抜けたくなったら
```
conda deactivate 
```

##### 今ある環境の一覧を表示
```
conda info -e
```
目的に応じて環境を使い分けた方がよい（じゃないと一回インストールしたらバージョン管理が不可能になる）
<br>

## Geant4を走らせる

##### このrepositoryをダウンロード

<img width="400" height="300" alt="Screenshot 2026-02-08 at 13 02 38" src="https://github.com/user-attachments/assets/bf3786c8-8f4a-454f-8662-5a06d7286bc7" />

`G4Simulation_a2-main`というフォルダがダウンロードされるので, 適当な場所に移動しておく。
<br>

##### コマンドプロンプトを開く
- macOS : ターミナル
- Windows : Miniconda3 or Anaconda (なかったらダウンロード)

##### 環境を起動
```
conda activate g4
```

##### CMakeを実行
G4Simulation_a2-main`の中で`build/`というフォルダを作り, 中でCMakeを実行する:
```
cmake -DCMAKE_PREFIX_PATH="$CONDA_PREFIX" -DZLIB_ROOT="$CONDA_PREFIX" ../
```
これはコンパイルのスクリプト (`Makefile`) を生成するコマンドである。

##### コンパイルを実行 
```
make -j 8 
```
8は同時に実行するスレッドの数（早くなる）


