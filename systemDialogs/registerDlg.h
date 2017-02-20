#ifndef _DIALOGS_REGISTER_H__
#define _DIALOGS_REGISTER_H__

//==========================================================================

#include <jpeglib/tjpeg.h>

class _EXPORT RegisterDialog
{
public:
	RegisterDialog( HINSTANCE, HWND );
	~RegisterDialog( void );

	void Execute( void );

	HINSTANCE	hInstance;
	HWND		parent;
	bool		initialised;

	static RegisterDialog* ptr;
	TJpeg		registerJpeg;

};

//==========================================================================

#endif
