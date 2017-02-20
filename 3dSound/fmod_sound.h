#ifndef _3DSOUND_FMODSOUND_H_
#define _3DSOUND_FMODSOUND_H_

//==========================================================================

#include <object/geometry.h>

#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>

//==========================================================================

void CloseFMOD( void );
bool InitFMOD( TString& errStr );
FSOUND_SAMPLE* LoadSample( const TString& fname, bool looped, TString& errStr );
int PlaySound( FSOUND_SAMPLE* sample, const TPoint& pos );
void StopSound( int channel );
bool SoundIsPlaying( int channel );
void UpdateSound( int channel, const TPoint& pos, const TPoint& prevPos );
void SetCamera( const TPoint& pos,
			    const TPoint& fwd,
				const TPoint& up );
void SetVolume( int channel, byte vol );

FSOUND_STREAM* OpenStream( const TString& fname );
int PlayStream( FSOUND_STREAM* stream );
bool StopStream( FSOUND_STREAM* stream );
bool CloseStream( FSOUND_STREAM* stream );

//==========================================================================

#endif

