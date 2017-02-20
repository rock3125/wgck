#include <precomp_header.h>

#include <common/compatability.h>
#include <win32/events.h>

#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#ifdef _USEGUI
#include <systemDialogs/gui.h>
#include <systemDialogs/dialogAssist.h>
#endif

//==========================================================================

TString logFile;
#ifndef _STORYBOARD
extern TString _dir;
#endif

//#define _ASSERT_OUTPUT

//==========================================================================
//
//	Throw an exception
//

Xmsg::Xmsg( char *msg )
{
	WriteString( "\n\n\nFATAL ERROR: %s\n\n\n", msg );
};

//==========================================================================

Xmsg::~Xmsg( void )
{
}

//==========================================================================

void AssertOut( const char* info, int l, const char* f )
{
	static char temp[1024];
	strcpy( temp, "PreCond: testing " );
	strcat( temp, info );
	strcat( temp, " (line " );

	char nmbr[10];
	itoa(l,nmbr,10);

	strcat( temp, nmbr );
	strcat( temp, ", file " );
	strcat( temp, f );
	strcat( temp, ")\n" );

	FILE* fh = fopen( "c:\\assert.txt", "a" );
	if ( fh!=NULL )
	{
		fprintf( fh, temp );
		fclose( fh );
	}
};

void PreCond1( bool pc, char *info, int l, char *f )
{
#ifdef _ASSERT_OUTPUT
	AssertOut( info,l,f);
#endif

	{
		char errmsg[256];
		if ( !pc )
		{
			sprintf(errmsg,"\nPRECONDITION FAILED.\n%s\nfile \"%s\"\nline %d.\n",info,f,l);
			WriteString( errmsg );
#ifdef _TANKGAME
			ResetScreenToStartupMode();
#endif
			exit(1);
		}
	}
}

//==========================================================================

void PostCond1( bool pc, char *info, int l, char *f )
{
#ifdef _ASSERT_OUTPUT
	AssertOut( info,l,f);
#endif
	{
		char errmsg[256];
		if ( !pc )
		{
			sprintf(errmsg,"\nPOSTCONDITION FAILED.\n%s\nfile \"%s\"\nline %d.\n",info,f,l);
			WriteString( errmsg );
#ifdef _TANKGAME
			ResetScreenToStartupMode();
#endif
			exit(1);
		}
	}
}

//==========================================================================

void SetLogFile( const TString& logFileName )
{
	if ( logFileName.length()==0 )
		logFile = "";
	else
	{
		logFile = logFileName;
		FILE* fh = fopen( logFile.c_str(), "w" );
		if ( fh!=NULL )
		{
			fprintf(fh,"-- LOG --\n" );
			fclose(fh);
		}
	}
};


void WriteString( const char* fmt, ... )
{
	char buf[1024];

	va_list ap;

	va_start(ap, fmt );
    vsprintf( buf, fmt, ap );
	va_end(ap);

	printf( buf );
	if ( logFile.length()>0 )
	{
		TString fname;
		ConstructPath( fname, _dir, logFile );
		FILE* fh = fopen( fname.c_str(), "a" );
		if ( fh!=NULL )
		{
			fprintf( fh, buf );
			fclose( fh );
		}
	}
}

//==========================================================================

size_t prev_percentage;

void ResetPercentage( void )
{
	prev_percentage = 101;
}

void DisplayPercentage( size_t current, size_t max )
{
	float p = (float(current) / float(max)) * 100.0f;
	size_t percentage = size_t(p);
	if ( percentage!=prev_percentage )
	{
		if ( prev_percentage<=101 )
		{
			size_t ssize;
			if ( prev_percentage<10 )
				ssize = 3;
			else if ( prev_percentage<100 )
				ssize = 4;
			else
				ssize = 5;
			for ( size_t i=0; i<ssize; i++ )
				printf( "\b" );
		}
		prev_percentage = percentage;
		printf( "%d %%", percentage );
	}
};


void FinishPercentage( void )
{
	if ( prev_percentage<=101 )
	{
		size_t ssize;
		if ( prev_percentage<10 )
			ssize = 3;
		else if ( prev_percentage<100 )
			ssize = 4;
		else
			ssize = 5;
		for ( size_t i=0; i<ssize; i++ )
			printf( "\b" );
	}
};


// "Software\\PDV\\Performance", "TestCompleted"
bool GetRegistryKey( const TString& keyName, const TString& keyItem, size_t& val )
{
	// write this info to registry
	HKEY  keyRef;
	LONG res = RegOpenKeyEx( HKEY_LOCAL_MACHINE, keyName.c_str(),
							 0, KEY_READ, &keyRef );
	if ( res==ERROR_SUCCESS )
	{
		DWORD size = sizeof(size_t);
		DWORD type = REG_DWORD;
		res = RegQueryValueEx( keyRef, keyItem.c_str(), 
								0, &type, (BYTE*)&val,
								&size );
		RegCloseKey( keyRef );

		if ( res==ERROR_SUCCESS )
		{
			return true;
		}
		else
			return false;
	}
	else
		return false;
};


bool GetRegistryKey( const TString& keyName, const TString& keyItem, TString& str )
{
	// write this info to registry
	HKEY  keyRef;
	LONG res = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, keyName.c_str(),
							   0, KEY_READ, &keyRef );
	if ( res==ERROR_SUCCESS )
	{
		char buf[1024];
		DWORD size = 1024;
		DWORD type = REG_SZ;
		res = ::RegQueryValueEx( keyRef, keyItem.c_str(), 
								0, &type, (BYTE*)buf, &size );
		if ( res==ERROR_SUCCESS )
		{
			str = buf;
			::RegCloseKey( keyRef );
			return true;
		}
		else
		{
			::RegCloseKey( keyRef );
			return false;
		}
	}
	else
		return false;
};


bool SetRegistryKey( const TString& keyName, const TString& keyItem, size_t val )
{
	// write this info to registry
	HKEY  keyRef;
	DWORD result;
	LONG res = RegCreateKeyEx( HKEY_LOCAL_MACHINE, keyName.c_str(),
							   0, NULL, REG_OPTION_NON_VOLATILE, 
							   KEY_SET_VALUE, NULL, &keyRef, &result );
	if ( res==ERROR_SUCCESS )
	{
		res = RegSetValueEx( keyRef, keyItem.c_str(), 
							 0, REG_DWORD, (const BYTE*)&val, 
							 sizeof(size_t) );
		if ( res==ERROR_SUCCESS )
		{
			RegCloseKey( keyRef );
			return true;
		}
		else
			return false;
		
	}
	else
		return false;
};


bool SetRegistryKey( const TString& keyName, const TString& keyItem, const TString& str )
{
	if ( str.length()==0 )
		TString astr = "";
	PreCond( str.length()>0 );

	// write this info to registry
	HKEY  keyRef;
	DWORD result;
	LONG res = RegCreateKeyEx( HKEY_LOCAL_MACHINE, keyName.c_str(),
							   0, NULL, REG_OPTION_NON_VOLATILE, 
							   KEY_SET_VALUE, NULL, &keyRef, &result );
	if ( res==ERROR_SUCCESS )
	{
		char buf[1024];
		strcpy( buf, str.c_str() );
		DWORD size = strlen(buf);
		res = RegSetValueEx( keyRef, keyItem.c_str(), 
							 0, REG_SZ, (const BYTE*)&buf, size );
		if ( res==ERROR_SUCCESS )
		{
			RegCloseKey( keyRef );
			return true;
		}
		else
			return false;
		
	}
	else
		return false;
};


TPersist::TPersist( void )
	: fh(NULL),
	  access(0)
{
	errorStr = "";
};


TPersist::TPersist( size_t _access )
	: fh(NULL),
	  access(_access)
{
};


TPersist::TPersist( const TPersist& f )
{
	operator=(f);
};


TPersist::~TPersist( void )
{
	access = 0;
	if ( fh!=NULL )
		fclose(fh);
	fh = NULL;
};


const TPersist& TPersist::operator=( const TPersist& f )
{
	fh = f.fh;
	errorStr = f.errorStr;
	filename = f.filename;
	access = f.access;

	return *this;
};


bool TPersist::FileOpen( const TString& fname )
{
	filename = fname;

	errorStr = "";

	if ( access == fileRead )
	{
		fh = fopen( fname.c_str(), "rb" );
		if ( fh==NULL )
		{
			errorStr = "Could not open file '" + fname + "' for reading";
			return false;
		}
		return true;
	}
	else if ( access == fileWrite )
	{
		fh = fopen( fname.c_str(), "wb" );
		if ( fh==NULL )
		{
			errorStr = "Could not open file '" + fname + "' for writing";
			return false;
		}
		return true;
	}
	else
	{
		errorStr = "File '" + fname + "' unknown access type";
		return false;
	}
}


bool TPersist::ReadLine( TString& line )
{
	if ( fh==NULL )
	{
		errorStr = "ReadLine: error, fh==NULL";
		return false;
	}
	else if ( access!=fileRead )
	{
		errorStr = "ReadLine: file not opened with read access";
		return false;
	}

	TString l;
	char ch;
	bool eol = false;
	do
	{
		// eof?
		if ( fread( &ch,1,1,fh )==0 )
		{
			line = l;
			return false;
		}

		if ( ch>=32 && ch<=127 )
		{
			l = l + TString(ch);
		}
		else if ( ch==10 || ch==13 || ch==0 )
		{
			if ( l.length()>0 )
				eol = true;
		}
	}
	while ( !eol );

	line = l;

	return true;
};


bool TPersist::ReadString( TString& line )
{
	if ( fh==NULL )
	{
		errorStr = "ReadString: error, fh==NULL";
		return false;
	}
	else if ( access!=fileRead )
	{
		errorStr = "ReadString: file not opened with read access";
		return false;
	}

	TString l;
	char ch;
	do
	{
		if ( fread( &ch,1,1,fh )==0 )
		{
			line = l;
			return false;
		}

		if ( ch>=32 && ch<=127 )
		{
			l = l + TString(ch);
		}
	}
	while ( ch!=0 );

	line = l;

	return true;
};


bool TPersist::FileRead( void* buf, size_t size )
{
	if ( fh==NULL )
	{
		errorStr = "FileRead: error, fh==NULL";
		return false;
	}
	else if ( access!=fileRead )
	{
		errorStr = "FileRead: file not opened with read access";
		return false;
	}
	if ( fread( buf, 1, size, fh )!=size )
	{
		errorStr = "FileRead: read past end of file";
		return false;
	}
	return true;
};


bool TPersist::FileWrite( const void* buf, size_t size )
{
	if ( fh==NULL )
	{
		errorStr = "FileWrite: error, fh==NULL";
		return false;
	}
	else if ( access!=fileWrite )
	{
		errorStr = "FileWrite: file not opened with write access";
		return false;
	}
	if ( fwrite( buf, 1, size, fh )!=size )
	{
		errorStr = "FileWrite: could not write to file";
		return false;
	}
	return true;
};


bool TPersist::FileClose( void )
{
	if ( fh==NULL )
	{
		errorStr = "FileClose: error, fh==NULL";
		return false;
	}
	fclose( fh );
	fh = NULL;
	return true;
};


bool TPersist::FileExists( const TString& fname )
{
	FILE* fh = fopen( fname.c_str(), "rb" );
	if ( fh==NULL )
		return false;
	else
	{
		fclose(fh);
		return true;
	}
};


long TPersist::FileSize( void )
{
	if ( fh==NULL )
	{
		errorStr = "FileSize: error, fh==NULL";
		return 0;
	}

    // aquire file size
    fseek(fh,0,SEEK_END);
    long fsize = ftell(fh);
    fseek(fh,0,SEEK_SET);

	return fsize;
};


const TString& TPersist::ErrorString( void ) const
{
	return errorStr;
};


const TString& TPersist::FileName( void ) const
{
	return filename;
};

//==========================================================================

void ConstructPath( TString& buf, const TString pathname, const TString fname )
{
	// make sure _dir and pathname aren't the same
	TString newPath;
#ifndef _STORYBOARD
	size_t index = 0;
	while ( index<_dir.length() && index<pathname.length() &&
			_dir[index]==pathname[index] ) index++;
	if ( index>0 )
	{
		newPath = pathname;
	}
	else
	{
		newPath = _dir + "\\" + pathname;
	}
#endif

	buf = "";
	if ( newPath.length()>0 )
    {
		buf = newPath;
        size_t index = buf.length();
        if ( buf[index-1]!='\\' )
        {
			buf = buf + "\\";
        }
    }
	buf = buf + fname;
};


TString SplitPath( const TString buf, TString& pathname, TString& fname )
{
	if ( buf.length()>0 )
    {
		size_t index = buf.length();
		while ( buf[index]!='\\' && index>0 ) index--;
		if ( index>0 )
		{
			pathname = buf.substr(0,index);
			fname = buf.substr(index+1);
		}
		else
		{
			pathname = "";
			fname = buf;
		}
    }
    return fname;
};

//==========================================================================
// return a number between [0..1]

float Random( void )
{
	double r = double(rand()) / double(RAND_MAX);
	return float(r);
};

//==========================================================================

#if !defined(_NETTEST) && !defined(_NETDLL)

void ProcessMessages( TEvent* app, MSG& msg, size_t& fpsTimer )
{
	PreCond( app!=NULL );

	size_t time = ::GetTickCount();
	BOOL bGotMsg = ::PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
	if( bGotMsg )
    {
		// Translate and dispatch the message
		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
    }
#if !defined(_NOGAME) && !defined(_SETUPPROGRAM) && !defined(_MDLLIB)
	else if ( app->Active() || app->LimitFPS() )
	{
		if ( app->LimitFPS() )
		{
			if ( time > fpsTimer )
			{
				fpsTimer = time + 67; // 15 fps for now
				::InvalidateRect( app->Window(), NULL, FALSE );
			}
			else
			{
				::WaitMessage();
			}
		}
		else
		{
			::InvalidateRect( msg.hwnd, NULL, FALSE );
		}
	}
	else if ( !app->Active() ) // be nice!
#endif
	{
		::WaitMessage();
	}
};

#endif

//==========================================================================

int Message( TEvent* app, const TString& msg, const TString& title, size_t flags )
{
	PreCond( app!=NULL );
#ifdef _USEGUI
	size_t i;
	size_t languageId = app->LanguageId();

	// display my own type of message box
	// first split msg into multiple lines according to \n
	TString lines[20];
	size_t cntr = 0;
	size_t prev = 0;
	for ( i=0; i<msg.length() && cntr<20; i++ )
	{
		if ( msg[i]=='\n' && (i+1)!=msg.length() )
		{
			if ( i!=prev)
				lines[cntr] = msg.substr(prev,(i-prev));
			prev = i+1;
			cntr++;
		}
	}
	lines[cntr++] = msg.substr(prev,(i-prev));

	float boxWidth = 350;
	float boxHeight = (float(cntr) * 16) + 70;

	TGUI* msgbox = NULL;
	AddItem( msgbox, new TGUI( app, guiWindow, 0,0, boxWidth, boxHeight, -4, title ) );

	TGUI* okButton = NULL;
	TGUI* yesButton = NULL;
	TGUI* noButton = NULL;
	if ( (flags&MB_YESNO) > 0 )
	{
		float half = boxWidth * 0.5f;
		yesButton = new TGUI( app, guiButton, half-45, boxHeight-34, 40, 25, -3, STR(strYes) );
		noButton = new TGUI( app, guiButton, half+5, boxHeight-34, 40, 25, -3, STR(strNo) );
		yesButton->ButtonKey( 13 );
		noButton->ButtonKey( VK_ESCAPE );
		AddItem( msgbox, yesButton );
		AddItem( msgbox, noButton );
	}
	else
	{
		float half = boxWidth * 0.5f;
		okButton = new TGUI( app, guiButton, half-20, boxHeight-34, 40, 25, -3, STR(strOK) );
		okButton->ButtonKey( 13 );
		AddItem( msgbox, okButton );
	}

	// add message labels
	for ( i=0; i<cntr; i++ )
	{
		if ( lines[i].length()>0 )
		AddItem( msgbox, new TGUI( app, guiLabel, 20, float(25+i*15), boxWidth-20, 14, -3, lines[i],
									255,255,255 ) );
	}

	// center box on screen
	msgbox->SetLocation( app->Width() * 0.5f - (boxWidth*0.5f), app->Height() * 0.5f - (boxHeight*0.5f) );

	// go into message processing loop for this box
	TGUI* prevbox = app->CurrentPage();
	app->CurrentPage( msgbox );
	bool exit = false;

	MSG winmsg;
	size_t fpsTimer = ::GetTickCount();
	while ( !exit )
	{
		::ProcessMessages( app, winmsg, fpsTimer );
		if ( okButton!=NULL )
		if ( okButton->Clicked() )
		{
			app->CurrentPage( prevbox );
			delete msgbox;
			msgbox = NULL;
			return IDOK;
		}
		if ( yesButton!=NULL )
		if ( yesButton->Clicked() )
		{
			app->CurrentPage( prevbox );
			delete msgbox;
			msgbox = NULL;
			return IDYES;
		}
		if ( noButton!=NULL )
		if ( noButton->Clicked() )
		{
			app->CurrentPage( prevbox );
			delete msgbox;
			msgbox = NULL;
			return IDNO;
		}
	}
	return IDOK;
#else
	return ::MessageBox( NULL, msg.c_str(), title.c_str(), flags );
#endif
}

//==========================================================================
// rego stuff
//

// list of 64 valid symbols
const char* validSymbols = "<J47=~5K;BS^&|NTY0Z`Q#/-L$*FV>X:.HC?\\)RAW\"UGDI'O2 %1+6@,MP8E(!93";

// list of 32 encode symbols
const char* alphaBet = "AEIU7B1CD9NM23YT465JK8LSZFGHPQRV";

// given an alphabet and an alphabet size - return its index
size_t CharToIndex( const TString& abc, size_t size, char ch )
{
	for ( size_t i=0; i<size; i++ )
		if ( abc[i]==ch )
			return i;
	return 0;
};

size_t HexToInt( const TString& str )
{
	size_t res = 0;
	for ( size_t i=0; i<str.length(); i++ )
	{
		size_t ch = (size_t)str[i];
		if ( ch>='0' && ch<='9' )
		{
			ch = ch - '0';
		}
		else if ( ch>='A' && ch<='F' )
		{
			ch = (ch - 'A') + 10;
		}
		else
		{
			return 0;
		};
		res = res * 16 + ch;
	}
	return res;
};

#ifdef _ISADMIN
TString GenerateCode( const TString& name )
{
	size_t i;
	TString process = name.ucase();

	size_t checksum[4];
	for ( i=0; i<4; i++ )
		checksum[i] = 0;

	char code[256];
	size_t len = process.length();
	for ( i=0; i<len; i++ )
	{
		code[i] = CharToIndex( validSymbols,64,process[i]);
		checksum[i%4] += size_t(code[i]);
	}
	code[len] = 0;
	len++;

	// convert these 6 bit symbols to 5 bit symbols
	char newCode[256];
	size_t ncIndex = 0;
	size_t index = 0;
	char ch1 = 0;
	char ch2 = code[0];
	while ( (index/6) < len )
	{
		if ( (index%5)==0 && index>0 )
		{
			newCode[ncIndex++] = ch1;
			ch1 = 0;
		}
		if ( (index%6)==0 && index>0 )
		{
			ch2 = code[index/6];
		}
		ch1 = ch1 << 1;
		if ( (ch2&32)>0 )
			ch1 = ch1 | 1;
		ch2 = ch2 << 1;
		index++;
	}
	newCode[ncIndex++] = ch1;

	// use newCode to generate the actual string of alphaBet characters
	// use code to return this value
	for ( i=0; i<256; i++ )
		code[i] = 0;
	for ( i=0; i<ncIndex; i++ )
	{
		code[i] = char(alphaBet[newCode[i]]);
	}

	// generate checksum
	for ( i=0; i<4; i++ )
	{
		checksum[i] = (checksum[i] & 31);
		code[ncIndex++] = alphaBet[checksum[i]];
	}
	code[ncIndex] = 0;

	return TString(code);
};
#endif


size_t GetNumDaysBeforeExpire( void )
{
	time_t t;
	time(&t);

	// "Software\\Microsoft\\Windows", "exp"
	size_t val;
	if ( !GetRegistryKey( "Software\\Microsoft\\Windows", "exp", val ) )
	{
		val = size_t(t);
		SetRegistryKey("Software\\Microsoft\\Windows", "exp", val );
		return 30;
	}
	else
	{
		time_t treg = (time_t)val;
		double sec = difftime( t, treg );
		double days = sec / (60*60*24);
		size_t d = size_t(days);
		if ( d>30 )
			return 0;
		else
			return (30-d);
	}
};


bool HasExpired( void )
{
	if ( GetNumDaysBeforeExpire()==0 )
		return true;
	return false;
};


//
// use GetAsyncKeyState()
// if you want to distinguish between RShift, LShift etc.
// otherwise WM_KEYDOWN will only give you Shift etc.
//
TString KeyToString( size_t languageId, size_t key )
{
	if ( key>='0' && key<='9' )
		return TString(char(key));
	if ( key>='A' && key<='Z' )
		return TString(char(key));
	switch ( key )
	{
	case VK_LBUTTON:
        return STR(strbLeftMouse);
	case VK_RBUTTON:        
        return STR(strbRightMouse);
	case VK_CANCEL:
		return STR(strbCancel);
	case VK_MBUTTON:        
        return STR(strbMiddleMouse);
	case VK_BACK:
		return STR(strbBack);
	case VK_TAB:
		return STR(strbTab);
	case VK_CLEAR:
		return STR(strbCLR);
	case VK_RETURN:
		return STR(strbEnter);
	case VK_MENU:
		return STR(strbMenu);
	case VK_PAUSE:
		return STR(strbPause);
	case VK_CAPITAL:
        return STR(strbCaps);
	case VK_ESCAPE:
		return STR(strbEscape);
	case VK_CONVERT:
        return STR(strbConvert);
	case VK_NONCONVERT:
		return STR(strbNonConvert);
	case VK_ACCEPT:
		return STR(strbAccept);
	case VK_MODECHANGE:
		return STR(strbModeChange);
	case VK_SPACE:
		return STR(strbSpaceBar);
	case VK_PRIOR:
		return STR(strbPrior);
	case VK_NEXT:
		return STR(strbNext);
	case VK_END:
		return STR(strbEnd);
	case VK_HOME:
		return STR(strbHome);
	case VK_LEFT:
		return STR(strbLeft);
	case VK_UP:
		return STR(strbUp);
	case VK_RIGHT:
		return STR(strbRight);
	case VK_DOWN:
		return STR(strbDown);
	case VK_SELECT:
		return STR(strbSelect);
	case VK_PRINT:
		return STR(strbPrint);
	case VK_EXECUTE:
        return STR(strbExec);
	case VK_SNAPSHOT:
		return STR(strbScreenshot);
	case VK_INSERT:
		return STR(strbInsert);
	case VK_DELETE:
		return STR(strbDel);
	case VK_HELP:
		return STR(strbHelp);
	case VK_LWIN:
		return "LWIN";
	case VK_RWIN:
		return "RWIN";
	case VK_APPS:
		return "Apps";
	case VK_NUMPAD0:
        return "Keypad 0";
	case VK_NUMPAD1:        
        return "Keypad 1";
	case VK_NUMPAD2:        
        return "Keypad 2";
	case VK_NUMPAD3:        
        return "Keypad 3";
	case VK_NUMPAD4:        
        return "Keypad 4";
	case VK_NUMPAD5:        
        return "Keypad 5";
	case VK_NUMPAD6:        
        return "Keypad 6";
	case VK_NUMPAD7:        
        return "Keypad 7";
	case VK_NUMPAD8:      
        return "Keypad 8";
	case VK_NUMPAD9:        
        return "Keypad 9";
	case VK_MULTIPLY:
		return "*";
	case VK_ADD:
		return "+";
	case VK_SEPARATOR:
		return "Sep";
	case VK_SUBTRACT:
		return "-";
	case VK_DECIMAL:
        return ".";
	case VK_DIVIDE:
		return "/";
	case VK_F1:
		return "F1";
	case VK_F2:             
		return "F2";
	case VK_F3:             
		return "F3";
	case VK_F4:            
		return "F4";
	case VK_F5:             
		return "F5";
	case VK_F6:             
		return "F6";
	case VK_F7:             
		return "F7";
	case VK_F8:             
		return "F8";
	case VK_F9:             
		return "F9";
	case VK_F10:            
		return "F10";
	case VK_F11:            
		return "F11";
	case VK_F12:            
		return "F12";
	case VK_F13:            
		return "F13";
	case VK_F14:            
		return "F14";
	case VK_F15:            
		return "F15";
	case VK_F16:            
		return "F16";
	case VK_F17:            
		return "F17";
	case VK_F18:            
		return "F18";
	case VK_F19:            
		return "F19";
	case VK_F20:            
		return "F20";
	case VK_F21:            
		return "F21";
	case VK_F22:            
		return "F22";
	case VK_F23:            
		return "F23";
	case VK_F24:            
		return "F24";
	case VK_NUMLOCK:
        return "NumLock";
	case VK_SCROLL:
		return "Scroll";
	case VK_LMENU:
		return "LMenu";
	case VK_RMENU:
		return "RMenu";
	case VK_ATTN:
		return "Attn";
	case VK_CRSEL:
		return "CrSel";
	case VK_EXSEL:
		return "ExSel";
	case VK_EREOF:
		return "Eof";
	case VK_PLAY:
		return STR(strbPlay);
	case VK_ZOOM:
		return STR(strbZoom);
	case VK_NONAME:
		return STR(strbNoName);
	case VK_PA1:
		return "PA1";
	case VK_OEM_CLEAR:
		return "OEMClear";
	}
	return "";
}

#ifdef _USEGUI
size_t GetUserDefinedKey( TEvent* app )
{
	size_t key = 0;
	size_t languageId = app->LanguageId();

	// Get user defined key dialog
	TGUI* userDefKey = NULL;
	AddItem( userDefKey, new TGUI( app, guiWindow, 0, 0, 300, 110, -2, STR(strPressAnyKey) ) );
	AddItem( userDefKey, new TGUI( app, guiLabel, 20, 20, 200, 14, -2, STR(strInstructions1), 255,255,255 ) );
	TGUI* buttonCancel = new TGUI( app, guiButton, 100, 70, 80, 25, -2, STR(strCancel) );

	buttonCancel->ButtonKey( VK_ESCAPE );
	AddItem( userDefKey, buttonCancel );

	userDefKey->SetLocation( 140,140 );

	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( userDefKey );

	MSG msg;
	size_t fpsTimer = ::GetTickCount();
	bool closing = false;
	while ( !closing )
	{
		::ProcessMessages( app, msg, fpsTimer );
		if ( buttonCancel->Clicked() )
		{
			closing = true;
		}
		else if ( msg.message==WM_LBUTTONDOWN )
		{
			closing = true;
			key = VK_LBUTTON;
		}
		else if ( msg.message==WM_RBUTTONDOWN )
		{
			closing = true;
			key = VK_RBUTTON;
		}
		else if ( msg.message==WM_KEYDOWN )
		{
			closing = true;
			key = msg.wParam;
		}
	}
	app->CurrentPage( prevPage );
	return key;
};
#endif

//==========================================================================
