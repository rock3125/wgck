#include <precomp_header.h>
#include <commctrl.h>

#include <systemDialogs/dialogAssist.h>

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


void EditPrintSingle( HWND ctrl, const char *text )
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


void EditGetText( HWND ctrl, char* buffer, size_t bufferSize )
{
	SendMessage( ctrl, WM_GETTEXT, (WPARAM)bufferSize, (LPARAM)buffer );
};


float EditGetFloat( HWND ctrl )
{
	char buf[256];
	SendMessage( ctrl, WM_GETTEXT, (WPARAM)256, (LPARAM)buf );
	return float(atof(buf));
};


int EditGetInt( HWND ctrl )
{
	char buf[256];
	SendMessage( ctrl, WM_GETTEXT, (WPARAM)256, (LPARAM)buf );
	return int(atoi(buf));
};


void ComboBoxAddString( HWND ctrl, const char* str )
{
	SendMessage( ctrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)str ); 
};


void ComboBoxDeleteString( HWND ctrl, size_t index )
{
	SendMessage( ctrl, CB_DELETESTRING, (WPARAM)index, (LPARAM)0 ); 
};


void ComboBoxClear( HWND ctrl )
{
	ComboBoxDeleteStrings(ctrl);
};


void ComboBoxDeleteStrings( HWND ctrl )
{
	SendMessage( ctrl, CB_RESETCONTENT, 0,0 );
};


void ListBoxAddString1( HWND ctrl, const char* str )
{
	SendMessage( ctrl, LB_ADDSTRING, 0, (LPARAM)str );
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


void DropDownBoxSetSelection( HWND ctrl, int sel )
{
	SendMessage(ctrl,CB_SETCURSEL,sel,0);
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
	{
		::EnableWindow( ctrl, TRUE );
		::PostMessage( ctrl, WM_ENABLE, TRUE, 0 );
	}
	else
	{
		::EnableWindow( ctrl, FALSE );
		::PostMessage( ctrl, WM_ENABLE, FALSE, 0 );
	}
};


bool Enabled( HWND ctrl )
{
	if ( IsWindowEnabled(ctrl)==TRUE )
		return true;
	return false;
};

void ShowControl( HWND ctrl, bool show )
{
	if ( show )
	{
		::EnableWindow( ctrl, TRUE );
		::ShowWindow( ctrl, SW_SHOW );
	}
	else
	{
		::EnableWindow( ctrl, FALSE );
		::ShowWindow( ctrl, SW_HIDE );
	}
};


void SliderSetFrequency( HWND ctrl, size_t freq )
{
	SendMessage( ctrl, TBM_SETTICFREQ, freq, 0 );
};


void SliderSetRange( HWND ctrl, size_t min, size_t max )
{
	SendMessage( ctrl, TBM_SETRANGE, TRUE, MAKELONG(min,max) );
};


void SliderSetPos( HWND ctrl, size_t pos )
{
	SendMessage( ctrl, TBM_SETPOS, TRUE, pos );
};


void SliderSetPos( HWND ctrl, float pos )
{
	size_t _pos = size_t(pos*10);
	SendMessage( ctrl, TBM_SETPOS, TRUE, _pos );
};


void SliderGetPos( HWND ctrl, size_t& pos )
{
	pos = SendMessage( ctrl, TBM_GETPOS, 0,0 );
};


void SliderGetPos( HWND ctrl, float& pos )
{
	size_t _pos = SendMessage( ctrl, TBM_GETPOS, 0,0 );
	pos = _pos * 0.1f;
};


int SliderGetPos( HWND ctrl )
{
	int pos = SendMessage( ctrl, TBM_GETPOS, 0,0 );
	return pos;
};

#ifdef _USEGUI
void AddItem( TGUI*& page, TGUI* item )
{
	if ( page==NULL )
		page = item;
	else
	{
		TGUI* list = page;
		while ( list->next!=NULL )
			list = list->next;
		list->next = item;
	}
};
#endif

void SetScrollInfo( HWND ctrl, SCROLLINFO* info, bool redraw)
{
	::SetScrollInfo(ctrl, SB_CTL, info, redraw);
}


void GetScrollInfo( HWND ctrl, SCROLLINFO* info)
{
	::GetScrollInfo(ctrl, SB_CTL, info);
}


void ScrollbarSetRange( HWND ctrl, int min, int max )
{
	SCROLLINFO info;
	ZeroMemory(&info, sizeof info);
	info.cbSize = sizeof info;
	info.fMask = SIF_RANGE;
	info.nMin = min;
	info.nMax = max;
	SetScrollInfo(ctrl,&info,true);
}


void ScrollbarSetPosition(HWND ctrl, int thumbPos)
{
	int  min, max;
	ScrollbarGetRange(ctrl,min,max);

	// Constrain "thumbPos" to be in the range "min .. max"
	//
	if (thumbPos > max)
		thumbPos = max;
	else if (thumbPos < min)
		thumbPos = min;

	if ( thumbPos!=ScrollbarGetPosition(ctrl)) 
	{
		SCROLLINFO info;
		ZeroMemory(&info, sizeof info);
		info.cbSize = sizeof info;
		info.fMask = SIF_POS;
		info.nPos = thumbPos;
		SetScrollInfo(ctrl,&info, true);
	}
}


int ScrollbarGetPosition( HWND ctrl )
{
	SCROLLINFO info;
	ZeroMemory(&info, sizeof info);
	info.cbSize = sizeof info;
	info.fMask = SIF_POS;
	GetScrollInfo(ctrl,&info);
	return info.nPos;
}


void ScrollbarGetRange( HWND ctrl, int& min, int& max)
{
	SCROLLINFO info;
	ZeroMemory(&info, sizeof info);
	info.cbSize = sizeof info;
	info.fMask = SIF_RANGE;
	GetScrollInfo(ctrl,&info);
	min = info.nMin;
	max = info.nMax;
}
