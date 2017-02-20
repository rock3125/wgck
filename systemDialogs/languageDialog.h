#ifndef _DIALOG_LANGUAGEDIALOG_H_
#define _DIALOG_LANGUAGEDIALOG_H_

//==========================================================================

class _EXPORT LanguageDialog
{
public:
	LanguageDialog( HINSTANCE _hInstance );
	~LanguageDialog( void );

	bool Execute( void );

	size_t LanguageId( void ) const;

private:
	friend LRESULT CALLBACK LanguageDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	HINSTANCE	hInstance;

	HWND		bEnglish;
	HWND		bGerman;
	HWND		bDutch;

	HBITMAP		bmpEnglish;
	HBITMAP		bmpGerman;
	HBITMAP		bmpDutch;

	size_t		languageId;

	static LanguageDialog* ptr;
};

//==========================================================================

#endif
