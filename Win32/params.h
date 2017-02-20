#ifndef __TANK_PARAMS_H__
#define __TANK_PARAMS_H__

//==========================================================================

class TPlane;
class TTank;
class TFlak;
class TArtillery;
class TMG42;
class TCharacter;

//==========================================================================

class _EXPORT TParams
{
private:
	TParams( const TParams& );
	const TParams& operator=( const TParams& );

public:
	TParams( void );
	~TParams( void );

	size_t		numPlanes;
	TPlane*		planes;
	size_t		numTanks;
	TTank*		tanks;
	size_t		numFlak;
	TFlak*		flak;
	size_t		numArtillery;
	TArtillery* artillery;
	size_t		numMG42s;
	TMG42*		mg42;
	size_t		numPlayers;
	TCharacter* players;
	bool*		inUse;
};

//==========================================================================

#endif

