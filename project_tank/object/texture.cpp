#include <precomp_header.h>

#include <object/texture.h>
#include <jpeglib/tjpeg.h>
#include <object/binobject.h>
#include <win32/resource.h>

//==========================================================================

extern HINSTANCE hInstance;

//==========================================================================

class TextureCache
{
public:
	TextureCache( void );
	TextureCache( const TTexture& );
	~TextureCache( void );

	void AddTexture( const TTexture& );
	const TTexture* GetTexture( const TString& fname ) const;

private:
	TextureCache*	next;
	const TTexture	texture;

public:
	static TextureCache first;
};


TextureCache TextureCache::first;

TextureCache::TextureCache( void )
	: next(NULL)
{
};


TextureCache::TextureCache( const TTexture& t )
	: next(NULL),
	  texture( t )
{
};


TextureCache::~TextureCache( void )
{
	next = NULL;
};


const TTexture* TextureCache::GetTexture( const TString& fname ) const
{
	TextureCache* list = next;
	while ( list!=NULL )
	{
		if ( stricmp(fname.c_str(),list->texture.Name().c_str())==0 )
			return &list->texture;
		list = list->next;
	}
	return NULL;
};

	
void TextureCache::AddTexture( const TTexture& t )
{
	if ( next==NULL )
	{
		next = new TextureCache( t );
	}
	else
	{
		TextureCache* list = next;
		bool found = false;
		while ( list->next!=NULL && !found )
		{
			if ( stricmp(t.Name().c_str(),list->texture.Name().c_str())==0 )
			{
				found = true;
			}
			else
				list = list->next;
		}
		if ( !found )
			list->next = new TextureCache( t );
	}
};

//==========================================================================
// GL TTexture class

TTexture::TTexture( void )
	: texture_id(NULL),
	  info(NULL),
	  fromCache(false),
	  rgb(NULL)
{
	Init();
};


TTexture::TTexture( const TTexture& texture )
	: texture_id(NULL),
	  info(NULL),
	  fromCache(false),
	  rgb(NULL){
	operator=(texture);
};


const TString& TTexture::Name( void ) const
{
	return filename;
};


const TTexture& TTexture::operator=( const TTexture& t )
{
	size_t i;

	used = t.used;
	mipmapped = t.mipmapped;
	width = t.width;
	height = t.height;

	filename = t.filename;

	// copy texture ids
	if ( texture_id!=NULL )
		delete texture_id;

	if ( t.texture_id!=NULL )
	{
		texture_id = new size_t[t.animationCount];
		PostCond( texture_id!=NULL );
		for ( i=0; i<t.animationCount; i++ )
			texture_id[i] = t.texture_id[i];
	}
	else
		texture_id = NULL;

	// copy info block
	if ( info!=NULL )
		delete info;

	if ( t.info!=NULL )
	{
		info = new BITMAPINFO;
		PostCond( info!=NULL );
		memcpy( info, t.info, sizeof(BITMAPINFO));
	}
	else
		info = NULL;

	// copy rgb block
	if ( rgb!=NULL )
		delete rgb;

	if ( t.rgb!=NULL )
	{
		size_t size = width*height*3;
		rgb = new unsigned char[size];
		PostCond( rgb!=NULL );
		for ( i=0; i<size; i++ )
			rgb[i] = t.rgb[i];
	}
	else
		rgb = NULL;

	// anim vars
	animated = t.animated;
	animationType = t.animationType;
	animationDirection = t.animationDirection;
	animationCount = t.animationCount;
	animationIndex = t.animationIndex;
	animationFPS = t.animationFPS;
	animationMS = t.animationMS;
	animationTickCount = 0;
	animationPaused = t.animationPaused;

	return *this;
};


TTexture::~TTexture()
{
	ClearData();
}


void TTexture::ClearData( void )
{
	Init();

	if ( texture_id!=NULL )
    	delete texture_id;
    texture_id = NULL;

	if ( rgb!=NULL )
    	delete rgb;
    rgb = NULL;
};


bool TTexture::LoadBinary( const TString& fname, 
						   const TString& path, 
						   TString& errStr, 
						   bool mipmap )
{
    TString specificError;

	ClearData();
	Mipmapped( mipmapped );
	errStr = "";

	// construct filename
	size_t start, end, frequency;
	int type;
	TString fileNameFormat;
    ConstructPath( filename, path, fname );

	// does it already exist in cache?
	// if so - just copy the cache item and return
	const TTexture* texture = TextureCache::first.GetTexture(filename);
	if ( texture!=NULL )
	{
		operator=(*texture);
		fromCache = true;
//		WriteString( "found texture '%s' in cache (saving %ld byes)\n", filename,
//					 (texture->Width()*texture->Height()*3) );
		return true;
	};

	//
    // is it an animation, bitmap or a jpeg?
	// animation [fname##.jpg,{-2,-1,0,1,2},start,end,freq_fps]
	//
	bool error = false;
	if ( isAnimation( fname, fileNameFormat, type, start, end, frequency, error, errStr ) )
	{
		if ( error )
			return false;

		PreCond( start<end );
		PreCond( frequency!=0 );
		PreCond( type>=-2 || type<=2 );
		PreCond( fileNameFormat.length()>0 );

		animated = true;
		AnimationType( type );
		animationCount = end-start;
		AnimationFPS( frequency );

		texture_id = new size_t[animationCount];

		// process x animations
		for ( size_t i=0; i<animationCount; i++ )
		{
			filename = FormatString( fileNameFormat.c_str(), i );
			TString tempFname;
		    ConstructPath( tempFname, path, filename );
			if ( isBitmap( tempFname ) )
			{
				if ( !LoadBitmap( tempFname, specificError ) )
				{
					error = true;
					errStr = "Error loading bitmap: \"" + filename;
					errStr = errStr + "\"\n(" + specificError;
					errStr = errStr + ")";
					return false;
				}
			}
			else // its gotta be a JPEG
			{
				if ( !LoadJpeg( tempFname, specificError ) )
				{
					error = true;
					errStr = "Error loading jpeg: \"" + filename;
					errStr = errStr + "\"\n(" + specificError;
					errStr = errStr + ")";
					return false;
				}
			}
			texture_id[i] = SetupRGB( mipmap );
		};
	}
    else if ( isBitmap( filename ) )
    {
        if ( !LoadBitmap( filename, specificError ) )
        {
			errStr = "Error loading bitmap: \"" + filename;
			errStr = errStr + "\"\n(" + specificError;
			errStr = errStr + ")";
            return false;
        }
		texture_id = new size_t;
		animationCount = 1;
		texture_id[0] = SetupRGB( mipmap );
    }
    else // its gotta be a JPEG
    {
        if ( !LoadJpeg( filename, specificError ) )
        {
			errStr = "Error loading jpeg: \"" + filename;
			errStr = errStr + "\"\n(" + specificError;
			errStr = errStr + ")";
            return false;
        }
		texture_id = new size_t;
		animationCount = 1;
		texture_id[0] = SetupRGB( mipmap );
    }
	TextureCache::first.AddTexture( *this );

	return true;
}


bool TTexture::LoadRGBAJPEG( const TString& fname, 
							 const TString& path, 
							 TString& errStr, 
							 unsigned char a )
{
	TString specificError, filename;

	ConstructPath( filename, path, fname );
	if ( !LoadJpeg( filename, specificError ) )
    {
		errStr = "Error loading jpeg: \"" + filename;
		errStr = errStr + "\"\n(" + specificError;
		errStr = errStr + ")";
        return false;
    }
	texture_id = new size_t;
	animationCount = 1;
	texture_id[0] = SetupRGBA( a );
	return true;
};


bool TTexture::LoadRGBAJPEG( int resource,
							 TString& errStr, 
							 unsigned char a )
{
	TString specificError, filename;

	if ( !LoadJpeg( resource, specificError ) )
    {
		errStr = "Error loading jpeg: \"" + filename;
		errStr = errStr + "\"\n(" + specificError;
		errStr = errStr + ")";
        return false;
    }
	texture_id = new size_t;
	animationCount = 1;
	texture_id[0] = SetupRGBA( a );
	return true;
};


size_t TTexture::SetupRGB( bool mipmapped )
{
	// get a TTexture id for this TTexture
    size_t tid;
	glGenTextures(1,&tid);

	if ( Rgb()!=NULL )
	{
		// setup openGL mipmap or normal
		glBindTexture(GL_TEXTURE_2D, tid );
		if ( mipmapped )
		{
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, Width(), Height(), GL_RGB, GL_UNSIGNED_BYTE, Rgb() );
/*
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width(), Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, Rgb());
*/
		}
		else
		{
    		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width(), Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, Rgb());
		}
	}

	//
	// remove RGB stub after use to clear up memory
	//
	if ( rgb!=NULL )
    	delete rgb;
    rgb = NULL;
	Used(true);

	return tid;
};


size_t TTexture::SetupRGBA( unsigned char a )
{
	// get a TTexture id for this TTexture
    size_t tid;
	glGenTextures(1,&tid);

	if ( Rgb()!=NULL )
	{
		// turn RGB into RGBA using a uniform a
		unsigned char* rgba = new unsigned char[Width()*Height()*4];
		for ( int i=0; i<Height(); i++ )
		for ( int j=0; j<Width(); j++ )
		{
			rgba[i*Width()*4+j*4+0] = rgb[i*Width()*3+j*3+0];
			rgba[i*Width()*4+j*4+1] = rgb[i*Width()*3+j*3+1];
			rgba[i*Width()*4+j*4+2] = rgb[i*Width()*3+j*3+2];
			rgba[i*Width()*4+j*4+3] = a;
		};

		glBindTexture(GL_TEXTURE_2D, tid );
   		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width(), Height(), 0, 
					 GL_RGBA, GL_UNSIGNED_BYTE, rgba );

		// remove rgba
		delete rgba;
	}

	//
	// remove RGB stub after use to clear up memory
	//
	if ( rgb!=NULL )
    	delete rgb;
    rgb = NULL;
	Used(true);

	return tid;
};


//
//	Initialise vars
//
void TTexture::Init()
{
	Used( false );
	Width( 0 );
	Height( 0 );
	Mipmapped( false );

	animated = false;
	animationType = 0;
	animationDirection = 1;
	animationCount = 0;
	animationIndex = 0;
	animationFPS = 0;
	animationMS = 0;
	animationTickCount = 0;
	animationPaused = false;
}



//
//	loadBitmap() load a DIB/BMP file from disk.
//
//	Returns a pointer to the bitmap if successful, NULL otherwise...
//
bool TTexture::LoadBitmap( const TString& filename, TString& _error )
{
	FILE	*fp;				// Open file pointer
	size_t	bitsize, infosize;	// Size of bitmap and header information
	BITMAPFILEHEADER header;	// File header
	int y;


	//
	// Try opening the file; use "rb" mode to read this *binary* file.
	//
    _error = "";
	if ((fp = fopen(filename.c_str(), "rb")) == NULL)
    {
    	_error = "can't open file";
		return false;
    }

	//
	//	Read the file header and any following bitmap information...
	//
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1 )
	{
		//
		// Couldn't read the file header - return false
		//
    	_error = "header size incorrect";
		fclose(fp);
		return false;
	}

#ifndef _MSVC
	if (header.bfType != 'BM')	// Check for BM
#else
	if (header.bfType != 'MB')	// Check for BM
#endif
	{
		//
		// Not a bitmap file - return NULL...
		//
    	_error = "not a bitmap";
		fclose(fp);
		return false;
	}

	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	if ((info = new(BITMAPINFO)) == NULL)
	{
		//
		// Couldn't allocate memory for bitmap info - return NULL...
		//
    	 _error = "out of memory";
		fclose(fp);
		return false;
	}

	if (fread(info, 1, infosize, fp) < infosize)
	{
		//
		// Couldn't read the bitmap header - return NULL...
		//
    	_error = "error reading bitmap header";
	    delete info;
	    fclose(fp);
		return false;
	}

	//
	//	Make sure its a 24 bit bitmap
	//
	if ( info->bmiHeader.biBitCount!=24 )
	{
    	_error = "not a 24 bit bitmap";
	    delete info;
	    fclose(fp);
		return false;
	}

	//
	//	Make sure width and height are powers of 2 (openGL requirement!)
	//
	int w = info->bmiHeader.biWidth;
	int h = info->bmiHeader.biHeight;
	if ( !(w==1 || w==2 || w==4 || w==8 || w==16 || w==32 || w==64 || w==128 || w==256 ||
		 w==512 || w==1024 || w==2048 || w==8192 || w==16384) )
    {
    	_error = "width not a power of 2";
		return false;
    }
	if ( !(h==1 || h==2 || h==4 || h==8 || h==16 || h==32 || h==64 || h==128 || h==256 ||
		   h==512 || h==1024 || h==2048 || h==8192 || h==16384) )
    {
    	_error = "height not a power of 2";
		return false;
    }

	//
	//	Now that we have all the header info read in, allocate memory for the
	//	bitmap and read *it* in...
	//
	if ((bitsize = info->bmiHeader.biSizeImage) == 0)
		 bitsize = (info->bmiHeader.biWidth * info->bmiHeader.biBitCount + 7) / 8 * abs(info->bmiHeader.biHeight);

    Rgb( new GLubyte[bitsize] );
	if (  Rgb()==NULL )
	{
		//
		// Couldn't allocate memory - return NULL!
		//
    	_error = "out of memory allocating RGB map";
	    delete info;
	    fclose(fp);
	    return false;
	}

	if (fread((void *)Rgb(), 1, bitsize, fp) < bitsize)
	{
		//
		// Couldn't read bitmap - free memory and return NULL!
		//
    	_error = "error reading rgb part of bitmap";
	    delete info;
	    delete rgb;
        rgb = NULL;
		fclose(fp);
		return false;
	}

	fclose(fp);

	GLubyte *s1,*s2;
	GLubyte t;

	Width( info->bmiHeader.biWidth );
	Height( info->bmiHeader.biHeight );

	//
	//	Swap R & Bs
	//
    unsigned char* p = rgb;
	for (y=0;y<Height();y++)
	{
		s1 = &p[y*Width()*3];
		for (int x=0;x<Width();x++)
		{
			t = s1[x*3];
			s1[x*3] = s1[x*3+2];
			s1[x*3+2] = t;
		}
	}

	//
	//	turn the BMP upside down
	//
	GLubyte *tempp = new GLubyte[Width()*3];
	for ( y=0; y<Height()/2; y++ )
	{
		s1 = &p[y*Width()*3];
		memcpy(tempp,s1,Width()*3);
		s2 = &p[((Height()-1)-y)*Width()*3];
		memcpy(s1,s2,Width()*3);
		memcpy(s2,tempp,Width()*3);
	}
	delete tempp;

	Used( true );
	return true;
}




//
//	OpenGL setup for TTexture
//
void TTexture::StartTexture( void ) const
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, TextureId() );

	// animate?
	if ( animated )
	if ( AnimationType()!=0 )
	{
		long tc = ::GetTickCount();
		if ( tc>animationTickCount )
		{
			NextAnimation();
			animationTickCount = tc + animationMS;
		}
	}
}



//
//	End openGL TTexture setup
//
void TTexture::EndTexture( void )
{
	glDisable(GL_TEXTURE_2D);
}



void TTexture::Draw( float x1, float y1, float x2, float y2 ) const
{
	glColor3ub(255,255,255);
	StartTexture();
	glBegin(GL_QUADS);
		glTexCoord2f(1,0);
		glNormal3f(0,0,-1);
		glVertex3f(x1,y1,0);

		glTexCoord2f(1,1);
		glNormal3f(0,0,-1);
		glVertex3f(x1,y2,0);

		glTexCoord2f(0,1);
		glNormal3f(0,0,-1);
		glVertex3f(x2,y2,0);

		glTexCoord2f(0,0);
		glNormal3f(0,0,-1);
		glVertex3f(x2,y1,0);
	glEnd();
	EndTexture();
}


void TTexture::DrawColour( float x1, float y1, float x2, float y2,
						   unsigned char r, unsigned char g, unsigned char b ) const
{
	glColor3ub(255,255,255);
	StartTexture();
	glBegin(GL_QUADS);
		glColor3ub(r,g,b);
		glTexCoord2f(0,0);
		glNormal3f(0,0,-1);
		glVertex3f(x1,y1,0);

		glColor3ub(r,g,b);
		glTexCoord2f(0,1);
		glNormal3f(0,0,-1);
		glVertex3f(x1,y2,0);

		glColor3ub(r,g,b);
		glTexCoord2f(1,1);
		glNormal3f(0,0,-1);
		glVertex3f(x2,y2,0);

		glColor3ub(r,g,b);
		glTexCoord2f(1,0);
		glNormal3f(0,0,-1);
		glVertex3f(x2,y1,0);
	glEnd();
	EndTexture();
}


bool TTexture::isBitmap( const TString& filename )
{
	int len = filename.length();
    if ( len<4 )
    	return false;

    TString temp = filename.lcase();
    if ( temp[len-3]=='b' && temp[len-2]=='m' && temp[len-1]=='p' )
    	return true;
    return false;
};



bool TTexture::LoadJpeg( const TString& filename, TString& _error )
{
	TJpeg jpeg;

	char tempError[256];
	tempError[0] = 0;

	if ( !jpeg.Load( filename.c_str(), tempError ) )
	{
		_error = tempError;
   		return false;
	}

	int w = jpeg.Width();
	int h = jpeg.Height();
	if ( !(w==1 || w==2 || w==4 || w==8 || w==16 || w==32 || w==64 || w==128 || w==256 ||
		 w==512 || w==1024 || w==2048 || w==8192 || w==16384) )
    {
    	_error = "width not a power of 2";
		return false;
    }
	if ( !(h==1 || h==2 || h==4 || h==8 || h==16 || h==32 || h==64 || h==128 || h==256 ||
		   h==512 || h==1024 || h==2048 || h==8192 || h==16384) )
    {
    	_error = "height not a power of 2";
		return false;
    }

    // set width & height
	Width( jpeg.Width() );
	Height( jpeg.Height() );

    // allocate and copy bitmap
    int rgbSize = Height()*Width()*3;
	GLubyte *tempp = new GLubyte[rgbSize];
    if ( tempp==NULL )
    {
    	_error = "error allocating JPEG memory";
		return false;
    }

    // copy memory
    memcpy( tempp, jpeg.Bits(), rgbSize );
    Rgb( tempp );

	//
	//	Swap R & Bs
	//
    unsigned char* p = rgb;
	for (int y=0;y<Height();y++)
	{
		unsigned char* s1 = &p[y*Width()*3];
		for (int x=0;x<Width();x++)
		{
			unsigned char t = s1[x*3];
			s1[x*3] = s1[x*3+2];
			s1[x*3+2] = t;
		}
	}

	Used( true );
	return true;
};



bool TTexture::LoadJpeg( int resource, TString& _error )
{
	TJpeg jpeg;

	char tempError[256];
	tempError[0] = 0;

	if ( !jpeg.Load( hInstance, MAKEINTRESOURCE( resource ), tempError ) )
	{
		_error = tempError;
   		return false;
	}

	int w = jpeg.Width();
	int h = jpeg.Height();
	if ( !(w==1 || w==2 || w==4 || w==8 || w==16 || w==32 || w==64 || w==128 || w==256 ||
		 w==512 || w==1024 || w==2048 || w==8192 || w==16384) )
    {
    	_error = "width not a power of 2";
		return false;
    }
	if ( !(h==1 || h==2 || h==4 || h==8 || h==16 || h==32 || h==64 || h==128 || h==256 ||
		   h==512 || h==1024 || h==2048 || h==8192 || h==16384) )
    {
    	_error = "height not a power of 2";
		return false;
    }

    // set width & height
	Width( jpeg.Width() );
	Height( jpeg.Height() );

    // allocate and copy bitmap
    int rgbSize = Height()*Width()*3;
	GLubyte *tempp = new GLubyte[rgbSize];
    if ( tempp==NULL )
    {
    	_error = "error allocating JPEG memory";
		return false;
    }

    // copy memory
    memcpy( tempp, jpeg.Bits(), rgbSize );
    Rgb( tempp );

	//
	//	Swap R & Bs
	//
    unsigned char* p = rgb;
	for (int y=0;y<Height();y++)
	{
		unsigned char* s1 = &p[y*Width()*3];
		for (int x=0;x<Width();x++)
		{
			unsigned char t = s1[x*3];
			s1[x*3] = s1[x*3+2];
			s1[x*3+2] = t;
		}
	}

	Used( true );
	return true;
};



void TTexture::Width( int w )
{
	width = w;
}




int	TTexture::Width( void ) const
{
	return width;
}




void TTexture::Height( int h )
{
	height = h;
}




int	TTexture::Height( void ) const
{
	return height;
}




const unsigned char* TTexture::Rgb( void ) const
{
	return rgb;
}




void TTexture::Rgb( const unsigned char* _rgb )
{
	rgb = const_cast<unsigned char*>(_rgb);
}




void TTexture::Used( bool u )
{
	used = u;
}




bool TTexture::Used( void ) const
{
	return used;
}




void TTexture::TextureId( int tid )
{
	PreCond( texture_id!=NULL );
	texture_id[0] = tid;
}




int	TTexture::TextureId( void ) const
{
	if ( !animated )
		return texture_id[0];
	else
		return texture_id[animationIndex];
}




void TTexture::Mipmapped( bool mm )
{
	mipmapped = mm;
}



bool TTexture::Mipmapped( void ) const
{
	return mipmapped;
}


bool TTexture::Animated( void ) const
{
	return animated;
};



size_t TTexture::NextAnimation( void ) const
{
	if ( animationPaused )
		return animationIndex;

	switch ( animationType )
	{
		case 1:
			{
				animationIndex++;
				if ( animationIndex>=animationCount )
				animationIndex = 0;
			}
			break;
		case -2:
		case 2:
			{
				animationIndex += animationDirection;
				if ( animationDirection==1 && animationIndex>=animationCount )
				{
					animationIndex = animationCount - 2;
					animationDirection = -1;
				}
				else if ( animationDirection==-1 && animationIndex==0 )
				{
					animationDirection = 1;
				}
			}
			break;

		case -1:
			{
				animationIndex--;
				if ( (animationIndex+1)==0 )
				{
					animationIndex = animationCount - 1;
				}
			}
			break;
	};
	return animationIndex;
};



void TTexture::NextAnimation( size_t next )
{
	animationIndex = next;
};



int	TTexture::AnimationType( void ) const
{
	return animationType;
};



void TTexture::AnimationType( int _type )
{
	animationType = _type;
	switch ( _type )
	{
		case 0:
		case 1:
		case 2:
			animationDirection = 1;
			break;
		default:
			animationDirection = -1;
	};
};



void TTexture::AnimationFPS( size_t animFPS )
{
	PreCond( animFPS!=0 );

	animationFPS = animFPS;
	animationMS = 1000 / animFPS;
};


size_t TTexture::AnimationFPS( void ) const
{
	return animationFPS;
};


size_t TTexture::AnimationCount( void ) const
{
	return animationCount;
};


void TTexture::PauseAnimation( bool tf )
{
	animationPaused = tf;
};


bool TTexture::isAnimation( const TString& filename, TString& _format, int& type, 
							size_t& start, size_t& end, size_t& fps, 
							bool& error, TString& errStr )
{
	char format[256];

	PreCond( filename.length()>0 );
	if ( filename[0]=='[' )
	{
		size_t i=0;
		size_t next = 0;
		while ( filename[i]!=',' && filename[i]!=0 ) i++;
		if ( filename[i]!=',' || i==1 )
		{
			error = true;
			errStr = "Error processing animation, missing comma or format @1\n(" + filename;
			errStr = errStr + ")";
			return true;
		}
		next = i+1;

		// setup format
		TString temp;
		temp = &filename[1];
		temp = temp.substr(0,i-1);

		// count number of #s in format
		i=0;
		bool set = false;
		size_t number = 0;
		size_t numberIndex = 0;
		size_t findex = 0;
		while ( temp[i]!=0 )
		{
			if ( temp[i]=='#' )
			{
				number++;
				if ( !set )
				{
					set = true;
					format[findex++] = '%';
					format[findex++] = '0';
					format[findex++] = '1';
					numberIndex = findex-1;
					format[findex++] = 'd';
				}
			}
			else
			{
				format[findex++] = temp[i];
			}
			i++;
		}
		format[findex]=0;

		if ( number==0 || findex==0 )
		{
			error = true;
			errStr = "Error processing animation, missing # @1\n(" + filename;
			errStr = errStr + ")";
			return true;
		};
		format[numberIndex] = char(48+number);
		_format = format;

		// type
		i = next;
		while ( filename[i]!=',' && filename[i]!=0 ) i++;
		if ( filename[i]!=',' )
		{
			error = true;
			errStr = "Error processing animation, missing type @2\n(" + filename;
			errStr = errStr + ")";
			return true;
		}
		temp = filename.substr(next,i-next);
		type = atoi(temp.c_str());
		next = i+1;

		// start
		i = next;
		while ( filename[i]!=',' && filename[i]!=0 ) i++;
		if ( filename[i]!=',' )
		{
			error = true;
			errStr = "Error processing animation, missing start @3\n(" + filename;
			errStr = errStr + ")";
			return true;
		}
		temp = filename.substr(next,i-next);
		start = size_t(atoi(temp.c_str()));
		next = i+1;
		
		// end
		i = next;
		while ( filename[i]!=',' && filename[i]!=0 ) i++;
		if ( filename[i]!=',' )
		{
			error = true;
			errStr = "Error processing animation, missing end @4\n(" + filename;
			errStr = errStr + ")";
			return true;
		}
		temp = filename.substr(next,i-next);
		end = size_t(atoi(temp.c_str()));
		next = i+1;

		// fps
		i = next;
		while ( filename[i]!=']' && filename[i]!=0 ) i++;
		if ( filename[i]!=']' )
		{
			error = true;
			errStr = "Error processing animation, missing fps] @5\n(" + filename;
			errStr = errStr + ")";
			return true;
		}
		temp = filename.substr(next,i-next);
		fps = size_t(atoi(temp.c_str()));

		return true;
	}
	return false;
};


long TTexture::ObjectSize( void ) const
{ 
	long objectSize = sizeof(TTexture);
	if ( !fromCache )
	{
		if ( Animated() )
			objectSize += (Width() * Height() * 3) * AnimationCount();
		else
			objectSize += (Width() * Height() * 3);
	}
	return objectSize;
}

//==========================================================================


