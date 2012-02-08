/*------------------------------------------------------------------------------
	WinSystem.h
	WINDOWS �̕K�v�ȃw�b�_���܂Ƃ߂�����
	������C���N���[�h����Α��̎��͏o����悤�ɂȂ�
--------------------------------------------------------------------------------*/

// ��������Ă����Ƒ��d��`���Ȃ��Ȃ� --
#ifndef __WINSYSTEM_H_
#define __WINSYSTEM_H_

// �f�o�b�N�֘A -------------
#define	DEBUG_			( 0x00 )

// �ʏ�}�N�� ------------------------------------------------------
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

// �E�C���h�E���[�h�}�N�� ------------------------------------------------
#define	MODE_FULLSCREEN_	( 0x00 )			// �t���X�N���[��
#define	MODE_WINDOW_		( 0x01 )			// �E�C���h�E���[�h

// �������p�}�N��( MODE_WINDOW_ �� MODE_FULLSCREEN_ ) --------------------
#if 1
#define	GAMEMODE_			MODE_WINDOW_		// ������ύX����ƃ��[�h�̕ύX
#else
#define	GAMEMODE_			MODE_FULLSCREEN_	// ������ύX����ƃ��[�h�̕ύX
#endif

#define	WINDOW_SIZE_X_		( 640 )				// �E�C���h�E�̕��i ���̂Ƃ���ύX�s�� �j
#define	WINDOW_SIZE_Y_		( 480 )				// �E�C���h�E�̍����i ���̂Ƃ���ύX�s�� �j

// �v�Z�p�}�N�� ----------------------------------------------------------
#define	PI_					( 3.141592f )		// �~����
#define	RD_					( PI_ / 180.0 )		// �p�x���烉�W�A���̎Z�o�p
#define	DEG_				( 180.0f / PI_ )	// ���W�A������p�x�̎Z�o�p

// �ݒ�֘A�}�N�� --------------------------------------------------------
#define	OFF_				( 0x00 )
#define	ON_					( 0x01 )

// �l�̒i�K�}�N�� --------------------------------------------------------
#define	STEP0_				( 0x00 )
#define	STEP1_				( 0x01 )
#define	STEP2_				( 0x02 )
#define	STEP3_				( 0x03 )
#define	STEP4_				( 0x04 )
#define	STEP5_				( 0x05 )
#define	STEP6_				( 0x06 )
#define	STEP7_				( 0x07 )
#define	STEP8_				( 0x08 )

// �I��
#endif

/*------------------------------------------------------------------------------
	WinSystem.h		End of File
--------------------------------------------------------------------------------*/
