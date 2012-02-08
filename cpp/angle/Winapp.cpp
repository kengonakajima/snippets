/*------------------------------------------------------------------------------
	WinApp.cpp
	�E�C���h�E�Y�v���O�����̊�{�I�Ȃ��Ƃ�����֐�
--------------------------------------------------------------------------------*/

/*-----------------------------------
	���C�u�����̃����N�ݒ�
-------------------------------------*/
#pragma comment( lib, "winmm.lib" )

/*------------------------------------
	�V�X�e���֐��w�b�_
--------------------------------------*/
#include "winsystem.h"			// WINDOWS �W������
#include "lib.h"				// �v���O�����⏕

// �Q�[���֘A -----------------------------------
#define	GAME_NAME_	"�x�N�g�����m�̓���"		// �A�v���P�[�V�����̖��O

// �P�U�r�b�g�s�N�Z���ɕϊ�����}�N��
#define	RGB_MAKE16( r, g, b )	( (( r >> 3 ) << 11 ) | (( g >> 2 ) << 5 ) | (( b >> 3 ) << 0 ) )

/*----------------------------------
	���b�Z�[�W�n���h���֐�
------------------------------------*/
LRESULT CALLBACK 	WndProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
static	HWND 		WindowCreate( char *appname, char *name, char mode, int sw, int sh, HINSTANCE hinst );

/*-------------------------------------------------------------------------
	�E�C���h�E�Y�v���O�����̃��C������
---------------------------------------------------------------------------*/
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	/*--------------------------
		�ϐ��̐錾
	----------------------------*/
	static char psAppName[] = "WndApp";
	MSG			msg;							// ���b�Z�[�W
	WNDCLASS	wc;								// �E�C���h�E�N���X
	HWND		hwnd;							// �E�C���h�E�n���h��

	//	�E�B���h�E�N���X�̐ݒ�
	wc.lpszClassName = psAppName;				// �E�C���h�E�N���X�l�[���i�ڍוs���j
	wc.lpfnWndProc   = WndProc;					// �E�C���h�E�v���V�[�W�����iWndProc�ɂ���j
	wc.style         = NULL;		
	wc.cbClsExtra    = 0;						// ���g�p�B0 �ɂ���B
	wc.cbWndExtra    = 0;						// ���g�p�B0 �ɂ���B
	wc.lpszMenuName  = "ICON_MENU";				// ���g�p�BNULL �ł��n�j
	wc.hbrBackground = ( HBRUSH)GetStockObject( BLACK_BRUSH );				// <-�w�i�F
	wc.hInstance     = hInstance;											// �C���X�^���X�n���h��
	wc.hIcon         = LoadIcon( 0, IDI_APPLICATION );						// �A�C�R��
	wc.hCursor       = LoadCursor( 0, IDC_ARROW );							// �J�[�\���^�C�v

	// �E�C���h�E�N���X��o�^
	RegisterClass( &wc );

	// �E�C���h�E�̍쐬
	hwnd = WindowCreate( psAppName, GAME_NAME_, 1, WINDOW_SIZE_X_, WINDOW_SIZE_Y_, hInstance );

	// �E�B���h�E��\��
	ShowWindow ( hwnd, nCmdShow );			// �E�C���h�E��\��
	UpdateWindow( hwnd );					// �E�C���h�E�̍X�V

	/*----------------------------------------------------
		�Q�[���̃��C������
	------------------------------------------------------*/
	// ���b�Z�[�W���[�v	�Q�[���I���܂ŉi�v���[�v
  	while( TRUE ){	
		// ���b�Z�[�W�̗L��̏ꍇ
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) ){	
			// �v���O�����I���v��
			if( msg.message == WM_QUIT )	break;
			// ���b�Z�[�W�̉��
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	// �A�v���I��
	return	0;
}

/*-------------------------------------------------------------------------------------
	���b�Z�[�W�n���h��	Windows���烁�b�Z�[�W������Ƃ����ɂ���
---------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	HDC				hdc;				// �f�o�C�X�R���e�L�X�g
	PAINTSTRUCT		ps;					// 
	D3DVECTOR		v1, v2;				// �x�N�g��
	float			dot, ang;			// ���όv�Z�p
	static float	angle1 = 0.0f;		// �x�N�g���̊p�x�i ���ꂩ��x�N�g�����v�Z �j
	static float	angle2 = 0.0f;		// �x�N�g���̊p�x�i ���ꂩ��x�N�g�����v�Z �j
	char			str[ 128];

	/*--------------------------------------------
		Windows ����̃��b�Z�[�W�ɂ�蕪��
	----------------------------------------------*/
	switch ( message ){						
	// �쐬�� ---------------------------------------------------------
	case WM_CREATE:
		break;
	// �E�C���h�E���A�N�e�B�u�ɂȂ����Ƃ� -----------------------------
	case WM_ACTIVATE:

		// �`�悷��
		InvalidateRect( hwnd, NULL, TRUE );

		break;
	// �L�[���͊֘A ----------------------------------------------------
	case WM_KEYDOWN:
        switch( wParam ){					// �L�[�ɉ����ď��� 
		case VK_F12:						// �����I���{�^��
			// Windows �փv���O�����I���̍��}
			PostMessage( hwnd, WM_CLOSE, 0, 0 );	
			break;
		/*----------------------------------
			�x�N�g���̉�]�i �p�x �j
		------------------------------------*/
		case VK_UP:
			// ��]
			angle1 += 2.0f;
			break;
		case VK_DOWN:
			// ��]
			angle1 -= 2.0f;
			break;
		case VK_RIGHT:
			// ��]
			angle2 += 2.0f;
			break;
		case VK_LEFT:
			// ��]
			angle2 -= 2.0f;
			break;
		}
		// �`�悷��
		InvalidateRect( hwnd, NULL, TRUE );
		break;	
	// �E�C���h�E����悤�Ƃ���Ƃ� ----------------------------------
	case WM_CLOSE:
// ���ݖ��g�p�@�\ ------------
#if 0
		// ���b�Z�[�W�{�b�N�X�ŏI���̊m�F
		Dx.SetMessageBox();
		ret = MessageBox( hwnd, "�c�[�����I�����܂����H", "�m�F", MB_YESNO | MB_ICONQUESTION ); 
		// �������Ȃ�I�����Ȃ�
		if( ret == IDNO )	return 0;
#endif
		break;
	// �v���O�����I�� --------------------------------------------------
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	// �`��v�����s�����Ƃ�-----------------------------------------------
	case WM_PAINT:

		// �x�N�g�����v�Z�i ���K�� �j
		v1 = D3DVECTOR( ( float)cos( angle1 * RD_ ), ( float)sin( angle1 * RD_ ), 0.0f );
		v2 = D3DVECTOR( ( float)cos( angle2 * RD_ ), ( float)sin( angle2 * RD_ ), 0.0f );

		// ���ς��v�Z
		dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

		// �p�x���擾
		// ���̊p�x�͂P�W�O�x�܂ł������ׂ��Ȃ��̂Œ���
		ang = ( float)acos( dot ) * DEG_;

		/*------------------------------------------
			�`��֘A
		--------------------------------------------*/

		// �`��J�n
		hdc = BeginPaint( hwnd, &ps );

		// �x�N�g����`��
		LineDisp( hdc, 320, 240, 320 + ( int)( 128 * v1.x ), 240 + ( int)( 128 * v1.y ), 0xFFFFFF );

		// �x�N�g����`��
		LineDisp( hdc, 320, 240, 320 + ( int)( 128 * v2.x ), 240 + ( int)( 128 * v2.y ), 0x00FF00 );

		/*------------------------------------
			�������
		--------------------------------------*/

		// �e�L�X�g�̐ݒ�
		SetTextColor( hdc, 0xFFFFFF ); 
		SetBkMode( hdc, TRANSPARENT );

		// �����\��
		TextOut( hdc, 0,   0, "�x�N�g���P", 10 );
 		TextOut( hdc, 0,  16, "�x�N�g���Q", 10 );
		// ���̎��
		LineDisp( hdc, 96, 8, 160, 8, 0xFFFFFF );
		LineDisp( hdc, 96, 24, 160, 24, 0x00FF00 );

		// �����`��
		sprintf( str, "���ό��� = %f", dot );
 		TextOut( hdc, 0,  32, str, strlen( str ) );
		sprintf( str, "�p�x = %f", ang );
 		TextOut( hdc, 0,  48, str, strlen( str ) );

		// ����
		sprintf( str, "�㉺���E�L�[�Ńx�N�g���̉�]", ang );
 		TextOut( hdc, 0, 464, str, strlen( str ) );

		// �`��I��
		EndPaint( hwnd, &ps );

		break;
	// �f�t�H���g ------------------------------------------------------
	default:

		// �ʏ�̊֐�����
		return DefWindowProc( hwnd, message, wParam, lParam);
	}

	// �ʏ�̊֐�����
	return DefWindowProc( hwnd, message, wParam, lParam);
}

/*--------------------------------------------------------------------------------
	�@�\�F	�E�C���h�E�̍쐬�֐�
	�����F	*appname	�E�C���h�E�N���X�̖��O
			*name		�A�v���P�[�V�����̖��O
			mode		�E�C���h�E�̃��[�h
				MODE_WINDOW_		�E�C���h�E���[�h
				MODE_FULLSCREEN_	�t���X�N���[��
----------------------------------------------------------------------------------*/
static HWND WindowCreate( char *appname, char *name, char mode, int sw, int sh, HINSTANCE hinst )
{
	HWND	hwnd;

	// �E�B���h�E�̍쐬
	if( mode == MODE_WINDOW_ ){					// �E�C���h�E���[�h�̏ꍇ

		hwnd = CreateWindowEx(
			0,									// ���ʂ̈ʒu�ɍ쐬
			appname,							// �E�B���h�E�����E�C���h�E�N���X�̖��O������
			name,								// �A�v���P�[�V�����̖��O
			// �E�C���h�E�߂ɂイ
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,	
#if 1
			GetSystemMetrics( SM_CXSCREEN ) / 2 - ( sw / 2 ),	// �w�i���j�̈ʒu
			GetSystemMetrics( SM_CYSCREEN ) / 2 - ( sh / 2 ),	// �x�i�c�j�̈ʒu
#else
			152,	// �w�i���j�̈ʒu
			24,		// �x�i�c�j�̈ʒu
#endif
			sw + 6,								// �w�i���j�̕�
			sh + 25,							// �x�i�c�j�̕�
			NULL,								// �e�E�C���h�E����
			NULL,								// �N���X�l�[���w�莞�ANULL
			hinst,								// �E�C���h�E�C���X�^���X�n���h��			
			NULL 								// NULL�łn�j
		);
	}
	else{										// �t���X�N���[�����[�h�̏ꍇ

		hwnd = CreateWindowEx(
			WS_EX_TOPMOST,						// �őO�ʃE�B���h�E���쐬�i��ԑO�j
			appname,							// �E�B���h�E�����E�C���h�E�N���X�̖��O������
			name,								// �A�v���P�[�V�����̖��O
			WS_POPUP,							// �E�C���h�E�߂ɂイ
			0,									// �w�i���j�̈ʒu
			0,									// �x�i�c�j�̈ʒu
			GetSystemMetrics( SM_CXSCREEN ),	// �w�i���j�̕�
			GetSystemMetrics( SM_CYSCREEN ),	// �x�i�c�j�̕�
			NULL,								// �e�E�C���h�E����
			NULL,								// �N���X�l�[���w�莞�ANULL
			hinst,								// �E�C���h�E�C���X�^���X�n���h��			
			NULL 								// NULL�łn�j
		);
	}

	// �쐬�����E�C���h�E�n���h����Ԃ�
	return ( hwnd );
}

/*------------------------------------------------------------------------------
	WinApp.cpp		End of File
--------------------------------------------------------------------------------*/
