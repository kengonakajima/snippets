#
# 10バイトごとにわけたり
#

a = <<EOF

            右ドラッグで回転させることができる．
        <LI>NPCセリフは4行で，「いらっさい，ここはほげほげです」
            「ありがとう」
            「これはほげほげです．」
            とか言うためのスペース
        <LI>右端は自分のアイテム(itemlistそのまま)
        <LI>右上の数字のところは，お金の金額を表示．
            これも通常のアイテム表示とおなじ．
        <LI>相手のアイテムをクリクすると
            NPCセリフ領域に出力する「これはほげほげです」
            服屋などの場合，試着してキャラを変更してもよい．
        <LI>自分のとこから相手のとこにアイテムをドラッグする(売りor渡し)．
            逆もできる(書い)．やっぱりやめるは右クリク．
        <LI>やめ = モード終了.
        <LI>OK = 取り引き開始，終了したらセリフ領域に出す
        <LI>CANまたはOKまたはコマンド入力(dlgclose)をしない限り終わらない．
      </ul>
      
      
  <LI><a name="textsel">テキスト選択肢から選ぶGUI</a><BR>
      <img src="images/textsel_gui.gif"><BR>
      髪型，皮膚の色などをテキストで選択する．
      基本的にアイテムをやりとりする系と同様だが，
      選択肢がアイテムではなく1行のテキストになっている．



          
  <LI><a name="itemtext">アイテムにテキストを設定するGUI</a><BR>
      <img src="images/itemtext_gui.gif"><BR>
      アイテムには1行のコメントをつけれるが，
      そのインターフェイス．日本語入力の都合上，
      下のEDITウインドウを使うのだ．
      カコワルイがIMEの地獄が嫌なので仕方なし．
      現在，数えられないアイテムには，
      「コメント」をつけることができるが，このGUIはそのために使われる．


  
  <LI><a name="letter">手紙の内容をエディットするGUI</a><BR>
      これはNPCではないが，手紙に宛先と本文をつけるためのインターフェイス．
      <img src="images/letter_gui.gif">
      Toや本文のところは，書類によっていろいろ変化したりするので，
      テキストとして入力できるようになっていなければならない．

      
  <LI><a name="board">たてカンバンのGUI(情報NPC)</a><BR>
      <img src="images/board_gui.gif"><BR>
      情報を表示するだけ．
      選択肢がある場合は以下のようになる．<BR>
      <img src="images/board_opt_gui.gif"><BR>
      これをつかってツリー構造やくりかえしのあるやりとりができる．
      また，このGUIダイアログが出ているときにチャット入力をすると，

asdf

EOF
;





def strsplit( s,len )
    sa = s.unpack( "c*" )
    
    if( (sa.size % len) == 0 ) then
        loop = sa.size / len
    else
        loop = sa.size / len + 1
    end

    ret = Array.new
    loop.times { |i|
        part = sa[(i*len) .. ((i+1)*len - 1)]
        ret.push(part.pack("c*"))
    }
    return ret
end

a = "adf"
ret = strsplit(a, 250)


if( ret.join("") == a ) then
    print "OK\n"
else
    print "ERROR\n"
end
