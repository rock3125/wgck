#ifndef __DIALOGS_DIALOGASSIST_H__
#define __DIALOGS_DIALOGASSIST_H__

#ifdef _USEGUI
#include <systemDialogs/gui.h>
#endif

class TJpeg;

// defined dialogs
#ifdef _SPEEDTEST
bool _EXPORT CreatePerformanceDlg( HINSTANCE hInstance, HWND parent );
void _EXPORT GetRating( TString& rating );
#else
void _EXPORT CreatePerformanceDlg( HINSTANCE hInstance, HWND parent );
#endif
void _EXPORT CreateVideoSetupDlg( HINSTANCE hInstance, HWND parent );
void _EXPORT CreateAboutDlg( HINSTANCE hInstance, HWND parent, TJpeg* _aboutJpeg );

// set width/height of a control without moving it
void _EXPORT SetControlHeight( HWND ctrl, int height );
void _EXPORT SetControlWidth( HWND ctrl, int width );
void _EXPORT SetControlFont( HWND ctrl, HFONT font, bool redraw );
void _EXPORT EnableControl( HWND ctrl, bool enable );
void _EXPORT ShowControl( HWND ctrl, bool show );
bool _EXPORT Enabled( HWND ctrl );

// edit assist routines
void _EXPORT EditPrint( HWND ctrl, char *text, ... );
void _EXPORT EditPrintSingle( HWND ctrl, const char *text );
void _EXPORT EditClear( HWND ctrl );
void _EXPORT EditGetText( HWND ctrl, char* buffer, size_t bufferSize );
int  _EXPORT EditGetInt( HWND ctrl );
float _EXPORT EditGetFloat( HWND ctrl );


// listbox assist routines
void _EXPORT ListBoxAddString1( HWND ctrl, const char* str );
void _EXPORT ComboBoxAddString( HWND ctrl, const char* str );
void _EXPORT ComboBoxDeleteString( HWND ctrl, size_t index );
void _EXPORT ComboBoxClear( HWND ctrl );
void _EXPORT ComboBoxDeleteStrings( HWND ctrl );

// checkbox/radio button assist routines
bool _EXPORT ButtonGetCheck( HWND ctrl );
void _EXPORT ButtonSetCheck( HWND ctrl, bool check );

// dropdownbox assist routines
int  _EXPORT DropDownBoxGetSelection( HWND ctrl ); // -1 if no selection
void _EXPORT DropDownBoxSetSelection( HWND ctrl, int sel );

// slider
void _EXPORT SliderSetFrequency( HWND ctrl, size_t freq );
void _EXPORT SliderSetRange( HWND ctrl, size_t min, size_t max );
void _EXPORT SliderSetPos( HWND ctrl, size_t pos );
void _EXPORT SliderSetPos( HWND ctrl, float pos );
void _EXPORT SliderGetPos( HWND ctrl, size_t& pos );
void _EXPORT SliderGetPos( HWND ctrl, float& pos );
int  _EXPORT SliderGetPos( HWND ctrl );

// point inside rect?
bool _EXPORT InsideRect( RECT& r, POINT& p );

char* TranslateString( char *inbuf, char* outbuf );

// additem to gui list
#ifdef _USEGUI
void _EXPORT AddItem( TGUI*& page, TGUI* item );
#endif

// scrollbars
void _EXPORT ScrollbarSetRange( HWND ctrl, int min, int max );
void _EXPORT ScrollbarSetPosition(HWND ctrl, int thumbPos);
int  _EXPORT ScrollbarGetPosition( HWND ctrl );
void _EXPORT ScrollbarGetRange( HWND ctrl, int& min, int& max);

#endif

