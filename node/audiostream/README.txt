BGMと効果音を重ねてリアルタイムにストリーミングするサンプル

gym1.mp3 : BGM
blll.wav : SE

デコードにはffmpegを使用。

OSXでのみ試しています。

1. brew install ffmpeg  (コマンドラインのffmpegがインストールされる)
2. brew install node
3. brew install npm
4. npm install ws
5. npm install pcm
6. node sv.js として起動
7. http://localhost:8888/をみる

接続するとBGMがストリーム再生されます。
Aキーを押すと効果音が重ねて表示されます。

ミキシングが超適当なのと、遅延の最適化が不十分なので、
音声がゆらいだり、かすれたり、ビビったりします。



