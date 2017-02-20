#ifndef _DIALOG_LANGUAGEDIALOG_H_
#define _DIALOG_LANGUAGEDIALOG_H_

//==========================================================================

#define kMaxFonts	512

//==========================================================================

class _EXPORT FontDialog
{
public:
	FontDialog( HINSTANCE _hInstance );
	~FontDialog( void );

	bool Execute( void );

	LOGFONT Font( void ) const;
	int		Size( void ) const;

	HINSTANCE	hInstance;

	HWND		bCreateFont;
	HWND		bExit;

	HWND		ddFont;
	HWND		ddSize;

	LOGFONT		currentFont;
	int			currentFontSize;

	LOGFONT		fonts[kMaxFonts];

	static FontDialog* ptr;
};

//==========================================================================

#endif
