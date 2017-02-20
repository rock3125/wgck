#include <precomp_header.h>

#include <Commdlg.h>
#include <shlobj.h>
#include <direct.h>
#include <Winbase.h>
#include <shellapi.h>

#include <io.h>

#include <object/landscape.h>
#include <object/createlandscape.h>

#include <dialogs/dialogassist.h>
#include <win32/resource.h>
#include <landscape/main.h>

//==========================================================================
//
// Entry point of all Windows programs
//
#ifndef _MESA

int APIENTRY WinMain( HINSTANCE hInst,
			   		  HINSTANCE hPrevInstance,
			   		  LPSTR lpCmdLine,
			   		  int nCmdShow )
{
	PreInit(hInst);

	// create app
	TApp* app = new TApp( hInst, RegisteredMessage() );
    PostCond( app!=NULL );
	SetApp( app );

	if ( !PostInit() )
	{
		return -1;
	}
	return StartGameSystem();
}

#endif

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.295779511273f;

//==========================================================================
// used by AIs - overall logic counter
size_t logicCounter = 0;
size_t aiId = 1;

//==========================================================================

TApp::TApp( HINSTANCE inst, const TString& _registeredMessage )
	: TEvent( inst, false ),
	  editor(NULL),
	  obj(NULL),
	  heightEdit(NULL),
	  objectEditor(NULL),
	  botEditor(NULL)
{
	// default settings
	registeredMessage = _registeredMessage;
	def.IslandName( "Island Hopper" );
	def.BMPName( "land1.bmp" );
	def.BMPStrip( "strip.bmp" );
	def.Material( "bump1.jpg" );
	def.WaterLevel( 10 );

	numObjects = 0;
	currentEdit = 0;
	objectyangle = 0;

	yAngle = 0.0f;
	xAngle = 0.0f;
	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;
    lx = 0;
	ly = 50;
    lz = 0;
	leftDown = false;
	showBoundingBox = false;
	keys = 0;
	currentLocation = NULL;
	currentObject = 0;

	initialised = false;
	mapLoaded = false;
	currentObjectInUse = false;
	showBotMarkers = true;

	storyPath = "data\\stories";

	scale = 1;

	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;

	rotX = 0;
	rotY = 0;
	rotZ = 0;

	for ( size_t i=0; i<kMaxObjects; i++ )
		objects[i] = NULL;

	cam.Type(1);
};


TApp::~TApp( void )
{
	if ( editor!=NULL )
		delete editor;
	editor = NULL;

	if ( botEditor!=NULL )
		delete botEditor;
	botEditor = NULL;

	if ( heightEdit!=NULL )
		delete heightEdit;
	heightEdit = NULL;

	if ( objectEditor!=NULL )
		delete objectEditor;
	objectEditor = NULL;

	for ( size_t i=0; i<kMaxObjects; i++ )
	{
		if ( objects[i]!=NULL )
			delete objects[i];
		objects[i] = NULL;
	}
};


void TApp::GetObjects( void )
{
	numObjects = 0;

	::SetProgressMessage( "Loading Objects" );
	::SetProgress(0);

	// do a quick file-count first
	_finddata_t fd;
	long fh = _findfirst( "data\\*.obj", &fd );
	float objCount = 0;
	if ( fh!=-1 )
	{
		objCount = objCount + 1;
		int fnd;
		do
		{
			fnd = _findnext( fh, &fd );
			if ( fnd==0 )
			{
				objCount = objCount + 1;
			}
		}
		while (fnd==0 && objCount<kMaxObjects);
		_findclose(fh);
	}

	// there must be at least one object!
	PostCond( objCount>0 );

	// load them
	fh = _findfirst( "data\\*.obj", &fd );
	if ( fh!=-1 )
	{
		TString errStr;

		objectNames[numObjects] = fd.name;
		if ( objects[numObjects]!=NULL )
			delete objects[numObjects];
		objects[numObjects] = new TCompoundObject();
		if ( objects[numObjects]->LoadBinary( fd.name, errStr, "data", "data\\textures" ) )
		{
//			objects[numObjects]->CenterAxes(true,true,true);
//			objects[numObjects]->Normalise();
			numObjects++;
		}
		int fnd;
		do
		{
			fnd = _findnext( fh, &fd );
			if ( fnd==0 )
			{
				objectNames[numObjects] = fd.name;
				if ( objects[numObjects]!=NULL )
					delete objects[numObjects];
				objects[numObjects] = new TCompoundObject();
				if ( objects[numObjects]->LoadBinary( fd.name, errStr, "data", "data\\textures" ) )
				{
//					objects[numObjects]->CenterAxes(true,true,true);
//					objects[numObjects]->Normalise();
					numObjects++;
				}
			}

			::SetProgress(  size_t((float(numObjects)/objCount) * 100.0f) );

		}
		while (fnd==0 && numObjects<kMaxObjects);
		_findclose(fh);
	}
};


size_t TApp::NumObjects( void ) const
{
	return numObjects;
};


TString TApp::GetObjectName( size_t index ) const
{
	PreCond( index<numObjects );
	return objectNames[index];
};


float TApp::GetLandscapeY( float x, float z )
{
	float y = 0;
	if ( x==0 )
		x = 0.01f;
	if ( z==0 )
		z = 0.01f;
	obj->GetLORTriangle( x,500,z,-1000,y);
	for ( size_t i=0; i<def.NumLandscapeObjects(); i++ )
	{
		float y1;
		TLandscapeObject& o = def.LandscapeObjects(i);
		if ( o.GetLORTriangle( x,500,z,-1000,y1) )
		{
			if ( y1 > y )
				y = y1;
		}
	}
	return y;
};


float TApp::GetLandscapeOnlyY( float x, float z )
{
	float y = 0;
	if ( x==0 )
		x = 0.01f;
	if ( z==0 )
		z = 0.01f;
	obj->GetLORTriangle( x,500,z,-1000,y);
	return y;
};


void TApp::SetCmdLine( const char* str )
{
	TEvent::SetCmdLine( str );

	if ( commandLine.NumItems( ' ' ) > 1 )
	{
		fileName = commandLine.GetItem( ' ', 0 );
		size_t len = fileName.length();
		while ( len>0 && fileName[len]!='\\' ) len--;
		if ( len>0 )
		{
			path = fileName.substr(0,len);
			fileName = fileName.substr(len+1);
		}
		else
			path = "";
		texturePath = commandLine.GetItem( ' ', 1 );
	}
	else
	{
		path = "data";
		fileName = "land1.def";
		texturePath = "data\\textures";
	}
};


bool TApp::Initialised( void )
{
	return initialised;
};


void TApp::Initialised( bool i )
{
	initialised = i;
};


bool TApp::StartupSystem( TString& errStr )
{
	if ( !TEvent::StartupSystem( errStr ) )
		return false;

	// must be read before landscape is loaded
	GetObjects();

	if ( heightEdit==NULL )
	{
		heightEdit = new HeightEditor( *this, hInstance, hWindow );
		heightEdit->SetPos( 0,480 );
		ShowWindow( heightEdit->Window(), SW_HIDE );
	}
	if ( botEditor==NULL )
	{
		botEditor = new BotEditor( *this, hInstance, hWindow );
		botEditor->SetPos( 640,360 );
		ShowWindow( botEditor->Window(), SW_HIDE );
	}
	if ( objectEditor==NULL )
	{
		objectEditor = new ObjectEditor( *this, hInstance, hWindow, vehicleList );
		objectEditor->SetPos( 280,480 );
		ShowWindow( objectEditor->Window(), SW_HIDE );
	}
	if ( editor==NULL )
	{
		editor = new MapEditor( *this, hInstance, hWindow, objectEditor, heightEdit, 
								vehicleList );
		editor->SetPos( 640,0 );
		ShowWindow( editor->Window(), SW_HIDE );
	}

	::BringWindowToTop( Window() );
	initialised = true;

    return true;
};


bool TApp::LoadObject( const TString& fname, TString& errStr )
{
	ConstructPath(saveFname,storyPath,fname);

	if ( !def.LoadBinary( fname, storyPath, errStr, false ) )
	{
		return false;
	}
	if ( !def.LoadObjects( numObjects, objectNames, objects, errStr ) )
	{
		return false;
	}

	obj = ::CreateBinaryMesh( def.BMPName(), def.BMPStrip(), 
							def.Material(), int(def.WaterLevel()), 
							errStr );
	if ( obj==NULL )
		return false;

	TString name = appTitle + " [" + def.IslandName() + "]";
	::SetWindowText( hWindow, name.c_str() );

	editor->SetMapInfo( def );
	botEditor->SetMapInfo( def );
	heightEdit->SetMapInfo( def );
	objectEditor->SelectionChanged( 0, true );

	mapLoaded = true;

	ShowWindow( heightEdit->Window(), SW_SHOW );
	ShowWindow( botEditor->Window(), SW_SHOW );
	ShowWindow( objectEditor->Window(), SW_SHOW );
	ShowWindow( editor->Window(), SW_SHOW );

	return true;
};


bool TApp::LoadObject( const TString& fname, TDefFile& def, TString& errStr )
{
	ConstructPath(saveFname,storyPath,fname);

	if ( !def.LoadObjects( numObjects, objectNames, objects, errStr ) )
	{
		return false;
	}

	obj = ::CreateBinaryMesh( def.BMPName(), def.BMPStrip(), 
							  def.Material(), int(def.WaterLevel()), errStr );
	if ( obj==NULL )
		return false;

	TString name = appTitle + " [" + def.IslandName() + "]";
	::SetWindowText( hWindow, name.c_str() );

//	editor->SetMapInfo( def );
	botEditor->SetMapInfo( def );
	heightEdit->SetMapInfo( def );
	objectEditor->SelectionChanged( 0, true );

	mapLoaded = true;

	ShowWindow( heightEdit->Window(), SW_SHOW );
	ShowWindow( botEditor->Window(), SW_SHOW );
	ShowWindow( objectEditor->Window(), SW_SHOW );
	ShowWindow( editor->Window(), SW_SHOW );

	return true;
};


bool TApp::NewIsland( const TString& _path, const TString& _texturePath, 
					  bool destroy, TString& errStr )
{
	path = _path;
	texturePath = _texturePath;
	saveFname = "";

	// clear old map
	if ( obj!=NULL )
		delete obj;

	obj = ::CreateBinaryMesh( def.BMPName(), def.BMPStrip(), 
							  def.Material(), int(def.WaterLevel()), 
							  errStr );
	if ( obj==NULL )
		return false;

	if ( destroy )
	{
		def.NumLandscapeObjects(0);
		def.NumLocations(0);
		if ( heightEdit!=NULL )
		{
			heightEdit->SetMapInfo( def );
		}
		if ( botEditor!=NULL )
		{
			botEditor->SetMapInfo( def );
		}
	}

	TString name = appTitle + " [" + def.IslandName() + "]";
	::SetWindowText( hWindow, name.c_str() );
	objectEditor->SelectionChanged( 0, true );

	mapLoaded = true;

	ShowWindow( heightEdit->Window(), SW_SHOW );
	ShowWindow( botEditor->Window(), SW_SHOW );
	ShowWindow( objectEditor->Window(), SW_SHOW );
	ShowWindow( editor->Window(), SW_SHOW );

	return true;
};


void TApp::SaveDefn( void )
{
	if ( def.NumLandscapeObjects()==0 )
	{
		TString msg = "Warning: this map has no landscape objects.\n\n";
		msg = msg + "Consequently - this map will not work in a game\n";
		::MessageBox( hWindow, msg.c_str(), "Warning", MB_ICONWARNING | MB_OK );
	}
	TString errStr;
	if ( !def.CheckMap(errStr) )
	{
		::MessageBox( hWindow, errStr.c_str(), "Warning", MB_ICONWARNING | MB_OK );
	}
	if ( saveFname.length()>0 )
	{
		def.SaveBinary( saveFname );
		editor->Modified( false );
		botEditor->Changed( false );
	}
	else
	{
		SaveDefnAs();
	}
};


void TApp::SaveDefnAs( void )
{
	{
		OPENFILENAME ofn;
		memset( &ofn,0,sizeof(OPENFILENAME) );
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hWindow;
		ofn.lpstrFilter = "Map definition files\0*.def\0All files\0*.*\0\0";
		char temp[256];
		strcpy(temp,saveFname.c_str());
		ofn.lpstrFile = temp;
		ofn.lpstrTitle = "Save map file";
		ofn.nMaxFileTitle = saveFname.length();
		ofn.nMaxFile = 256;
		ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
		if ( GetSaveFileName( &ofn )==TRUE )
		{
			saveFname = ofn.lpstrFile;
			SaveDefn();
		};
	}
};


bool TApp::LoadBinAs( TString& binName )
{
	OPENFILENAME ofn;
	memset( &ofn,0,sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWindow;
	ofn.lpstrFilter = "BMP file\0*.bmp\0\0";
	char temp[256];
	temp[0] = 0;
	ofn.lpstrFile = temp;
	ofn.lpstrTitle = "Load BMP File";
	ofn.nMaxFileTitle = 256;
	ofn.nMaxFile = 256;
	ofn.Flags = OFN_EXPLORER;
	if ( GetOpenFileName( &ofn )==TRUE )
	{
		binName = ofn.lpstrFile;
		return true;
	};
	return false;
};


void TApp::Logic( void )
{
	TEvent::Logic();

	if ( currentObjectInUse )
	{
		float ox = currObj.TX();
		float oy = currObj.TY();
		float oz = currObj.TZ();

		yAngle += deltaYAngle;
		if ( yAngle>360.0f ) yAngle -= 360.0f;
		if ( yAngle<0.0f ) yAngle += 360.0f;

		float yangle = yAngle*degToRad;
	
		float v1fcos = (float)cos(yangle);
		float v1fsin = (float)sin(yangle);
	
		float v2fcos = (float)cos(yangle + 0.5f*kPI );
		float v2fsin = (float)sin(yangle + 0.5f*kPI );

		float v3fcos = (float)cos(yangle - 0.5f*kPI );
		float v3fsin = (float)sin(yangle - 0.5f*kPI );

		// key codes
		float speed = 0.5f;

		if ( keys & aUp )
		{
			oz = oz + v1fcos*speed;
			ox = ox - v1fsin*speed;
		}
		if ( keys & aDown )
		{
			oz = oz - v1fcos*speed;
			ox = ox + v1fsin*speed;
		}
		if ( keys & aLeft )
		{
			oz = oz - v2fcos*speed;
			ox = ox + v2fsin*speed;
		}
		if ( keys & aRight )
		{
			oz = oz - v3fcos*speed;
			ox = ox + v3fsin*speed;
		}
		if ( keys & 16 )
			oy = oy + speed*0.25f;
		if ( keys & 32 )
			oy = oy - speed*0.25f;

		if ( keys!=0 && editor!=NULL )
		{
			objectEditor->SetTranslation(ox,oy,oz);
		}

		currObj.SetTranslation(ox,oy,oz);
		currObj.SetScale(scaleX,scaleY,scaleZ);
		currObj.SetRotation(rotX,rotY,rotZ);

		if ( currentEdit<def.NumLandscapeObjects() )
			def.LandscapeObjects(currentEdit,currObj);

		cam.SetCamera( ox,oy,oz, yAngle, deltaXAngle, deltaYAngle, scale );
	}
	else
	{
		xAngle += deltaXAngle;
		yAngle += deltaYAngle;
		if ( yAngle>360.0f ) yAngle -= 360.0f;
		if ( yAngle<0.0f ) yAngle += 360.0f;

		float yangle = yAngle*degToRad;
	
		float v1fcos = (float)cos(yangle);
		float v1fsin = (float)sin(yangle);
	
		float v2fcos = (float)cos(yangle + 0.5f*kPI );
		float v2fsin = (float)sin(yangle + 0.5f*kPI );

		float v3fcos = (float)cos(yangle - 0.5f*kPI );
		float v3fsin = (float)sin(yangle - 0.5f*kPI );

		// key codes
		float speed = 1;
		if (ctrlDown)
			speed = 10;

		if ( keys & aUp )
		{
			lz = lz + v1fcos*speed;
			lx = lx - v1fsin*speed;
		}
		if ( keys & aDown )
		{
			lz = lz - v1fcos*speed;
			lx = lx + v1fsin*speed;
		}
		if ( keys & aLeft )
		{
			lz = lz - v2fcos*speed;
			lx = lx + v2fsin*speed;
		}
		if ( keys & aRight )
		{
			lz = lz - v3fcos*speed;
			lx = lx + v3fsin*speed;
		}
		if ( keys & 16 )
			ly = ly + speed*0.25f;
		if ( keys & 32 )
			ly = ly - speed*0.25f;

		cam.SetCamera( lx,ly,lz, yAngle, deltaXAngle, deltaYAngle, scale );
	}

	objectyangle += 5;

	deltaXAngle = 0.0f;
	deltaYAngle = 0.0f;
};

//==========================================================================

void TApp::Render( void )
{
	if ( mapLoaded )
	{
		if ( heightEdit!=NULL )
			heightEdit->DoPaint();

		SetupGL2d( vdepth, vwidth, vheight );
		glLoadIdentity();

		glEnable( GL_COLOR_MATERIAL );
		glColor4ub(255,255,255,255);

		if ( currentObject < numObjects )
		{
			float scale = 30;

			glPushMatrix();
			glTranslatef(scale*2,vheight-scale*2,-(scale*2));
			glRotatef( objectyangle, 0,1,0 );
			glScalef( scale,scale,scale );
			objects[currentObject]->Draw(false);
			glPopMatrix();
		}

		SetupGL3d( vdepth, vwidth, vheight );
		glLoadIdentity();

		glRotatef( cam.XAngle(), 1,0,0 );
		glRotatef( cam.YAngle(), 0,1,0 );
		glTranslatef( cam.CameraX(),
					  cam.CameraY(),
					  cam.CameraZ() );

		// draw landscape
		glScalef( scale, scale, scale );
		if ( obj!=NULL )
			obj->Draw();

/*
		float x1,z1,x2,z2;
		heightEdit->GetBoundingRect( x1,z1,x2,z2 );
		glLineWidth(4);
		glEnable( GL_COLOR_MATERIAL );
		glBegin( GL_LINES );
			glColor3ub(0,0,0);
			glVertex3f( x1,-500,z1);
			glVertex3f( x1, 500,z1);
			glColor3ub(50,50,0);
			glVertex3f( x1,-500,z2);
			glVertex3f( x1, 500,z2);
			glColor3ub(50,50,50);
			glVertex3f( x2,-500,z2);
			glVertex3f( x2, 500,z2);
			glColor3ub(50,0,0);
			glVertex3f( x2,-500,z1);
			glVertex3f( x2, 500,z1);
		glEnd();
		glLineWidth(1);
*/
		// show bounding box around selected object
		if ( currentObjectInUse )
		{
			currObj.DrawBoundingBox(255,255,255);
		}
		else
		{
			// show editor location
			glPushMatrix();

			glTranslatef( lx,ly,lz );
			glColor3ub( 255,255,255 );
			glBegin( GL_LINES );
				glVertex3f( -10,0,0 );
				glVertex3f( 10,0,0 );
				glVertex3f( 0,-10,0 );
				glVertex3f( 0,10,0 );
				glVertex3f( 0,0,-10 );
				glVertex3f( 0,0,10 );
			glEnd();

			glPopMatrix();
		}

		// draw all objects
		for ( size_t i=0; i<def.NumLandscapeObjects(); i++ )
		{
			def.LandscapeObjects(i).Draw();
		}

		// draw bot markers
		if ( showBotMarkers )
		{
			glEnable( GL_COLOR_MATERIAL );
			glLineWidth( 3 );
			size_t count = 0;
			size_t selectedAI = 0;
			if ( botEditor!=NULL )
			{
				selectedAI = botEditor->SelectedAI();;
				switch ( selectedAI )
				{
				case 0:
				{
					count = def.NumLocations();
					break;
				}
				case 1:
				{
					count = def.NumPatrolLocations();
					break;
				}
				case 2:
				{
					count = def.NumSequenceLocations();
					break;
				}
				}
			}

			for ( size_t i=0; i<count; i++ )
			{
				float x,y,z;
				TConnections c;

				switch ( selectedAI )
				{
				case 0:
				{
					c = def.Connection(i);
					break;
				}
				case 1:
				{
					c = def.PatrolConnection(i);
					break;
				}
				case 2:
				{
					c = def.SequenceConnection(i);
					break;
				}
				}

				glBegin( GL_LINES );

				x = c.x;
				y = c.y;
				z = c.z;
				if ( selectedAI==2 )
				{
					glColor3ub( 100,100,0 );
				}
				else
				{
					glColor3ub( 0,0,0 );
				}
				glVertex3f( x,y-5,z );
				glVertex3f( x,y+5,z );

				glEnd();

				for ( size_t j=0; j<c.numConnections; j++ )
				{
					float x1,y1,z1;
					size_t index = c.neighbours[j];

					switch ( selectedAI )
					{
					case 0:
					{
						def.GetAILocation( index, x1,y1,z1 );
						glColor3ub( 255,255,255 );
						break;
					}
					case 1:
					{
						def.GetAIPatrolLocation( index, x1,y1,z1 );
						glColor3ub( 255,100,100 );
						break;
					}
					case 2:
					{
						def.GetSequenceLocation( index, x1,y1,z1 );
						glColor3ub( 255,255,100 );
						break;
					}
					}
					glBegin( GL_LINES );
					glVertex3f( x,y+1.5f,z );
					glVertex3f( x1, y1+1, z1 );
					glEnd();
				}
			}
			glLineWidth( 1 );
		}
	}
	else
	{
		if ( heightEdit!=NULL )
			heightEdit->DoPaint();
	}
};

//==========================================================================

void TApp::MouseDown( int button, int dx, int dy)
{
	TEvent::MouseDown(button,dx,dy);
};

void TApp::MouseDown( HWND hwnd, int button, int x, int y, int dx, int dy )
{
	if ( heightEdit!=NULL )
	{
		if ( heightEdit->Window()==hwnd )
			heightEdit->MouseDown( button,x,y,dx,dy );
	}

	if ( hwnd==Window() )
	{
		if ( button==1 )
		{
	    	leftDown = true;
			startX = dx;
			startY = dy;
		}

		if ( button==2 )
		{
			// get object clicked on
			bool found = false;
			for ( size_t i=0; i<def.NumLandscapeObjects() && !found; i++ )
			{
				if ( ClickedObject( def.LandscapeObjects(i), x, y ) )
				{
					found = true;

					if ( currentObjectInUse )
						def.LandscapeObjects(currentEdit, currObj);

					currentEdit = i;
					currentObjectInUse = true;
					currObj = def.LandscapeObjects(currentEdit);

					currObj.GetScale( scaleX,scaleY,scaleZ );
					currObj.GetRotation( rotX,rotY,rotZ );
					currObj.GetTranslation( lx,ly,lz );

					objectEditor->SetScale( scaleX,scaleY,scaleZ );
					objectEditor->SetTranslation( lx,ly,lz );
					objectEditor->SetRotation( rotX,rotY,rotZ );

					editor->SetCurrentEdit(currentEdit);
				}
			}
		}
	}
};

void TApp::MouseDown( int button, int x, int y, int dx, int dy )
{
	TEvent::MouseDown(button,x,y,dx,dy);
};


void TApp::MouseUp( int button, int dx, int dy )
{
	TEvent::MouseUp(button,dx,dy);
};


void TApp::MouseUp( HWND hwnd, int button, int x, int y, int dx, int dy )
{
	if ( heightEdit!=NULL )
	{
		if ( heightEdit->Window()==hwnd )
			heightEdit->MouseUp( button,x,y,dx,dy );
	}

	if ( hwnd==Window() )
	{
		if ( button==1 )
			leftDown = false;
	}
};

void TApp::MouseUp( int button, int x, int y, int dx, int dy )
{
	TEvent::MouseUp(button,x,y,dx,dy);
};


void TApp::MouseMove( int dx, int dy )
{
	TEvent::MouseMove(dx,dy);
};


void TApp::MouseMove( HWND hwnd, int x, int y, int dx, int dy )
{
	if ( heightEdit!=NULL )
	{
		if ( heightEdit->Window()==hwnd )
			heightEdit->MouseMove( x,y,dx,dy );
	}
	if ( hwnd==Window() )
	{
		if ( leftDown )
		{
			deltaYAngle = float(dx);
			deltaXAngle = float(dy);
		}
	}
};


void TApp::MouseMove( int x, int y, int dx, int dy )
{
	TEvent::MouseMove(x,y,dx,dy);
};


void TApp::KeyDown( size_t kd )
{
	TEvent::KeyDown( kd );
	switch ( kd )
	{
		case VK_LEFT:
			keys = keys | aLeft;
			break;

		case VK_RIGHT:
			keys = keys | aRight;
			break;

		case VK_UP:
			keys = keys | aUp;
			break;

		case VK_DOWN:
			keys = keys | aDown;
			break;

		case 'a':
		case 'A':
			keys = keys | 16;
			break;

		case 'z':
		case 'Z':
			keys = keys | 32;
			break;

		case 'b':
		case 'B':
			showBoundingBox = !showBoundingBox;
			break;
	}
};


void TApp::KeyUp( size_t ku )
{
	TEvent::KeyUp( ku );
	switch ( ku )
	{
		case VK_LEFT:
			keys = keys & ~aLeft;
			break;

		case VK_RIGHT:
			keys = keys & ~aRight;
			break;

		case VK_UP:
			keys = keys & ~aUp;
			break;

		case VK_DOWN:
			keys = keys & ~aDown;
			break;

		case 'a':
		case 'A':
			{
				keys = keys & ~16;
				break;
			}

		case 'z':
		case 'Z':
			{
				keys = keys & ~32;
				break;
			}

		// stop editing
		case VK_ESCAPE:
			{
				if ( def.NumLandscapeObjects()>0 )
				{
					if ( currentObjectInUse )
					{
						def.LandscapeObjects(currentEdit, currObj);
						currentEdit = def.NumLandscapeObjects()-1;
						currentObjectInUse = false;
						currObj.GetTranslation(lx,ly,lz);
					}
				}
				break;
			}

	}
};


void TApp::KeyPress( size_t key )
{
	TEvent::KeyPress(key);
};


// try and read resolution settings from registry
bool TApp::GetRegistrySettings( void )
{
	TEvent::GetRegistrySettings();

	size_t lfps;
	if ( GetRegistryKey( "Software\\PDV\\Performance", "LimitFPS", lfps ) )
		if ( lfps==1 )
			limitFPS = true;
		else
			limitFPS = false;
	return true;
};


TCompoundObject* TApp::GetObjectByName( const TString& name,
									    size_t, size_t )
{
	for ( size_t i=0; i<numObjects; i++ )
		if ( objectNames[i]==name )
			return objects[i];
	return NULL;
};


size_t TApp::GetObjectIndexByName( const TString& name ) const
{
	for ( size_t i=0; i<numObjects; i++ )
		if ( objectNames[i]==name )
			return i;
	return 0;
};


//	openGL pick-list: see what object was clicked on
bool TApp::ClickedObject( TLandscapeObject& obj, int mx, int my )
{
	GLuint buf[256];
	GLint  viewport[4];

	glGetIntegerv(GL_VIEWPORT,viewport); // get viewport info
	glSelectBuffer(256,buf);
	glRenderMode(GL_SELECT);

	glInitNames();
	glPushName(0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPickMatrix(mx,viewport[3]-my,5,5,viewport);
	gluPerspective(60.0f,vwidth / vheight, 1.0f, vdepth );

	// set world transformation
	glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glRotatef( cam.XAngle(), 1,0,0 );
	glRotatef( cam.YAngle(), 0,1,0 );
	glTranslatef( cam.CameraX(),
				  cam.CameraY(),
				  cam.CameraZ() );

	glScalef( scale, scale, scale );

	// get object abs coords in this world
	float x1,x2,x3,x4,x5,x6,x7,x8;
	float y1,y2,y3,y4,y5,y6,y7,y8;
	float z1,z2,z3,z4,z5,z6,z7,z8;
	obj.GetCorners( x1,y1,z1, x2,y2,z2, x3,y3,z3, x4,y4,z4,
					x5,y5,z5, x6,y6,z6, x7,y7,z7, x8,y8,z8 );

	glBegin(GL_QUADS);
		glVertex3f( x1,y1,z1 );
		glVertex3f( x3,y3,z3 );
		glVertex3f( x4,y4,z4 );
		glVertex3f( x2,y2,z2 );

		glVertex3f( x1,y1,z1 );
		glVertex3f( x2,y2,z2 );
		glVertex3f( x6,y6,z6 );
		glVertex3f( x5,y5,z5 );

		glVertex3f( x6,y6,z6 );
		glVertex3f( x8,y8,z8 );
		glVertex3f( x7,y7,z7 );
		glVertex3f( x5,y5,z5 );

		glVertex3f( x4,y4,z4 );
		glVertex3f( x3,y3,z3 );
		glVertex3f( x7,y7,z7 );
		glVertex3f( x8,y8,z8 );

		glVertex3f( x8,y8,z8 );
		glVertex3f( x6,y6,z6 );
		glVertex3f( x2,y2,z2 );
		glVertex3f( x4,y4,z4 );

		glVertex3f( x5,y5,z5 );
		glVertex3f( x7,y7,z7 );
		glVertex3f( x3,y3,z3 );
		glVertex3f( x1,y1,z1 );

	glEnd();

	glMatrixMode(GL_PROJECTION);
	glFlush();

	// any hits?
	int hits = glRenderMode(GL_RENDER);
	if ( hits>0 )
		return true;

	return false;
}


void TApp::DoMenu( int cmd )
{
	if ( editor!=NULL )
		editor->DoMenu(cmd);
};


void TApp::Scale( float _scale )
{
	scale = _scale;
};


bool TApp::ShowBotMarkers( void ) const
{
	return showBotMarkers;
};


void TApp::ShowBotMarkers( bool show )
{
	showBotMarkers = show;
};


void TApp::SetCurrentObject( const TString& name )
{
	currentObject = GetObjectIndexByName( name );
	currentObjectName = name;
};


TCompoundObject* TApp::GetCurrentObject( TString& name ) const
{
	name = currentObjectName;
	return objects[currentObject];
};

bool TApp::Modified( void ) const
{
	bool modified = false;
	if ( editor!=NULL )
		modified = editor->Modified();
	if ( botEditor!=NULL && !modified )
		modified = botEditor->Changed();
		
	return modified;
};

void TApp::RegisteredMessage( const TString& _registeredMessage )
{
	registeredMessage = _registeredMessage;
};

//==========================================================================

