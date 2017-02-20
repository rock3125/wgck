#ifndef _OBJECT_SOUNDTRACK_H_
#define _OBJECT_SOUNDTRACK_H_

//==========================================================================

#define kMaxSoundtracks	64

//==========================================================================

class _EXPORT SoundTrack
{
public:
	SoundTrack( void );
	~SoundTrack( void );

	// access soundtracks
	size_t	Count( void ) const;
	void	Get( size_t index, TString& filename, TString& title );

private:
	size_t		count;
	TString		filename[kMaxSoundtracks];
	TString		title[kMaxSoundtracks];
};

//==========================================================================

#endif
