/*------------------------------------------------------------------------------
	WinSystem.h
	WINDOWS の必要なヘッダをまとめたもの
	これをインクルードすれば大抵の事は出来るようになる
--------------------------------------------------------------------------------*/

// これをしておくと多重定義がなくなる --
#ifndef __WINSYSTEM_H_
#define __WINSYSTEM_H_

// デバック関連 -------------
#define	DEBUG_			( 0x00 )

// 通常マクロ ------------------------------------------------------
#include <windows.h>		// Windows
#include <winnls32.h>		// IME
#include <winsock.h>
#include <mmsystem.h>		//
#include <stdlib.h>			//
#include <stdio.h>			//
#include <time.h>			// srand(...) rand(...)
#include <math.h>			// sin(...) cos(...) acos(..)
#include <conio.h>			// 
#include <string.h>			//
#include <memory.h>			//
#include <io.h>				//
#include <fcntl.h>			//
#include <nspapi.h>			//
#include <sys/stat.h>		//
#include <fcntl.h>			//

#define D3D_OVERLOADS		// D3DVECTOR() ... other
#include <d3d.h>			// Direct3D

#include <ddraw.h>			// Direct3D
#include <dinput.h>			// DirectInput
#include <dsound.h>			// DirectSound
#include <d3dx.h>			// Direct3DX
#include <dmusicc.h>
#include <dmusici.h>

// ウインドウモードマクロ ------------------------------------------------
#define	MODE_FULLSCREEN_	( 0x00 )			// フルスクリーン
#define	MODE_WINDOW_		( 0x01 )			// ウインドウモード

// 初期化用マクロ( MODE_WINDOW_ か MODE_FULLSCREEN_ ) --------------------
#if 1
#define	GAMEMODE_			MODE_WINDOW_		// ここを変更するとモードの変更
#else
#define	GAMEMODE_			MODE_FULLSCREEN_	// ここを変更するとモードの変更
#endif

#define	WINDOW_SIZE_X_		( 640 )				// ウインドウの幅（ 今のところ変更不可 ）
#define	WINDOW_SIZE_Y_		( 480 )				// ウインドウの高さ（ 今のところ変更不可 ）

// 計算用マクロ ----------------------------------------------------------
#define	PI_					( 3.141592f )		// 円周率
#define	RD_					( PI_ / 180.0 )		// 角度からラジアンの算出用
#define	DEG_				( 180.0f / PI_ )	// ラジアンから角度の算出用

// 設定関連マクロ --------------------------------------------------------
#define	OFF_				( 0x00 )
#define	ON_					( 0x01 )

// 値の段階マクロ --------------------------------------------------------
#define	STEP0_				( 0x00 )
#define	STEP1_				( 0x01 )
#define	STEP2_				( 0x02 )
#define	STEP3_				( 0x03 )
#define	STEP4_				( 0x04 )
#define	STEP5_				( 0x05 )
#define	STEP6_				( 0x06 )
#define	STEP7_				( 0x07 )
#define	STEP8_				( 0x08 )

// 終了
#endif

/*------------------------------------------------------------------------------
	WinSystem.h		End of File
--------------------------------------------------------------------------------*/
