#ifndef __SOUND_SYSTEM_H__
#define __SOUND_SYSTEM_H__

//==========================================================================

// interface into win32.cpp message intialisation system
void AddMessage( const char* m );

//==========================================================================

#ifndef _MSVC
typedef void* pascal (*tInitDirectSound)( HWND hDlg, int nChannels,
										  int samplesPerSec, int bitDepth );
typedef bool pascal (*tFreeDirectSound)( void* ptr );

typedef void pascal (*tStopBuffer)( bool bResetPosition, int index, void* ptr );
typedef bool pascal (*tLoadWaveFile)( const char* strFileName, int index, void* ptr );
typedef bool pascal (*tIsBufferPlaying)( int index, void* ptr ) ;
typedef bool pascal (*tPlayBuffer)( bool bLooped, int index, void* ptr );

//	fDopplerFactor  0 - 10
//	fRolloffFactor	0 - 10
//  fMinDistance	0.1 - 11
//  fMaxDistance	0.1 - 11
typedef void pascal (*tSetParameters)( float fDopplerFactor, float fRolloffFactor,
									   float fMinDistance, float fMaxDistance,
                                       int index, void* ptr );
typedef void pascal (*tSetObjectProperties)( float x, float y, float z,
								 			 float vx, float vy, float vz,
                                             int index, void*ptr );
#else
#include <3dsound\play3dsound.h>
#endif
//==========================================================================

class TSound
{
public:
	TSound( HWND _hwnd, char* errStr=NULL );
    ~TSound( void );

	bool Load( const char* fname, float minDist=10.0f, float maxDist=200.0f );
	void SetLocation( int soundIndex, float listenerAngle, 
    				  float dx, float dy, float dz,
                      float velX=0, float velY=0, float velZ=0 );
	void Play( int soundIndex, bool looped=false );
	void Stop( int soundIndex );
	bool IsPlaying( int soundIndex );

#ifndef _MSVC
private:
	tInitDirectSound		InitDirectSound;
	tStopBuffer				StopBuffer;
	tLoadWaveFile			LoadWaveFile;
	tIsBufferPlaying		IsBufferPlaying;
	tPlayBuffer				PlayBuffer;
	tFreeDirectSound		FreeDirectSound;
    tSetParameters			SetParameters;
    tSetObjectProperties	SetObjectProperties;
#endif

private:
	HWND		hwnd;
    HINSTANCE	hSound;
    void*		dxSound;
    int			soundCntr;
};

//==========================================================================

#endif


