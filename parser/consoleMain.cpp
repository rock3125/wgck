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
#include <common/string.h>

#include <common/compatability.h>
#include <object/binobject.h>
#include <object/landscape.h>
#include <object/compoundObject.h>
#include <object/anim.h>
#include <object/deffile.h>

#include <conio.h>
#include <direct.h>
#include <process.h>

//==========================================================================
// used by AIs - overall logic counter
size_t logicCounter = 0;
size_t aiId = 1;

//==========================================================================

bool useNormals = true;
bool batchMode = false;
bool globalQuit = false;
HINSTANCE hInstance = NULL;
FILE* batchFile = NULL;
size_t kMinRenderDepth = 3;
size_t kMaxRenderDepth = 10;
size_t renderDepth = 10;
HWND mainWindow = NULL;
HGLRC hglrc = NULL;
TString _dir;

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
	whoStr = "\nGame Object Converter Version 1.26\nWritten by Peter de Vocht, 2002\n\n";

	WriteString( whoStr );
    WriteString( "help  -- list of commands\n");
    WriteString( "end   -- exit\n\n\n");
};

//==========================================================================

DXConsole::~DXConsole( void )
{
};

//==========================================================================

int ChoosePixelFormatEx( HDC hdc,int& bpp,int& depth,
						 int& dbl,int& acc, int& qFlag,
						 int& accelType )
{
	int wbpp; 
	int wdepth; 
	int wdbl; 
	int wacc; 

	wbpp= bpp; 
	wdepth=depth;
	wdbl=dbl; 
	wacc=acc; 
	accelType = 0;

	PIXELFORMATDESCRIPTOR pfd; 
	
	ZeroMemory(&pfd,sizeof(pfd)); 
	pfd.nSize=sizeof(pfd); 
	pfd.nVersion=1;
	
	int num = DescribePixelFormat(hdc,1,sizeof(pfd),&pfd);

	if (num==0) 
		return 0;
  
	int maxqual=0; 
	int maxindex=0;
	int max_bpp, max_depth, max_dbl, max_acc;
	bool mcd,icd,soft;
	
	for (int i=1; i<=num; i++)
	{
		ZeroMemory(&pfd,sizeof(pfd)); 
		pfd.nSize = sizeof(pfd); 
		pfd.nVersion=1;
    
		DescribePixelFormat(hdc,i,sizeof(pfd),&pfd);
		int bpp=pfd.cColorBits;
		int depth=pfd.cDepthBits;
		bool pal=(pfd.iPixelType==PFD_TYPE_COLORINDEX);

		mcd=((pfd.dwFlags & PFD_GENERIC_FORMAT) && (pfd.dwFlags & PFD_GENERIC_ACCELERATED));
		soft=((pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));
		icd=(!(pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED));

		if ( mcd ) 
			accelType = 1;
		else if ( icd )
			accelType = 2;

		bool opengl = (pfd.dwFlags & PFD_SUPPORT_OPENGL)>0;
		bool window = (pfd.dwFlags & PFD_DRAW_TO_WINDOW)>0;
		bool bitmap = (pfd.dwFlags & PFD_DRAW_TO_BITMAP)>0;
		bool dbuff  = (pfd.dwFlags & PFD_DOUBLEBUFFER)>0;
		bool stncl  = (pfd.cStencilBits > 0 );

		TString buf;
		switch (accelType)
		{
			case 1:
				buf = "mcd";
				break;
			case 2:
				buf = "icd";
				break;
			default:
				buf = "software";
				break;
		}
		
		//
		int q=0;
		if (opengl && window) 
			q=q+0x8000;
		if (wdepth==-1 || (wdepth>0 && depth>0)) 
			q=q+0x4000;
		if (wdbl==-1 || (wdbl==0 && !dbuff) || (wdbl==1 && dbuff)) 
			q=q+0x2000;
		if (wacc==-1 || (wacc==0 && soft) || (wacc==1 && (mcd || icd))) 
			q=q+0x1000;
		if (mcd || icd) 
			q=q+0x0040; 
		if (icd) 
			q=q+0x0002;
		if (wbpp==-1 || (wbpp==bpp)) 
			q=q+0x0800;
		if (bpp>=16) 
			q=q+0x0020; 
		if (bpp==16) 
			q=q+0x0008;
		if (wdepth==-1 || (wdepth==depth)) 
			q=q+0x0400;
		if (depth>=16) 
			q=q+0x0010; 
		if (depth==16) 
			q=q+0x0004;
		if (stncl)
			q=q+0x0100;
		if (!pal) 
			q=q+0x0080;
		if (bitmap) 
			q=q+0x0001;

		if (q>maxqual) 
		{
			maxqual=q; 
			maxindex=i;
			max_bpp=bpp; 
			max_depth=depth; 
			max_dbl=dbuff?1:0; 
			max_acc=soft?0:1;
		}
	}

	if (maxindex==0) 
		return maxindex;

	bpp=max_bpp;
	depth=max_depth;
	dbl=max_dbl;
	acc=max_acc;
	qFlag = maxqual;

	return maxindex;
}


LRESULT CALLBACK WndProc(   HWND    hWnd,
							UINT    message,
							WPARAM  wParam,
							LPARAM  lParam	)
{
	switch (message)
	{
		case WM_CREATE:
			{
				HDC hdc = ::GetWindowDC(hWnd);

				int bpp=16;
				int depth=16;
				int dbl=1; // we want double-buffering. (or -1 for 'don't care', or 0 for 'none')
				int acc=1; // we want acceleration. (or -1 or 0)
				int qFlag = 0; // quality bitmask
				int accelType = 0;
				int pf = ChoosePixelFormatEx(hdc,bpp,depth,dbl,acc,qFlag,accelType);
				if ( pf==0 )
				{
					return 0;
				}

				// set pixel format
				PIXELFORMATDESCRIPTOR pfd;
				ZeroMemory(&pfd,sizeof(pfd)); 
				pfd.nSize = sizeof(pfd); 
				pfd.nVersion=1;

				DescribePixelFormat(hdc,pf,sizeof(pfd),&pfd);

				if ( !SetPixelFormat(hdc,pf,&pfd ) )
				{
					return 0;
				}
				
				hglrc = ::wglCreateContext( hdc );
				if ( hglrc==NULL )
				{
					DWORD err = ::GetLastError();
					return false;
				}

				if ( !::wglMakeCurrent( hdc, hglrc ) )
				{
					return false;
				}

				::ShowWindow( hWnd, SW_HIDE );
				break;
			}
		default:   // Passes it on if unproccessed
		    return (DefWindowProc(hWnd, message, wParam, lParam));

	}
    return (0L);
}

bool DXConsole::SetupOpenGL( HINSTANCE hInst )
{
	//
	// Register the window class
	//
	WNDCLASSEX wc;
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC)WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "OpenGLApp";
	wc.hIconSm			= NULL;

	if(::RegisterClassEx(&wc) == 0)
    {
		return false;
    }

	long windowStyle = WS_CAPTION | WS_SYSMENU |
				  WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
				  WS_VISIBLE | WS_OVERLAPPED | 
				  WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	mainWindow = ::CreateWindow(
				"OpenGLApp",
				"OpenGL Window",
                windowStyle,
				0,0,
				100,100,
				NULL,
				NULL,
				hInst,
				NULL);
	::ShowWindow( mainWindow, SW_HIDE );
	return true;
}


void DoTransform( float* matrix,
				  float tx, float ty, float tz,
				  float rx, float ry, float rz,
				  float sx, float sy, float sz )
{
	glPushMatrix();
		glLoadIdentity();
		glTranslatef( tx, ty, tz );
		glRotatef( rz, 0,0,1 );
		glRotatef( ry, 0,1,0 );
		glRotatef( rx, 1,0,0 );
		glScalef( sx, sy, sz );
		glGetFloatv( GL_MODELVIEW_MATRIX, matrix );
	glPopMatrix();

	WriteString( "Transformation Matrix\n" );
	for ( size_t i=0; i<4; i++ )
	{
		WriteString( "%2.4f %2.4f %2.4f %2.4f\n",
					 matrix[i*4+0], matrix[i*4+1],
					 matrix[i*4+2], matrix[i*4+3] );
	}
};

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
    else if ( strncmp( line.c_str(), "createmesh ", 11 )==0 )
    {
		Lex lex;
		TString errStr;
		TString temp;
		temp = &line[11];

		TString fname, strip, seaStr;
		fname = temp.GetItem( ' ', 0 );
		strip = temp.GetItem( ' ', 1 );
		seaStr = temp.GetItem( ' ', 2 );
		if ( fname.length()==0 )
		{
			WriteString( "\nError - first parameter must be bitmap name\n" );
			return finish;
		}
		if ( strip.length()==0 )
		{
			WriteString( "\nError - needs second parameter, strip file .bmp\n" );
			return finish;
		}
		if ( seaStr.length()==0 )
		{
			WriteString( "\nError - needs third parameter, integer seaHeight\n" );
			return finish;
		}
		size_t seaHeight = atoi( seaStr.c_str() );
		if ( !lex.CreateMesh( fname, strip, seaHeight, errStr ) )
		{
			WriteString( errStr.c_str() );
		}
		return finish;
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
            //delete obj;
        }
        else
        	WriteString( "%s\n", lex.ErrorString() );
    }
    else if ( strncmp( line.c_str(), "rename ", 7 )==0 )
    {
		TString str = &line[7];
		TString fname = str.GetItem(' ',0);
		TString to = str.GetItem(' ',1);

		if ( fname.length()==0 || to.length()==0 )
		{
			WriteString( "rename takes two parameters, file to rename from and new filename\n" );
		}
		else
		{
			if ( rename( fname.c_str(), to.c_str() )==0 )
			{
				WriteString( "success\n" );
			}
			else
			{
				WriteString( "FAILED\n" );
			}
		}
	}
    else if ( strncmp( line.c_str(), "delete ", 7 )==0 )
    {
		TString str = &line[7];
		if ( remove( str.c_str() )==0 )
		{
			WriteString( "success\n" );
		}
		else
		{
			WriteString( "FAILED\n" );
		}
	}
    else if ( strncmp( line.c_str(), "copy ", 5 )==0 )
    {
		TString str = &line[5];
		TString fname = str.GetItem(' ',0);
		TString to = str.GetItem(' ',1);

		if ( fname.length()==0 || to.length()==0 )
		{
			WriteString( "copy takes two parameters, file to copy from and new filename\n" );
		}
		else
		{
			FILE* fh = fopen( fname.c_str(), "rb" );
			if ( fh==NULL )
			{
				WriteString( "Error opening file %s\n", fname.c_str() );
			}
			else
			{
				FILE* fto = fopen( to.c_str(), "wb" );
				if ( fto==NULL )
				{
					WriteString( "Error opening file %s for writing\n", to.c_str() );
				}
				else
				{
					size_t bufSize = 1024;
					size_t numRead = 0;
					char buf[1024];
					do
					{
						numRead = fread( buf, 1, bufSize, fh );
						if ( numRead>0 )
							fwrite(buf,1,numRead,fto);
					}
					while (numRead==bufSize);
					fclose(fto);
					WriteString( "success\n" );
				}
				fclose(fh);
			}
		}
	}
    else if ( strncmp( line.c_str(), "convert2 ", 9 )==0 )
    {
        WriteString( "converting file \"%s\"\n", &line[9] );
    	Lex lex;
        TBinObject* obj = lex.DirectX( &line[9] );
        if ( obj!=NULL )
        {
        	TString newFname = &line[9];
			newFname = newFname.GetItem('.',0) + ".bin";
			// strip path from newFname
			size_t i = newFname.length();
			while ( i>0 && newFname[i]!='\\' ) i--;
			if ( newFname[i]=='\\' )
				newFname = newFname.substr(i+1);

			TString prefixpath = &line[9];
			prefixpath = prefixpath.GetItem(' ',2);
			if ( prefixpath.length() > 0 )
			{
				if ( prefixpath[prefixpath.length()]!='\\' )
					prefixpath = prefixpath + "\\";
				newFname = prefixpath + newFname;
			};
        	WriteString( "saving as \"%s\"\n", newFname.c_str() );

        	if ( !obj->SaveBinary( newFname ) )
				WriteString( "error writing file %s\n", newFname.c_str() );
            delete obj;
        }
        else
        	WriteString( "%s\n", lex.ErrorString() );
    }
	else if ( strncmp( line.c_str(), "parsegltext ", 12 )==0 )
	{
		TString temp = &line[12];

		TString inname = temp.GetItem(' ',0);
		TString reffile = temp.GetItem(' ',1);
		if ( inname.length()==0 || reffile.length()==0 )
		{
        	WriteString( "Error: takes 2 parameters, infile.txt, reference file.bin\n" );
		}
		else
		{
			TString errStr;
			TBinObject obj;
			TBinObject refObj;
			if ( !refObj.LoadBinary( reffile, errStr, "data\\binfiles", "data\\textures" ) )
			{
				WriteString( "%s\n", errStr.c_str() );
			}
			else if ( !ImportOpenGL( inname, obj, &refObj, errStr ) )
			{
				WriteString( "%s\n", errStr.c_str() );
			}
			else
			{
				TString newfname = inname.GetItem('.',0) + ".x";
				if ( !obj.SaveText( newfname, errStr ) )
				{
					WriteString( "%s\n", errStr.c_str() );
				}
			}
		}
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
		WriteString( "Creating Compound object %s\n", objName.c_str() );
		TCompoundObject obj;
		obj.Create( objName, count-2, texturePath, (TString*)names );
	}
    else if ( strncmp( line.c_str(), "createnobj ", 11 )==0 )
    {
		TString temp;
		TString objName;
		TString texturePath;

		TString names[256];
		size_t j;
		for ( j=0; j<256; j++ )
			names[j] = "";

		temp = &line[11];
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
		WriteString( "Creating Normalised Compound object %s\n", objName.c_str() );
		TCompoundObject obj;
		obj.Create( objName, count-2, texturePath, (TString*)names );

		TCompoundObject obj2;

		// strip path
		TString fname, path, errStr;
		size_t index = objName.length()-1;
		while ( index>0 && objName[index]!='\\' ) index--;
		if ( index>0 )
		{
			path = objName.substr(0,index);
			fname = objName.substr(index+1);
		}
		else
		{
			fname = objName;
		}
		if ( !obj2.LoadBinary( fname, errStr, path, texturePath ) )
		{
			WriteString( "Error: %s\n", errStr.c_str() );
		}
		obj2.Normalise();
		obj2.SaveBinary( objName, texturePath );
	}
	else if ( strncmp( line.c_str(), "transformbin ", 13 )==0 )
	{
		TString fileName;
		TString temp = &line[13];
		TString path;
		TString fname = temp.GetItem(' ',0);
		SplitPath( fname, path, fileName );
		TString tpath = temp.GetItem(' ',1);

		TString trn = temp.GetItem(' ',2);
		TString rot = temp.GetItem(' ',3);
		TString scl = temp.GetItem(' ',4);

		trn = trn.GetItem('=',1);
		rot = rot.GetItem('=',1);
		scl = scl.GetItem('=',1);

		if ( fileName.length()==0 || tpath.length()==0 ||
			 trn.length()==0 || rot.length()==0 || scl.length()==0 )
		{
			WriteString( "\nError: incorrect number of parameters\nType help from more information\n" );
			return finish;
		}

		TBinObject obj;
		TString errStr;
		if ( !obj.LoadBinary( fileName, errStr, path, tpath ) )
		{
			WriteString( "\nError: %s\n", errStr.c_str() );
		}

		float tx,ty,tz;
		float rx,ry,rz;
		float sx,sy,sz;
		float matrix[16];

		tx = float(atof(trn.GetItem(',',0).c_str()));
		ty = float(atof(trn.GetItem(',',1).c_str()));
		tz = float(atof(trn.GetItem(',',2).c_str()));

		rx = float(atof(rot.GetItem(',',0).c_str()));
		ry = float(atof(rot.GetItem(',',1).c_str()));
		rz = float(atof(rot.GetItem(',',2).c_str()));

		sx = float(atof(scl.GetItem(',',0).c_str()));
		sy = float(atof(scl.GetItem(',',1).c_str()));
		sz = float(atof(scl.GetItem(',',2).c_str()));

		DoTransform( matrix, tx,ty,tz, rx,ry,rz, sx,sy,sz );
		obj.MatrixMult( matrix );
		TString newFname = "t" + fileName;
		ConstructPath( newFname, path, newFname );
		if ( obj.SaveBinary( newFname ) )
		{
			WriteString( "done\n" );
		}
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
	else if ( strncmp( line.c_str(), "matrix ", 7 )==0 )
	{
		TString temp = &line[7];
		TString trn = temp.GetItem(' ',0);
		TString rot = temp.GetItem(' ',1);
		TString scl = temp.GetItem(' ',2);
		if ( trn.length()==0 ||
			 rot.length()==0 ||
			 scl.length()==0 )
		{
			WriteString( "Error: takes 3 parameters\n" );
			return finish;
		}
		trn = trn.GetItem('=',1);
		rot = rot.GetItem('=',1);
		scl = scl.GetItem('=',1);

		float tx,ty,tz;
		float rx,ry,rz;
		float sx,sy,sz;
		float matrix[16];

		tx = float(atof(trn.GetItem(',',0).c_str()));
		ty = float(atof(trn.GetItem(',',1).c_str()));
		tz = float(atof(trn.GetItem(',',2).c_str()));

		rx = float(atof(rot.GetItem(',',0).c_str()));
		ry = float(atof(rot.GetItem(',',1).c_str()));
		rz = float(atof(rot.GetItem(',',2).c_str()));

		sx = float(atof(scl.GetItem(',',0).c_str()));
		sy = float(atof(scl.GetItem(',',1).c_str()));
		sz = float(atof(scl.GetItem(',',2).c_str()));

		DoTransform( matrix, tx,ty,tz, rx,ry,rz, sx,sy,sz );
	}
	else if ( strncmp( line.c_str(), "mdltobin ", 9 )==0 )
	{
		TString temp = &line[9];
		TString fname = temp.GetItem(' ',0);
		TString charName = temp.GetItem('\"',1);

		if ( fname.length()==0 || charName.length()==0 )
		{
			WriteString( "Error: incorrect number of parameters\n" );
			return finish;
		}

		// load mdl into memory
		TPersist file(fileRead);
		if ( !file.FileOpen( fname ) )
		{
			WriteString( "Error: can't open file \"%s\"\n", fname.c_str() );
			return finish;
		}
		size_t fs = file.FileSize() - 4;
		char id[4];
		file.FileRead( id,4 );
		if ( strncmp(id,"IDST",4)!=0 &&
			 strncmp(id,"IDSQ",4)!=0 )
		{
			file.FileClose();
			WriteString( "Error: \"%s\" is not an MDL file\n", fname.c_str() );
			return finish;
		}
		char* buf = new char[fs];
		PostCond( buf!=NULL );
		file.FileRead( buf, fs );
		file.FileClose();

		// write new file
		TPersist outfile(fileWrite);
		TString outfname = "ch_" + fname.GetItem('.',0) + ".bin";
		if ( !outfile.FileOpen( outfname ) ) 
		{
			delete buf;
			WriteString( "Error: \"%s\" can't open for write\n", outfname.c_str() );
			return finish;
		}
		// new header
		TString header = "PDVCHAR01";
		outfile.FileWrite( header.c_str(), header.length()+1 );
		// character name
		outfile.FileWrite( charName.c_str(), charName.length()+1 );
		// old mdl
		outfile.FileWrite( buf, fs );
		outfile.FileClose();
		WriteString( "Character converted to \"%s\"\n", outfname.c_str() );
	}
    return finish;
};


bool DXConsole::CreateBinMesh( const TString& fname, const TString& strip,
							   const TString& seaStr, const TString& material,
							   TString& errStr )
{
	size_t seaHeight = atoi( seaStr.c_str() );
	TBinObject* bin;
	Lex lex;
	if ( (bin=TBinObject::CreateBinaryMesh( fname, strip, material, seaHeight, errStr ))==NULL )
	{
		return false;
	}
	else
	{
		TPersist file(fileWrite);
		TString newfname = fname;
		newfname = newfname.GetItem('.',0) + ".bin";
		if ( !file.FileOpen( newfname ) )
		{
			errStr = "\nError: could not open file \"" + newfname + "\" for output\n";
			return false;
		}
		else
		{
			if ( bin->SaveBinary( file ) )
				WriteString( "Created binfile \"%s\"\n", newfname );
			else
			{
				errStr = "\nError writing to file \"" + newfname + "\"\n";
				return false;
			}
			file.FileClose();
		}
	}
	return true;
}


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
    WriteString( "parsegltext import.txt reffile.bin           -- create a bin file from\n");
	WriteString( "                                             -- a C++ text file for RR\n");
    WriteString( "createobj objName texturepath file1.bin ...  -- merge .bin objects into one to\n");
	WriteString( "          fileX.bin                          -- form a compound .obj\n");
    WriteString( "createnobj objName texturepath file1.bin ... -- merge .bin objects into one to\n");
	WriteString( "          fileX.bin                          -- form a normalised compound .obj\n");
    WriteString( "convert filename.x texturepath [-merge]      -- load directX text file\n");
    WriteString( "        [-centerx] [-centery] [-centerz]     -- opt: merge all meshes in\n");
    WriteString( "        [-normalise] [-createnormals]        -- file into a single mesh\n");
	WriteString( "        [-revw] [-minx0] [-miny0]            -- & convert it to .bin\n");
	WriteString( "        [-minz0] [-maxx0] [-maxy0] [-maxz0]  -- force minx=0, etc.\n");
	WriteString( "        [-bounds(x1,y1,z1,x2,y2,z2)]         -- obj scale to bounding box\n");
	WriteString( "        [-flipuv] [-scale=#]                 -- flip UVs for truespace\n");
    WriteString( "convert2 filename.x texturepath destPath ... -- same as convert, added destPath\n");
    WriteString( "createanim filename.anm anim%%%%d.bin           -- Create animation meshes\n");
	WriteString( "           texturePath numAnims              -- from a series of .bin\n");
	WriteString( "                                             -- (zero indexed)\n");
	WriteString( "                                             -- files\n");
    WriteString( "show filename[.bin|.lnd|.anm|.obj]           -- load .bin, .lnd, .obj, or .anm\n");
	WriteString( "                                             -- file & view it graphically\n");
	WriteString( "createmesh land24.bmp strip.bmp seaHeight    -- create a mesh from a b&w\n" );
	WriteString( "                                             -- 24 bit 2^ sized heightmap\n" );
	WriteString( "transformbin fname.bin texturePath t=x,y,z   -- matrix transform the\n" );
	WriteString( "             r=x,y,z s=x,y,z                 -- vertices of a bin object\n" );
	WriteString( "matrix t=x,y,z r=x,y,z s=x,y,z               -- get matrix for given\n" );
	WriteString( "                                             -- Translation, Rotation, Scale\n" );
	WriteString( "mdltobin fname.mdl \"Character name\"          -- Convert an MDL to my BIN\n" );
	WriteString( "rename file1 file2                           -- rename file1 to file2\n" );
	WriteString( "delete fileName                              -- delete file\n" );
	WriteString( "copy file1 file2                             -- copy file1 to file2\n" );
    WriteString( "======================================================================\n");
    WriteString( "\n");
};

//==========================================================================

void main( int argc, char* argv[] )
{
	DXConsole* console = new DXConsole();
	console->SetupOpenGL(HINSTANCE(::GetCurrentProcess()));

	// get cwd
	char cwd[1024];
	_getcwd( cwd, 1024 );
	_dir = cwd;

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


