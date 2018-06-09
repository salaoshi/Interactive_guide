//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1998  Microsoft Corporation.  All Rights Reserved.
//  Modify Sasha P 2009
//--------------------------------------------------------------------------;
#define UNICODE
#include "stdwin.h"
#include "cplay.h"
#include "about.h"
#include "file.h"
#include "media.h"
#include "toolbar.h"
#include "resource.h"
#include "database.h"

//
//
// What this sample illustrates
//
// A simple ActiveMovie application written in C.
//
//
// Summary
//
// This is an ActiveMovie application that is written in C. We create a window
// that has a simple menu allowing files to be opened and closed. The window
// also has three buttons for stopping, pausing and running the media. We are
// written to call the standard filtergraph interfaces. The only complication
// we have added is two UI alternatives. The filter can either be compiled to
// rewind to the start of the media when it is stopped, or alternatively it
// can leave the current position (and therefore frame) wherever it is stopped
//
// Files
//
// about.c          Run of the mill about box
// about.h          Header file for about box
// assert.c         Display a message box
// cplay.c          Main application code
// cplay.h          And its function protoypes
// file.c           Looks after file open dialogs
// file.h           And its two function prototypes
// makefile         How we build it
// media.c          Functions that call the graph
// media.h          And their prototypes
// player.rc        Visual C++ resource file
// resource.h       Header file for resources
// stdwin.c         Precompiled header file
// stdwin.h         Precompiled header file
// toolbar.c        Looks after the app toolbar
// toolbar.h        Public function prototypes
//
//


#define UNTITLED_STRING L" - Untitled"
#define nMaxResourceStrSize 128

//int list_mode=VIDEOCLIP_LIST;// PROGRAM_LIST	 CATEGORY_LIST
//int list_mode=PROGRAM_LIST;//	 CATEGORY_LIST
int list_mode=CATEGORY_LIST;
double cur_time=0;
int nListHeight=200;
int nInfoHeight=60;
AppVars appVars;

int Language=0;

//
// PlayerMessageBox
//
// Load and display an error message
//
void PlayerMessageBox( UINT nResource )
{
    TCHAR szStr[ nMaxResourceStrSize ];

    LoadString( appVars.hInstance, nResource, szStr, nMaxResourceStrSize );
    MessageBox( appVars.hwndMainFrame, szStr, appVars.szAppName,
                MB_APPLMODAL | MB_OK | MB_ICONEXCLAMATION );

} // PlayerMessageBox


//
// ProcessCommand
//
// Process a WM_COMMAND message to the main window
//
long ProcessCommand( HWND hwnd, UINT wParam, LONG lParam )
{
	static selected_item=-1;
	int i=0;
	int wmId, wmEvent;
	wmId    = LOWORD(wParam); 
	wmEvent = HIWORD(wParam); 
	//wmEvent2 = LOWORD(wParam); 
	
	 
    switch( wmId ){


        case ID_FILE_EXIT:
            PostQuitMessage( 0 );
            break;

        case ID_HELP_ABOUT:
		
				DoAboutDialog( appVars.hInstance,hwnd );
            break;

        case ID_FILE_OPEN:
            OpenMediaFile( hwnd, NULL );
            break;

        case ID_MEDIA_PLAY:
			switch(list_mode)
			{
			case VIDEOCLIP_LIST:
				if(FileSelected(hwnd))
				{
				OnMediaPlay( );
				OnMediaWindowSet(hwnd);
				}
				break;
			case PROGRAM_LIST:
				if(FileSelected(hwnd))
				{
					Select_Program( hwnd);
					list_mode=VIDEOCLIP_LIST;
					InvalidateRect(hwnd, NULL, TRUE );
				}
				break;
			case CATEGORY_LIST:
				if(FileSelected(hwnd))
				{
					Select_Category( hwnd);
					list_mode=PROGRAM_LIST;
					InvalidateRect(hwnd, NULL, TRUE );
				}
				break;
			default:
				break;
			}			 			
            break;

        case ID_MEDIA_PAUSE:
			//OnMediaWindowSet(hwnd);
            OnMediaPause( );
            break;

        case ID_MEDIA_STOP:
            OnMediaStop( );
            break;

		case ID_LIST:
			InvalidateRect((HWND)lParam, NULL, TRUE );
			
			if(wmEvent==LBN_SELCHANGE)
				i=i;
			if(wmEvent==LBN_SETFOCUS)
				i=i;

			
			if(list_mode==VIDEOCLIP_LIST)
			{
				if(wmEvent==LBN_DBLCLK)
					if(FileSelected(hwnd))
						{
						OnMediaPlay( );
						OnMediaWindowSet(hwnd);
						}
				
				i=SendMessage( (HWND)lParam, LB_GETCURSEL, 0, 0 );
		
				if(i>0)
				ListActivate( hwnd);
				ShowItemInfo( hwnd);
						
			}
			else

			if(list_mode==PROGRAM_LIST)
			{
				if(wmEvent==LBN_DBLCLK)
					if(FileSelected(hwnd))
						{
						Select_Program( hwnd);
						list_mode=VIDEOCLIP_LIST;
						InvalidateRect(hwnd, NULL, TRUE );
						}
				
				//i=SendMessage( (HWND)lParam, LB_GETCURSEL, 0, 0 );
				//if(i>0)
					ShowItemInfo( hwnd);
			}
			else

			if(list_mode==CATEGORY_LIST)
			{
				if(wmEvent==LBN_DBLCLK)
					if(FileSelected(hwnd))
						{
						 Select_Category(hwnd);
						list_mode=PROGRAM_LIST;
						InvalidateRect(hwnd, NULL, TRUE );
						}
				
				//i=SendMessage( (HWND)lParam, LB_GETCURSEL, 0, 0 );
				//if(i>0)
					ShowItemInfo( hwnd);
			}
			 
			break;


		//case WM_SIZE:
			//	break;
		case ID_PROGRAM_LIST:
			//ProgramListActivate( hwnd);
			break;
		case ID_USING_GUIDE:
			MyOpenMediaFile(hwnd,pDB[1].FileName,0);
			OnMediaWindowSetUseGuide(hwnd);
			OnMediaPlay( );
			OnMediaWindowSetUseGuide(hwnd);
			Sleep(10000);
			DeleteContents();
			break;
		case ID_LANGUAGE1:
			Language=0;
			DeleteDB();
			CreateDB(hwnd);
			ResizeWindows(hwnd);
			InvalidateRect(hwnd, NULL, TRUE );
			break;
		case ID_LANGUAGE2:
			Language=1;
			DeleteDB();
			CreateDB(hwnd);
			ResizeWindows(hwnd);
			InvalidateRect(hwnd, NULL, TRUE );
			break;


        default:
            return DefWindowProc( hwnd, WM_COMMAND, wParam, lParam );
    }
	

    return (LRESULT) 0;

} // ProcessCommand


//
// OnGetMinMaxInfo
//
// Sets the minimum size of the main window and the maximum height
//
void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
    RECT rectRequired;
    SIZE sizeToolbar;

    // Our client area is going to be the toolbar, so find our its size
    CalcRequiredSize( &sizeToolbar );

    rectRequired.left = rectRequired.top = 0;
    rectRequired.right = sizeToolbar.cx;
    rectRequired.bottom = sizeToolbar.cy;

    // Take into account the menu, caption and thick frame
    AdjustWindowRect( &rectRequired, WS_CAPTION|WS_THICKFRAME, TRUE );

    // Set the min/max sizes
    lpMMI->ptMinTrackSize.x = rectRequired.right - rectRequired.left;
    lpMMI->ptMinTrackSize.y = lpMMI->ptMaxTrackSize.y =
            rectRequired.bottom - rectRequired.top;

} // OnGetMinMaxInfo


//
// MainFrameProc
//
// Handles the message sent to the main window
//
long FAR PASCAL MainFrameProc( HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
WCHAR s1[10];
//	PAINTSTRUCT ps;
			HDC hDC;
			HGDIOBJ hgdiOldBitmap;
			HDC hSourceDC;
			//UINT nUpperBrush, nLowerBrush;
		//ShowItemInfo( hwnd);


		/*	memset(s1,0,20);
			hDC=GetDC(hwnd);
			swprintf(s1,L"%f",(float)cur_time);
			//SetBkMode(hDC,TRANSPARENT);
			TextOutW(hDC, 5, 30,s1,wcslen(s1));
			ReleaseDC(hwnd,hDC);*/



    switch( message ){

		case WM_CREATE:
			SetTimer(hwnd,TIMER_ID,TIMER_RATE,NULL);
			break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
			DeleteDB();
			KillTimer(hwnd,TIMER_ID);
            break;

        case WM_GETMINMAXINFO:
            OnGetMinMaxInfo( (MINMAXINFO FAR *) lParam );
            break;

		/*case WM_PAINT:
			 //DrawButton( appVars.hInstance, (DRAWITEMSTRUCT FAR *) lParam );
				hDC=GetDC(hwnd);

			swprintf(s1,L"%f",(float)cur_time);
			SetBkMode(hDC,TRANSPARENT);
			TextOutW(hDC, 5, 30,s1,wcslen(s1));
			ReleaseDC(hwnd,hDC);


			 break;*/

        case WM_DRAWITEM:
            DrawButton( appVars.hInstance, (DRAWITEMSTRUCT FAR *) lParam );
			  
			
			hDC=GetDC(hwnd);

			swprintf(s1,L"%f",(float)cur_time);
			SetBkMode(hDC,TRANSPARENT);
			TextOutW(hDC, 5, 30,s1,wcslen(s1));

			hSourceDC= CreateCompatibleDC( hDC );
			
			hgdiOldBitmap = SelectObject( hSourceDC,
            (HGDIOBJ) LoadBitmap( appVars.hInstance, MAKEINTRESOURCE( IDR_MAINFRAME ) ));
			
			//(HDC, int, int, int, int, HDC, int, int, DWORD);
			BitBlt( hDC,84,3,160,	26,
					hSourceDC,
					48,	0,
					SRCCOPY);

			//StretchBlt(hDC,0,60,100,100,hSourceDC,0,0,40,40,SRCCOPY);
			 SelectObject( hSourceDC, hgdiOldBitmap );
				DeleteDC(hSourceDC);
				ReleaseDC(hwnd,hDC);
            break;
			
        case WM_INITMENUPOPUP:
            if( lParam == 1 ){          // Media popup menu
                EnableMenuItem( (HMENU) wParam, ID_MEDIA_PLAY,  CanPlay()  ? MF_ENABLED : MF_GRAYED );
                EnableMenuItem( (HMENU) wParam, ID_MEDIA_PAUSE, CanPause() ? MF_ENABLED : MF_GRAYED );
                EnableMenuItem( (HMENU) wParam, ID_MEDIA_STOP,  CanStop()  ? MF_ENABLED : MF_GRAYED );
            } else
                return DefWindowProc( hwnd, message, wParam, lParam );
            break;

			case WM_SIZE:
				
				ResizeWindows( hwnd);
				//hDC=GetDC(hwnd);
				 //DrawRect(hDC, 0, 0, 10, 10, BLACK_BRUSH  );
				//ReleaseDC(hwnd,hDC);
			
			case WM_TIMER:
				//GetTime();
				memset(s1,0,20);
				InvalidateRect( hwnd, NULL, TRUE );
				//hDC=GetDC(hwnd);
				//swprintf(s1,L"%f",(float)cur_time);
				//SetBkMode(hDC,TRANSPARENT);
				//TextOutW(hDC, 5, 30,s1,wcslen(s1));
				//ReleaseDC(hwnd,hDC);

				break;


        case WM_COMMAND:
            return ProcessCommand( hwnd, wParam, lParam );

        default:
            return DefWindowProc( hwnd, message, wParam, lParam );
    }
    return (LRESULT) 0;

} // MainFrameProc


//
// InitApplication
//
BOOL InitApplication()
{
//	SetTimer(hwnd,TIMER_ID,TIMER_RATE,NULL);

    wcscpy( appVars.szAppName, APP_NAME );

    // Filter interface initialize?
    if( SUCCEEDED( CoInitialize( NULL )))
		return TRUE;

    return FALSE;
}


//
// UnInitApplication
//
void UnInitApplication()
{

    CoUninitialize( );
	//DeleteDB();
	//KillTimer(hwnd,TIMER_ID);
}


//
// InitInstance
//
// Set the specific instance data and register our main
// window class if it has not been registered already
//
BOOL InitInstance( HANDLE hInstance, HANDLE hPrevInstance )
{
    appVars.hInstance = hInstance;

    if(!hPrevInstance){
        WNDCLASS wndClass;

        wndClass.style          = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpfnWndProc    = MainFrameProc;
        wndClass.cbClsExtra     = 0;
        wndClass.cbWndExtra     = 0;
        wndClass.hInstance      = appVars.hInstance;
        wndClass.hIcon          = LoadIcon( appVars.hInstance, MAKEINTRESOURCE( IDR_MAINFRAME ));
        wndClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
        wndClass.hbrBackground  = GetStockObject( LTGRAY_BRUSH );
        wndClass.lpszMenuName   = MAKEINTRESOURCE( IDR_MAINFRAME );
        wndClass.lpszClassName  = appVars.szAppName;

        RegisterClass( &wndClass );
    }


    return TRUE;

} // InitInstance


//
// InitMainFrame
//
// Create our main window
//
BOOL InitMainFrame( int nCmdShow )
{
    SIZE sizeToolbar;
	int x,y;
	const DWORD Styles = WS_OVERLAPPEDWINDOW &~ WS_MAXIMIZEBOX;
    WCHAR szTitle[ 30 ];

    wcscpy( szTitle, APP_NAME );
    //wcscat( szTitle, UNTITLED_STRING );

	
    CalcRequiredSize( &sizeToolbar );
   
    x = sizeToolbar.cx;
    y = sizeToolbar.cy;

    appVars.hwndMainFrame =
        CreateWindowW( appVars.szAppName,    // Our class name
                      szTitle,              // Window title
                      Styles,               // It's styles
					 //(GetSystemMetrics(SM_CXSCREEN)-85)/2,
					 (GetSystemMetrics(SM_CXSCREEN)-x-8),
					 
					0,//(GetSystemMetrics(SM_CYSCREEN)-y),
					//CW_USEDEFAULT,        // No x position
                    // CW_USEDEFAULT,        // And no y either
					
                     x, y,                // Initial sizes
                      NULL,                 // No parent window
                      NULL,                 // And no menu
                      appVars.hInstance,    // App instance
                      NULL);                // Creation data

    ShowWindow( appVars.hwndMainFrame, nCmdShow );
    UpdateWindow( appVars.hwndMainFrame );
	CreateDB(appVars.hwndMainFrame);
    return TRUE;

} // InitMainFrame


//
// DoMainLoop
//
// Main message loop
//
UINT DoMainLoop()
{
    MSG msg;
    HANDLE  ahObjects[1];       // Handles that need to be waited on
    const int cObjects = 1;     // Number of objects that we have

    // Message loop lasts until we get a WM_QUIT message
    // Upon which we shall return from the function

    while (TRUE) {
        if( (ahObjects[ 0 ] = GetGraphEventHandle()) == NULL ) {
            WaitMessage();
        } else {

            // Wait for any message or a graph notification

            DWORD Result = MsgWaitForMultipleObjects( cObjects,
                                                      ahObjects,
                                                      FALSE,
                                                      INFINITE,
                                                      QS_ALLINPUT);

            // Have we received an event notification

            if( Result != (WAIT_OBJECT_0 + cObjects) )
            {
                if( Result == WAIT_OBJECT_0 ) {
                    OnGraphNotify();
                }
                continue;
            }
				GetTime();
		

        }

        // Read all of the messages in this next loop
        // removing each message as we read it

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return msg.wParam;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

} // DoMainLoop


//
// WinMain
//
// Application entry point
//
int PASCAL WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszCmdParam,
                    int nCmdShow )
{
    UINT nReturn;

    // Initialise COM and the application
    if ( InitApplication() == FALSE ) return 0;

    if( InitInstance( hInstance, hPrevInstance ) &&
        InitMainFrame( nCmdShow ) &&
        InitToolbar( hInstance, appVars.hwndMainFrame ) &&
        InitMedia( ) &&
        InitFileOpenDialog( appVars.hwndMainFrame ))
    {
        //if( lpszCmdParam[0] != '\0' ) 
		//{
	    //OpenMediaFile( appVars.hwndMainFrame, lpszCmdParam );
        //}

        nReturn = DoMainLoop();

	// Stop the graph if we can
	if( CanStop() )
            OnMediaStop();

        // Release the filter graph
	DeleteContents();
    }

    UnInitApplication();
    return nReturn;

} // WinMain

