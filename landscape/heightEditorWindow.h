#ifndef _LANDSCAPE_HEIGHTEDITORWINDOW_H_
#define _LANDSCAPE_HEIGHTEDITORWINDOW_H_

//==========================================================================

class HeightEditor
{
public:
	HeightEditor( TApp& _app, HINSTANCE _instance, HWND _parent );
	~HeightEditor( void );

	void SetMapInfo( TDefFile& def );

	HWND Window( void ) const;

	void Timer( void );
	void DoMenu( int cmd );
	void DoPaint( void );

	void SetPos( int x, int y );

	void MouseDown( int button, int x, int y, int dx, int dy );
	void MouseMove( int x, int y, int dx, int dy );
	void MouseUp( int button, int x, int y, int dx, int dy );

	void GetBoundingRect( float& x1, float& z1, float& x2, float& z2 );

private:
	friend LRESULT CALLBACK HeightEditorProc(HWND, UINT, WPARAM, LPARAM);

	static HeightEditor* ptr;

	TApp&		app;
	HWND		parent;
	HINSTANCE	instance;
	HBRUSH		backgroundBrush;
	HBRUSH		blackBrush;

	HWND		dialog;

	int			startX, startY;
	int			endX, endY;

	TTexture	map;
	bool		loaded;
};

//==========================================================================

#endif
