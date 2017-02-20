#ifndef _DIALOG_TIMELIMIT_H_
#define _DIALOG_TIMELIMIT_H_

//==========================================================================

class _EXPORT TimeLimitDialog
{
public:
	TimeLimitDialog( HINSTANCE _hInstance, const TString& msg );
	~TimeLimitDialog( void );

	void Execute( void );

private:
	friend LRESULT CALLBACK TimeLimitDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	HINSTANCE	hInstance;
	TString		msg;
	size_t		showMessage;
	HWND		enableShowAgain;
	HWND		labelMessage;

	static TimeLimitDialog* ptr;
};

//==========================================================================

#endif
