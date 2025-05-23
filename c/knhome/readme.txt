
                     Kunoichi API for Linux リリース・ノート
                     ￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣        1999/05/27
                                                               (株)ＮＴＴデータ
                                                       Copyright(c)1999 NTTDATA 

１. ディスク内容

    提供するarchiveには、以下のファイルが含まれます。

      ファイル             内容

      kunoichilinux19990527.tar.Z      サーバ用ライブラリ

    後述するインストール手順に従い、サーバにインストールして下さい。

２. ディレクトリ構成

 ２.1 サーバ・ディレクトリ構成

    サーバでは、先頭ディレクトリ (ディレクトリ名は任意ですがここでは $KN_HOME
    とします) の下に、以下のディレクトリ構成を持ちます。

        $(KN_HOME)
        +-------lib/                           ライブラリ格納ディレクトリ
        +-------h/                             ゲームベンダー提供ヘッダ

３. インストール手順

 ３.1 サーバ・インストール

    サーバ用ライブラリのインストールは以下の手順で行なって下さい。

    (1) tar ファイルの転送
       kunoichilinux19990527.tar.Z をインストール先のサーバ・マシンの $(KN_HOME)
       配下に転送して下さい。

    (2) 展開
       以下のコマンドで tar ファイルを展開します。

           % cd $(KN_HOME)
           % uncompress kunoichilinux19990527.tar.Z
           % tar xvf kunoichilinux19990527.tar

       tar コマンドを実行したディレクトリの直下に上述のディレクトリ構成が展開さ
       れます。

    (3) ライブラリのコピー
       $ (KN_HOME)/lib にある API ライブラリをお使いになる場所にコピーします。
       コピーには UNIX の cp コマンドを使って下さい。なお、提供物には以下のAPI 
       ライブラリを含みます。

           libknsvr.a ........ KUNOICH API ライブラリ(server)

    (4) ゲーム・サーバ・プログラムのリンク
        ゲーム・サーバのリンクには、上記 libknsvr.aをリンクして下さい。

       以下にリンク用コマンド実行例を示します。

           % cc -o xxx *.o $(KN_HOME)/libknsvr.a
  
    (5) アプリケーションコンパイルの際のコンパイルオプション(プリプロセッサ)に以
        下のコンパイルオプションを追加してください。

	_KND_SERVER_, (KND_DEBUG)

	※KND_DEBUGは、DEBUG LOGが必要な場合のみご指定ください。


４. 制限事項およびＡＰＩ変更点

  ■ユーザ管理ＡＰＩ
  
    ・ユーザ認証機能は、下記のような単体テスト用仕様で実装しています。

	（１）実際にユーザ認証サーバに問い合わせを行いません。
	（２）会員IDの先頭文字が「Z」の場合、２０秒間停止します。
	（３）パスワードの先頭文字が「Z」の場合、エラーを返します。

  ■その他

    ・今回は、ユーザ認証関数(suCheckPassword)のみ提供します。

