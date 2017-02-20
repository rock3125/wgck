#ifndef _NETWORK_LOGIC_H_
#define _NETWORK_LOGIC_H_

//==========================================================================

#include <winsock.h>
//#include <Winsock2.h>

#include <network/packet.h>

#include <win32/win32.h>
#include <win32/events.h>
#include <tank/tankapp.h>

//==========================================================================

#define WM_SOCKMESSAGE	WM_USER + 1

class TMessage;

//==========================================================================

class TNetwork
{
	TNetwork( const TNetwork& );
	const TNetwork& operator=( const TNetwork& );
public:
	TNetwork( TApp* app, const short port );

	~TNetwork( void );

	// callback for async socket ops
	void DoSocket( HWND hWnd, TApp* app, WPARAM wParam, LPARAM lParam );

	// start host with these parameters
	void SetupHost( HWND hWnd, TApp* app, const TString&  playerName,
					const TString& gameName, size_t port );

	// join with these parameters
	void SetupGuest( HWND hWnd, const TString&  playerName, 
					 size_t port, const TString& hostName );

	// access network functions
	byte	MyId( void ) const;
	void	MyId( byte );

	byte	ServerId( void ) const;
	void	ServerId( byte );

	byte	ClientSide( void ) const;
	void	ClientSide( byte );

	byte	ClientTeam( void ) const;
	void	ClientTeam( byte );

	SOCKET	Sockets( byte ) const;
	void	Sockets( byte, SOCKET );

	bool	Used( byte ) const;
	void	Used( byte, bool );

	const TString&	PlayerName( void ) const;
	void			PlayerName( const TString& );

	const TString&	HostName( void ) const;
	void			HostName( const TString& );

	const TString&	GameName( void ) const;
	void			GameName( const TString& );

	const TString&	Names( byte ) const;
	void			Names( byte, const TString& );

private:
	friend class TApp;

	void StopNetwork( void );
	void CloseSocket( void );
	void CloseSocket( SOCKET sock );
	size_t DoClientRead( SOCKET sock, TApp* app );
	size_t DoServerRead( SOCKET sock, TApp* app );

	bool StartWinsock( TString& errStr );
	void CloseWinsock( void );
	bool StartServer( HWND hWindow, TString& errStr );
	bool StartClient( HWND hWindow, const TString& server, TString& errStr );

	size_t	Read( SOCKET socket, byte* buffer, size_t size );
	void	WriteSock( SOCKET sock, const byte* buffer, size_t size );

	bool	ProcessOutQueue( void );

private:
	TApp*		app;

	// winsock & client server details
	bool		winsockStarted;
	bool		serverStarted;
	bool		clientStarted;
	bool		clientConnected;
	bool		readyToWrite;
	bool		scoreUpdated;

	SOCKET		tcpInSock;				// for comms
	SOCKET		sockets[kMaxPlayers];	// connections
	bool		used[kMaxPlayers];		// player slot in use?
	TString		names[kMaxPlayers];		// players name
	bool		isBot[kMaxPlayers];		// bots

	byte		myId;
	byte		serverId;
	byte		clientSide;
	byte		clientTeam;
	TString		playerName;
	TString		gameName;
	TString		hostName;

	short		kGamePort;

	// out queue
	DataPacketQueue		outqueue;
};

//==========================================================================

#endif
