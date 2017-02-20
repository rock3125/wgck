#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#pragma hdrstop

#include <parser/lex.h>
#include <parser/ConsoleMain.h>
#include <common/tstring.h>

#include <common/compatability.h>
#include <object/binobject.h>
#include <object/landscape.h>
#include <object/compoundObject.h>
#include <object/anim.h>

#include <conio.h>
#ifndef _MSVC
#include <dir.h>
#else
#include <direct.h>
#endif
#include <process.h>


//==========================================================================

bool useNormals = true;
bool batchMode = false;
bool globalQuit = false;
HINSTANCE hInstance = NULL;
FILE* batchFile = NULL;
size_t kMinRenderDepth = 3;
size_t kMaxRenderDepth = 10;
size_t renderDepth = 10;

//==========================================================================

void ConsolePrint( char* fmt, ... )
{
	char buf[1024];

	va_list ap;

	va_start(ap, fmt );
    vsprintf( buf, fmt, ap );
	va_end(ap);

	WriteString( "%s\n", buf );
};

//==========================================================================

DXConsole::DXConsole( void )
{
	whoStr = "\nGame Object Converter Version 1.12\nWritten by Peter de Vocht, 2001\n\n";

	WriteString( whoStr );
    WriteString( "help  -- list of commands\n");
    WriteString( "end   -- exit\n\n\n");
};

//==========================================================================

DXConsole::~DXConsole( void )
{
};

//==========================================================================

bool DXConsole::ProcessInput( void )
{
	bool finish = false;
    TString line = GetInput();

    if ( line=="end" || line=="bye" ||
    	 line=="exit" || line=="quit" )
    {
        finish = true;
		globalQuit = true;
    }
    else if ( line=="help" )
        Help();
#ifndef _MSVC
    else if ( line=="cls" )
    {
        clrscr();
    }
#endif
    else if ( strncmp( line.c_str(), "show ", 5 )==0 )
    {
    	WriteString( "executing \"Viewer %s\"\n", &line[5] );
        char buf[256];
        getcwd(buf,255);
        strcat(buf, "\\win32viewer.exe" );
		spawnl( P_NOWAIT, buf, "win32viewer.exe", &line[5], NULL);
    }
    else if ( strncmp( line.c_str(), "batch ", 6 )==0 )
    {
		finish = DoBatch( &line[6] );
    }
    else if ( strncmp( line.c_str(), "convert ", 8 )==0 )
    {
        WriteString( "converting file \"%s\"\n", &line[8] );
    	Lex lex;
        TBinObject* obj = lex.DirectX( &line[8] );
        if ( obj!=NULL )
        {
        	TString newFname = &line[8];
			newFname = newFname.GetItem('.',0) + ".bin";
        	WriteString( "saving as \"%s\"\n", newFname.c_str() );

        	if ( !obj->SaveBinary( newFname ) )
				WriteString( "error writing file %s\n", newFname.c_str() );
            delete obj;
        }
        else
        	WriteString( "%s\n", lex.ErrorString() );
    }
    else if ( strncmp( line.c_str(), "createobj ", 10 )==0 )
    {
		TString temp;
		TString objName;
		TString texturePath;

		TString names[256];
		size_t j;
		for ( j=0; j<256; j++ )
			names[j] = "";

		temp = &line[10];
		size_t count = temp.NumItems(' ');
		for ( j=0; j<count; j++ )
		{
			TString ttemp = temp.GetItem(' ',j);
			if ( j==0 )
			{
				objName = ttemp;
			}
			else if ( j==1 )
			{
				texturePath = ttemp;
			}
			else
			{
				names[j-2] = ttemp;
			}
		}

		if ( j<3 )
		{
			WriteString( "\nError: takes at least 3 parameters, objectName, texturePath, objects...\n" );
			return finish;
		}
		TCompoundObject obj;
		if ( obj.Create( objName, count-2, texturePath, (TString*)names ) )
			WriteString( "done\n" );
	}
    else if ( strncmp( line.c_str(), "createanim ", 11 )==0 )
    {
		TString temp = &line[11];
		TString fname,anim,texturePath,numAnimsStr;
		size_t numAnims;

		fname = temp.GetItem(' ',0);
		anim = temp.GetItem(' ',1);
		texturePath = temp.GetItem(' ',2);
		numAnimsStr = temp.GetItem(' ',3);

		numAnims = atoi(numAnimsStr.c_str());
		if ( numAnims<=0 )
		{
			WriteString( "this command takes 4 parameters.  Type 'help'\n" );
			WriteString( "to find out what these parameters are.\n" );
			WriteString( "fourth parameter must be a number >= 1\n" );
			return finish;
		}

		TAnimation animation;
		TString errStr;
		if ( !animation.CreateAnimation( anim, texturePath, numAnims, errStr ) )
		{
			WriteString( errStr.c_str() );
			return finish;
		}

		if ( !animation.SaveBinary( fname ) )
		{
			WriteString( "could not save as \"%s\"\n", fname.c_str() );
		}
		else
		{
			WriteString( "done\n" );
		}
	}
    else if ( strncmp( line.c_str(), "createland ", 11 )==0 )
    {
		TString temp = &line[11];
		TString fname;
		TString path;
		TString tpath;
		TString divx;
		TString divy;

		fname = temp.GetItem(' ',0);
		tpath = temp.GetItem(' ',1);
		divx = temp.GetItem(' ',2);
		divy = temp.GetItem(' ',3);

		SplitPath( fname, path, fname );

		TString newFname = fname;
		if ( newFname.NumItems('.')>1 )
		{
			newFname = newFname.GetItem('.',0) + ".lnd";
		}
		else
		{
			newFname = newFname + ".lnd";
		}

        WriteString( "Creating landscape file \"%s\"\n", newFname.c_str() );
        WriteString( "\nNB. this might take a few minutes as I am creating\n    a directional graph of the landscape to save.\n\n", newFname );

		TString errStr;
		TObject tempObj;
		TLandscape obj;
		if ( !tempObj.LoadBinary( fname, errStr, path, tpath ) )
		{
			WriteString( errStr.c_str() );
		 	return finish;
		}
	
		if ( !obj.DivideMeshes( tempObj, atoi(divx.c_str()), 
								atoi(divy.c_str()), errStr, 
								fname, path, tpath ) )
		{
			WriteString( errStr.c_str() );
		 	return finish;
		};
		obj.MaterialList( tempObj.MaterialList() );
		tempObj.MaterialList(NULL);
		tempObj.MeshList(NULL);

		ConstructPath(temp,path,newFname);
		obj.SaveBinary(temp);
		WriteString( "\ndone\n" );
	}
    return finish;
};


bool DXConsole::DoBatch( const TString& fname )
{
   	WriteString( ">>> executing batch file \"%s\"\n", fname.c_str() );
	batchFile = fopen( fname.c_str(), "rb" );
	if ( batchFile!=NULL )
	{
		batchMode = true;
		bool done = false;
		while ( !feof(batchFile) && !done )
		{
			WriteString( "\n" );
			done = ProcessInput();
		}
		fclose( batchFile );
		batchFile = NULL;
		batchMode = false;

		return done;
	}
	else
		WriteString( "can't open \"%s\"\n", fname.c_str() );
	return false;
};


TString DXConsole::GetInput( void )
{
	TString line;

	if ( !batchMode )
	{
		WriteString( ">" );
		int index = 0;
		bool done = false;
	    do
	    {
    		char ch;
    		fscanf( stdin, "%c", &ch );
			if ( ch=='\n' )
	        	done = true;
			else
			{
	        	line = line + TString(ch);
			}
		}
		while ( !done );
	}
	else if ( batchFile!=NULL )
	{
		int index = 0;
		bool found = false;
		while ( !found && !feof(batchFile) )
		{
			char ch;
			fread(&ch,1,1,batchFile);
			if ( ch==13 || ch==10 || ch=='\n' || ch=='\r' )
				found = true;
			else
				line = line + TString(ch);
		}
	}
	return line;
};

//==========================================================================

void DXConsole::Help( void )
{
    WriteString( "======================================================================\n");
	WriteString( whoStr );
    WriteString( "======================================================================\n");
    WriteString( "end (or bye, or exit)                        -- exit\n");
    WriteString( "help                                         -- this text\n");
#ifndef _MSVC
    WriteString( "cls                                          -- clear screen\n");
#endif
    WriteString( "batch filename.txt                           -- execute a series of dxparser\n");
	WriteString( "                                             -- commands from file\n");
    WriteString( "createobj objName texturepath file1.bin ...  -- merge objects into one at\n");
	WriteString( "          fileX.bin                          -- same location\n");
    WriteString( "convert filename.x texturepath [-merge]      -- load directX text file\n");
    WriteString( "        [-centerx] [-centery] [-centerz]     -- opt: merge all meshes in\n");
    WriteString( "        [-normalise] [-createnormals]        -- file into a single mesh\n");
	WriteString( "                                             -- & convert it to .bin\n");
    WriteString( "createland filename.x texturePath Xdiv Ydiv  -- Create subdivision .lnd\n");
	WriteString( "                                             -- file from a .bin\n");
    WriteString( "createanim filename.anm anim%%%%d.bin texturePath numAnims --\n");
	WriteString( "                                             -- Create animation meshes\n");
	WriteString( "                                             -- from a series of .bin\n");
	WriteString( "                                             -- files\n");
    WriteString( "show filename[.bin|.lnd|.anm]                -- load .bin, .lnd, or .anm file\n");
	WriteString( "                                             -- & view it graphically\n");
    WriteString( "======================================================================\n");
    WriteString( "\n");
};

//==========================================================================

void main( int argc, char* argv[] )
{
	DXConsole* console = new DXConsole();

    // forever (until quit is entered)
    bool finished = false;
	bool execOnce = false;
	while (!finished && !globalQuit)
	{
		if ( argc>1 && !execOnce )
		{
			execOnce = true;
			finished = console->DoBatch( argv[1] );
		}
		else
    		finished = console->ProcessInput();
	}

    if ( console!=NULL )
    	delete console;
};

//==========================================================================


