#ifndef _DIALOGS_ENTERCODE_H__
#define _DIALOGS_ENTERCODE_H__

//==========================================================================

class _EXPORT EnterCodeDialog
{
public:
	EnterCodeDialog( HINSTANCE, HWND );
	~EnterCodeDialog( void );

	bool Execute( void );

	HINSTANCE	hInstance;
	HWND		parent;
	bool		initialised;

	HWND		hName;
	HWND		hCode;
	HWND		hGenButton;
	HWND		hUserName;
	HWND		okButton;

	static EnterCodeDialog* ptr;
};

//==========================================================================

#endif
