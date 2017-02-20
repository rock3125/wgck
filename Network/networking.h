#ifndef _NETTEST_NETWORKING_H_
#define _NETTEST_NETWORKING_H_

//==========================================================================

// message must be smaller than this!!!
#define kNetworkBufferSize  1024 

//==========================================================================

bool _EXPORT nwStartWinsock( TString& errStr ); // startup winsock, auto called by
												// network thread
void _EXPORT nwStopWinsock( void ); // stop winsock

bool _EXPORT nwGetLocalNetDetails( TString& hostName, TString& ipStr ); // get local name and ip

void _EXPORT nwStartClient( const TString& str, size_t port ); // start a client connection
bool _EXPORT nwIsClientRunning( TString& errStr );  // have we connected to a server

void _EXPORT nwStartHost( const TString& gameName, size_t port ); // start a server on port
bool _EXPORT nwIsConnected( size_t socketId ); // is socketId a valid connection?

void _EXPORT nwStopNetwork( void ); // kill the network for either server or client

bool _EXPORT nwServerActive( void ); // are the client or server active/running?
bool _EXPORT nwClientActive( void );

// client routines
											// or are we still connected?
bool _EXPORT nwReceiveBufferFull( void ); // have we received info?
bool _EXPORT nwReadNetworkBuffer( byte* buf, size_t& size ); // read info - returns false if none avail
bool _EXPORT nwWriteBufferFree( void ); // is the write buffer empty?
void _EXPORT nwWriteNetworkBuffer( const byte* buf, size_t size ); // write info to buffer- returns false if already full

// server routines
bool _EXPORT nwReceiveBufferFull( size_t socketId ); // has socketId sent me a message?
bool _EXPORT nwReadNetworkBuffer( size_t socketId, byte* buf, size_t& size ); // read msg from socketId
void _EXPORT nwWriteBufferFree( size_t socketId ); // can we send a message to socketId?
void _EXPORT nwWriteNetworkBuffer( size_t socketId, const byte* buf, size_t size ); // send message to socketId

// find a host on this subnet
//
// returns: hostName, hostIp, gamePort, and numPlayers in current game
// as comma seperated lists (in case of multiple replies)
//
bool _EXPORT nwFindHost( TString& hostNames, TString& hostIPs, 
						 TString& hostPorts, TString& numPlayers,
						 TString& gameNames );

//==========================================================================

#endif

