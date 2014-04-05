// PaintApp.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "PaintApp.h"

#define MAX_LOADSTRING 100

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �����ɃR�[�h��}�����Ă��������B
	MSG msg;
	HACCEL hAccelTable;

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PAINTAPP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINTAPP));

	// ���C�� ���b�Z�[�W ���[�v:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAINTAPP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PAINTAPP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

   //�E�B���h�E�̈ʒu�ƕ��̐ݒ�
   int pos_x = 200;
   int pos_y = 100;
   int witdh = 512;
   int height = 512;


   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   pos_x, pos_y, witdh, height, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//�A�j���[�V��������p��BITMAP���m��

	static HDC hFrame;
	static HBITMAP hBmp_a[8];
	int maxFrame = 9;
	int CurrentFrame = 0;

	static RECT window_rc; //�E�B���h�E�T�C�Y

	//BMP�ۑ��̂��߂̗p��
	static BITMAPFILEHEADER head = { 0 };
	static BITMAPINFOHEADER info = { 0 };


	//Color
	unsigned char color[2][3] ={
		{0,0,0},		//BLACK
		{255,255,255}   //White
	};

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static POINT pick_old, pick_new;
	static RECT erase_rc,rc;
	static bool pen = false,eraser = false;
	static int current_color = 0;
	static int pen_size = 10, pen_scale = 1;

	//static bool erase = false;

	//
	//----- UNdo -----//
	//
	static HDC hMem = NULL;  //�ۑ��p�f�o�C�X�R���e�L�X�g�p

	//Undo,Redo,���ݗp��3�̃o�b�t�@�ɃA�N�Z�X���邽�߂̃n���h��
	static HBITMAP hBmp[3] = {0}; //�r�b�g�}�b�v���w�������n���h��
	static RECT wnd_rc;			//�N���C�A���g�̈�T�C�Y���w�肷��Ƃ���



	// �J���[�_�C�A���O�Ɏg�p����\����
	static CHOOSECOLOR color_dialog = {0};
	static COLORREF		custom_colors[16];



	switch (message)
	{
	case WM_CREATE:

	//�A�j���[�V�����t���[������̂��߂̍s
		GetClientRect(hWnd, &window_rc);

		if( hdc = GetDC( hWnd ) ) {
			hFrame = CreateCompatibleDC( hdc );
			for ( int i = 0; i < maxFrame; i++){
				hBmp_a[i] = CreateCompatibleBitmap( hdc, window_rc.right, window_rc.bottom);
				SelectObject( hFrame, hBmp_a[i]);
				PatBlt( hFrame,0, 0, window_rc.right, window_rc.bottom, WHITENESS);
			}
			SelectObject( hFrame, hBmp_a[CurrentFrame]);
			ReleaseDC( hWnd,hdc);
		}


	//�I���

		//�A�v���P�[�V�����E�B���h�E�����߂ĊJ�����Ƃ���x�������������
		color_dialog.lStructSize	= sizeof(CHOOSECOLOR);
		color_dialog.hwndOwner		= hWnd;
		color_dialog.lpCustColors	= custom_colors;

		//�E�B���h�E�Y�̃T�C�Y���擾
		GetClientRect( hWnd, &wnd_rc );

		// 1. �ۑ��pBMP�f�o�C�X�R���e�L�X�Ƃ̏���
		hMem = CreateCompatibleDC( NULL );

		//�O�̂��߂�BitMap���N���A
		if ( hBmp[0] ) DeleteObject( hBmp[0] );
		if ( hBmp[1] ) DeleteObject( hBmp[1] );
		if ( hBmp[2] ) DeleteObject( hBmp[2] );

		//3���݂̃E�B���h�̃f�o�C�X�R���e�L�X�g�̎擾
		if( hdc = GetDC( hWnd ) ) {

			//�f�o�C�X�R���e�L�X�g�̎擾�ɐ���������
			//���݂̉�ʂƓ�����ނ́A�ۑ��p��BMP���쐬
			hBmp[0] = CreateCompatibleBitmap( hdc, wnd_rc.right, wnd_rc.bottom);
			hBmp[1] = CreateCompatibleBitmap( hdc, wnd_rc.right, wnd_rc.bottom);
			hBmp[2] = CreateCompatibleBitmap( hdc, wnd_rc.right, wnd_rc.bottom);

			//1�����I�����A���ɓh��Ԃ�
			//Undo
			SelectObject( hMem, hBmp[0] );
			PatBlt( hMem, wnd_rc.right, wnd_rc.top, wnd_rc.right,  wnd_rc.bottom, WHITENESS);

			//Redo�p
			SelectObject( hMem,hBmp[1]);
			PatBlt( hMem, wnd_rc.left, wnd_rc.top, wnd_rc.right, wnd_rc.bottom, WHITENESS);

			//Now�p
			SelectObject( hMem,hBmp[2] );
			PatBlt( hMem, wnd_rc.left, wnd_rc.top, wnd_rc.right, wnd_rc.bottom, WHITENESS);


			//5 ��t�f�o�C�X�R���e�L�X�Ƃ̉��
			ReleaseDC( hWnd, hdc);
		
		}

		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{

			//resorce.h���Ł@#define IDM_UNDO 600
			#define IDM_REDO 605

			//�ƒ�`

		case IDM_NEXT:
			if(CurrentFrame < maxFrame)
				CurrentFrame++;
			SelectObject(hFrame, hBmp_a[CurrentFrame]);
			InvalidateRect( hWnd,NULL, NULL );
			break;

		case IDM_PREVIOUS:
			if(CurrentFrame > 0)
				CurrentFrame--;
			SelectObject( hFrame, hBmp_a[CurrentFrame]);
			InvalidateRect( hWnd, NULL, NULL);
			break;


		case IDM_SAVE:
			//Ctrl+S�L�[
			{
				DWORD buffSize;
				DWORD buffOneLine;
				DWORD writeSize;
				DWORD imgWidth, imgHeight;
				HANDLE fh;
				LPBYTE Pixels;

				imgWidth = wnd_rc.right;
				imgHeight = wnd_rc.bottom;

				//bmp��1���C�����v�Z
				//bmp�̕���4�o�C�g�Ŋ���؂��K�v������
				if(!( (imgWidth*3) %4 ) )
					buffOneLine = imgWidth * 3;
				else
					buffOneLine = imgWidth * 3 + ( 4- ( imgWidth * 3) % 4);

				//�ۑ��p�o�b�t�@�T�C�Y�̌v�Z
				buffSize = buffOneLine * imgHeight;

				//�ۑ��p�o�b�t�@���m��
				Pixels = ( LPBYTE )GlobalAlloc( GPTR, buffSize );

				//�w�b�_�ɏ����i�[
				head.bfType = 'MB';
				head.bfSize = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER ) + buffSize;
				head.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );
				info.biSize = sizeof( BITMAPINFOHEADER );
				info.biHeight = wnd_rc.bottom;
				info.biWidth = wnd_rc.right;
				info.biBitCount = 24;
				info.biPlanes = 1;

				if( hdc = GetDC( hWnd) ){
					//BMP����摜�������o��
					GetDIBits( hMem, hBmp[0], 0, info.biHeight, Pixels, ( LPBITMAPINFO )( &info ), DIB_RGB_COLORS );

					//�o�b�t�@���t�@�C���ɏ����o��
					fh = CreateFile( L"C:\\Users\\kawabata_iMAC\\Documents\\test.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
					WriteFile( fh, &head, sizeof( BITMAPFILEHEADER ), &writeSize, NULL );
					WriteFile( fh, &info, sizeof( BITMAPINFOHEADER ), &writeSize, NULL );
					WriteFile( fh, Pixels, buffSize, &writeSize, NULL);
					CloseHandle( fh );
					
					ReleaseDC( hWnd, hdc );
				}
				//�ۑ��p�o�b�t�@�����
				GlobalFree( Pixels );
			}
			break;


		case IDM_UNDO:

			//���݂̃f�o�C�X�R���e�L�X���擾
			if( hdc = GetDC( hWnd ) ){
				//Undo�p�r�b�g�}�b�v����ɃZ�b�g

				SelectObject( hMem, hBmp[0]);
				//�o�b�t�@�����݂̉摜�ɃR�s�[
				//���݂̉�ʂɈ�M�O�̏����R�s�[
				BitBlt( hdc, 0, 0, wnd_rc.right,wnd_rc.bottom, hMem,0,0, SRCCOPY );

				//���݂�BitMap��I��
				SelectObject( hMem, hBmp[2] );
				//���݂̉摜�����ݗp��BitMap�ɃR�s�[
				BitBlt(hMem, 0, 0, wnd_rc.right, wnd_rc.bottom, hdc, 0, 0, SRCCOPY );

				//��Еt��
				ReleaseDC( hWnd, hdc);
			}
			break;
		case IDM_REDO:
			//���݂̃f�o�C�X�R���e�L�X�g�́i��p��������)
			if( hdc = GetDC(hWnd) ){
				//REDO�p�r�b�g�}�b�v����p���ɕۑ�
				SelectObject( hMem, hBmp[1] );
				//�o�b�t�@�i�r�b�g�}�b�v�j�����݂̉�ʂɃR�s�[
				BitBlt( hdc, 0, 0, wnd_rc.right, wnd_rc.bottom, hMem, 0, 0, SRCCOPY);

				//���݂̉摜��ۑ�
				SelectObject( hMem, hBmp[2] );
				//���݂̉摜�����ݗp�̃r�b�g�}�b�v�ɃR�s�[
				BitBlt( hMem, 0, 0, wnd_rc.right, wnd_rc.bottom, hdc, 0, 0, SRCCOPY );


				//��Еt��
				ReleaseDC( hWnd, hdc );
			}
			break;

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		case IDM_CHANGECOLORDLG:
			//�ݒ� > �F�̑I�� > ���j���[���邢��Ctrl + Shift + c
			//1�J���[�_�C�A���O���J��
			ChooseColor(&color_dialog);
			break;

		case IDM_CLEAR:
			//��ʂ������@�S����
			InvalidateRect( hWnd, NULL, TRUE );
			/*
			//RECT�^�̕ϐ�erase_rc���`
			RECT erase_rc;
			erase_rc.bottom = 250;
			erase_rc.left = 200;
			erase_rc.right	 = 450;
			erase_rc.top	= 100;
			//erase_rc�Œ�`�����ꏊ���폜
			InvalidateRect( hWnd, &erase_rc, TRUE);
			*/
			break;

		case IDM_CHANGECOLOR:
			//x,X�������ꂽ���̐F�̕ύX
			if( ++current_color >= 2 ) current_color = 0;
			break;

		case IDM_CHANGEPENSIZEMAINA:
			//[�@�������ꂽ�Ƃ��y���T�C�Y���k���@1/2��
			if(pen_size <= 2) {
				pen_size = 1;
			}else{
				pen_size = (unsigned)(pen_size * 0.5f);
			}
			break;
		case IDM_CHANGEPENSIZEPLUS:
			pen_size = pen_size * 2;
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_LBUTTONDOWN:

		//�}�E�X�̍��{�^�������������̏���
		pen = true;

		//Undo�pBitMap��I��
		SelectObject( hMem, hBmp[0] );

		if ( hdc = GetDC(hWnd) ){
			//Undo�pBitMap�Ɍ��݂̉摜���R�s�[
			BitBlt( hMem, 0, 0, wnd_rc.right, wnd_rc.bottom, hdc, 0, 0, SRCCOPY);
			//SelectObject( hdc.
			ReleaseDC(hWnd, hdc);
		}

		//�y���̏�����
		pick_old.x = pick_new.x = LOWORD( lParam );
		pick_old.y = pick_new.y = HIWORD( lParam );

		break;

	case WM_LBUTTONUP:
		{
			SelectObject( hMem, hBmp[1]);

			//���{�^�������������̏���
			pen_scale = 1;

			//�}�E�X�̍��{�^���𗣂����Ƃ��̏���
			pen = false;

			//�Ō�̂����ۂ��c��Ȃ��悤�Ɉʒu��������
			pick_old.x = pick_new.x = LOWORD( lParam );
			pick_old.y = pick_new.y = HIWORD( lParam );

			//Redo�p�r�b�g�}�b�v��I��
			SelectObject( hMem, hBmp[1] );
			//���݂̉摜�����h�D�pBitMap�ɃR�s�[
			if( hdc = GetDC( hWnd) ){
				BitBlt( hMem, 0, 0, wnd_rc.right, wnd_rc.bottom, hdc, 0, 0, SRCCOPY );
				//SelectObject(hdc, hOldObj);
				ReleaseDC(hWnd, hdc);
			}
		}


		break;

	case WM_RBUTTONDOWN:
		//�}�E�X�̉E�{�^�������������̏���
		eraser = true; //�폜
		break;

	case WM_RBUTTONUP:
		eraser = false;
		break;

	case WM_MOUSEMOVE:
		//�}�E�X���ړ������Ƃ��̏���


		if( pen ){

			HPEN hPen,hOldPen;

			int pen_width;
			int distance;
			int max_Width = pen_size;
			int min_Width = 1;
			hdc = BeginPaint(hWnd,&ps);

			//�y���T�C�Y�̕ύX
			//if(pen_scale++ > max_Width) pen_scale = max_Width;
			
			//�ЂƂO�̈ʒu��ۑ�
			pick_old = pick_new;
			pick_new.x = LOWORD( lParam );
			pick_new.y = HIWORD( lParam );

			POINT move;

			move.x = abs(pick_new.x - pick_old.x);
			move.y = abs(pick_new.y - pick_old.y);

			//�}���n�b�^������
			distance = move.x + move.y;

			//�������y���̕��𒴂���悤�ɂȂ�����
			if( distance > max_Width )
				distance = max_Width;

			if( distance < min_Width )
				distance = min_Width;

			//�y�������v�Z
			pen_width = max_Width - distance + min_Width;

			//�y���̓���
			if(pen_scale++ < pen_width) pen_width = pen_scale;

			//���݂̃r�b�g�}�b�v��I��
			SelectObject( hMem, hBmp_a[CurrentFrame] );

			//�y����p�ӂ��I��
			hPen = CreatePen( PS_SOLID, pen_width, RGB(64, 0, 0 ) );
			hOldPen = (HPEN)SelectObject(hMem,(HPEN)hPen);

			//���ݗpBitMap�ɐ���`��
			MoveToEx( hMem, pick_old.x,pick_old.y, NULL);
			LineTo( hMem, pick_new.x, pick_new.y);

			//MagickPen
			if(true){
				MoveToEx( hMem, wnd_rc.right - pick_old.x,pick_old.y, NULL);
				LineTo( hMem, wnd_rc.right - pick_new.x, pick_new.y);
			}


			//��n��
			SelectObject( hMem, hOldPen);
			DeleteObject( hPen );
			//ReleaseDC( hWnd, hdc );

			InvalidateRect( hWnd, NULL, NULL);
		}
		else if( eraser ){
			pick_new.x = LOWORD( lParam );
			pick_new.y = HIWORD( lParam );
			
			//�����S���̑傫��(�y���𒆐S��20�s�N�Z��)
			erase_rc.left = pick_new.x -10;
			erase_rc.right = pick_new.x + 10;
			erase_rc.bottom = pick_new.y -10;
			erase_rc.top = pick_new.y + 10;

			//�ݒ肵�������S���̈���폜
			InvalidateRect( hWnd, & erase_rc, TRUE);
		}

		break;

	case WM_PAINT:

		SelectObject( hMem, hBmp[2] );
		hdc = BeginPaint( hWnd, &ps );

		//���݂̉�ʂɌ��ݗpBitMap���R�s�[
		BitBlt(hdc, 0, 0, wnd_rc.right, wnd_rc.bottom, hMem, 0, 0, SRCCOPY );
		EndPaint( hWnd, &ps);
		break;

	case WM_DESTROY:
		//�o�b�n�p�ɗp�ӂ����f�o�C�X�R���e�L�X�g���폜
		DeleteDC( hMem );

		//�eBitMap�p�ɍ�����r�b�g�}�b�v�n���h�����폜
		DeleteObject(hBmp[0]);
		DeleteObject(hBmp[1]);
		DeleteObject(hBmp[2]);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���[�ł��B
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
