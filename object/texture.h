#ifndef __TEXTURE_H__
#define __TEXTURE_H__

//==========================================================================

enum textureType
{
	ttNone,
	ttRGB,
	ttRGBA,
	ttAnimation
};

class _EXPORT TTexture
{
public:
	// constructors, with filename & mipmap options
	TTexture( void );
	virtual ~TTexture( void );

	TTexture( const TTexture& );
	const TTexture& operator=( const TTexture& );

	// clears the texture cache and destroys all textures in memory!
	static void ClearCache( void );

    // draw the TTexture in a rect (cf. decal)
	void	Draw( float x1, float y1, float x2, float y2 ) const;
	void	Draw( float x1, float y1, float x2, float y2,
				  float u1, float v1, float u2, float v2 ) const;
	void	DrawCCW( float x1, float y1, float x2, float y2 ) const;
	void	DrawCCW( float x1, float y1, float x2, float y2,
					 float u1, float v1, float u2, float v2 ) const;

	void	DrawColour( float x1, float y1, float x2, float y2,
						unsigned char r, unsigned char g, unsigned char b ) const;

	// reload an existing texture - used for res changes
	bool	Reload( TString& errStr );

	bool	LoadBinary( TString fname, TString path, 
						TString& errStr, bool mipmap=true );
	
	bool	LoadBinary( TString fname, 
						TString path, 
						TString& errStr, 
						unsigned char r,
						unsigned char g,
						unsigned char b,
						unsigned char a1,
						unsigned char a2,
						unsigned char threshold,
						bool mipmap = true );

	bool	LoadBinary( TString fname, 
						TString path, 
						TString& errStr, 
						unsigned char a,
						bool mipmap );

	bool	LoadRGBAJPEG( TString fname, 
						  TString path, 
						  TString& errStr, 
						  unsigned char a,
						  bool mipmapped=true );

	bool	LoadRGBAJPEG( TString fname, 
						  TString path, 
						  TString& errStr, 
						  unsigned char r,
						  unsigned char g,
						  unsigned char b,
						  unsigned char a1,
						  unsigned char a2,
						  unsigned char threshold,
						  bool mipmapped=true );

	bool	LoadRGBAJPEG( HINSTANCE hInstance, int resourceId,
						  TString& errStr, 
						  unsigned char a,
						  bool mipmapped=true );

	bool	LoadRGBAJPEG( HINSTANCE hInstance, int resourceId,
						  TString& errStr, 
						  unsigned char r,
						  unsigned char g,
						  unsigned char b,
						  unsigned char a1,
						  unsigned char a2,
						  unsigned char threshold,
						  bool mipmapped=true );

	void	ClearData( void );

	// transformations on RGB
	void MirrorHorizontally( void );
	void MirrorVertically( void );

    // property access
    void	Width( int w );
    int		Width( void ) const;

	const TString& Name( void ) const;

    void	Height( int h );
    int		Height( void ) const;

    void	NumBytes( int nb );
    int		NumBytes( void ) const;

    const unsigned char*	Rgb( void ) const; // RGB data
    void					Rgb( const unsigned char* rgb );

    void	Used( bool u ); // got a bitmap that is valid?
    bool	Used( void ) const;

	// animation set for textures
    bool	Animated( void ) const;
	void	PauseAnimation( bool tf );

	int		AnimationType( void ) const;
	void	AnimationType( int _type );
	
	size_t	NextAnimation( void ) const;
	void	NextAnimation( size_t next );

	size_t	AnimationCount( void ) const;

	long	ObjectSize( void ) const;

	void	AnimationFPS( size_t anim );
	size_t	AnimationFPS( void ) const;

    void	TextureId( int tid ); // gl TTexture id
    int		TextureId( void ) const;

    void	Mipmapped( bool mm );
    bool	Mipmapped( void ) const;

	void			StartTexture( void ) const;
	void			EndTexture( void ) const;

	void			DontDestroyRGB( bool ddr );

	const BITMAPINFOHEADER& BitmapInfoHeader( void ) const;
	void Draw( const HDC& dc, const RECT& rect ) const;

	// is this texture an RGBA texture (does it need blending?)
	void IsRGBA( bool _rgba );
	bool IsRGBA( void ) const;

private:
	void			Init( void );

	// setup RGB and RGBA with mask and RGB
	size_t			SetupRGB( byte* r, size_t w, size_t h, bool mipmapped ); // normal RGB
	size_t			SetupRGBA( byte* r, size_t w, size_t h, bool mipmapped ); // TGA RGBA
	size_t			SetupRGBA( byte* r, byte* m, size_t w, size_t h, bool mipmapped ); // masked RGBA
	size_t			SetupRGBA( byte* rgb, size_t w, size_t h, bool mipmapped,
							   byte r, byte g, byte b, byte a1, byte a2, byte threshold );
	size_t			SetupRGBA( byte* rgb, size_t w, size_t h, bool mipmapped, byte a );

    bool			isBitmap( const TString& filename );
    bool			isMaskedBitmap( const TString& filename );
	bool			isJPEG( const TString& filename );
	bool			isPNG( const TString& filename );
	bool			isTGA( const TString& filename );
	bool			isAnimation( const TString& filename, TString& format, int& type, size_t& start, 
								 size_t& end, size_t& fps, bool& error, TString& errStr );

	bool			RGB2RGBA( byte* rgb, size_t w, size_t h, byte r,
							  byte g, byte b, byte a1, byte a2, byte threshold );

	size_t			LoadGraphic( const TString& fname, const TString& path, 
								 size_t*& textureId, TString& errStr, 
								 bool mipmapped );

	byte*			LoadGraphic( const TString& filename, size_t& width, size_t& height,
								 size_t& bytes, TString& errStr );

	byte*			LoadBitmapMask( TString filename, size_t& width, size_t& height,
									size_t& bytes, TString& _error );
	byte*			LoadBitmap( TString filename, size_t& width, size_t& height,
								size_t& bytes, TString& _error );
	byte*			LoadPaletteBitmap( TString filename, size_t& width, size_t& height,
									   size_t& bytes, TString& _error );
	byte*			LoadJpeg( TString filename, size_t& width, size_t& height, 
							  size_t& bytes, TString& _error );
	byte*			LoadPNG( TString filename, size_t& width, size_t& height,
							 size_t& bytes, TString& _error );
	byte*			LoadJpeg( HINSTANCE hInstance, int resource, 
							  size_t& width, size_t& height, size_t& bytes, 
							  TString& _error );
	byte*			LoadTGA( const TString& filename, size_t& width, size_t& height, 
							 size_t& bytes, TString& _error );

	// flip the RGB upside down
	void			FlipUpsideDown( size_t w, size_t h, size_t bytes, byte* rgb );

private:
	friend class TextureCache;

	bool			used;
	bool			fromCache;
	bool			mipmapped;
	int				width;
	int				height;
	int				numBytes;
	size_t*			texture_id;
	unsigned char*	rgb;

	bool			isRGBA;

	// anim vars
	bool			animated;
	int				animationType;
	mutable int		animationDirection;
	size_t			animationCount;
	mutable size_t	animationIndex;
	size_t			animationFPS;
	long			animationMS;
	mutable long	animationTickCount;
	bool			animationPaused;

	bool			dontDestroyRGB;

	TString			filename;

    mutable BITMAPINFOHEADER bmiHeader;
	BITMAPINFO*		 info; // raw bitmap structure - os dependent
};

//==========================================================================

#endif


