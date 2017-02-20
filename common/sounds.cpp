#include <precomp_header.h>

#include <win32/win32.h>
#include <common/sounds.h>

#include <objbase.h>
#include <initguid.h>
#include <mmreg.h>

#include <object/geometry.h>

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.295779511273f;

//==========================================================================

#ifdef _USEFMOD

TSound::TSound( HWND _hwnd )
	: hwnd( _hwnd ),
	  stream(NULL)
{
	PreCond( hwnd!=NULL );

	WriteString( "setting up 3D Sound System\n" );

	streamChannel = -1;
	streamVolume = 128;

	soundCntr = 0;
	soundVolume = 255;
	for ( size_t i=0; i<kMaxSamples; i++ )
	{
		samples[i] = NULL;
	}

	TString errStr;
	initialised = ::InitFMOD( errStr );
	if ( initialised )
	{
		WriteString( "done\n\n" );
	}
	else
	{
		WriteString( errStr.c_str() );
	}
	streamPos = 0;
};

TSound::~TSound( void )
{
	::CloseFMOD();
};

// returns -1 on failure
int TSound::Load( const TString& fname, bool looped, TString& errStr )
{
	FSOUND_SAMPLE* samp = ::LoadSample( fname, looped, errStr );
	if ( samp==NULL )
	{
		return -1;
	}
	else
	{
		samples[soundCntr++] = samp;
		return (soundCntr-1);
	}
};

void TSound::SetLocation( int channel, const TPoint& pos, const TPoint& prevPos )
{
	if ( channel!=-1 )
		::UpdateSound( channel, pos, prevPos );
};

void TSound::SetCameraLocation( const TPoint& pos, const TPoint& fwd, 
							    const TPoint& up )
{
	::SetCamera( pos, fwd, up );
};

int TSound::Play( size_t soundIndex, const TPoint& pos )
{
	if ( soundIndex < soundCntr )
	{
		FSOUND_SAMPLE* samp = samples[soundIndex];
		int channel = ::PlaySound( samp, pos );
		if ( channel!=-1 )
			::SetVolume( channel, soundVolume );
		return channel;
	}
	return -1;
};

void TSound::Stop( int channel )
{
	if ( channel!=-1 && channel!=streamChannel )
		::StopSound( channel );
};

bool TSound::IsPlaying( int channel )
{
	if ( channel!=-1 )
		return false;
	else
		return ::SoundIsPlaying(channel);
};


bool TSound::Initialised( void ) const
{
	return initialised;
};

void TSound::Volume( byte vol )
{
	soundVolume = vol;
};

void TSound::Volume( int channel, byte vol )
{
	::SetVolume( channel, vol );
};

bool TSound::OpenStream( const TString& fname )
{
	if ( stream!=NULL )
	{
		if ( IsStreamPlaying() )
		{
			StopStream();
		}
		CloseStream();
		stream = NULL;
	}
	streamFname = fname;
	stream = ::OpenStream( fname );
	return (stream!=NULL);
};

bool TSound::PlayStream( void )
{
	if ( stream!=NULL )
	{
		streamChannel = ::PlayStream( stream );
		StreamVolume( streamVolume );
		return (streamChannel!=-1);
	}
	else
	{
		streamChannel = -1;
	}
	return false;
};

bool TSound::StopStream( void )
{
	if ( stream!=NULL )
	{
		if ( streamChannel!=-1 )
		{
			streamChannel = -1;
			return ::StopStream( stream );
		}
	}
	return false;
};


void TSound::PauseStream( void )
{
	if ( stream!=NULL )
	{
		if ( streamChannel!=-1 )
		{
			pauseFname = streamFname;
			streamPos = ::FSOUND_Stream_GetPosition( stream );
		}
	}
	StopStream();
};


void TSound::ResumeStream( void )
{
	StopStream();
	if ( pauseFname.length()>0 )
	{
		if ( OpenStream( pauseFname ) )
		{
			::FSOUND_Stream_SetPosition( stream, streamPos );
			PlayStream();
		}
	}
};


bool TSound::CloseStream( void )
{
	if ( stream!=NULL )
	{
		StopStream();
		bool success = ::CloseStream( stream );
		stream = NULL;
		return success;
	}
	return false;
};

bool TSound::IsStreamPlaying( void )
{
	if ( stream!=NULL )
		return IsPlaying( streamChannel );
	return false;
};

void TSound::StreamVolume( byte vol )
{
	if ( streamChannel!=-1 )
		::SetVolume( streamChannel, vol );
	streamVolume = vol;
};

#else

//==========================================================================

TSound::TSound( HWND _hwnd, char* errStr )
	: dxSound( NULL ),
      hSound( NULL ),
      soundCntr(0),
      hwnd( _hwnd )
{
	PreCond( hwnd!=NULL );

	minDistance = 0;
	maxDistance = 0;

	WriteString( "setting up 3D Sound System\n" );

    dxSound = InitDirectSound( hwnd, 2, 22050, 16 );
	if ( dxSound == NULL )
    {
    	if ( errStr!=NULL )
	    	strcpy( errStr, "TSound: InitDirectSound(), error initialising 3D sound system" );
        return;
    }
	WriteString( "done\n\n" );
};


TSound::~TSound( void )
{
	if ( dxSound!=NULL )
	{
		FreeDirectSound( dxSound );
	}
    dxSound = NULL;
};


bool TSound::Initialised( void ) const
{
	return (dxSound!=NULL);
}


int TSound::Load( const char* fname, float minDist, float maxDist )
{
	if ( dxSound==NULL )
    	return false;

	minDistance = minDist;
	maxDistance = maxDist;

    char buf[256];
    sprintf( buf, "loading %s\n", fname );
    WriteString( buf );

    if ( !LoadWaveFile( fname, soundCntr, dxSound ) )
        return -1;

#ifdef _USE3DSOUND_
    ::SetParameters( 1,1,minDist,maxDist, soundCntr, dxSound );
#endif

    // keep track of number of sounds
    return soundCntr++;
};

void TSound::SetParameters( int soundIndex, 
						    float fDopplerFactor, float fRolloffFactor,
							float fMinDistance,   float fMaxDistance )
{
	minDistance = fMinDistance;
	maxDistance = fMaxDistance;

	if ( dxSound!=NULL && soundIndex<soundCntr )
    {
#ifdef _USE3DSOUND_
		::SetParameters( fDopplerFactor, fRolloffFactor, fMinDistance, fMaxDistance,
					     soundIndex, dxSound );
#endif
    }
};

void TSound::SetLocation( int soundIndex, 
						  float lx, float ly, float lz,
                          float ox, float oy, float oz )
{
	if ( dxSound!=NULL && soundIndex<soundCntr )
    {
#ifdef _USE3DSOUND_
		::SetObjectProperties( (lx-ox),(ly-oy),(lz-oz), 0,0,0, soundIndex, dxSound );
#else
		float dx = lx-ox;
		float dy = ly-oy;
		float dz = lz-oz;
		float dist = (dx*dx + dy*dy + dz*dz);
		float comp = (maxDistance*maxDistance*0.025f);
		if ( dist>comp )
		{
			dist = 0;
		}
		else
		{
			dist = 1 - (dist/comp);
			if ( dist < 0.75f )
				dist = 0.75f;
		}
		SetVolume( soundIndex, dist );

		float angle = ::VectorsToAngle( lx,lz, ox,oz );
//		angle = float(fabs(cos(angle)));
//		SetPan( soundIndex, angle );
//		TString msg = FloatToString( 1,2,angle );
//		::SetMessage( msg );
#endif
    }
};

void TSound::SetVolume( int soundIndex, float vol )
{
	if ( dxSound!=NULL )
		::SetVolume( vol, soundIndex, dxSound );
};

void TSound::SetPan( int soundIndex, float pan )
{
	if ( dxSound!=NULL )
		::SetPan( pan, soundIndex, dxSound );
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

//    if ( IsBufferPlaying( soundIndex, dxSound ) )
//		StopBuffer( false, soundIndex, dxSound );

	PlayBuffer( looped, soundIndex, dxSound );
};


bool TSound::IsPlaying( int soundIndex )
{
	if ( dxSound==NULL )
    	return false;

    return IsBufferPlaying( soundIndex, dxSound );
};

float TSound::MinDistance( void ) const
{
	return minDistance;
};

float TSound::MaxDistance( void ) const
{
	return maxDistance;
};

#endif

//==========================================================================


