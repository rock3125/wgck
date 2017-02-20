#ifndef __TEXTURE_H__
#define __TEXTURE_H__

//==========================================================================

class TTexture
{
public:
	// constructors, with filename & mipmap options
	TTexture( void );
	virtual ~TTexture( void );

	TTexture( const TTexture& );
	const TTexture& operator=( const TTexture& );

    // draw the TTexture in a rect (cf. decal)
	void	Draw( float x1, float y1, float x2, float y2 ) const;
	void	DrawColour( float x1, float y1, float x2, float y2,
						unsigned char r, unsigned char g, unsigned char b ) const;

	bool	LoadBinary( const TString& fname, const TString& path, 
						TString& errStr, bool mipmap=true );
	
	bool	LoadRGBAJPEG( const TString& fname, 
						  const TString& path, 
						  TString& errStr, 
						  unsigned char a );

	bool	LoadRGBAJPEG( int resourceId,
						  TString& errStr, 
						  unsigned char a );

	void	ClearData( void );

    // property access
    void	Width( int w );
    int		Width( void ) const;

	const TString& Name( void ) const;

    void	Height( int h );
    int		Height( void ) const;

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
	static void		EndTexture( void );

private:
	void			Init( void );
	size_t			SetupRGB( bool mipmapped );
	size_t			SetupRGBA( unsigned char a );
    bool			isBitmap( const TString& filename );
	bool			isAnimation( const TString& filename, TString& format, int& type, size_t& start, 
								 size_t& end, size_t& fps, bool& error, TString& errStr );
	bool			LoadBitmap( const TString& filename, TString& _error );
	bool			LoadJpeg( const TString& filename, TString& _error );
	bool			LoadJpeg( int resourceId, TString& _error );

private:
	bool			used;
	bool			fromCache;
	bool			mipmapped;
	int				width;
	int				height;
	size_t*			texture_id;
	unsigned char*	rgb;

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

	TString			filename;

	BITMAPINFO*		info; // raw bitmap structure - os dependent
};

//==========================================================================

#endif


