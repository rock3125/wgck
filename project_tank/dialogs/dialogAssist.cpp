#include <precomp_header.h>

#include <dialogs/dialogAssist.h>

const int SCROLLBACK_MAX_LINES = 600;
const int SCROLLBACK_DEL_CHARS = 500;

char *TranslateString( char *inbuf, char* outbuf )
{
	int		i, l;

	l = strlen(inbuf);
	char* out = outbuf;
	for ( i=0; i<l ;i++ )
	{
		if ( inbuf[i]=='\n' )
		{
			*out++ = '\r';
			*out++ = '\n';
		}
		else
			*out++ = inbuf[i];
	}
	*out++ = '\r';
	*out++ = '\n';
	*out++ = 0;
	return outbuf;
}


void EditPrint( HWND ctrl, char *text, ... )
{
	va_list argptr;
	char	buf[1024];
	char	out[1024];
	LRESULT	result;
	DWORD	oldPosS, oldPosE;

	va_start (argptr,text);
	vsprintf (buf, text,argptr);
	va_end (argptr);

	TranslateString(buf,out);

	result = SendMessage (ctrl, EM_GETLINECOUNT, 0, 0);
	if( result>SCROLLBACK_MAX_LINES && ctrl!=NULL )
	{
		char replaceText[5];
		
		replaceText[0] = '\0';

		SendMessage(ctrl, WM_SETREDRAW, (WPARAM)0, (LPARAM)0);
		SendMessage(ctrl, EM_GETSEL, (WPARAM)&oldPosS, (LPARAM)&oldPosE);
		SendMessage(ctrl, EM_SETSEL, 0, SCROLLBACK_DEL_CHARS);
		SendMessage(ctrl, EM_REPLACESEL, (WPARAM)0, (LPARAM)replaceText);
		SendMessage(ctrl, EM_SETSEL, oldPosS, oldPosE);
		SendMessage(ctrl, WM_SETREDRAW, (WPARAM)1, (LPARAM)0);
	}
	if ( ctrl!=NULL )
		SendMessage (ctrl, EM_REPLACESEL, 0, (LPARAM)out);
}


void EditPrintSingle( HWND ctrl, char *text )
{
	LRESULT	result;
	DWORD	oldPosS, oldPosE;

	result = SendMessage (ctrl, EM_GETLINECOUNT, 0, 0);
	char replaceText[5];
	replaceText[0] = '\0';

	SendMessage(ctrl, WM_SETREDRAW, (WPARAM)0, (LPARAM)0);
	SendMessage(ctrl, EM_GETSEL, (WPARAM)&oldPosS, (LPARAM)&oldPosE);
	SendMessage(ctrl, EM_SETSEL, 0, SCROLLBACK_DEL_CHARS);
	SendMessage(ctrl, EM_REPLACESEL, (WPARAM)0, (LPARAM)replaceText);
	SendMessage(ctrl, EM_SETSEL, oldPosS, oldPosE);
	SendMessage(ctrl, WM_SETREDRAW, (WPARAM)1, (LPARAM)0);
	SendMessage (ctrl, EM_REPLACESEL, 0, (LPARAM)text);
}


void EditClear( HWND ctrl )
{
	char text[4];
	text[0] = 0;
	SendMessage( ctrl, WM_SETTEXT, 0, (LPARAM)text );
}


void ListBoxAddString( HWND ctrl, const char* str )
{
	SendMessage( ctrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)str ); 
};


bool ButtonGetCheck( HWND ctrl )
{
	return SendMessage(ctrl,BM_GETCHECK,0,0)==BST_CHECKED;
};


void ButtonSetCheck( HWND ctrl, bool check )
{
	if ( check )
		SendMessage(ctrl,BM_SETCHECK,BST_CHECKED,0);
	else
		SendMessage(ctrl,BM_SETCHECK,BST_UNCHECKED,0);
};


int DropDownBoxGetSelection( HWND ctrl )
{
	int sel = SendMessage(ctrl,CB_GETCURSEL,0,0);
	if ( sel==CB_ERR )
		return -1;
	return sel;
};


void SetControlHeight( HWND ctrl, int height )
{
	RECT r;
	GetWindowRect(ctrl,&r);
	SetWindowPos( ctrl, NULL, r.left, r.top, r.right-r.left, height, SWP_NOMOVE|SWP_NOZORDER);
}


void SetControlWidth( HWND ctrl, int width )
{
	RECT r;
	GetWindowRect(ctrl,&r);
	SetWindowPos( ctrl, NULL, r.left, r.top, width, r.bottom-r.top, SWP_NOMOVE|SWP_NOZORDER);
}


bool InsideRect( RECT& r, POINT& p )
{
	if ( p.x >= r.left && p.x <= r.right )
	if ( p.y >= r.top && p.y <= r.bottom )
		return true;
	return false;
};


void SetControlFont( HWND ctrl, HFONT font, bool redraw )
{
	SendMessage( ctrl, WM_SETFONT, (WPARAM)font, (LPARAM)redraw);
};


void EnableControl( HWND ctrl, bool enable )
{
	
	if ( enable )
		EnableWindow( ctrl, TRUE );
	else
		EnableWindow( ctrl, FALSE );
};

