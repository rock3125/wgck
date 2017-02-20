#include <precomp_header.h>

#include <win32/win32.h>
#include <common/sounds.h>

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.295779511273f;

//==========================================================================

TSound::TSound( HWND _hwnd, char* errStr )
	: dxSound( NULL ),
      hSound( NULL ),
      soundCntr(0),
      hwnd( _hwnd )
{
	PreCond( hwnd!=NULL );

	WriteString( "setting up 3D Sound System\n" );

    dxSound = InitDirectSound( hwnd, 2, 22050, 16 );
	if ( dxSound == NULL )
    {
    	if ( errStr!=NULL )
	    	strcpy( errStr, "TSound: InitDirectSound(), error initialising 3D sound system" );
        return;
    }
};


TSound::~TSound( void )
{
	FreeDirectSound( dxSound );
    dxSound = NULL;

#ifndef _MSVC
	if ( hSound!=NULL )
		FreeLibrary( hSound );
    hSound = NULL;
#endif
};


bool TSound::Load( const char* fname, float minDist, float maxDist )
{
	if ( dxSound==NULL )
    	return false;

    char buf[256];
    sprintf( buf, "loading %s\n", fname );
    WriteString( buf );

    if ( !LoadWaveFile( fname, soundCntr, dxSound ) )
        return false;

    SetParameters( 1,1,minDist,maxDist, soundCntr, dxSound );

    // keep track of number of sounds
    soundCntr++;

    return true;
};


void TSound::SetLocation( int soundIndex, float listenerAngle,
						  float dx, float dy, float dz,
                          float velX, float velY, float velZ )
{
	if ( dxSound!=NULL && soundIndex<soundCntr )
    {
	    float yangle = float((listenerAngle+90) * degToRad);
        float v1fcos = float(cos(yangle));
        float v1fsin = float(sin(yangle));

     	dx *= -v1fsin;
        dz *= v1fcos;
	    SetObjectProperties( dx,dy,dz, velX,velY,velZ, soundIndex, dxSound );
    }
};


void TSound::Stop( int soundIndex )
{
	if ( dxSound==NULL )
    	return;

    if ( IsBufferPlaying( soundIndex, dxSound ) )
		StopBuffer( false, soundIndex, dxSound );
};


void TSound::Play( int soundIndex, bool looped )
{
	if ( dxSound==NULL )
    	return;

    if ( IsBufferPlaying( soundIndex, dxSound ) )
		StopBuffer( false, soundIndex, dxSound );

	PlayBuffer( looped, soundIndex, dxSound );
};


bool TSound::IsPlaying( int soundIndex )
{
	if ( dxSound==NULL )
    	return false;

    return IsBufferPlaying( soundIndex, dxSound );
};

//==========================================================================


