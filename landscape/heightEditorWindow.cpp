#include <precomp_header.h>

#include <Commdlg.h>
#include <shlobj.h>
#include <direct.h>
#include <Winbase.h>
#include <shellapi.h>

#include <io.h>

#include <dialogs/dialogassist.h>
#include <win32/resource.h>
#include <landscape/main.h>
#include <landscape/heightEditorWindow.h>

//==========================================================================

HeightEditor::HeightEditor( TApp& _app, HINSTANCE _instance, HWND _parent )
	: app(_app),
	  parent( _parent ),
	  instance( _instance )
{
	loaded = false;

	long style = DS_MODALFRAME | WS_POPUP | WS_CAPTION;
	dialog = ::CreateWindow( app.ApplicationName().c_str(), "Top View",
							 style, 0,480,280,280, _parent,NULL,_instance,NULL );

	backgroundBrush = ::CreateSolidBrush( RGB(100,100,100) );
	blackBrush = ::CreateSolidBrush( RGB(0,0,0) );

	startX = 0;
	startY = 0;
	endX = 512;
	endY = 512;
};


HeightEditor::~HeightEditor( void )
{
	loaded = false;
};


void HeightEditor::DoMenu( int cmd )
{
};


void HeightEditor::Timer( void )
{
};


HWND HeightEditor::Window( void ) const
{
	return dialog;
};


void HeightEditor::SetMapInfo( TDefFile& def )
{
	TString errStr;
	map.DontDestroyRGB( true );
	loaded = map.LoadBinary( def.BMPName(), "data\\stories", errStr );
//	map.MirrorHorizontally();
//	map.MirrorVertically();

	if ( loaded )
	{
		RECT crect,wrect;
		::GetClientRect( dialog, &crect );
		::GetWindowRect( dialog, &wrect );
		int extraWidth  = (wrect.right-wrect.left) - (crect.right-crect.left);
		int extraHeight = (wrect.bottom-wrect.top) - (crect.bottom-crect.top);

		::MoveWindow( dialog, 
					  0,480, 
					  (map.Width()*2)+extraWidth, 
					  (map.Height()*2)+extraHeight, 
					  TRUE );
		::InvalidateRect( dialog, NULL, TRUE );
	}
};

void HeightEditor::MouseDown( int button, int x, int y, int dx, int dy )
{
	startX = x;
	startY = y;
};

void HeightEditor::MouseMove( int x, int y, int dx, int dy )
{
};

void HeightEditor::MouseUp( int button, int x, int y, int dx, int dy )
{
	endX = x;
	endY = y;

	if ( startX > endX )
	{
		int temp = startX;
		startX = endX;
		endX = temp;
	}
	if ( startY > endY )
	{
		int temp = startY;
		startY = endY;
		endY = temp;
	}
	::InvalidateRect( dialog, NULL, TRUE );
};

void HeightEditor::DoPaint( void )
{
	PAINTSTRUCT ps;
	HDC dc = ::BeginPaint(dialog,&ps);
	RECT rect;
	::GetClientRect( dialog, &rect );

	if ( loaded )
	{
		::FillRect( dc, &rect, backgroundBrush );

		rect.right = rect.left + map.Width()*2;
		rect.bottom = rect.top + map.Height()*2;
		map.Draw( dc, rect );

		::MoveToEx( dc, startX, startY, NULL );
		::LineTo( dc, startX, endY );
		::LineTo( dc, endX, endY );
		::LineTo( dc, endX, startY );
		::LineTo( dc, startX, startY );
	}
	else
	{
		::FillRect( dc, &rect, blackBrush );
	};

	::EndPaint(dialog,&ps);
};

void HeightEditor::SetPos( int x, int y )
{
	RECT r;
	::GetWindowRect( dialog, &r );
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	::MoveWindow( dialog, x,y,w,h,TRUE );
};

//
// -500..500 both directions
//
void HeightEditor::GetBoundingRect( float& x1, float& z1, float& x2, float& z2 )
{
	x1 = -((float(startX) / 256.0f) - 0.5f) * 1000;
	z1 = -((float(startY) / 256.0f) - 0.5f) * 1000;
	x2 = -((float(endX) / 256.0f) - 0.5f) * 1000;
	z2 = -((float(endY) / 256.0f) - 0.5f) * 1000;
};

//==========================================================================


