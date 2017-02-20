#include <precomp_header.h>

#include <win32/params.h>

//==========================================================================

TParams::TParams( void )
{
	numPlanes = 0;
	numTanks = 0;
	numFlak = 0;
	numArtillery = 0;
	numMG42s = 0;
	numPlayers = 0;

	planes = NULL;
	tanks = NULL;
	flak = NULL;
	artillery = NULL;
	mg42 = NULL;
	players = NULL;
	inUse = NULL;
};

TParams::~TParams( void )
{
	numPlanes = 0;
	numTanks = 0;
	numFlak = 0;
	numArtillery = 0;
	numMG42s = 0;
	numPlayers = 0;

	planes = NULL;
	tanks = NULL;
	flak = NULL;
	artillery = NULL;
	mg42 = NULL;
	players = NULL;
	inUse = NULL;
};

//==========================================================================
