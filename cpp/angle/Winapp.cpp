/*------------------------------------------------------------------------------
	WinApp.cpp
	ウインドウズプログラムの基本的なことをする関数
--------------------------------------------------------------------------------*/

/*-----------------------------------
	ライブラリのリンク設定
-------------------------------------*/
#pragma comment( lib, "winmm.lib" )

/*------------------------------------
	システム関数ヘッダ
--------------------------------------*/
#include "winsystem.h"			// WINDOWS 標準装備
#include "lib.h"				// プログラム補助

// ゲーム関連 -----------------------------------
#define	GAME_NAME_	"ベクトル同士の内積"		// アプリケーションの名前

// １６ビットピクセルに変換するマクロ
#define	RGB_MAKE16( r, g, b )	( (( r >> 3 ) << 11 ) | (( g >> 2 ) << 5 ) | (( b >> 3 ) << 0 ) )

/*----------------------------------
	メッセージハンドラ関数
------------------------------------*/
LRESULT CALLBACK 	WndProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static	HWND 		WindowCreate( char *appname, char *name, char mode, int sw, int sh, HINSTANCE hinst );

/*-------------------------------------------------------------------------
	ウインドウズプログラムのメイン部分
---------------------------------------------------------------------------*/
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	/*--------------------------
		変数の宣言
	----------------------------*/
	static char psAppName[] = "WndApp";
	MSG			msg;							// メッセージ
	WNDCLASS	wc;								// ウインドウクラス
	HWND		hwnd;							// ウインドウハンドル

	//	ウィンドウクラスの設定
	wc.lpszClassName = psAppName;				// ウインドウクラスネーム（詳細不明）
	wc.lpfnWndProc   = WndProc;					// ウインドウプロシージャ名（WndProcにする）
	wc.style         = NULL;		
	wc.cbClsExtra    = 0;						// 未使用。0 にする。
	wc.cbWndExtra    = 0;						// 未使用。0 にする。
	wc.lpszMenuName  = "ICON_MENU";				// 未使用。NULL でもＯＫ
	wc.hbrBackground = ( HBRUSH)GetStockObject( BLACK_BRUSH );				// <-背景色
	wc.hInstance     = hInstance;											// インスタンスハンドル
	wc.hIcon         = LoadIcon( 0, IDI_APPLICATION );						// アイコン
	wc.hCursor       = LoadCursor( 0, IDC_ARROW );							// カーソルタイプ

	// ウインドウクラスを登録
	RegisterClass( &wc );

	// ウインドウの作成
	hwnd = WindowCreate( psAppName, GAME_NAME_, 1, WINDOW_SIZE_X_, WINDOW_SIZE_Y_, hInstance );

	// ウィンドウを表示
	ShowWindow ( hwnd, nCmdShow );			// ウインドウを表示
	UpdateWindow( hwnd );					// ウインドウの更新

	/*----------------------------------------------------
		ゲームのメイン部分
	------------------------------------------------------*/
	// メッセージループ	ゲーム終了まで永久ループ
  	while( TRUE ){	
		// メッセージの有りの場合
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ){	
			// プログラム終了要求
			if( msg.message == WM_QUIT )	break;
			// メッセージの解析
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	// アプリ終了
	return	0;
}

/*-------------------------------------------------------------------------------------
	メッセージハンドラ	Windowsからメッセージが来るとここにくる
---------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	HDC				hdc;				// デバイスコンテキスト
	PAINTSTRUCT		ps;					// 
	D3DVECTOR		v1, v2;				// ベクトル
	float			dot, ang;			// 内積計算用
	static float	angle1 = 0.0f;		// ベクトルの角度（ これからベクトルを計算 ）
	static float	angle2 = 0.0f;		// ベクトルの角度（ これからベクトルを計算 ）
	char			str[ 128];

	/*--------------------------------------------
		Windows からのメッセージにより分岐
	----------------------------------------------*/
	switch ( message ){						
	// 作成時 ---------------------------------------------------------
	case WM_CREATE:
		break;
	// ウインドウがアクティブになったとき -----------------------------
	case WM_ACTIVATE:

		// 描画する
		InvalidateRect( hwnd, NULL, TRUE );

		break;
	// キー入力関連 ----------------------------------------------------
	case WM_KEYDOWN:
        switch( wParam ){					// キーに応じて処理 
		case VK_F12:						// 強制終了ボタン
			// Windows へプログラム終了の合図
			PostMessage( hwnd, WM_CLOSE, 0, 0 );	
			break;
		/*----------------------------------
			ベクトルの回転（ 角度 ）
		------------------------------------*/
		case VK_UP:
			// 回転
			angle1 += 2.0f;
			break;
		case VK_DOWN:
			// 回転
			angle1 -= 2.0f;
			break;
		case VK_RIGHT:
			// 回転
			angle2 += 2.0f;
			break;
		case VK_LEFT:
			// 回転
			angle2 -= 2.0f;
			break;
		}
		// 描画する
		InvalidateRect( hwnd, NULL, TRUE );
		break;	
	// ウインドウを閉じようとするとき ----------------------------------
	case WM_CLOSE:
// 現在未使用機能 ------------
#if 0
		// メッセージボックスで終了の確認
		Dx.SetMessageBox();
		ret = MessageBox( hwnd, "ツールを終了しますか？", "確認", MB_YESNO | MB_ICONQUESTION ); 
		// いいえなら終了しない
		if( ret == IDNO )	return 0;
#endif
		break;
	// プログラム終了 --------------------------------------------------
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	// 描画要求を行ったとき-----------------------------------------------
	case WM_PAINT:

		// ベクトルを計算（ 正規化 ）
		v1 = D3DVECTOR( ( float)cos( angle1 * RD_ ), ( float)sin( angle1 * RD_ ), 0.0f );
		v2 = D3DVECTOR( ( float)cos( angle2 * RD_ ), ( float)sin( angle2 * RD_ ), 0.0f );

		// 内積を計算
		dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

		// 角度を取得
		// この角度は１８０度までしか調べられないので注意
		ang = ( float)acos( dot ) * DEG_;

		/*------------------------------------------
			描画関連
		--------------------------------------------*/

		// 描画開始
		hdc = BeginPaint( hwnd, &ps );

		// ベクトルを描画
		LineDisp( hdc, 320, 240, 320 + ( int)( 128 * v1.x ), 240 + ( int)( 128 * v1.y ), 0xFFFFFF );

		// ベクトルを描画
		LineDisp( hdc, 320, 240, 320 + ( int)( 128 * v2.x ), 240 + ( int)( 128 * v2.y ), 0x00FF00 );

		/*------------------------------------
			操作説明
		--------------------------------------*/

		// テキストの設定
		SetTextColor( hdc, 0xFFFFFF ); 
		SetBkMode( hdc, TRANSPARENT );

		// 文字表示
		TextOut( hdc, 0,   0, "ベクトル１", 10 );
 		TextOut( hdc, 0,  16, "ベクトル２", 10 );
		// 線の種類
		LineDisp( hdc, 96, 8, 160, 8, 0xFFFFFF );
		LineDisp( hdc, 96, 24, 160, 24, 0x00FF00 );

		// 文字描画
		sprintf( str, "内積結果 = %f", dot );
 		TextOut( hdc, 0,  32, str, strlen( str ) );
		sprintf( str, "角度 = %f", ang );
 		TextOut( hdc, 0,  48, str, strlen( str ) );

		// 説明
		sprintf( str, "上下左右キーでベクトルの回転", ang );
 		TextOut( hdc, 0, 464, str, strlen( str ) );

		// 描画終了
		EndPaint( hwnd, &ps );

		break;
	// デフォルト ------------------------------------------------------
	default:

		// 通常の関数処理
		return DefWindowProc( hwnd, message, wParam, lParam);
	}

	// 通常の関数処理
	return DefWindowProc( hwnd, message, wParam, lParam);
}

/*--------------------------------------------------------------------------------
	機能：	ウインドウの作成関数
	引数：	*appname	ウインドウクラスの名前
			*name		アプリケーションの名前
			mode		ウインドウのモード
				MODE_WINDOW_		ウインドウモード
				MODE_FULLSCREEN_	フルスクリーン
----------------------------------------------------------------------------------*/
static HWND WindowCreate( char *appname, char *name, char mode, int sw, int sh, HINSTANCE hinst )
{
	HWND	hwnd;

	// ウィンドウの作成
	if( mode == MODE_WINDOW_ ){					// ウインドウモードの場合

		hwnd = CreateWindowEx(
			0,									// 普通の位置に作成
			appname,							// ウィンドウを作るウインドウクラスの名前を入れる
			name,								// アプリケーションの名前
			// ウインドウめにゅう
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,	
#if 1
			GetSystemMetrics( SM_CXSCREEN ) / 2 - ( sw / 2 ),	// Ｘ（横）の位置
			GetSystemMetrics( SM_CYSCREEN ) / 2 - ( sh / 2 ),	// Ｙ（縦）の位置
#else
			152,	// Ｘ（横）の位置
			24,		// Ｙ（縦）の位置
#endif
			sw + 6,								// Ｘ（横）の幅
			sh + 25,							// Ｙ（縦）の幅
			NULL,								// 親ウインドウ無し
			NULL,								// クラスネーム指定時、NULL
			hinst,								// ウインドウインスタンスハンドル			
			NULL 								// NULLでＯＫ
		);
	}
	else{										// フルスクリーンモードの場合

		hwnd = CreateWindowEx(
			WS_EX_TOPMOST,						// 最前面ウィンドウを作成（一番前）
			appname,							// ウィンドウを作るウインドウクラスの名前を入れる
			name,								// アプリケーションの名前
			WS_POPUP,							// ウインドウめにゅう
			0,									// Ｘ（横）の位置
			0,									// Ｙ（縦）の位置
			GetSystemMetrics( SM_CXSCREEN ),	// Ｘ（横）の幅
			GetSystemMetrics( SM_CYSCREEN ),	// Ｙ（縦）の幅
			NULL,								// 親ウインドウ無し
			NULL,								// クラスネーム指定時、NULL
			hinst,								// ウインドウインスタンスハンドル			
			NULL 								// NULLでＯＫ
		);
	}

	// 作成したウインドウハンドルを返す
	return ( hwnd );
}

/*------------------------------------------------------------------------------
	WinApp.cpp		End of File
--------------------------------------------------------------------------------*/
