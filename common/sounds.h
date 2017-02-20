#ifndef __SOUND_SYSTEM_H__
#define __SOUND_SYSTEM_H__

//==========================================================================

// interface into win32.cpp message intialisation system
void AddMessage( const char* m );

//==========================================================================


//==========================================================================

#ifdef _USEFMOD

#include <3dsound\fmod_sound.h>

#define kMaxSamples		128


class _EXPORT TSound
{
public:
	TSound( HWND _hwnd );
    ~TSound( void );

	// returns -1 on failure
	int Load( const TString& fname, bool looped, TString& errStr );

	void SetCameraLocation( const TPoint& pos, const TPoint& fwd, const TPoint& up );

	int  Play( size_t soundIndex, const TPoint& pos ); // returns channel

	void Stop( int channel );
	bool IsPlaying( int channel );
	void SetLocation( int channel, const TPoint& pos, const TPoint& prevPos );
	void Volume( byte vol ); // global volume for all new sounds
	void Volume( int channel, byte vol );

	bool Initialised( void ) const;

	// mp3 and other codec streamers
	bool	OpenStream( const TString& fname );
	bool	PlayStream( void );
	bool	StopStream( void );
	bool	CloseStream( void );
	bool	IsStreamPlaying( void );
	void	StreamVolume( byte vol );

	void	PauseStream( void );
	void	ResumeStream( void );

private:
	HWND			hwnd;
    bool			initialised;

	FSOUND_SAMPLE*	samples[kMaxSamples];
    size_t			soundCntr;
	byte			soundVolume;

	FSOUND_STREAM*	stream;
	int				streamChannel;
	byte			streamVolume;
	TString			streamFname;
	TString			pauseFname;
	size_t			streamPos;
};

#else

#include <3dsound\play3dsound.h>

//==========================================================================

class _EXPORT TSound
{
public:
	TSound( HWND _hwnd, char* errStr=NULL );
    ~TSound( void );

	// returns -1 on failure
	int Load( const char* fname, float minDist=0, float maxDist=500.0f );

	void SetLocation( int soundIndex,
					  float lx, float ly, float lz,
                      float ox, float oy, float oz );

	void SetParameters( int soundIndex, 
						float fDopplerFactor, float fRolloffFactor,
						float fMinDistance,   float fMaxDistance );

	void SetPan( int soundIndex, float pan );           // [0=left,0.5=center,1=right]
	void SetVolume( int soundIndex, float vol );		// [0=none, 1=default]

	void Play( int soundIndex, bool looped=false );
	void Stop( int soundIndex );
	bool IsPlaying( int soundIndex );

	bool Initialised( void ) const;

	float	MinDistance( void ) const;
	float	MaxDistance( void ) const;

private:
	HWND		hwnd;
    HINSTANCE	hSound;
    void*		dxSound;

	float		minDistance;
	float		maxDistance;

    int			soundCntr;
};

#endif

//==========================================================================

#endif


