#ifndef __DIALOGS_DIALOGASSIST_H__
#define __DIALOGS_DIALOGASSIST_H__

class TJpeg;

// defined dialogs
void CreatePerformanceDlg( HINSTANCE hInstance, HWND parent );
void CreateVideoSetupDlg( HINSTANCE hInstance, HWND parent );
void CreateAboutDlg( HINSTANCE hInstance, HWND parent, TJpeg* _aboutJpeg );

// set width/height of a control without moving it
void SetControlHeight( HWND ctrl, int height );
void SetControlWidth( HWND ctrl, int width );
void SetControlFont( HWND ctrl, HFONT font, bool redraw );
void EnableControl( HWND ctrl, bool enable );

// edit assist routines
void EditPrint( HWND ctrl, char *text, ... );
void EditPrintSingle( HWND ctrl, char *text );
void EditClear( HWND ctrl );

// listbox assist routines
void ListBoxAddString( HWND ctrl, const char* str );

// checkbox/radio button assist routines
bool ButtonGetCheck( HWND ctrl );
void ButtonSetCheck( HWND ctrl, bool check );

// dropdownbox assist routines
int  DropDownBoxGetSelection( HWND ctrl ); // -1 if no selection

// point inside rect?
bool InsideRect( RECT& r, POINT& p );

#endif

