// PaintApp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "PaintApp.h"

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
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

 	// TODO: ここにコードを挿入してください。
	MSG msg;
	HACCEL hAccelTable;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PAINTAPP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAINTAPP));

	// メイン メッセージ ループ:
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
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
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
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   //ウィンドウの位置と幅の設定
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
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//アニメーション制作用のBITMAPを確保

	static HDC hFrame;
	static HBITMAP hBmp_a[8];
	int maxFrame = 9;
	int CurrentFrame = 0;

	static RECT window_rc; //ウィンドウサイズ

	//BMP保存のための用意
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
	static HDC hMem = NULL;  //保存用デバイスコンテキスト用

	//Undo,Redo,現在用の3つのバッファにアクセスするためのハンドル
	static HBITMAP hBmp[3] = {0}; //ビットマップを指し示すハンドル
	static RECT wnd_rc;			//クライアント領域サイズを指定するところ



	// カラーダイアログに使用する構造体
	static CHOOSECOLOR color_dialog = {0};
	static COLORREF		custom_colors[16];



	switch (message)
	{
	case WM_CREATE:

	//アニメーションフレーム制作のための行
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


	//終わり

		//アプリケーションウィンドウが初めて開いたとき一度だけ処理される
		color_dialog.lStructSize	= sizeof(CHOOSECOLOR);
		color_dialog.hwndOwner		= hWnd;
		color_dialog.lpCustColors	= custom_colors;

		//ウィンドウズのサイズを取得
		GetClientRect( hWnd, &wnd_rc );

		// 1. 保存用BMPデバイスコンテキスとの準部
		hMem = CreateCompatibleDC( NULL );

		//念のためにBitMapをクリア
		if ( hBmp[0] ) DeleteObject( hBmp[0] );
		if ( hBmp[1] ) DeleteObject( hBmp[1] );
		if ( hBmp[2] ) DeleteObject( hBmp[2] );

		//3現在のウィンドのデバイスコンテキストの取得
		if( hdc = GetDC( hWnd ) ) {

			//デバイスコンテキストの取得に成功したら
			//現在の画面と同じ種類の、保存用のBMPを作成
			hBmp[0] = CreateCompatibleBitmap( hdc, wnd_rc.right, wnd_rc.bottom);
			hBmp[1] = CreateCompatibleBitmap( hdc, wnd_rc.right, wnd_rc.bottom);
			hBmp[2] = CreateCompatibleBitmap( hdc, wnd_rc.right, wnd_rc.bottom);

			//1枚ずつ選択し、白に塗りつぶす
			//Undo
			SelectObject( hMem, hBmp[0] );
			PatBlt( hMem, wnd_rc.right, wnd_rc.top, wnd_rc.right,  wnd_rc.bottom, WHITENESS);

			//Redo用
			SelectObject( hMem,hBmp[1]);
			PatBlt( hMem, wnd_rc.left, wnd_rc.top, wnd_rc.right, wnd_rc.bottom, WHITENESS);

			//Now用
			SelectObject( hMem,hBmp[2] );
			PatBlt( hMem, wnd_rc.left, wnd_rc.top, wnd_rc.right, wnd_rc.bottom, WHITENESS);


			//5 後付デバイスコンテキスとの解放
			ReleaseDC( hWnd, hdc);
		
		}

		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{

			//resorce.h内で　#define IDM_UNDO 600
			#define IDM_REDO 605

			//と定義

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
			//Ctrl+Sキー
			{
				DWORD buffSize;
				DWORD buffOneLine;
				DWORD writeSize;
				DWORD imgWidth, imgHeight;
				HANDLE fh;
				LPBYTE Pixels;

				imgWidth = wnd_rc.right;
				imgHeight = wnd_rc.bottom;

				//bmpの1ラインを計算
				//bmpの幅は4バイトで割り切れる必要がある
				if(!( (imgWidth*3) %4 ) )
					buffOneLine = imgWidth * 3;
				else
					buffOneLine = imgWidth * 3 + ( 4- ( imgWidth * 3) % 4);

				//保存用バッファサイズの計算
				buffSize = buffOneLine * imgHeight;

				//保存用バッファを確保
				Pixels = ( LPBYTE )GlobalAlloc( GPTR, buffSize );

				//ヘッダに情報を格納
				head.bfType = 'MB';
				head.bfSize = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER ) + buffSize;
				head.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );
				info.biSize = sizeof( BITMAPINFOHEADER );
				info.biHeight = wnd_rc.bottom;
				info.biWidth = wnd_rc.right;
				info.biBitCount = 24;
				info.biPlanes = 1;

				if( hdc = GetDC( hWnd) ){
					//BMPから画像情報を取り出す
					GetDIBits( hMem, hBmp[0], 0, info.biHeight, Pixels, ( LPBITMAPINFO )( &info ), DIB_RGB_COLORS );

					//バッファをファイルに書き出す
					fh = CreateFile( L"C:\\Users\\kawabata_iMAC\\Documents\\test.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
					WriteFile( fh, &head, sizeof( BITMAPFILEHEADER ), &writeSize, NULL );
					WriteFile( fh, &info, sizeof( BITMAPINFOHEADER ), &writeSize, NULL );
					WriteFile( fh, Pixels, buffSize, &writeSize, NULL);
					CloseHandle( fh );
					
					ReleaseDC( hWnd, hdc );
				}
				//保存用バッファを解放
				GlobalFree( Pixels );
			}
			break;


		case IDM_UNDO:

			//現在のデバイスコンテキスつを取得
			if( hdc = GetDC( hWnd ) ){
				//Undo用ビットマップを画板にセット

				SelectObject( hMem, hBmp[0]);
				//バッファを現在の画像にコピー
				//現在の画面に一筆前の情報をコピー
				BitBlt( hdc, 0, 0, wnd_rc.right,wnd_rc.bottom, hMem,0,0, SRCCOPY );

				//現在のBitMapを選択
				SelectObject( hMem, hBmp[2] );
				//現在の画像を現在用のBitMapにコピー
				BitBlt(hMem, 0, 0, wnd_rc.right, wnd_rc.bottom, hdc, 0, 0, SRCCOPY );

				//後片付け
				ReleaseDC( hWnd, hdc);
			}
			break;
		case IDM_REDO:
			//現在のデバイスコンテキストの（画用紙を準備)
			if( hdc = GetDC(hWnd) ){
				//REDO用ビットマップを画用紙に保存
				SelectObject( hMem, hBmp[1] );
				//バッファ（ビットマップ）を現在の画面にコピー
				BitBlt( hdc, 0, 0, wnd_rc.right, wnd_rc.bottom, hMem, 0, 0, SRCCOPY);

				//現在の画像を保存
				SelectObject( hMem, hBmp[2] );
				//現在の画像を現在用のビットマップにコピー
				BitBlt( hMem, 0, 0, wnd_rc.right, wnd_rc.bottom, hdc, 0, 0, SRCCOPY );


				//後片付け
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
			//設定 > 色の選択 > メニューあるいはCtrl + Shift + c
			//1カラーダイアログを開く
			ChooseColor(&color_dialog);
			break;

		case IDM_CLEAR:
			//画面を消去　全消去
			InvalidateRect( hWnd, NULL, TRUE );
			/*
			//RECT型の変数erase_rcを定義
			RECT erase_rc;
			erase_rc.bottom = 250;
			erase_rc.left = 200;
			erase_rc.right	 = 450;
			erase_rc.top	= 100;
			//erase_rcで定義した場所を削除
			InvalidateRect( hWnd, &erase_rc, TRUE);
			*/
			break;

		case IDM_CHANGECOLOR:
			//x,Xが押された時の色の変更
			if( ++current_color >= 2 ) current_color = 0;
			break;

		case IDM_CHANGEPENSIZEMAINA:
			//[　を押されたときペンサイズを縮小　1/2に
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

		//マウスの左ボタンを押した時の処理
		pen = true;

		//Undo用BitMapを選択
		SelectObject( hMem, hBmp[0] );

		if ( hdc = GetDC(hWnd) ){
			//Undo用BitMapに現在の画像をコピー
			BitBlt( hMem, 0, 0, wnd_rc.right, wnd_rc.bottom, hdc, 0, 0, SRCCOPY);
			//SelectObject( hdc.
			ReleaseDC(hWnd, hdc);
		}

		//ペンの初期化
		pick_old.x = pick_new.x = LOWORD( lParam );
		pick_old.y = pick_new.y = HIWORD( lParam );

		break;

	case WM_LBUTTONUP:
		{
			SelectObject( hMem, hBmp[1]);

			//左ボタンを押した時の処理
			pen_scale = 1;

			//マウスの左ボタンを離したときの処理
			pen = false;

			//最後のしっぽが残らないように位置を初期化
			pick_old.x = pick_new.x = LOWORD( lParam );
			pick_old.y = pick_new.y = HIWORD( lParam );

			//Redo用ビットマップを選択
			SelectObject( hMem, hBmp[1] );
			//現在の画像をリドゥ用BitMapにコピー
			if( hdc = GetDC( hWnd) ){
				BitBlt( hMem, 0, 0, wnd_rc.right, wnd_rc.bottom, hdc, 0, 0, SRCCOPY );
				//SelectObject(hdc, hOldObj);
				ReleaseDC(hWnd, hdc);
			}
		}


		break;

	case WM_RBUTTONDOWN:
		//マウスの右ボタンを押した時の処理
		eraser = true; //削除
		break;

	case WM_RBUTTONUP:
		eraser = false;
		break;

	case WM_MOUSEMOVE:
		//マウスを移動したときの処理


		if( pen ){

			HPEN hPen,hOldPen;

			int pen_width;
			int distance;
			int max_Width = pen_size;
			int min_Width = 1;
			hdc = BeginPaint(hWnd,&ps);

			//ペンサイズの変更
			//if(pen_scale++ > max_Width) pen_scale = max_Width;
			
			//ひとつ前の位置を保存
			pick_old = pick_new;
			pick_new.x = LOWORD( lParam );
			pick_new.y = HIWORD( lParam );

			POINT move;

			move.x = abs(pick_new.x - pick_old.x);
			move.y = abs(pick_new.y - pick_old.y);

			//マンハッタン距離
			distance = move.x + move.y;

			//距離がペンの幅を超えるようになったら
			if( distance > max_Width )
				distance = max_Width;

			if( distance < min_Width )
				distance = min_Width;

			//ペン幅を計算
			pen_width = max_Width - distance + min_Width;

			//ペンの入り
			if(pen_scale++ < pen_width) pen_width = pen_scale;

			//現在のビットマップを選択
			SelectObject( hMem, hBmp_a[CurrentFrame] );

			//ペンを用意し選択
			hPen = CreatePen( PS_SOLID, pen_width, RGB(64, 0, 0 ) );
			hOldPen = (HPEN)SelectObject(hMem,(HPEN)hPen);

			//現在用BitMapに線を描く
			MoveToEx( hMem, pick_old.x,pick_old.y, NULL);
			LineTo( hMem, pick_new.x, pick_new.y);

			//MagickPen
			if(true){
				MoveToEx( hMem, wnd_rc.right - pick_old.x,pick_old.y, NULL);
				LineTo( hMem, wnd_rc.right - pick_new.x, pick_new.y);
			}


			//後始末
			SelectObject( hMem, hOldPen);
			DeleteObject( hPen );
			//ReleaseDC( hWnd, hdc );

			InvalidateRect( hWnd, NULL, NULL);
		}
		else if( eraser ){
			pick_new.x = LOWORD( lParam );
			pick_new.y = HIWORD( lParam );
			
			//消しゴムの大きさ(ペンを中心に20ピクセル)
			erase_rc.left = pick_new.x -10;
			erase_rc.right = pick_new.x + 10;
			erase_rc.bottom = pick_new.y -10;
			erase_rc.top = pick_new.y + 10;

			//設定した消しゴム領域を削除
			InvalidateRect( hWnd, & erase_rc, TRUE);
		}

		break;

	case WM_PAINT:

		SelectObject( hMem, hBmp[2] );
		hdc = BeginPaint( hWnd, &ps );

		//現在の画面に現在用BitMapをコピー
		BitBlt(hdc, 0, 0, wnd_rc.right, wnd_rc.bottom, hMem, 0, 0, SRCCOPY );
		EndPaint( hWnd, &ps);
		break;

	case WM_DESTROY:
		//バッハ用に用意したデバイスコンテキストを削除
		DeleteDC( hMem );

		//各BitMap用に作ったビットマップハンドルを削除
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

// バージョン情報ボックスのメッセージ ハンドラーです。
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
