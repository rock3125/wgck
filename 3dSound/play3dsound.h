#ifndef _PLAY3D_SOUND_H_
#define _PLAY3D_SOUND_H_

void* WINAPI InitDirectSound( HWND hDlg, int nChannels, int samplesPerSec, int bitDepth );
bool  WINAPI FreeDirectSound( void* ptr );

void  WINAPI StopBuffer( bool bResetPosition, int index, void* ptr );
bool  WINAPI LoadWaveFile( const char* strFileName, int index, void* ptr );
bool  WINAPI IsBufferPlaying( int index, void* ptr ) ;
bool  WINAPI PlayBuffer( bool bLooped, int index, void* ptr );
void  WINAPI SetVolume( float vol, int index, void* ptr );
void  WINAPI SetPan( float pan, int index, void* ptr );

//	fDopplerFactor  0 - 10
//	fRolloffFactor	0 - 10
//  fMinDistance	0.1 - 11
//  fMaxDistance	0.1 - 11
void WINAPI SetParameters( float fDopplerFactor, float fRolloffFactor,
						   float fMinDistance, float fMaxDistance,
                           int index, void* ptr );
void WINAPI SetObjectProperties( float x, float y, float z,
								 float vx, float vy, float vz,
                                 int index, void*ptr );
#endif
