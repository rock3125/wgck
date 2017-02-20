#ifndef __NETWORK_SERVER_H__
#define __NETWORK_SERVER_H__

//==========================================================================

#include <object/tank.h>

#define kMaxPlayers 16

class DataParticulars;

//==========================================================================

class TServer
{
	const TServer& operator=( const TServer& );
	TServer( const TServer& );
public:
	TServer( void );
	virtual ~TServer( void );

	bool	StartServer( short nPort, TString& errStr );
	void	StopServer( void );

	size_t	Read( SOCKADDR_IN& clientAddress, byte* buffer, size_t size );
	void	Write( SOCKADDR_IN& clientAddress, byte* buffer, size_t size );
	void	Write( byte playedId, byte* buffer, size_t size );

	bool	Started( void ) const;

	// first - a client requests to be added and we do a check
	// second the client accepts if it is possible to join, and we add
	byte	CheckNewPlayer( const TString& name, const byte& side, 
						    SOCKADDR_IN& clientAddress, TString& errStr );
	void	AddNewPlayer( byte playerId, TString& name );
	void	RemovePlayer( byte playerId );

	// get and set player posn data
	void	SetData( byte playerId, const DataParticulars& d );
	DataParticulars& GetData( byte playerId ) const;

	const TString& Name( byte playerId ) const;

private:
	friend class TNetwork;

	unsigned short port;
	bool		winsockStarted;
	bool		serverStarted;
	SOCKET		theSocket;
	SOCKADDR_IN	saServer;

	unsigned long		commsTime[kMaxPlayers];			// time the process
	TString				name[kMaxPlayers];				// new player name
	bool				used[kMaxPlayers];				// slot in use?
	bool				active[kMaxPlayers];			// slot accepted by joinee
	byte				side[kMaxPlayers];				// whose side is the player on?
	SOCKADDR_IN			clientAddresses[kMaxPlayers];	// address of joinee
	DataParticulars*	players[kMaxPlayers];			// player game data
	TTank				tanks[kMaxPlayers];				// vehicles
};

//==========================================================================

#endif

