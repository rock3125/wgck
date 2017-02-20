#ifndef __NETWORK_CLIENT_H__
#define __NETWORK_CLIENT_H__

//==========================================================================

#include <network/server.h>
#include <object/tank.h>

//==========================================================================

class DataParticulars;

//==========================================================================

class TClient
{
	const TClient& operator=( const TClient& );
	TClient( const TClient& );
public:
	TClient( void );
	virtual ~TClient( void );

	bool	StartWinsock( TString& errStr );
	void	CloseWinsock( void );

	bool	StartClient( const TString& server, size_t port, TString& errStr );
	void	StopClient( void );

	size_t	Read( byte* buffer, size_t size );
	bool	Write( const byte* buffer, size_t size, TString& errStr );

	bool	Connected( void ) const;

	void	AddPlayer( byte playerId, byte side, 
					   const TString& _name, 
					   const DataParticulars& d );
	void	RemovePlayer( byte playerId );
	void	SetData( byte playerId, const DataParticulars& d );
	const DataParticulars& GetData( byte playerId ) const;
	const TString& Name( byte playerId ) const;

private:
	friend class TNetwork;

	unsigned short port;
	bool	winsockStarted;
	bool	clientConnected;

	SOCKADDR_IN saServer;
	SOCKET		theSocket;

	bool				used[kMaxPlayers];
	TString				name[kMaxPlayers];
	byte				side[kMaxPlayers];
	DataParticulars*	players[kMaxPlayers]; // player game data
	TTank				tanks[kMaxPlayers]; // vehicle data
};

//==========================================================================

#endif

