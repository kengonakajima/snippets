
#ifdef WIN32

#include <windows.h>

#elif linux || defined(__APPLE__)

#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>

#endif

#ifndef NO_STL
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#endif

#pragma once

namespace vce
{
	////↓自動更新
	unsigned static const int build=87;////build
	unsigned static const int release=5;////release
	unsigned static const int version_date=20060607;////version_date
	////ここまで
#ifdef WIN32

	typedef unsigned __int64 QWORD;

#elif linux || defined(__APPLE__)

	typedef unsigned long long QWORD;

#endif

#ifdef WIN32
	typedef unsigned long DWORD;
#else
	typedef unsigned int DWORD;
#endif
	typedef unsigned short WORD;
	typedef unsigned char BYTE;

#ifdef WIN32
	typedef wchar_t wchar;
#else
	typedef unsigned short wchar;
#endif
	typedef char utf8;

	enum VCE_EXCEPTION
	{
		VCE_EXCEPT_UNKNOWN=0,//!不明
		VCE_EXCEPT_CREATESOCKETFAIL,//!Socket作成しっぱい
		VCE_EXCEPT_BINDFAIL,//!ポート割り当て失敗
		VCE_EXCEPT_LISTENINGFAIL,//!Listening失敗
		VCE_EXCEPT_CONNECTFAIL,//!接続失敗
		VCE_EXCEPT_NAMERESOLUTIONFAIL,//!名前解決失敗
		VCE_EXCEPT_RECVBUFFER_FULL,//!受信バッファがいっぱい
		VCE_EXCEPT_SENDBUFFER_FULL//!送信バッファがいっぱい
	};

	enum VCE_STATE
	{
		VCE_STATE_UNKNOWN=0,//!不明
		VCE_STATE_CONNECTING,//!接続中...
		VCE_STATE_PREPROCESS,//!いろいろ...
		VCE_STATE_ESTABLISHED,//!つかえる
		VCE_STATE_LISTENING,//!リスニング中...
		VCE_STATE_CLOSED//!閉じられた
	};

	enum VCE_CLOSEREASON
	{
		VCE_CLOSE_UNKNOWN=0,//!不明
		VCE_CLOSE_LOCAL,//!自分から切った
		VCE_CLOSE_REMOTE,//!相手に切られた
		VCE_CLOSE_RECVBUFFERFULL,//!受信バッファがいっぱいになってた
		VCE_CLOSE_RECVERROR,//!受信エラー
	};

	class Base;
	class Session;
	class Listener;

	//! VCEメインクラス
	/*!
	直接生成せずvceCreate()の戻り値を受け取るようにする。
	すべてを使い終わり削除するときはvceDeleteに渡す。
	複数生成する事も出来る。通常は一つ生成すれば良い。

	*/

	class VCE//abstract
	{
	public:
		//!VCEのメッセージポーリング
		/*!
		定期的に呼び出す事によりVCE内部メッセージを取り出す。
		このメソッドを呼び出したスレッドから各クラスのユーザ定義仮想関数が呼び出される。
		*/
		virtual void Poll()=0;

		//!VCEのメッセージポーリング時のWait
		/*!
        falseに設定することによりPoll関数を呼び出したときに何も処理するデータが無くてもWaitがかからないようにする。
		デフォルトではtrueなのでPollのみをループさせても内部でCPUリソースを独占しないようになっている。
		処理するデータがあるときはWait間隔が短くなるのでひとつのアプリケーションでCPUを独占しても良いときはfalseに。
		waitがtrueのままでも100MBPSの処理速度は十分ある。
		*/
		virtual void SetPollingWait(bool iswait)=0;

		//!接続する
		/*!
		セッションを使いhostname:portに接続する。
		\param s Sessionを派生させたクラスへのポインタを渡す。渡したポインタは内部で管理され、内部で使用されなくなると切り離される。
		\param hostname 接続先ホスト名若しくはIPアドレスを記述した文字列へのポインタ
		\param port 接続先TCPポート番号
		\param timeout 接続を試みる時間、ミリ秒単位。これ過ぎてもつながらなかったらEXCEPTIONコール。0ならずっと試行する
		\sa Session
		\return ユニークID
		*/
		virtual DWORD Connect(Session *s,const char *hostname,WORD port,unsigned int timeout=0)=0;
		virtual DWORD Connect(Session *s,const wchar *hostname,WORD port,unsigned int timeout=0)=0;
		//!接続を待ち受ける
		/*!
		portをリスニング(待ちうけ)する。
		\param l リスニングの待ちうけに使うリスナークラスへのポインタを渡す。渡したポインタは内部で管理され、使用されなくなると切り離される。
		\param port リスニングするTCPポート番号を指定する
		\param bindaddress リスニングするIPアドレスを指定する。NULLなら自分の持つ全アドレス。まだホスト名解決に対応してない
		\return ユニークID
		*/
		virtual DWORD Listening(Listener *l,WORD port,const char *bindaddress=NULL)=0;

		//!インターフェイスオブジェクトを取得する
		/*!
		param uID セッションまたはリスナーのuID
		return 有効なセッションまたはリスナー、無効な場合はNULL
		sa uid_cast
		*/
		virtual void*Get(DWORD uID)=0;

		template <typename T> T uid_cast(DWORD uID)
		{
			return (T)Get(uID);
		}

		//!セッション/リスニングオブジェクトを切り離す
		/*!
		ConnectやListeningに渡したオブジェクトをVCEの内部処理から切り離す。
		DetachされたオブジェクトからAPIは使用できなくなる。
        通常はBaseクラス内のデストラクタから呼ばれるので明示的にDetachする必要はない。
		\param s 切り離すオブジェクト
		\sa Connect
		\sa Listening
		*/
		virtual void Detach(Base *s)=0;

		//!使用中のListener/Sessionの数を返す
		virtual unsigned int GetUsed()=0;

		//!ログ出力用ストリーム
		/*!
		\return NULLもしくは出力ストリーム
		*/
		virtual std::ostream *SetLogger(std::ostream *target)=0;
		virtual std::ostream *Log()=0;
//		inline std::stringstream& Log_Warning(){return GetLogStream(0);}
	};

	//Utility
	//!16進の2文字を数値にする
	/*!
	"10"→16 "aa"→170 "FF"→255
	*/
	int XXtoi(const char *XX);

	//selfIP(s)
	size_t GetSelfAddress(unsigned int *iparray,size_t array_qt);


	//!DWORDなアドレスを文字列に変換
	/*!
	IPv4アドレスを文字列に変換する。
	戻り値はconst char*で、メモリは内部で確保してあるのを使いまわすので連続で使うときは注意する。
	メモリ内容は次に呼ばれるまで有効
	param ip アドレス
	return 文字列
	sa StringToAddr
	*/
	const char* AddrToString(DWORD ip);
	//!IPアドレスを数値に変換する
	/*!
	param ip アドレス。"192.168.1.2"のような。
	return 正しくないとき0xffffffffが返る。
	sa AddrToString
	*/
	DWORD StringToAddr(const char *ip);

	//!エンディアン変換
	/*!
	binのエンディアン（バイトオーダー）を変換する。
	\param bin 変換する32ビット値
	\return 変換された32ビット値
	*/
	DWORD bswap(DWORD bin);
	//!エンディアン変換
	/*!
	binのエンディアン（バイトオーダー）を変換する。
	\param bin 変換する16ビット値
	\return 変換された16ビット値
	*/
	WORD xchg(WORD bin);

	//!メモリ検索
	/*!
	メモリ範囲内同士の検索
	\param p 検索されるメモリ位置への先頭ポインタ
	\param sz_p 検索されるメモリサイズ
	\param cmp 検索するメモリ位置への先頭ポインタ
	\param sz_cmp 検索するメモリサイズ
	\return 見つかった場合は見つかった場所への先頭ポインタを返す。それ以外の場合はNULL
	*/
	void *memmem(const void *p,size_t sz_p,const void *cmp,size_t sz_cmp);
	//!文字列→数値変換
	/*!
	文字列を数字に変換する標準関数のatoi関数の廉価版
	標準関数のatoiほど精度は高くなく純粋な数字のみint型へ変換可能
	\param str 変換する文字列
	\return 数値、正常に変換出来なかった場合は0
	*/
	int atoi(const char *str);

	//!時間を取得する
	/*!
	ミリ秒単位の時間を取得する。32ビットなので49日に1回カウンタが一周する。
	\return 時間
	*/
	DWORD GetTime();

	//!プログラムを止める
	/*!
	ミリ単位(精度荒い)でプログラムを止める。止まってる間はCPUは他のプロセス/スレッドの処理が出来る。
	\param millsec 止める時間
	*/
	void ThreadSleep(DWORD millsec);

	//!文字列のバイト数を調べる。Null終端除く。
	/*
	strlenのバッファサイズ指定版。通常のstrlenはNULL終端されてない場合バッファオーバーランしてしまう。
	*/
//	size_t strnlen(const char *str,size_t buflen=(size_t)-1);
	size_t strlen_s(const char *str,size_t buflen);
	size_t strlen(const char *str);
	//!バッファの中の文字列がlen以下がチェックする
	/*!
	文字列の長さを調べて判断までしてくれる。
	*/
	bool isstrinbuf(const BYTE *start,const BYTE *end,size_t lesslen);
	//!文字列をコピーする
	/*!
	strcpyのバッファサイズ制限版
	*/
	void strncpy(char *dest,size_t destlen,const char *src,size_t srclen);
	//!文字列をコピーする
	/*!
	strcpyのバッファサイズ制限版
	*/
	void strcpy_s(char *dest,size_t destlen,const char *src);
	//!文字列を追加する
	/*!
	\return 追加した文字列の終端
	*/
	char *strpush(char *dest,const char *src);
	//!UTF8文字列の文字数を調べる
	/*!
	UTF8は一文字に１～４バイト使用してるので、文字数を数えにくい。そんなときはこの関数を使う。
	\param utf8string UTF8文字列
	\param buflen バッファサイズ
	\return 文字数
	*/
	size_t utfstrnlen(const utf8 *utf8string,size_t buflen);
	//!UTF16の文字数を調べる
	/*!
	UTF16の文字数を調べる。サロゲートペアに対応してない
	*/
	size_t wcslen(const wchar *str);
	//!UTF16→8の変換をする
	/*!
	UTF16→8の変換をする
	\param dest 出力先のchar配列
	\param destlen 出力先のバッファサイズ
	\param src UTF16の文字列
	*/
	size_t utf16to8(utf8 *dest,size_t destlen,const wchar *src);
	//!UTF8→16の変換をする
	/*!UTF8→16の変換をする
	\param dest 出力先のwchar配列
	\param destlen 出力先のバッファサイズ(バイト単位)
	\param src UTF8の文字列
	*/
	size_t utf8to16(wchar *dest,size_t destlen,const utf8 *src);
	//STL使うUtl
	//!設定ファイルを読み込みマップする
	/*!
	name=value 形式の文字列をconf["name"]==valueの状態にmapする。#記号があるとそこから後ろは無視される
	\param filename 設定ファイル
	*/
	std::map<std::string,std::string> LoadSimpleConfig(const char *filename);
	//!設定ファイルを保存する
	/*!
	未実装
	*/
	bool SaveSimpleConfig(const char *filename,std::map<std::string,std::string> &config);
	//!時間を文字列で取得する
	/*!
    "YYYY/MM/DD hh:mm:ss.msec"形式の文字列を取得する。ログ出力とかに使う
	\return 時間の文字列
	*/
	const char* NowTimeString();

	//!文字列を分ける
	/*!
	「aaa bbb "c c c"」があれば、三つに分けられる。二重引用符で囲める以外はエスケープ文字は無し。
	\return 分けられた文字列
	*/
	std::vector<std::string> SplitString(const char *string);
	//!POST文字列を分ける
	/*!
    「aaa=bb&ccc=a%20bc」があれば二つに分けられる。%XXは出コードされる。
	*/
	std::map<std::string,std::string> SplitPost(const char *post);

	//!URIエンコードする
	/*!
	a-z,A-Z,0-9を除くすべて。
	*/
	std::string encodeURI(const char *text);
	//!URIをデコードする
	/*!
	*/
	std::string decodeURI(const char *text);


	//!VCE内部向けクラス
	/*!
	内部で管理させるためのベースクラス
	ユーザがこのクラスを直接使用することはない
	*/

	class Base
	{
	protected:
		Base();
		virtual ~Base();
	public:
		QWORD cache;//未実装:(
		//!固有識別子
		DWORD uID;
		//!管理元
		VCE *api;
		
		virtual void Attached();
		//!VCE内部管理から切り離されたときに呼ばれる
		/*!
		ConnectやListening等に渡したクラスが使用されなくなると呼ばれる。
		Detachされた後ならdeleteする事ができる。
		*/
		virtual void Detached();
		//!思考関数
		/*!
		vce::Pollが呼ばれたときに一緒にこの関数も呼ばれる。オーバーライド可能。
		*/
		virtual void Think();

		bool operator ==(Base &target);
		bool operator !=(Base &target);
		//!同じオブジェクトかチェックする
		bool IsEqual(Base &target);

		//!状態を取得する
		/*!
		\sa VCE_STATE
		*/
		virtual VCE_STATE GetState();

		//!例外取得用関数
		/*!
		クリティカルでない例外が起こったときに呼ばれる。
		\sa VCE_EXCEPTION
		*/
		virtual void Exception(VCE_EXCEPTION type);

		//!socketハンドルを取得する
		int GetSocket();

		//!VCE内部で使われてるかどうか(deleteしたらダメかどうか)
		bool IsUsing();
	};


	//!VCEリスナークラス
	/*!
	Listenigされているポートへ接続が来たときに動作を定義するためのクラス。いわゆるサーバ。
	\sa vce::Listening
	\sa AutoAccepter
	*/
	class Listener
		:public Base
	{
	public:
		Listener();
		virtual ~Listener();
		//!接続要求
		/*!
		この関数をオーバーライドし、接続を受けるセッションクラスを返す。接続を拒否する場合はNULLを返す
		\param remote_addr 接続してきたアドレス
		\param remote_port 接続してきたポート
		\return NULLなら拒否
		*/
		virtual Session *Accepter(DWORD remote_addr,WORD remote_port)=0;

		void Close();
		//!セッション数、リストを取る
		/*!
		このリスナーから派生したセッション一覧を取得する。
		\param child 受け取るポインタ配列
		\param array_maxlen 受け取る配列に入れられる最大要素数
		\return セッションの数,若しくは配列にコピーした数
		*/
		unsigned int GetChilds(Session **child,unsigned int array_maxlen);
		unsigned int GetChilds();
		VCE_STATE GetState();
#ifndef NO_STL
		template <typename T> void GetChilds(std::vector<T*> &ch)
		{
			ch.resize(GetChilds());
			GetChilds((Session**)&ch[0],(unsigned int)ch.capacity());
		}
#endif
	};

	//!VCEストリームセッションクラス
	/*!
	セッションを操作するのに使う。このクラスを継承し、各仮想関数をオーバーライドしていく。
	\sa vce::Connect
	\sa Listener::Accept
	*/

	class Session
		:public Base
	{
	public:
		enum LowLevelFlags
		{
			LLF_NONE=0,
			LLF_ENCRYPT_DEFAULT=1,//未実装
		}llflags;

		//!リスナーへのポインタ
		/*!
		パッシブ接続の場合はリスニングしてた親クラスへのポインタが入る。
        アクティブ接続の場合はNULL
		*/
		Listener *parent;

		Session();
		virtual ~Session();

		//!セッションを閉じる
		/*!
		すぐ閉じる。

		\sa Close
		\sa Closed
		*/
		void ForceClose();
		//!送信予定のデータを送信し終わったらセッションを閉じる
		/*!
		すぐ切断せず、送信バッファが空になると送信を閉じ、受信し終わると受信も閉じる。
		単発のリクエストを出し、それに対するリプライだけを受け取る時等に使うと便利。
		\sa ForceClose
		\sa Closed
		*/
		void Close();
		//!接続試行をやめる
		/*!
		Connectしたあとにtimeoutを待たずにあきらめるときに使う。この後にExceptionにConnectFailコールされる。
        \sa vce::Connect
		*/
        void CancelConnecting();
		//!データを送信する
		/*!
		データを送信バッファに書き込み、送信する。
		\param p 送信データへの先頭ポインタ
		\param sz 送信データのバイトサイズの長さ
		\return 送信したデータ長、szの値がそのまま返る
		*/
		size_t Send(const void *p,size_t sz);
		//!受信バッファをロックし、受信バッファのデータを参照する
		/*!
		受信バッファの内容を直接参照するにはこの関数を使う。
		ロックしたら必ずアンロックしなければならない。

		\param sz OUT,受信バッファのバイトサイズの長さ
		\return 受信バッファへの先頭ポインタ、szまでの間は参照のみ可能。NULLの場合は失敗。
		\sa Unlock_Readbuf
		*/
		const BYTE *Lock_Readbuf(size_t &sz);
		//!受信バッファをアンロックし、受信バッファのデータを削除する
		/*!
		ロックした受信バッファをアンロックするのに使う。
		また受信バッファの先頭からのデータを同時に削除する事が出来る。
		\param shrink 削除するバイトサイズの長さ
		*/
		void Unlock_Readbuf(size_t shrink);

		//!接続が完了したときに呼ばれる
		virtual void Connected();
		//!接続が閉じたときに呼ばれる
		virtual void Closed(VCE_CLOSEREASON type);
		//!なんらかのデータを受信したときに呼ばれる
		/*!
		この仮想関数をオーバーライドし受信処理を書く。
		TCPセッションなら受信したTCPストリームのデータがそのまま来る。
	      
		\param p 受信データへのポインタ
		\param sz 受信データのバイトサイズ
		\return 処理したバイト数、0なら受信バッファのデータは削除しない。1以上ならバッファの先頭からバイト単位で削除する。
		*/
		virtual size_t Recv(const BYTE *p,size_t sz);

		//!接続先のIPv4TCPアドレスを取得する
		/*!
		\param addr IPアドレスを格納する変数への参照
		\param port ポート番号を格納する変数への参照
		\return 取得できたらtrue、できなかったらfalse
		*/
		bool GetRemoteAddress(DWORD &addr,WORD &port);

		//!接続元のIPv4TCPアドレスを取得する
		/*!
		\param addr IPアドレスを格納する変数への参照
		\param port ポート番号を格納する変数への参照
		\return 取得できたらtrue、できなかったらfalse
		*/
		bool GetLocalAddress(DWORD &addr,WORD &port);

		//!送信バッファの使用量
		/*!
		送信バッファが使用量が増えるとメモリ再確保が発生するため、シビアな状況ではこの関数でチェックしながら送信するようにする。
		\return 送信バッファ使用量
		*/
		size_t GetUsageSendBuffer();

		VCE_STATE GetState();
	};

	//!自動セッション開始クラス
	/*!
	接続が来たときに自動でサーバ用セッションを生成し、返すためのテンプレートクラス。
	このクラスを生成しListeningに直接渡すだけでよい。
	\sa net::Listening
	*/
	template <class T>class AutoAccepter:public Listener
	{
		bool enable;
		const unsigned int maxconnections;
	protected:
		virtual Session *Accepter(DWORD remote_addr,WORD remote_port){
			if(maxconnections&&GetChilds()>=maxconnections)
				return NULL;
			return enable?new T:NULL;};
	public:
		AutoAccepter(int maxconn=0):maxconnections(maxconn){
			enable=true;}
		//!今後の接続を許可する。
		void Enable(){
			enable=true;}
		//!今後の接続を拒否する。
		void Disable(){
			enable=false;}
		void Detached(){
			delete this;}
	};

	//!VCEパケットレイヤークラス
	/*!
	Sessionの派生クラスでストリームデータをパース/マージし、パケットデータにしてデータの送受信をする。
	送信(マージ)したパケットサイズ分、受信(パース)されて処理する事ができる。
	ストリームの場合、5バイトを2回送信すると10バイトいっぺんに受信され、データに区切りが無いので、区切るときはこちらを使う。
	*/
	class Codec
		:public Session
	{
	private:
		//!内部で定義されている受信関数。使わない。
		size_t Recv(const BYTE *p,size_t sz);
	protected:
		//!パースされたパケットを受信するための関数
		/*!
		パースされ、パケット単位のデータが見つかると呼ばれる。
		\param p 受信データへのポインタ
		\param sz 
		*/
		virtual void Parsed(const BYTE *p,size_t sz)=0;
	public:
		//!データをマージし、送信する
		/*!
		データを送信するときに使う。szサイズ分のデータが受信側に到達し、パースされる
		\prama p 送信するデータへのポインタ
		\param sz 送信するデータのバイトサイズの長さ
		\return 失敗すると-1、成功した場合は0以上のパケットサイズ
		*/
		virtual int Merge(const void *p,size_t sz);
	};

	//!Multi Protocol Interface Node.
	class MPINode
		:public Codec
	{
		friend class MPISession;
	public:
		MPINode(unsigned char number);
		class MPISession *pnode;
		const unsigned char MPINumber;
	public:
		int Merge(const void *p,size_t sz);
	};

	//!Multi Protocol Interface System
	class MPISession
		:public Codec
	{
		std::map<unsigned char,MPINode*> node;
	protected:
		void Parsed(const BYTE *p,size_t sz);
	public:
//		int Merge(const void *p,size_t sz,unsigned char MPInum);
		void Register(MPINode *node);
		template <typename N>N*GetNode(){for(std::map<unsigned char,MPINode*>::iterator i=node.begin();i!=node.end();i++)if(dynamic_cast<N*>(i->second))return dynamic_cast<N*>(i->second);return NULL;}
	protected://wrapper
		void Attached();//全nodeのAttachedを呼ぶ
		void Connected();//以下同じ
		void Closed(VCE_CLOSEREASON type);
		void Detached();
		void Exception(VCE_EXCEPTION type);
		void Think();
	};
	//!単純HTTPサーバ
	/*!
	*/
	class HTTPServer
		:public Session
	{
		size_t Recv(const vce::BYTE *p,size_t sz);
	protected:
		virtual bool Request(std::string method,std::string URI,std::map<std::string,std::string> header,const char *body,size_t body_len)=0;
		void Response(int status,std::map<std::string,std::string> header,const char *body,size_t body_len);
	};



	//Monitorを使うにはdynamic_castを有効にしなければならない
	//!VCE応用クラス、内部情報を見るためのモニター
	/*!
	HTTPを使うブラウザ上からVCEの状況を見ることが出来る。
	vce->Listening(new vce::AutoAccepter<vce::Monitor>,8080);とすると8080番ポートを使いモニタリングできる
	*/
	class Monitor
		:public Session
	{
		char lastpath[1024];
	public:
		Monitor();
		~Monitor();
	private:
		void Connected();
		void Closed(VCE_CLOSEREASON type);
		void Exception(VCE_EXCEPTION type);
		size_t Recv(const BYTE *p,size_t sz);
		void SendHTML(const char *str);
		void SendText(const char *str);
		void Reply(const char *path,const char *get_param,const char *post);
	};

	//!Monitor Extension
    /*!
	モニターと関連していろいろ使うためのベースクラス。SessionやListenerと一緒に多重継承することで使えるようになる。dynamic_castが無効になってると使えない。
	*/
	class MonitorConsole
	{
		friend class Monitor;
	protected:
		MonitorConsole();
		virtual ~MonitorConsole();
		//!コマンドを受け取ったときに呼ばれる。
		virtual void Request(const char *command );
	public:
		//!ログ出力用のストリーム
		std::stringstream clog;
	};

	//!Monitor おまけ
	/*!
    MonitorとそれにMonitorConsoleをくっつけたオマケクラス。
	これを継承してRequestを再定義する。
	*/
	class MonitorConsoleListener
		:public AutoAccepter<Monitor>
		,public MonitorConsole
	{
	};

	class MonitorStatusDetail
	{
		friend class Monitor;
	protected:
		MonitorStatusDetail();
		~MonitorStatusDetail();
		//!コマンドとは別のPOSTが来た
		virtual void Post(const char *post);
	public:
		//!詳細ページ用のステータスHTML
		std::stringstream html;
	};

	//!vceオブジェクトを生成する
	/*!
	まずVCEオブジェクトを作ってAPIを使えるようにする。複数作ることも出来る
	*/
	VCE *VCECreate(std::ostream *log=NULL);
	//!vceオブジェクトを削除する
	/*!
	VCECreateで作られたオブジェクトは必ずこの関数で削除しなければならない。メモリ開放や各種後処理をする
	*/
	void VCEDelete(VCE *n);
}
