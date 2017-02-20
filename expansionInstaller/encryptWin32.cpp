#include <precomp_header.h>

#include <dos.h>
#include <direct.h>
#include <Commdlg.h>

#include <common/compatability.h>

//==========================================================================

bool LoadBinaryAs( TString& fname )
{
	OPENFILENAME ofn;
	memset( &ofn,0,sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "exe file\0*.exe\0All files\0*.*\0\0";
	char temp[256];
	temp[0] = 0;
	ofn.lpstrFile = temp;
	ofn.lpstrTitle = "Load payload to encrypt";
	ofn.nMaxFileTitle = 256;
	ofn.nMaxFile = 256;
	ofn.Flags = OFN_EXPLORER;
	if ( GetOpenFileName( &ofn )==TRUE )
	{
		fname = ofn.lpstrFile;
		return true;
	};
	return false;
};

//==========================================================================
//
// Entry point of all Windows programs
//
TString	_dir;

int APIENTRY WinMain( HINSTANCE hInst,
			   		  HINSTANCE hPrevInstance,
			   		  LPSTR lpCmdLine,
			   		  int nCmdShow )
{
	// select file to encrypt
	TString fname;
	if ( LoadBinaryAs(fname) )
	{
		// load the exe into a buffer
		FILE* fin = fopen( fname.c_str(), "rb" );
		if ( fin==NULL )
		{
			TString msg = "FATAL\n\n";
			msg = msg + "could not open payload for input\n";
			::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
			return 0;
		};

		// get size
		fseek( fin, 0, SEEK_END );
		size_t size = ftell( fin );
		fseek( fin, 0, SEEK_SET );

		unsigned char* ptr = (unsigned char*)malloc( size );
		if ( ptr==NULL )
		{
			fclose( fin );
			TString msg = "FATAL\n\n";
			msg = msg + "could not allocate memory for read\n";
			::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
			return 0;
		}

		if ( fread( ptr, 1, size, fin )!=size )
		{
			fclose( fin );
			free( ptr );
			TString msg = "FATAL\n\n";
			msg = msg + "could not read input file\n";
			::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
			return 0;
		}
		fclose( fin );

		// encrypt payload
		const char* code = "*;U2DQ4#C\"|)THF9$Y0V>~3^\\(GIN5=EA,Z!R.1-`WMKL/8@?OJ<P%&+B:6XS7 '";
		size_t codeLength = strlen(code);

		for ( size_t i=0; i<size; i++ )
		{
			ptr[i] = ptr[i] ^ (unsigned char)code[i%codeLength];
		}

		// open for output
		FILE* fh = fopen( "payload.bin", "wb" );
		if ( fh==NULL )
		{
			free( ptr );
			TString msg = "FATAL\n\n";
			msg = msg + "could not open payload.bin for output\n";
			::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
			return 0;
		}

		// write payload
		if ( fwrite( (char*)ptr, 1, size, fh ) != size )
		{
			fclose(fh);
			TString msg = "FATAL\n\n";
			msg = msg + "could not write payload to output file\n";
			::MessageBox( NULL, msg.c_str(), "Error", MB_OK | MB_ICONERROR );
			return 0;
		}
		fclose(fh);
		free( ptr );

		TString msg = "SUCCESS\n\n";
		msg = msg + "file encrypted as \"payload.bin\"\n";
		::MessageBox( NULL, msg.c_str(), "Information", MB_OK | MB_ICONINFORMATION );
	};

	return 0;
}

//==========================================================================

