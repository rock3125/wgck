#include <precomp_header.h>

#include <object/texture.h>
#include <jpeglib/tjpeg.h>
#include <pnglib/tpng.h>
#include <object/tga.h>
#include <object/binobject.h>
#include <win32/resource.h>

extern HWND mainWindow;

//==========================================================================

class TextureCache
{
public:
	TextureCache( void );
	TextureCache( TTexture* );
	~TextureCache( void );

	void AddTexture( TTexture* );
	void RemoveTexture( TTexture* );

	const TTexture* GetTexture( const TString& fname ) const;

	// verify if there is a copy of this texture in the cache
	bool Exists( const TTexture& );

	static void ClearCache( void ); // force clear

private:
	TextureCache*	next;
	TTexture*		texture;
	bool			used;
	size_t			refCount;

public:
	static TextureCache first;
};


// static - define for system
TextureCache TextureCache::first;


TextureCache::TextureCache( void )
	: next(NULL),
	  texture(NULL)
{
	refCount = 0;
	used = false;
};


TextureCache::TextureCache( TTexture* t )
	: next(NULL),
	  texture( t )
{
	refCount = 1;
	used = true;
};


TextureCache::~TextureCache( void )
{
	if ( texture!=NULL )
	{
//		glDeleteTextures( texture->animationCount, texture->texture_id );
		texture = NULL;
	}
};


void TextureCache::ClearCache( void )
{
	TextureCache* list = first.next;
	while ( list!=NULL )
	{
		TextureCache* temp = list;
		list = list->next;
		temp->next = NULL;
		delete temp;
	}
	first.next = NULL;
};


const TTexture* TextureCache::GetTexture( const TString& fname ) const
{
	return NULL;

	TextureCache* list = next;
	while ( list!=NULL )
	{
		if ( list->texture!=NULL )
		if ( stricmp(fname.c_str(),list->texture->Name().c_str())==0 )
			return list->texture;
		list = list->next;
	}
	return NULL;
};


bool TextureCache::Exists( const TTexture& t )
{
	return false;

	TextureCache* list = next;
	while ( list!=NULL )
	{
		if ( list->texture!=NULL )
		if ( list->texture->animationCount == t.animationCount )
		{
			size_t cntr = t.animationCount;
			size_t i=0;
			bool finished = false;
			while ( i<cntr && !finished )
			{
				if ( list->texture->texture_id[i]!=t.texture_id[i] )
					finished = true;
				i++;
			}
			if ( !finished )
				return true;
		}
		list = list->next;
	}
	return false;
};


void TextureCache::AddTexture( TTexture* t )
{
	return;

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
			if ( list->texture!=NULL )
			{
				if ( stricmp(t->Name().c_str(),list->texture->Name().c_str())==0 )
				{
					found = true;
					list->refCount++;
				}
				else
					list = list->next;
			}
			else
				list = list->next;
		}
		if ( !found )
			list->next = new TextureCache( t );
	}
};

void TextureCache::RemoveTexture( TTexture* t )
{
	return;

	if ( next!=NULL )
	{
		TextureCache* list = next;
		TextureCache* prev = NULL;
		bool found = false;
		while ( list!=NULL && !found )
		{
			if ( list->texture!=NULL )
			{
				if ( stricmp(t->Name().c_str(),list->texture->Name().c_str())==0 )
				{
					found = true;
					list->refCount--;
					if ( list->refCount==0 ) // remove
					{
						if ( prev!=NULL )
						{
							prev->next = list->next;
						}
						else
						{
							next = list->next;
						}
						list->next = NULL;
						delete list;
					}
				}
				else
				{
					prev = list;
					list = list->next;
				}
			}
			else
			{
				prev = list;
				list = list->next;
			}
		}
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
	dontDestroyRGB = false;
	Init();
};

TTexture::TTexture( const TTexture& texture )
	: texture_id(NULL),
	  info(NULL),
	  fromCache(false),
	  rgb(NULL)
{
	Init();
	dontDestroyRGB = false;
	operator=(texture);
};


TTexture::~TTexture()
{
	TextureCache::first.RemoveTexture( this );
	ClearData();
}

void TTexture::ClearCache( void )
{
	TextureCache::ClearCache();
};

void TTexture::ClearData( void )
{	
	if ( texture_id!=NULL )
    	delete texture_id;
    texture_id = NULL;

	if ( rgb!=NULL )
    	delete []rgb;
    rgb = NULL;
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
	numBytes = t.numBytes;

	isRGBA = t.isRGBA;

	dontDestroyRGB = t.dontDestroyRGB;

	filename = t.filename;

	// copy texture ids
	if ( texture_id!=NULL )
		delete texture_id;
	texture_id = NULL;

	if ( t.texture_id!=NULL )
	{
		size_t size = t.animationCount;
		if ( size==0 )
			size = 1;
		texture_id = new size_t[size];
		for ( i=0; i<size; i++ )
			texture_id[i] = t.texture_id[i];
	}

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
		delete []rgb;

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

	if ( info!=NULL )
		delete info;

	// add this new texture to the cache
	TextureCache::first.AddTexture( this );

	return *this;
};

byte* TTexture::LoadGraphic( const TString& filename, size_t& width, size_t& height,
							 size_t& bytes, TString& errStr )
{
	TString specificError;

	width = 0;
	height = 0;
	bytes = 0;

    if ( isBitmap( filename ) )
    {
        byte* rgb = LoadBitmap( filename, width, height, bytes, specificError );
		if ( rgb==NULL )
        {
			errStr = "Error loading bitmap: \"" + filename;
			errStr = errStr + " \"(" + specificError;
			errStr = errStr + ")";
            return NULL;
        }
		return rgb;
	}
    else if ( isJPEG( filename ) )
    {
        byte* rgb = LoadJpeg( filename, width, height, bytes, specificError );
		if ( rgb==NULL )
        {
			errStr = "Error loading JPEG: \"" + filename;
			errStr = errStr + " \"(" + specificError;
			errStr = errStr + ")";
            return NULL;
        }
		return rgb;
	}
    else if ( isPNG( filename ) )
    {
        byte* rgb = LoadPNG( filename, width, height, bytes, specificError );
		if ( rgb==NULL )
        {
			errStr = "Error loading PNG: \"" + filename;
			errStr = errStr + " \"(" + specificError;
			errStr = errStr + ")";
            return NULL;
        }
		return rgb;
	}
    else if ( isTGA( filename ) )
    {
        byte* rgb = LoadTGA( filename, width, height, bytes, specificError );
		if ( rgb==NULL )
        {
			errStr = "Error loading TGA: \"" + filename;
			errStr = errStr + " \"(" + specificError;
			errStr = errStr + ")";
            return NULL;
        }
		return rgb;
	}
	errStr = "Error: graphic file-type not recognised \"" + filename + "\"";
	return NULL;
};


size_t TTexture::LoadGraphic( const TString& fname, const TString& path, 
						      size_t*& textureId, TString& errStr, 
							  bool mipmapped )
{
	size_t start, end, frequency;
	int type;
	TString fileNameFormat;
	TString specificError;

	// clear previous data
	ClearData();

	//
    // is it an animation, bitmap or a jpeg?
	// animation [fname##.jpg,{-2,-1,0,1,2},start,end,freq_fps]
	//
	bool error = false;
	if ( isAnimation( fname, fileNameFormat, type, start, end, 
					  frequency, error, errStr ) )
	{
		if ( error )
			return ttNone;

		PreCond( start<end );
		PreCond( frequency!=0 );
		PreCond( type>=-2 || type<=2 );
		PreCond( fileNameFormat.length()>0 );

		animated = true;
		AnimationType( type );
		animationCount = end-start;
		AnimationFPS( frequency );

		textureId = new size_t[animationCount];

		// process x animations
		for ( size_t i=0; i<animationCount; i++ )
		{
			TString _filename = FormatString( fileNameFormat.c_str(), i );
			TString tempFname;
		    ConstructPath( tempFname, path, _filename );

			size_t w,h,b;
			byte* r = LoadGraphic( tempFname, w,h,b, errStr );
			if ( r==NULL )
			{
				return ttNone;
			}
			if ( b==3 )
			{
				textureId[i] = SetupRGB( r, w, h, mipmapped );
			}
			else
			{
				textureId[i] = SetupRGBA( r, w, h, mipmapped );
			}
			Width( w );
			Height( h );
			NumBytes( b );

			delete[] r;
		};

		isRGBA = false;
		filename = fname;
		return ttAnimation;
	}
	else if ( isMaskedBitmap( filename ) )
	{
		TString bmp1, bmp2;
		bmp1 = filename.GetItem(',',0);
		bmp2 = filename.GetItem(',',1);

		// put path on bmp2
		size_t i=bmp1.length();
		while ( i>0 && bmp1[i]!='\\' ) i--;
		if ( bmp1[i]=='\\' )
		{
			bmp2 = bmp1.substr(0,i+1) + bmp2;
		}

		size_t w,h,b;
		byte* r = LoadGraphic( bmp1, w,h,b, errStr );
		if ( r==NULL )
		{
			return ttNone;
		}

		FlipUpsideDown(w,h,b,r);

		Width( w );
		Height( h );
		NumBytes( 4 );

		byte* maskRgb;
		size_t maskWidth, maskHeight, maskByte;
		maskRgb = LoadBitmapMask( bmp2, maskWidth, maskHeight, 
								  maskByte, specificError );
		if ( maskRgb==NULL )
		{
			error = true;
			errStr = "Error loading bitmap mask: \"" + filename;
			errStr = errStr + "\"\n(" + specificError;
			errStr = errStr + ")";
			return ttNone;
		}
		if ( maskWidth!=w || maskHeight!=h )
		{
			error = true;
			errStr = "Error loading masked bitmap: \"" + filename;
			errStr = errStr + "\"\n( mask and original are not the same size)";
			delete[] maskRgb;
			return ttNone;
		}
		// now create an RGBA out of the two ???
		if ( texture_id!=NULL )
			delete texture_id;
		texture_id = new size_t;
		animationCount = 1;
		texture_id[0] = SetupRGBA( r, maskRgb, maskWidth, maskHeight, mipmapped );

		isRGBA = true;

		// finally - remove the mask's memory
		delete[] maskRgb;
		delete[] r;

		return ttRGBA;
	}
	else
	{
		size_t w,h,b;
		byte* r = LoadGraphic( fname, w,h,b, errStr );
		if ( r==NULL )
		{
			return ttNone;
		}

		Width( w );
		Height( h );
		NumBytes( b );

		isRGBA = false;
		if ( b==4 )
		{
			isRGBA = true;
		}

		if ( texture_id!=NULL )
			delete texture_id;
		texture_id = new size_t;
		animationCount = 1;

		if ( b==4 )
		{
			texture_id[0] = SetupRGBA( r, w, h, mipmapped );

			if ( dontDestroyRGB )
			{
				rgb = r;
			}
			else
			{
				delete[] r;
			}

			return ttRGBA;
		}
		else
		{
			texture_id[0] = SetupRGB( r, w, h, mipmapped );

			if ( dontDestroyRGB )
			{
				rgb = r;
			}
			else
			{
				delete[] r;
			}

			return ttRGB;
		}
	}
};

	
bool TTexture::LoadBinary( TString fname, 
						   TString path, 
						   TString& errStr, 
						   bool mipmap )
{
    TString specificError;

	ClearData();
	Mipmapped( mipmap );
	errStr = "";

	// construct filename
    ConstructPath( filename, path, fname );

	// does it already exist in cache?
	// if so - just copy the cache item and return
	const TTexture* texture = TextureCache::first.GetTexture(filename);
	if ( texture!=NULL )
	{
		operator=(*texture);
		fromCache = true;
		return true;
	};

	if ( LoadGraphic( filename, path, texture_id, errStr, mipmapped )==ttNone )
	{
		return false;
	}

	Used(true);
	TextureCache::first.AddTexture( this );
	return true;
}


bool TTexture::LoadRGBAJPEG( TString fname, 
							 TString path, 
							 TString& errStr, 
							 unsigned char _r,
							 unsigned char _g,
							 unsigned char _b,
							 unsigned char a1,
							 unsigned char a2,
							 unsigned char threshold,
							 bool mipmapped )
{
	TString specificError;

	ClearData();

	::ConstructPath( filename, path, fname );

	size_t w,h,b;
	byte* r = LoadJpeg( filename, w,h,b, specificError );
	if ( r==NULL )
    {
		errStr = "Error loading jpeg: \"" + filename;
		errStr = errStr + "\"\n(" + specificError;
		errStr = errStr + ")";
        return false;
    }
	texture_id = new size_t;
	animationCount = 1;
	texture_id[0] = SetupRGBA( rgb, w, h, mipmapped, _r,_g,_b,a1,a2,threshold );
	Width( w );
	Height( h );
	NumBytes( 4 );

	//
	// remove RGB stub after use to clear up memory
	//
	if ( r!=NULL && !dontDestroyRGB )
	{
    	delete []r;
		rgb = NULL;
	}
	else
	{
		rgb = r;
	}
	Used(true);
	return true;
};


bool TTexture::LoadBinary( TString fname, 
						   TString path, 
						   TString& errStr, 
						   unsigned char a,
						   bool mipmap )
{
    TString specificError;

	ClearData();
	Mipmapped( mipmapped );
	errStr = "";

	::ConstructPath( filename, path, fname );

	// does it already exist in cache?
	// if so - just copy the cache item and return
	const TTexture* texture = TextureCache::first.GetTexture(filename);
	if ( texture!=NULL )
	{
		operator=(*texture);
		fromCache = true;
		return true;
	};

    if ( isBitmap( filename ) )
    {
		size_t w,h,b;
        byte* r = LoadBitmap( filename, w,h,b, specificError );
		if ( r==NULL )
        {
			errStr = "Error loading bitmap: \"" + filename;
			errStr = errStr + "\"\n(" + specificError;
			errStr = errStr + ")";
            return false;
        }
		if ( texture_id!=NULL )
			delete texture_id;
		texture_id = new size_t;
		animationCount = 1;

		Width( w );
		Height( h );
		NumBytes( 4 );
		Used(true);
		texture_id[0] = SetupRGBA( r,w,h,mipmap, a );

		if ( dontDestroyRGB )
		{
			rgb = r;
		}
		else
		{
			delete[] r;
			rgb = NULL;
		}
    }
    else if ( isJPEG( filename ) )
    {
		size_t w,h,b;
        byte* r = LoadJpeg( filename, w,h,b, specificError );
		if ( r==NULL )
        {
			errStr = "Error loading jpeg: \"" + filename;
			errStr = errStr + "\"\n(" + specificError;
			errStr = errStr + ")";
            return false;
        }
		if ( texture_id!=NULL )
			delete texture_id;
		texture_id = new size_t;
		animationCount = 1;

		Width( w );
		Height( h );
		NumBytes( 4 );
		Used(true);
		texture_id[0] = SetupRGBA( r,w,h, mipmap, a );

		if ( dontDestroyRGB )
		{
			rgb = r;
		}
		else
		{
			delete[] r;
			rgb = NULL;
		}
    }
    else if ( isPNG( filename ) )
    {
		size_t w,h,b;
        byte* r = LoadJpeg( filename, w,h,b, specificError );
		if ( r==NULL )
        {
			errStr = "Error loading png: \"" + filename;
			errStr = errStr + "\"\n(" + specificError;
			errStr = errStr + ")";
            return false;
        }
		if ( texture_id!=NULL )
			delete texture_id;
		texture_id = new size_t;
		animationCount = 1;

		Width( w );
		Height( h );
		NumBytes( 4 );
		Used(true);
		texture_id[0] = SetupRGBA( r,w,h,mipmap, a );

		if ( dontDestroyRGB )
		{
			rgb = r;
		}
		else
		{
			delete[] r;
			rgb = NULL;
		}
    }
	else
	{
		errStr = "Error: TTexture: unknown file type (" + filename + ")";
		return false;
	}
	TextureCache::first.AddTexture( this );

	return true;
};

	
size_t TTexture::SetupRGB( byte* rgb, size_t w, size_t h, bool mipmapped )
{
	// get a TTexture id for this TTexture
    size_t tid;
	glGenTextures(1,&tid);

	if ( rgb!=NULL )
	{
		// setup openGL mipmap or normal
		glBindTexture(GL_TEXTURE_2D, tid );
		if ( mipmapped )
		{
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, rgb );
		}
		else
		{
    		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb );
		}
	}
	return tid;
};


size_t TTexture::SetupRGBA( byte* rgb, size_t w, size_t h, bool mipmapped )
{
	// get a TTexture id for this TTexture
    size_t tid;
	glGenTextures(1,&tid);

	if ( rgb!=NULL )
	{
		// setup openGL mipmap or normal
		glBindTexture(GL_TEXTURE_2D, tid );
/*
		if ( mipmapped )
		{
    		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgb );
		}
		else
*/
		{
    		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgb );
		}
	}

	return tid;
};


size_t TTexture::SetupRGBA( byte* rgb, byte* maskRgb, size_t w, size_t h, bool mipmapped )
{
	// get a TTexture id for this TTexture
    size_t tid;
	glGenTextures(1,&tid);

	if ( rgb!=NULL )
	{
		// turn RGB into RGBA
		unsigned char* rgba = new unsigned char[w*h*4];
		for ( size_t i=0; i<h; i++ )
		for ( size_t j=0; j<w; j++ )
		{
			rgba[i*w*4+j*4+0] = rgb[i*w*3+j*3+0];
			rgba[i*w*4+j*4+1] = rgb[i*w*3+j*3+1];
			rgba[i*w*4+j*4+2] = rgb[i*w*3+j*3+2];
			rgba[i*w*4+j*4+3] = maskRgb[i*w+j];
		};

		if ( mipmapped )
		{
			glBindTexture(GL_TEXTURE_2D, tid );
	   		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
						 GL_RGBA, GL_UNSIGNED_BYTE, rgba );
		}
		else
		{
    		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
						 GL_RGBA, GL_UNSIGNED_BYTE, rgba );
		}

		// remove rgba
		delete rgba;
	}
	return tid;
};


size_t TTexture::SetupRGBA( byte* rgb, size_t w, size_t h, bool mipmapped,
						    byte r, byte g, byte b,
							byte a1, byte a2, byte threshold )
{
	// get a TTexture id for this TTexture
    size_t tid;
	glGenTextures(1,&tid);

	{
		RGB2RGBA( rgb, w, h, r,g,b, a1, a2, threshold );

		if ( mipmapped )
		{
			glBindTexture(GL_TEXTURE_2D, tid );
	   		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
						 GL_RGBA, GL_UNSIGNED_BYTE, rgb );
		}
		else
		{
    		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
						 GL_RGBA, GL_UNSIGNED_BYTE, rgb );
		}
	}
	return tid;
};

size_t TTexture::SetupRGBA( byte* rgb, size_t w, size_t h, bool mipmapped, 
							byte a )
{
	// get a TTexture id for this TTexture
    size_t tid;
	glGenTextures(1,&tid);

	if ( rgb!=NULL )
	{
		// turn RGB into RGBA using a uniform a
		unsigned char* rgba = new unsigned char[w*h*4];
		for ( size_t i=0; i<h; i++ )
		for ( size_t j=0; j<w; j++ )
		{
			rgba[i*Width()*4+j*4+0] = rgb[i*Width()*3+j*3+0];
			rgba[i*Width()*4+j*4+1] = rgb[i*Width()*3+j*3+1];
			rgba[i*Width()*4+j*4+2] = rgb[i*Width()*3+j*3+2];
			rgba[i*Width()*4+j*4+3] = a;
		};

		if ( mipmapped )
		{
			glBindTexture(GL_TEXTURE_2D, tid );
	   		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
						 GL_RGBA, GL_UNSIGNED_BYTE, rgba );
		}
		else
		{
    		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, 
						 GL_RGBA, GL_UNSIGNED_BYTE, rgba );
		}

		// remove rgba
		delete[] rgba;
	}
	return tid;
};


bool TTexture::RGB2RGBA( byte* rgb, size_t w, size_t h,
						 unsigned char r,
						 unsigned char g,
						 unsigned char b,
						 unsigned char a1,
						 unsigned char a2,
						 unsigned char threshold )
{
	if ( Rgb()!=NULL )
	{
		// turn RGB into RGBA using a uniform a
		size_t size = Width()*Height()*4;
		unsigned char* rgba = new unsigned char[size];
		PostCond( rgba!=NULL );
		memset( rgba,0,size);

		for ( int i=0; i<Height(); i++ )
		for ( int j=0; j<Width(); j++ )
		{
			unsigned char _r = rgb[i*Width()*3+j*3+0];
			unsigned char _g = rgb[i*Width()*3+j*3+1];
			unsigned char _b = rgb[i*Width()*3+j*3+2];

			rgba[i*Width()*4+j*4+0] = _r;
			rgba[i*Width()*4+j*4+1] = _g;
			rgba[i*Width()*4+j*4+2] = _b;
			if ( unsigned char(abs(int(r)-int(_r))) < threshold &&
				 unsigned char(abs(int(g)-int(_g))) < threshold &&
				 unsigned char(abs(int(b)-int(_b))) < threshold )
				rgba[i*Width()*4+j*4+3] = a1;
			else
				rgba[i*Width()*4+j*4+3] = a2;
		}

		// switch
		delete []rgb;
		rgb = rgba;

		return true;
	}
	return false;
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

	isRGBA = false;
	numBytes = 3;

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


byte* TTexture::LoadPaletteBitmap( TString filename, size_t& width, size_t& height,
								   size_t& bytes, TString& _error )
{
	HBITMAP hbmp = (HBITMAP)::LoadImage( 0, filename.c_str(), IMAGE_BITMAP, 0,0, 
					LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	if ( hbmp==NULL )
	{
		_error = "Error: loading paletted bitmap #" + Int2Str(GetLastError()) + "\n";
		return NULL;
	}

	BITMAP* info = new BITMAP;
	ZeroMemory( info, sizeof(BITMAP) );
	GetObject( hbmp, sizeof(BITMAP), info );

	size_t w = info->bmWidth;
	size_t h = info->bmHeight;
	if ( !(w==1 || w==2 || w==4 || w==8 || w==16 || w==32 || w==64 || w==128 || w==256 ||
		 w==512 || w==1024 || w==2048 || w==8192 || w==16384) )
    {
    	_error = "width not a power of 2";
		delete info;
		return NULL;
    }
	if ( !(h==1 || h==2 || h==4 || h==8 || h==16 || h==32 || h==64 || h==128 || h==256 ||
		   h==512 || h==1024 || h==2048 || h==8192 || h==16384) )
    {
    	_error = "height not a power of 2";
		delete info;
		return NULL;
    }
	//
	//	Now that we have all the header info read in, allocate memory for the
	//	bitmap and read *it* in...
	//
	bytes = (info->bmBitsPixel / 8);
	if ( bytes!=1 )
	{
    	_error = "palette bitmap must be 256 colours";
		delete info;
		return NULL;
	};
	size_t bitsize = w * h * bytes;
    byte* rgb = new byte[bitsize];
	if (  rgb==NULL )
	{
		//
		// Couldn't allocate memory - return NULL!
		//
    	_error = "out of memory allocating RGB map";
		delete info;
	    return NULL;
	}

	ZeroMemory( rgb, bitsize );
	memcpy( rgb, info->bmBits, bitsize );

	width = w;
	height = h;

	FlipUpsideDown(w,h,bytes,rgb);
	delete info;
	return rgb;
};


//
//	loadBitmap() load a DIB/BMP file from disk.
//
//	Returns a pointer to the bitmap if successful, NULL otherwise...
//
byte* TTexture::LoadBitmap( TString filename, size_t& width, size_t& height,
						    size_t& bytes, TString& _error )
{
	FILE	*fp;				// Open file pointer
	size_t	bitsize, infosize;	// Size of bitmap and header information
	BITMAPFILEHEADER header;	// File header
	size_t y;

	//
	// Try opening the file; use "rb" mode to read this *binary* file.
	//
    _error = "";
	if ((fp = fopen(filename.c_str(), "rb")) == NULL)
    {
    	_error = "can't open file";
		return NULL;
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
		return NULL;
	}

	if (header.bfType != 'MB')	// Check for BM
	{
		//
		// Not a bitmap file - return NULL...
		//
    	_error = "not a bitmap";
		fclose(fp);
		return NULL;
	}

	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
	if ((info = (BITMAPINFO*)new byte[infosize+sizeof(BITMAPINFO)]) == NULL)
	{
		//
		// Couldn't allocate memory for bitmap info - return NULL...
		//
    	 _error = "out of memory";
		fclose(fp);
		return NULL;
	}

	if (fread(info, 1, infosize, fp) < infosize)
	{
		//
		// Couldn't read the bitmap header - return NULL...
		//
    	_error = "error reading bitmap header";
	    delete info;
	    fclose(fp);
		return NULL;
	}

	//
	//	Make sure its a 24 bit bitmap - otherwise load palette bitmap
	//
	if ( info->bmiHeader.biBitCount!=24 )
	{
		delete info;
		info = NULL;
	    fclose(fp);
		return LoadPaletteBitmap( filename, width, height, bytes, _error );
	}
	bytes = 3;

	//
	//	Make sure width and height are powers of 2 (openGL requirement!)
	//
	size_t w = info->bmiHeader.biWidth;
	size_t h = info->bmiHeader.biHeight;
	if ( !(w==1 || w==2 || w==4 || w==8 || w==16 || w==32 || w==64 || w==128 || w==256 ||
		 w==512 || w==1024 || w==2048 || w==8192 || w==16384) )
    {
		delete info;
    	_error = "width not a power of 2";
		return NULL;
    }
	if ( !(h==1 || h==2 || h==4 || h==8 || h==16 || h==32 || h==64 || h==128 || h==256 ||
		   h==512 || h==1024 || h==2048 || h==8192 || h==16384) )
    {
		delete info;
    	_error = "height not a power of 2";
		return NULL;
    }

	//
	//	Now that we have all the header info read in, allocate memory for the
	//	bitmap and read *it* in...
	//
	if ((bitsize = info->bmiHeader.biSizeImage) == 0)
		 bitsize = (info->bmiHeader.biWidth * info->bmiHeader.biBitCount + 7) / 8 * abs(info->bmiHeader.biHeight);

    byte* rgb = new byte[bitsize];
	if (  rgb==NULL )
	{
		//
		// Couldn't allocate memory - return NULL!
		//
    	_error = "out of memory allocating RGB map";
	    delete info;
	    fclose(fp);
	    return NULL;
	}

	if (fread((void *)rgb, 1, bitsize, fp) < bitsize)
	{
		//
		// Couldn't read bitmap - free memory and return NULL!
		//
    	_error = "error reading rgb part of bitmap";
	    delete info;
	    delete []rgb;
        rgb = NULL;
		fclose(fp);
		return NULL;
	}

	fclose(fp);

	byte *s1;
	byte t;

	width = info->bmiHeader.biWidth;
	height = info->bmiHeader.biHeight;

	//
	//	Swap R & Bs
	//
    byte* p = rgb;
	for ( y=0; y<h; y++ )
	{
		s1 = &p[y*w*3];
		for ( size_t x=0; x<w; x++ )
		{
			t = s1[x*3];
			s1[x*3] = s1[x*3+2];
			s1[x*3+2] = t;
		}
	}

	FlipUpsideDown(width,height,bytes,rgb);
	if ( info!=NULL )
		delete info;

	return rgb;
}


//
//	loadBitmap() load a DIB/BMP mask file from disk.
//
byte* TTexture::LoadBitmapMask( TString filename, size_t& width, size_t& height,
							    size_t& bytes, TString& _error )
{
	byte* rgb = LoadPaletteBitmap( filename, width, height, bytes, _error );
	return rgb;
}


void TTexture::FlipUpsideDown( size_t w, size_t h, size_t bytes, byte* rgb )
{
	if ( rgb!=NULL )
	{
		//
		//	turn the RGB upside down
		//
	    byte* p = rgb;
		byte* tempp = new byte[w*bytes];
		if ( tempp!=NULL )
		{
			byte *s1,*s2;
			for ( size_t y=0; y<h/2; y++ )
			{
				s1 = &p[y*w*bytes];
				memcpy(tempp,s1,w*bytes);
				s2 = &p[((h-1)-y)*w*bytes];
				memcpy(s1,s2,w*bytes);
				memcpy(s2,tempp,w*bytes);
			}
			delete tempp;
		}
	}
};


//
//	OpenGL setup for TTexture
//
void TTexture::StartTexture( void ) const
{
	glEnable(GL_TEXTURE_2D);
	if ( isRGBA )
	{
		glEnable( GL_BLEND );
	}

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
void TTexture::EndTexture( void ) const
{
	glDisable(GL_TEXTURE_2D);
	if ( isRGBA )
	{
		glDisable( GL_BLEND );
	}
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


void TTexture::DrawCCW( float x1, float y1, float x2, float y2 ) const
{
	glColor3ub(255,255,255);
	StartTexture();
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glNormal3f(0,0,-1);
		glVertex3f(x2,y1,0);

		glTexCoord2f(0,1);
		glNormal3f(0,0,-1);
		glVertex3f(x2,y2,0);

		glTexCoord2f(1,1);
		glNormal3f(0,0,-1);
		glVertex3f(x1,y2,0);

		glTexCoord2f(1,0);
		glNormal3f(0,0,-1);
		glVertex3f(x1,y1,0);
	glEnd();
	EndTexture();
}


void TTexture::Draw( float x1, float y1, float x2, float y2,
					 float u1, float v1, float u2, float v2 ) const
{
	glColor3ub(255,255,255);
	StartTexture();
	glBegin(GL_QUADS);
		glTexCoord2f(u2,v1);
		glNormal3f(0,0,-1);
		glVertex3f(x1,y1,0);

		glTexCoord2f(u2,v2);
		glNormal3f(0,0,-1);
		glVertex3f(x1,y2,0);

		glTexCoord2f(u1,v2);
		glNormal3f(0,0,-1);
		glVertex3f(x2,y2,0);

		glTexCoord2f(u1,v1);
		glNormal3f(0,0,-1);
		glVertex3f(x2,y1,0);
	glEnd();
	EndTexture();
}


void TTexture::DrawCCW( float x1, float y1, float x2, float y2,
						float u1, float v1, float u2, float v2 ) const
{
	glColor3ub(255,255,255);
	StartTexture();
	glBegin(GL_QUADS);
		glTexCoord2f(u2,v2);
		glNormal3f(0,0,1);
		glVertex3f(x2,y1,0);

		glTexCoord2f(u1,v2);
		glNormal3f(0,0,1);
		glVertex3f(x1,y1,0);

		glTexCoord2f(u1,v1);
		glNormal3f(0,0,1);
		glVertex3f(x1,y2,0);

		glTexCoord2f(u2,v1);
		glNormal3f(0,0,1);
		glVertex3f(x2,y2,0);
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
	TString item2;
	item2 = filename.GetItem(',',1);
	if ( item2.length()>0 )
		return false;

	int len = filename.length();
    if ( len<4 )
    	return false;

    TString temp = filename.lcase();
    if ( temp[len-3]=='b' && temp[len-2]=='m' && temp[len-1]=='p' )
    	return true;
    return false;
};


bool TTexture::isJPEG( const TString& filename )
{
	TString item2;
	item2 = filename.GetItem(',',1);
	if ( item2.length()>0 )
		return false;

	int len = filename.length();
    if ( len<4 )
    	return false;

    TString temp = filename.lcase();
    if ( temp[len-3]=='j' && temp[len-2]=='p' && temp[len-1]=='g' )
    	return true;
    return false;
};


bool TTexture::isTGA( const TString& filename )
{
	TString item2;
	item2 = filename.GetItem(',',1);
	if ( item2.length()>0 )
		return false;

	int len = filename.length();
    if ( len<4 )
    	return false;

    TString temp = filename.lcase();
    if ( temp[len-3]=='t' && temp[len-2]=='g' && temp[len-1]=='a' )
    	return true;
    return false;
};


bool TTexture::isPNG( const TString& filename )
{
	TString item;
	item = filename.GetItem('.',1);
	if ( item.length()>0 )
	{
		if ( item.lcase() == "png" )
			return true;
	}
	return false;
};


bool TTexture::isMaskedBitmap( const TString& filename )
{
	TString item1, item2;
	item1 = filename.GetItem(',',0);
	item2 = filename.GetItem(',',1);
	if ( item2.length()>0 )
	{
		if ( (isBitmap(item1) || isJPEG(item1) || isPNG(item1)) && isBitmap(item2) )
		{
			return true;
		}
	}
	return false;
};


byte* TTexture::LoadJpeg( TString filename, size_t& width, size_t& height, 
						  size_t& bytes, TString& _error )
{
	TJpeg jpeg;

	char tempError[256];
	tempError[0] = 0;

	if ( !jpeg.Load( filename.c_str(), tempError ) )
	{
		_error = tempError;
   		return NULL;
	}

	int w = jpeg.Width();
	int h = jpeg.Height();
	if ( !(w==1 || w==2 || w==4 || w==8 || w==16 || w==32 || w==64 || w==128 || w==256 ||
		 w==512 || w==1024 || w==2048 || w==8192 || w==16384) )
    {
    	_error = "width not a power of 2";
		return NULL;
    }
	if ( !(h==1 || h==2 || h==4 || h==8 || h==16 || h==32 || h==64 || h==128 || h==256 ||
		   h==512 || h==1024 || h==2048 || h==8192 || h==16384) )
    {
    	_error = "height not a power of 2";
		return NULL;
    }

    // set width & height
	width = jpeg.Width();
	height = jpeg.Height();
	bytes = 3;

    // allocate and copy bitmap
    int rgbSize = width * height * 3;
	byte* tempp = new byte[rgbSize];
    if ( tempp==NULL )
    {
    	_error = "error allocating JPEG memory";
		return NULL;
    }

    // copy memory
    memcpy( tempp, jpeg.Bits(), rgbSize );

	//
	//	Swap R & Bs
	//
    byte* p = tempp;
	for ( size_t y=0; y<height; y++ )
	{
		byte* s1 = &p[y*width*bytes];
		for ( size_t x=0; x<width; x++ )
		{
			byte t = s1[x*bytes];
			s1[x*3] = s1[x*bytes+2];
			s1[x*3+2] = t;
		}
	}
	return tempp;
};


byte* TTexture::LoadPNG( TString filename, size_t& width, size_t& height,
						 size_t& bytes, TString& _error )
{
	TPNG png;

	char tempError[256];
	tempError[0] = 0;

	if ( !png.Load( filename.c_str(), tempError ) )
	{
		_error = tempError;
   		return NULL;
	}

	int w = png.Width();
	int h = png.Height();
	if ( !(w==1 || w==2 || w==4 || w==8 || w==16 || w==32 || w==64 || w==128 || w==256 ||
		 w==512 || w==1024 || w==2048 || w==8192 || w==16384) )
    {
    	_error = "width not a power of 2";
		return NULL;
    }
	if ( !(h==1 || h==2 || h==4 || h==8 || h==16 || h==32 || h==64 || h==128 || h==256 ||
		   h==512 || h==1024 || h==2048 || h==8192 || h==16384) )
    {
    	_error = "height not a power of 2";
		return NULL;
    }

	bytes = 3;
    // allocate and copy bitmap
    int rgbSize = h*w*bytes;
	byte *tempp = new byte[rgbSize];
    if ( tempp==NULL )
    {
    	_error = "error allocating PNG memory";
		return NULL;
    }

    // copy memory
    memcpy( tempp, png.Bits(), rgbSize );

	width = size_t(w);
	height = size_t(h);

	//
	//	Swap R & Bs
	//
    byte* p = tempp;
	for ( int y=0; y<h; y++ )
	{
		byte* s1 = &p[y*w*bytes];
		for ( int x=0; x<w; x++ )
		{
			byte t = s1[x*bytes];
			s1[x*3] = s1[x*bytes+2];
			s1[x*3+2] = t;
		}
	}
	return tempp;
};


byte* TTexture::LoadJpeg( HINSTANCE hInstance, int resource, 
						  size_t& width, size_t& height, size_t& bytes,
						  TString& _error )
{
	TJpeg jpeg;

	char tempError[256];
	tempError[0] = 0;

	if ( !jpeg.Load( hInstance, MAKEINTRESOURCE( resource ), tempError ) )
	{
		_error = tempError;
   		return NULL;
	}

	int w = jpeg.Width();
	int h = jpeg.Height();
	bytes = 3;
	if ( !(w==1 || w==2 || w==4 || w==8 || w==16 || w==32 || w==64 || w==128 || w==256 ||
		 w==512 || w==1024 || w==2048 || w==8192 || w==16384) )
    {
    	_error = "width not a power of 2";
		return NULL;
    }
	if ( !(h==1 || h==2 || h==4 || h==8 || h==16 || h==32 || h==64 || h==128 || h==256 ||
		   h==512 || h==1024 || h==2048 || h==8192 || h==16384) )
    {
    	_error = "height not a power of 2";
		return NULL;
    }

    // allocate and copy bitmap
    int rgbSize = h*w*3;
	byte *tempp = new byte[rgbSize];
    if ( tempp==NULL )
    {
    	_error = "error allocating JPEG memory";
		return false;
    }
    // copy memory
    memcpy( tempp, jpeg.Bits(), rgbSize );

	//
	//	Swap R & Bs
	//
    byte* p = tempp;
	for ( int y=0; y<h; y++ )
	{
		byte* s1 = &p[y*w*bytes];
		for ( int x=0; x<w; x++ )
		{
			byte t = s1[x*bytes];
			s1[x*3] = s1[x*bytes+2];
			s1[x*3+2] = t;
		}
	}
	return tempp;
};


byte* TTexture::LoadTGA( const TString& filename, size_t& width, size_t& height, 
						 size_t& bytes, TString& _error )
{
	TGA tga;

	if ( !tga.LoadBinary( filename, _error ) )
	{
   		return NULL;
	}

	size_t w = tga.Width();
	size_t h = tga.Height();
	if ( !(w==1 || w==2 || w==4 || w==8 || w==16 || w==32 || w==64 || w==128 || w==256 ||
		 w==512 || w==1024 || w==2048 || w==8192 || w==16384) )
    {
    	_error = "width not a power of 2";
		return NULL;
    }
	if ( !(h==1 || h==2 || h==4 || h==8 || h==16 || h==32 || h==64 || h==128 || h==256 ||
		   h==512 || h==1024 || h==2048 || h==8192 || h==16384) )
    {
    	_error = "height not a power of 2";
		return NULL;
    }

	bytes = 3;
	if ( tga.isRGBA() )
	{
		bytes = 4;
	}
	width = w;
	height = h;

    // allocate and copy bitmap
    size_t rgbSize = tga.Size();
	byte *tempp = new byte[rgbSize];
    if ( tempp==NULL )
    {
    	_error = "error allocating TGA memory";
		return false;
    }

    // copy memory
    memcpy( tempp, tga.Rgb(), rgbSize );
	return tempp;
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


bool TTexture::isAnimation( const TString& _filename, TString& _format, int& type, 
							size_t& start, size_t& end, size_t& fps, 
							bool& error, TString& errStr )
{
	char format[256];

	// strip any potential path off the filename in the first place
	TString filename = _filename;
	size_t len = filename.length();
	while ( len>0 && filename[len]!='\\' ) len--;
	if ( filename[len]=='\\' )
	{
		filename = &_filename[len+1];
	}

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


void TTexture::DontDestroyRGB( bool ddr )
{
	dontDestroyRGB = ddr;
};


void TTexture::MirrorHorizontally( void )
{
	if ( rgb==NULL )
		return;

	byte* tempp = new byte[Width()*3];
	byte* p = rgb;
	for ( int y=0; y<Height()/2; y++ )
	{
		byte* s1 = &p[y*Width()*3];
		memcpy(tempp,s1,Width()*3);
		byte* s2 = &p[((Height()-1)-y)*Width()*3];
		memcpy(s1,s2,Width()*3);
		memcpy(s2,tempp,Width()*3);
	}
	delete tempp;
};


void TTexture::MirrorVertically( void )
{
	if ( rgb==NULL )
		return;

	byte* p = rgb;
	for ( int y=0; y<Height(); y++ )
	for ( int x=0; x<Width()/2; x++ )
	{
		byte* s1 = &p[y*Width()*3+x*3];
		byte* s2 = &p[y*Width()*3+((Width()-1)-x)*3];

		byte t1 = s1[0];
		byte t2 = s1[1];
		byte t3 = s1[2];

		s1[0] = s2[0];
		s1[1] = s2[1];
		s1[2] = s2[2];

		s2[0] = t1;
		s2[1] = t2;
		s2[2] = t3;
	}
};


const BITMAPINFOHEADER& TTexture::BitmapInfoHeader( void ) const
{
    //
    // set up a bitmap header, internal use for blitting
    //
    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiHeader.biWidth = Width();
    bmiHeader.biHeight = Height();
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 24;
    bmiHeader.biCompression = BI_RGB;
    bmiHeader.biSizeImage = 0;
    bmiHeader.biXPelsPerMeter = 0;
    bmiHeader.biYPelsPerMeter = 0;
    bmiHeader.biClrUsed = 0;
    bmiHeader.biClrImportant = 0;

	return bmiHeader;
};


void TTexture::Draw( const HDC& dc, const RECT& rect ) const
{
	// if we don't have an image, get out of here
	if ( rgb==NULL )
    	return;

	if (dc!=NULL)
    {
		::StretchDIBits(dc,
        			  rect.left, rect.top,
                      rect.right - rect.left, rect.bottom - rect.top,
					  0,0,
					  Width(),
					  Height(),
					  rgb,
					  (LPBITMAPINFO)&BitmapInfoHeader(),
					  DIB_RGB_COLORS,
					  SRCCOPY );
	}
}

void TTexture::IsRGBA( bool _rgba )
{
	isRGBA = _rgba;
};

bool TTexture::IsRGBA( void ) const
{
	return isRGBA;
};


void TTexture::NumBytes( int nb )
{
	numBytes = nb;
};

int TTexture::NumBytes( void ) const
{
	return numBytes;
};

// force a texture to destroy itself and reload
// for resolution changes
bool TTexture::Reload( TString& errStr )
{
	TString fname,path;

	PreCond( filename.length()>0 );

	size_t start, end, frequency;
	int type;
	TString fileNameFormat;
	bool error = false;
	if ( isAnimation( filename, fileNameFormat, type, start, end, frequency, error, errStr ) )
	{
		fname = filename;
		path = "data\\textures";
	}
	else
	{
		SplitPath( filename, path, fname );
		if ( path.length()==0 )
		{
			path = "data\\textures";
		}
	}

	// previous data will be cleared by LoadBinary()
	return LoadBinary( fname, path, errStr );
};

//==========================================================================

