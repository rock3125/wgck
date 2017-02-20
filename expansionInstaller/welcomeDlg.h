#ifndef _DIALOG_LANGUAGEDIALOG_H_
#define _DIALOG_LANGUAGEDIALOG_H_

//==========================================================================

class WelcomeDlg
{
public:
	WelcomeDlg( HINSTANCE _hInstance );
	~WelcomeDlg( void );

	bool Execute( void );

private:
	friend LRESULT CALLBACK WelcomeDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	HINSTANCE	hInstance;

	static WelcomeDlg* ptr;
};

//==========================================================================

#endif
