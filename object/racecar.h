#ifndef __OBJECT_RACECAR_H_
#define __OBJECT_RACECAR_H_

//==========================================================================

#include <object/vehicle.h>
#include <object/anim.h>
#include <object/landscape.h>
#include <object/recordSystem.h>

class TRaceCar : public TVehicle
{
public:
	TRaceCar( const float speed );
	~TRaceCar( void );

    bool LoadBinary( class TSound* soundSystem,
					 size_t _engineIdleIndex,
					 size_t _engineIndex,
					 const TString& bodyFname, 
					 const TString& wheelFname, 
					 TString& errStr, 
					 const TString& pathname,
					 const TString& texturePath );

	// recording system
	void SetupRecording( const TString& filename );
	bool StopRecording( TString& errSr );

	bool SetupPlayback( const TString& filename, TString& errSr );
	void StopPlayback( void );

	bool PlaybackFinished( void ) const;

	// logic
	void Move( TLandscape& landObj, size_t& key, bool ctrlDown, 
			   float dxa, float dya );

	void Draw( void );

	float	Speed( void ) const;
	void	Speed( float speed );

	float	MaxSpeed( void ) const;

private:
	TSound*	soundSystem;
	size_t	engineIdleIndex;
	size_t	engineIndex;
	bool	enginePlaying;
	bool	engineIdlePlaying;

	TBinObject wheel;

	float	speed; // no prizes for guessing this one
	float	maxSpeed;
	float	wheelRotAngles; // rotating wheels
	float	wheelY;			// turning wheels

	// jumping vars
	float	prevMy;			// for jumping
	bool	jumping;		// in jumping mode?
	float	jumpDuration;	// counter

	bool initialised;	// object loaded ok?

	// record tank movements?
	bool			playback;
	bool			record;
	TCarRecord*		recording;
	TString			recordingFname;
};

#endif
