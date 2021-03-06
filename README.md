# KeiHirano

## 掟
study and research
研究用のスクリプトを保存しておく場所．
Kinect及びdepthデータを扱う関係のスクリプトがC++の中に
それ以外のものは，pythonscriptの中に
pythonスクリプトは，基本的には扱うものとかで場合分けしている．
基本的にはタイトルのまんまのスクリプトであることが多い．

__作業フローに変更や追加があったらREADMEを更新すること__

## 実験ディレクトリについては以下のフォルダ構成を守ること(2019/11/15現在)
__予め以下のディレクトリを作っておくこと__
(初期状態はcolorDirとdepthDirとposは中身がある状態.その後正田方式を撮るか平野方式を取るかで出力されるファイルが異なる．)  
basepath/  
┠  color  
┠  color2  
┠  depth  
┠  depth_mirror  
┠  json  
┠  pos  
┠  regi  
┠  regi2    
┠  regi_mirror  
┠  regi_mirror_fixed  
┗  render

## 環境によるスクリプトの変更について
python関係のスクリプトで，ROSシステムが入っているせいで，
opencvのライブラリ使用時に追加で記述している部分が有る．
[showOpenPoseResult.py](/pythonscript/OpenPose/showOpenPoseResult.py)などは，ROS有りの環境で使用する際には，[showOpenPoseResult_Hirano.py](/pythonscript/OpenPose/showOpenPoseResult_Hirano.py)など若干名称を変更したものを使用すること．引数や出力などは変わらない．なので，こちらは本人の許可がない限り変更しないこと．

## 動作作成シェルスクリプト
[shoda_exe.sh](./shoda_exe.sh)
[Hirano_exe.sh](./Hirano_exe.sh)
両者でレジストレーション画像の生成方法に違いが有る．
[Hirano_exe.sh](./Hirano_exe.sh)内での処理や作業フローについては，[Hirano_exe使い方.md](./Hirano_exe使い方.md)を参照
## 簡易作業フロー

1. レジストレーション画像生成  
[DepthMapper.cpp](C++script/DepthMapper.cpp)    
__コード内入力__ colorDir depthDir
__入力__ basepath  
__出力__ regiDir
<br>

1. ミラー画像生成  
[regimirror.cpp](regimirror)
__入力__ basepath
__出力__ regiMirrorDir
<br>

2. レジストレーション画像アフィン変換(option)  
[fixregi.py](pythonscript/ImageTool/fixregi.py)  
__入力__ basepath dx
__出力__ regiMirrorFixDir  
<br>

3. OPENPOSE解析(render生成)   
__例__  
`./build/examples/openpose/openpose.bin --image_dir /home/shoda/Documents/mitsu/regi_mirror -write_images /home/shoda/Documents/mitsu/render --write_json /home/shoda/Documents/mitsu/json`
<br>

4. OPENPOSE結果確認(複数人表示)(option)  
[showOpenPoseResult.py](pythonscript/OpenPose/showOpenPoseResult.py)  
__入力__ basepath  
<br>

5. jsonfileを読み込んで一人の人の時系列データ行列に変換するためのコード  
[csvposer.py](pythonscript/csvpose/csvposer.py)  
__入力__ basepath peopleID startframe endframe  
__出力__ output.csv probability.csv test.csv
<br>

6. OPENPOSE結果確認(指定した1人表示)(option)  
[showCSVResult.py](pythonscript/OpenPose/showCSVResult.py)  
__入力__ regi_mirrorDir output.csv  
__例__   
`python pythonscript/OpenPose/showCSVResult.py /home/shoda/Documents/mitsu/regi_mirror /home/shoda/Documents/mitsu/output.csv`
<br>

7. 両肩座標を使用したDepthとRGBの位置誤差の確認(option)   
[joint_diff.py](pythonscript/ImageTool/joint_diff.py)  
__入力__ basepath  
__出力__ diff.png

8. 三次元姿勢推定  
[3DposeMaker.cpp](C++script/3DPoseMaker.cpp)  
__入力__ basepath  
__出力__ save.csv   
<br>

9. save.csvに３次元関節名をヘッダに記載する  
[Column.py](pythonscript/Liner/Column.py)  
__入力__ save.csv   
__出力__ 3dbone.csv  
<br>

10. 線形補完して各関節の時系列データを出力（カメラ座標系）(option)  
[3DInterrupt.py](pythonscript/Liner/3DInterrupt.py)  
__入力__ 3dbone.csv  
__出力__ 3DInterrupt.csv  
<br>

11. regi画像においてピクセルを指定して平面の2D点群を取得する  
[BoundMaker.py](C++script/BoundMaker.cpp)  
__コード内入力__ basepath 対象のregi画像  
__出力__ 2DGround.csv
Z平面左上→Z平面右下→X平面左上→X平面右下→原点→その他5点  
<br>

12. 平面上のすべての3D点群を取得する  
[3DChair.cpp](C++script/3DChair.cpp)  
__コード内入力__ basepath 対象のdepth画像  
__出力__ Xplane.csv Zplane.csv edge.csv  
edge.csvの一行目が原点を表す  
<br>

13. 軸、回転行列R、製品座標系の姿勢データを生成する  
[CalProductaxis.py](pythonscript/GroundCal/CalProductaxis.py)  
__入力__ basepath  
__出力__ axisData.csv 3dboneRotated.csv

(その他)動画，若しくは画像にモザイク処理を行う
もし，顔にモザイク，もしくは黒塗りにした状態にして顔が見えないようにした動画を作成したい場合
高齢者行動ライブラリで公開されていない自分で発掘した，若しくは別途自分で顔を消した動画を作成する必要が有る場合，
対象の元画像に対してopenposeを使用し，Face_jsonというファイルにjsonファイルを格納し，
python3 /pythonscript/Face_Mosaic.py
を実行すると顔に黒塗りした画像が生成できる．まだ，開発段階であるため稀に止まることがあるため鋭意開発を進めたい．
