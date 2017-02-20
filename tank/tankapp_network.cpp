#include <precomp_header.h>

#include <direct.h>
#include <process.h>
#include <shellapi.h>

#include <win32/events.h>
#include <win32/win32.h>
#include <win32/resource.h>

#include <common/sounds.h>

#include <object/geometry.h>
#include <object/tank.h>
#include <object/anim.h>
#include <object/camera.h>
#include <object/recordSystem.h>

#include <dialogs/hostdialog.h>
#include <dialogs/joindialog.h>
#include <systemDialogs/aboutdlg.h>
#include <dialogs/videoSettings.h>

#include <tank/tankapp.h>

#include <network/networking.h>
#include <network/packet.h>

#include <io.h>

//==========================================================================

#include <mmsystem.h>

//==========================================================================

const float Deg2Rad = 0.01745329252f;
const float Rad2Deg = 57.295779511273f;

//==========================================================================

void TApp::NetworkFireProjectile( const TProjectile& proj )
{
	byte temp[kMaxSingleCoordSize];
	size_t s = proj.netSet( myId, temp );

	if ( serverGame )
	{
		for ( size_t i=1; i<kMaxPlayers; i++ )
		{
			if ( multiplayerPlaying[i] )
			{
				nwWriteNetworkBuffer( i, temp, s );
			}
		}
	}
	else if ( clientGame )
	{
		nwWriteNetworkBuffer( temp, s );
	}
};

void TApp::NetworkFireProjectile( size_t playerId, const TProjectile& proj )
{
	byte temp[kMaxSingleCoordSize];
	size_t s = proj.netSet( myId, temp );
	if ( multiplayerPlaying[playerId] )
	{
		nwWriteNetworkBuffer( playerId, temp, s );
	}
};

void TApp::NetworkDropExplosive( const TExplosive& expl )
{
	byte temp[kMaxSingleCoordSize];
	size_t s = expl.netSet( myId, temp );

	if ( serverGame )
	{
		for ( size_t i=1; i<kMaxPlayers; i++ )
		{
			if ( multiplayerPlaying[i] )
			{
				nwWriteNetworkBuffer( i, temp, s );
			}
		}
	}
	else if ( clientGame )
	{
		nwWriteNetworkBuffer( temp, s );
	}
};

void TApp::NetworkDropExplosive( size_t playerId, const TExplosive& expl )
{
	byte temp[kMaxSingleCoordSize];
	size_t s = expl.netSet( myId, temp );
	if ( multiplayerPlaying[playerId] )
	{
		nwWriteNetworkBuffer( playerId, temp, s );
	}
};

void TApp::UpdateStrengths( void )
{
	size_t i;
	byte buf[kNetworkBufferSize];
	DataStrengthUpdate up;

	// update strengths
	bool full = false;
	size_t cntr = 0;
	for ( i=0; i<numPlanes && !full; i++ )
	{
		if ( planes[i].StrengthUpdated() )
		{
			full = !up.Add( vtPlane, i, planes[i].Strength() );
			if ( !full )
			{
				planes[i].StrengthUpdated( false );
				cntr++;
			}
		}
	}

	for ( i=0; i<numTanks && !full; i++ )
	{
		if ( tanks[i].StrengthUpdated() )
		{
			full = !up.Add( vtTank, i, tanks[i].Strength() );
			if ( !full )
			{
				tanks[i].StrengthUpdated( false );
				cntr++;
			}
		}
	}

	for ( i=0; i<numMG42s && !full; i++ )
	{
		if ( mg42[i].StrengthUpdated() )
		{
			full = !up.Add( vtMG42, i, mg42[i].Strength() );
			if ( !full )
			{
				mg42[i].StrengthUpdated( false );
				cntr++;
			}
		}
	}

	for ( i=0; i<numArtillery && !full; i++ )
	{
		if ( artillery[i].StrengthUpdated() )
		{
			full = !up.Add( vtArtillery, i, artillery[i].Strength() );
			if ( !full )
			{
				artillery[i].StrengthUpdated( false );
				cntr++;
			}
		}
	}

	for ( i=0; i<numFlak && !full; i++ )
	{
		if ( flak[i].StrengthUpdated() )
		{
			full = !up.Add( vtFlak, i, flak[i].Strength() );
			if ( !full )
			{
				flak[i].StrengthUpdated( false );
				cntr++;
			}
		}
	}

	for ( i=0; i<numV2 && !full; i++ )
	{
		if ( v2[i].StrengthUpdated() )
		{
			full = !up.Add( vtV2, i, v2[i].Strength() );
			if ( !full )
			{
				v2[i].StrengthUpdated( false );
				cntr++;
			}
		}
	}

	for ( i=0; i<kMaxPlayers && !full; i++ )
	{
		if ( multiplayerPlaying[i] )
		if ( characters[i].StrengthUpdated() )
		{
			full = !up.Add( vtNone, i, characters[i].Strength() );
			if ( !full )
			{
				characters[i].StrengthUpdated( false );
				cntr++;
			}
		}
	}

	if ( cntr > 0 )
	{
		size_t s = up.Set(buf);
		for ( i=1; i<kMaxPlayers; i++ )
		{
			if ( multiplayerPlaying[i] )
			{
				nwWriteNetworkBuffer( i, buf, s );
			}
		}
	}
};

size_t TApp::GetStrengthUpdatePacket( byte* buf )
{
	size_t i;
	DataStrengthUpdate up;

	// update strengths
	bool full = false;
	size_t cntr = 0;
	for ( i=0; i<numPlanes && !full; i++ )
	{
		full = !up.Add( vtPlane, i, planes[i].Strength() );
		if ( !full )
		{
			cntr++;
		}
	}

	for ( i=0; i<numTanks && !full; i++ )
	{
		full = !up.Add( vtTank, i, tanks[i].Strength() );
		if ( !full )
		{
			cntr++;
		}
	}

	for ( i=0; i<numMG42s && !full; i++ )
	{
		full = !up.Add( vtMG42, i, mg42[i].Strength() );
		if ( !full )
		{
			cntr++;
		}
	}

	for ( i=0; i<numArtillery && !full; i++ )
	{
		full = !up.Add( vtArtillery, i, artillery[i].Strength() );
		if ( !full )
		{
			cntr++;
		}
	}

	for ( i=0; i<numFlak && !full; i++ )
	{
		full = !up.Add( vtFlak, i, flak[i].Strength() );
		if ( !full )
		{
			cntr++;
		}
	}

	for ( i=0; i<numV2 && !full; i++ )
	{
		full = !up.Add( vtV2, i, v2[i].Strength() );
		if ( !full )
		{
			cntr++;
		}
	}

	for ( i=0; i<kMaxPlayers && !full; i++ )
	{
		if ( multiplayerPlaying[i] )
		{
			full = !up.Add( vtNone, i, characters[i].Strength() );
			if ( !full )
			{
				cntr++;
			}
		}
	}

	if ( cntr > 0 )
	{
		size_t s = up.Set(buf);
		return s;
	}
	return 0;
};

void TApp::AddPlayer( size_t playerId, size_t _team, size_t _character, TString name )
{
	PreCond( _team < kMaxTeams );
	PreCond( playerId < kMaxPlayers );
	PreCond( _character < 6 );

	team[playerId] = _team;
	multiplayerNames[playerId] = name;
	multiplayerSlotInUse[playerId] = true;

	// and finally - setup this character on the server
	TLandscapeObject startPos;
	if ( gameType==0 )
	{
		size_t teamId;
		startPos = landObj->GetPlayerPositionNoTeams( playerId, teamId );
		team[playerId] = teamId;
	}
	else
	{
		startPos = landObj->GetPlayerPosition( team[playerId], playerId );
	}

	TString errStr;
	if ( !characters[playerId].Load( GetCharacterFilename( _character ), errStr ) )
	{
		::MessageBox( hWindow, errStr.c_str(), STR(strError), MB_OK | MB_ICONERROR );	
		return;
	}

	characters[playerId].Team( team[playerId] );

	// give model weapons
	character[playerId] = _character;
	characters[playerId].SetWeapons( numWeapons, weapons );
	characters[playerId].SetSounds( swalk, sgun, smachinegun, 
									scanon, grShell, parachute );
	characters[playerId].SetSoundSystem( soundSystem );
	characters[playerId].SetApp( this );
	characters[playerId].Reset();
	characters[playerId] = startPos;

	characters[playerId].VehicleId( 0 );
	characters[playerId].VehicleType( vtNone );
};

// this start message goes from the client to the server
// when the client has finished loading all its assets and
// is ready to start receiving coordinate information from
// the server.
void TApp::SendClientStartMessageToServer( void )
{
	byte buf[kNetworkBufferSize];
	DataClientStart dat;

	PreCond( myId>0 );

	dat.playerId = myId;
	dat.team = team[myId];
	dat.character = character[myId];
	dat.name = multiplayerNames[myId];

	size_t s = dat.Set( buf );
	nwWriteNetworkBuffer( buf, s );
};

// the Client messages are for the join dialog system
// they are a series of flags indicating connection progress to server
bool TApp::ClientReceivedWelcome( void ) const
{
	return clientReceivedWelcome;
};

bool TApp::ClientReceivedStart( void ) const
{
	return clientReceivedStart;
};

bool TApp::ClientError( void ) const
{
	return clientError;
};

void TApp::ClientError( bool b )
{
	clientError = b;
};

TString TApp::ClientErrorStr( void ) const
{
	return clientErrorStr;
};

// check a name isn't already used by someone else
bool TApp::CheckPlayerName( const TString& _name, TString& errStr )
{
	size_t i;
	TString _nm = _name.lcase();

	size_t firstAvail = -1;
	bool duplicateName = false;
	for ( i=0; i<kMaxPlayers; i++ )
	{
		if ( nwIsConnected(i) || i==myId )
		{
			if ( multiplayerNames[i].length() > 0 )
			{
				if ( multiplayerNames[i].lcase() == _nm )
				{
					duplicateName = true;
				}
			}
		}
		else
		{
			firstAvail = 0;
		}
	}
	if ( duplicateName )
	{
		errStr = STR(strNameTaken);
		return false;
	}
	if ( firstAvail==-1 )
	{
		errStr = STR(strGameFull);
		return false;
	}
	return true;
};

// see if there is an opening in the requested team for this player
bool TApp::CanJoinTeam( size_t playerId, size_t _team, TString& errStr )
{
	size_t i;

	TDefFile def;
	if ( !GetStoryDef( map, errStr, def ) )
	{
		errStr = STR(strNoMap);
		return false;
	}

	TMapPlayerInfo mapInfo = def.GetPlayerInfo();

	// team type game?
	size_t currentTeamPlayers = 0;
	if ( gameType==1 ) 
	{
		for ( i=0; i<kMaxPlayers; i++ )
		{
			if ( nwIsConnected(i) && playerId!=i && team[i]==_team )
			{
				currentTeamPlayers++;
			}
		}
		if ( currentTeamPlayers>=mapInfo.teamCount[_team] )
		{
			switch ( _team )
			{
			case redTeam:
				{
					errStr = STR(strNoSlotRed);
					break;
				};
			case greenTeam:
				{
					errStr = STR(strNoSlotGreen);
					break;
				};
			case blueTeam:
				{
					errStr = STR(strNoSlotBlue);
					break;
				};
			case yellowTeam:
				{
					errStr = STR(strNoSlotYellow);
					break;
				};
			}
			errStr = errStr + STR(strChooseOtherTeam);
			return false;
		}
	}
	return true;
};

// see if there's enough room on the map for another player to join
bool TApp::CanJoinMap( size_t playerId, size_t map, TString& errStr )
{
	size_t i;

	TDefFile def;
	if ( !GetStoryDef( map, errStr, def ) )
	{
		errStr = STR(strNoMap);
		return false;
	}

	TMapPlayerInfo mapInfo = def.GetPlayerInfo();
	size_t numPlayers = 0;
	for ( i=0; i<kMaxTeams; i++ )
	{
		numPlayers = numPlayers + mapInfo.teamCount[i];
	}

	// server always counts as one
	size_t currentNumPlayers = 1;
	for ( i=1; i<kMaxPlayers; i++ )
	{
		if ( i!=playerId && nwIsConnected(i) )
		{
			currentNumPlayers++;
		}
	}

	if ( currentNumPlayers>=numPlayers )
	{
		errStr = STR(strGameFull);
		return false;
	}
	return true;
};


void TApp::SetupGuest( const TString& _playerName, size_t port, const TString& hostName )
{
	byte buf[kNetworkBufferSize];

	nwStartClient( hostName, port );

	DataCanIJoin dat;
	dat.name = _playerName;

	size_t s = dat.Set( buf );
	nwWriteNetworkBuffer( buf, s );
};


void TApp::ServerNetworkLogic( void )
{
	byte buf[kNetworkBufferSize];
	size_t i;

	// update character position parameters according
	// to vehicle s/he is in for multi-player characters
	for ( i=1; i<kMaxPlayers; i++ )
	{
		if ( multiplayerPlaying[i] )
		{
			size_t vehicleId = characters[i].VehicleId();
			switch ( characters[i].VehicleType() )
			{
			case vtNone:
				{
					break;
				}
			case vtTank:
				{
					characters[i].X( tanks[vehicleId].X() );
					characters[i].Y( tanks[vehicleId].Y() );
					characters[i].Z( tanks[vehicleId].Z() );
					break;
				};
			case vtPlane:
				{
					characters[i].X( planes[vehicleId].X() );
					characters[i].Y( planes[vehicleId].Y() );
					characters[i].Z( planes[vehicleId].Z() );
					break;
				};
			case vtMG42:
				{
					characters[i].X( mg42[vehicleId].X() );
					characters[i].Y( mg42[vehicleId].Y() );
					characters[i].Z( mg42[vehicleId].Z() );
					break;
				};
			case vtArtillery:
				{
					characters[i].X( artillery[vehicleId].X() );
					characters[i].Y( artillery[vehicleId].Y() );
					characters[i].Z( artillery[vehicleId].Z() );
					break;
				};
			case vtFlak:
				{
					characters[i].X( flak[vehicleId].X() );
					characters[i].Y( flak[vehicleId].Y() );
					characters[i].Z( flak[vehicleId].Z() );
					break;
				};
			}
		}
	}

	// update system strengths to all clients
	UpdateStrengths();

	// update landscape object strengths for clients
	if ( landObj!=NULL )
	{
		size_t s = landObj->GetLandscapeObjectUpdateStrengths( buf );
		if ( s > 0 )
		{
			for ( i=1; i<kMaxPlayers; i++ )
			if ( multiplayerPlaying[i] )
			{
				nwWriteNetworkBuffer( i, buf, s );
			}
		}
	};

	// see if there's a message waiting on any of the active sockets
	// zero is always skipped since it is assumed to be the server
	for ( i=1; i<kMaxPlayers; i++ )
	{
		size_t size;
		while ( nwReadNetworkBuffer( i, buf, size ) )
		if ( size>0 )
		{
			int index = 0;
			byte type = buf[index];
			switch ( type )
			{
				// first message received by server - join request
				case id_canijoin:
				{
					DataCanIJoin resp;
					resp.Get( buf );

					TString msgStr, errStr;

					// check for duplicate names
					byte playerId = i; // it is!
					bool validName = CheckPlayerName( resp.name, errStr );

					if ( kNetworkVersion!=resp.version )
					{
						msgStr = STR(strIncorrectVersion);
						DataYouCan reply( false, playerId, 0, msgStr );
						size_t s = reply.Set(buf);
						nwWriteNetworkBuffer( i, buf, s );
					}
					else if ( !validName )
					{
						msgStr = STR(strNameTaken);
						DataYouCan reply( false, playerId, 0, msgStr );
						size_t s = reply.Set(buf);
						nwWriteNetworkBuffer( i, buf, s );
					}
					else
					{
						DataYouCan reply( true, playerId, map, gameName );
						size_t s = reply.Set(buf);
						nwWriteNetworkBuffer( i, buf, s );
						msgStr = resp.name + " ";
						msgStr = msgStr + STR(strIsJoining);
						multiplayerNames[playerId] = resp.name; // temporary name
						msg->AddMessage( msgStr );
					}
					break;
				}
				
				case id_icanjoin:
				{
					DataICanJoin resp(0);
					resp.Get( buf );

					// can't be - is server id
					PreCond( resp.playerId!=myId );

					if ( resp.team==255 ) // first time round team==255
					{
						// send a s_youcan(false) if too many players have joined
						// a map with too few locations - see IDM_SINGLE
						// for setup for different game types
						TString errStr;
						if ( !CanJoinMap( resp.playerId, map, errStr ) )
						{
							DataYouCan reply( false, resp.playerId, 0, errStr );
							size_t s = reply.Set(buf);
							nwWriteNetworkBuffer( i, buf, s );
							return;
						}

						byte playerId = resp.playerId;
						multiplayerSlotInUse[playerId] = true;
						scores[playerId] = 0; // restart score

						// first welcome message
						DataWelcome wlcm;
						wlcm.gameType = gameType;
						wlcm.map = map;
						wlcm.maxPoints = maxPoints;
						size_t j;
						size_t index = 0;
						for ( j = 0; j<kMaxPlayers; j++ )
						{
							if ( nwIsConnected(j) && j!=playerId )
							{
								wlcm.Add( j, multiplayerNames[j], team[j], character[j] );
							}
						}
						size_t s = wlcm.Set( buf );
						nwWriteNetworkBuffer( playerId, buf, s );
					}
					else // second time around check team
					{
						byte playerId = resp.playerId;

						// check team validity?
						if ( gameType==1 )
						{
							TString errStr;
							if ( !CanJoinTeam( playerId, resp.team, errStr ) )
							{
								DataYouCan reply( false, playerId, 0, errStr );
								size_t s = reply.Set(buf);
								nwWriteNetworkBuffer( i, buf, s );

								multiplayerSlotInUse[playerId] = false;
								team[playerId] = 0;
								scores[playerId] = 0;

								return;
							}
							team[playerId] = resp.team;
						}

						// welcome second time around
						DataWelcome wlcm;
						wlcm.gameType = gameType;
						wlcm.map = map;
						wlcm.maxPoints = maxPoints;
						size_t j;
						for ( j = 0; j<kMaxPlayers; j++ )
						{
							if ( nwIsConnected(j) && j!=playerId )
							{
								wlcm.Add( j, multiplayerNames[j], team[j], character[j] );
							}
						}
						size_t s = wlcm.Set( buf );
						nwWriteNetworkBuffer( playerId, buf, s );
					}

					break;
				}

				case id_quit:
				{
					DataQuit resp;
					resp.Get( buf );
				
					size_t playerId = resp.playerId;

					PreCond( playerId!=myId );

					if ( multiplayerNames[playerId].length()>0 )
					{
						TString msgStr = multiplayerNames[playerId] + " ";
						msgStr = msgStr + STR(strQuit);
						msg->AddMessage( msgStr );
					}

					multiplayerNames[playerId] = "";
					multiplayerSlotInUse[playerId] = false;
					multiplayerPlaying[playerId] = false;

					// notify all other players of this sad event
					for ( size_t i=1; i<kMaxPlayers; i++ )
					{
						if ( multiplayerSlotInUse[i] )
						{
							DataOtherPlayerQuit msg( playerId );
							size_t s = msg.Set( buf );
							nwWriteNetworkBuffer( i, buf, s );
						}
					}
					break;
				}

				case id_characterData:
				{
					size_t playerId = buf[1];
					characters[playerId].netGet( buf );
					characters[playerId].VehicleType( vtNone );
					characters[playerId].VehicleId( 0 );
					break;
				}

				case id_tankData:
				{
					size_t playerId = buf[1];
					size_t vehicleId = buf[2];
					tanks[vehicleId].netGet( buf );
					characters[playerId].VehicleType( vtTank );
					characters[playerId].VehicleId( vehicleId );
					break;
				}

				case id_flakData:
				{
					size_t playerId = buf[1];
					size_t vehicleId = buf[2];
					flak[vehicleId].netGet( buf );
					characters[playerId].VehicleType( vtFlak );
					characters[playerId].VehicleId( vehicleId );
					break;
				}

				case id_mg42Data:
				{
					size_t playerId = buf[1];
					size_t vehicleId = buf[2];
					mg42[vehicleId].netGet( buf );
					characters[playerId].VehicleType( vtMG42 );
					characters[playerId].VehicleId( vehicleId );
					break;
				}

				case id_v2Data:
				{
					size_t playerId = buf[1];
					size_t vehicleId = buf[2];
					v2[vehicleId].netGet( buf );
					break;
				}

				case id_planeData:
				{
					size_t playerId = buf[1];
					size_t vehicleId = buf[2];
					planes[vehicleId].netGet( buf );
					characters[playerId].VehicleType( vtPlane );
					characters[playerId].VehicleId( vehicleId );
					break;
				}

				case id_artilleryData:
				{
					size_t playerId = buf[1];
					size_t vehicleId = buf[2];
					artillery[vehicleId].netGet( buf );
					characters[playerId].VehicleType( vtArtillery );
					characters[playerId].VehicleId( vehicleId );
					break;
				}

				case id_projectile:
				{
					// don't retransmit this one manually
					size_t playerId = buf[1];
					TProjectile* proj = FireProjectile( buf );

					// and notify remaining clients
					if ( proj!=NULL )
					{
						for ( size_t i=1; i<kMaxPlayers; i++ )
						{
							if ( multiplayerPlaying[i] && i!=playerId )
							{
								NetworkFireProjectile( i, *proj );
							}
						}
					}
					break;
				}

				case id_explosive:
				{
					// don't retransmit this one manually
					size_t playerId = buf[1];
					TExplosive* expl = DropExplosive( buf );

					// and notify remaining clients
					if ( expl!=NULL )
					{
						for ( size_t i=1; i<kMaxPlayers; i++ )
						{
							if ( multiplayerPlaying[i] && i!=playerId )
							{
								NetworkDropExplosive( i, *expl );
							}
						}
					}
					break;
				}

				// client sends start message - meaning it is ready now to
				// start receiving information from server
				case id_start:
				{
					DataClientStart resp;
					resp.Get( buf );

					byte playerId = resp.playerId;
					PreCond( playerId!=myId );

					multiplayerPlaying[playerId] = true;

					// now we actually join the player
					PreCond( resp.team < kMaxTeams );
					AddPlayer( playerId, resp.team, resp.character, resp.name );

					TString msgStr = multiplayerNames[playerId] + " ";
					msgStr = msgStr + STR(strJoined);
					msg->AddMessage( msgStr );

					// send landscape info to this player
					DataLandscapeStatus stat;
					byte buf[kNetworkBufferSize];
					byte landData[kNetworkBufferSize];
					PreCond( landObj!=NULL );
					size_t actualSize = landObj->GetLandscapeObjectStatus( kNetworkBufferSize,landData );
					stat.SetData( actualSize, landData );
					size_t s = stat.Set( buf );
					nwWriteNetworkBuffer( playerId, buf, s );

					s = GetStrengthUpdatePacket( buf );
					if ( s > 0 )
					{
						nwWriteNetworkBuffer( playerId, buf, s );
					}

					// write a c_otherplayerjoined to all other clients
					for ( size_t j=0; j<kMaxPlayers; j++ )
					{
						if ( j!=playerId && j!=0 )
						if ( nwIsConnected(j) )
						{
							DataOtherPlayerJoined msg( playerId,
													   multiplayerNames[j],
													   team[playerId], 
													   character[playerId] );
							size_t s = msg.Set( buf );
							nwWriteNetworkBuffer( j, buf, s );
						}
					};

					break;
				}

			}
		} // end if while


		if ( serverGame )
		{
			byte temp[kMaxSingleCoordSize]; // const defined in packet.h

			// send coordinates to all players
			size_t j;
			size_t count = 0;
			size_t index = 0;
			DataGlobalCoords gc;
			for ( j=0; j<kMaxPlayers; j++ )
			{
				if ( multiplayerPlaying[j] )
				if ( i!=j )
				{
					size_t s = characters[j].netSet(myId,j,temp);
					if ( characters[j].SendOverNetwork() )
					{
						gc.Add( vtNone, j, s, temp );
						count++;
						if ( characters[j].Strength()==0 )
						{
							characters[j].SendOverNetwork( false );
						}
					}
				}
			}

			// write AI coordinates to all players
			for ( j=0; j<numAITanks; j++ )
			{
				size_t s = aitanks[j].Tank().netSet(myId,j,temp);
				if ( aitanks[j].SendOverNetwork() )
				{
					gc.Add( vtTank, j, s, temp );
					count++;
					if ( aitanks[j].Tank().Strength()==0 )
					{
						aitanks[j].SendOverNetwork( false );
					}
				}
			}
			for ( j=0; j<numAIPlanes; j++ )
			{
				size_t s = aiplanes[j].Plane().netSet(myId,j,temp);
				if ( aiplanes[j].SendOverNetwork() )
				{
					gc.Add( vtPlane, j, s, temp );
					count++;
					if ( aiplanes[j].Plane().Strength()==0 )
					{
						aiplanes[j].SendOverNetwork( false );
					}
				}
			}
			for ( j=0; j<numAIFlak; j++ )
			{
				size_t s = aiflak[j].Flak().netSet(myId,j,temp);
				if ( aiflak[j].SendOverNetwork() )
				{
					gc.Add( vtFlak, j, s, temp );
					count++;
					if ( aiflak[j].Flak().Strength()==0 )
					{
						aiflak[j].SendOverNetwork( false );
					}
				}
			}
			for ( j=0; j<numAIMG42; j++ )
			{
				size_t s = aimg42[j].MG42().netSet(myId,j,temp);
				if ( aimg42[j].SendOverNetwork() )
				{
					gc.Add( vtMG42, j, s, temp );
					count++;
					if ( aimg42[j].MG42().Strength()==0 )
					{
						aimg42[j].SendOverNetwork( false );
					}
				}
			}
			for ( j=0; j<numV2; j++ )
			{
				size_t s = v2[j].netSet(myId,j,temp);
				if ( v2[j].SendOverNetwork() )
				{
					gc.Add( vtV2, j, s, temp );
					count++;
					if ( v2[j].Strength()==0 )
					{
						v2[j].SendOverNetwork( false );
					}
				}
			}
			if ( count>0 )
			if ( multiplayerPlaying[i] )
			{
//				gc.SetScores( scores );
				size_t s = gc.Set( buf );
				nwWriteNetworkBuffer( i, buf, s );
			}
		} // end of if
	}
};


void TApp::ClientNetworkLogic( void )
{
	// see if there's a message waiting on any of the active sockets
	byte buf[kNetworkBufferSize];
	size_t size;
	while ( nwReadNetworkBuffer( buf, size ) )
	if ( size>0 )
	{
		int index = 0;
		byte type = buf[index];
		switch ( type )
		{
			case id_youcan:
			{
				DataYouCan resp;
				resp.Get( buf );
				if ( resp.accept )
				{
					TString msgStr = resp.reason;
					map = resp.map; // set map for client
					myId = resp.playerId;
					msg->AddMessage( msgStr );
					clientError = false;
					clientErrorStr = msgStr;

					DataICanJoin reply(myId,true,255);
					size_t s = reply.Set(buf);
					nwWriteNetworkBuffer( buf, s );
				}
				else
				{
					nwStopNetwork();
					TString msgStr = STR(strServerRefused) + resp.reason;
					msg->AddMessage( msgStr );
					clientError = true;
					clientErrorStr = msgStr;
				}
				break;
			}

			case id_landscapeStatus:
			{
				DataLandscapeStatus stat;
				stat.Get( buf );
				PreCond( landObj!=NULL );
				landObj->SetLandscapeObjectStatus( stat.size, stat.data );
				break;
			}

			case id_otherplayerjoined:
			{
				DataOtherPlayerJoined resp;
				resp.Get( buf );
				
				PreCond( resp.team < kMaxTeams );
				AddPlayer( resp.playerId, resp.team, resp.character, resp.name );

				TString msgStr = resp.name + " ";
				msgStr = msgStr + STR(strJoined);
				msg->AddMessage( msgStr );
				break;
			}

			case id_otherplayerquit:
			{
				DataOtherPlayerQuit resp;
				resp.Get( buf );

				size_t playerId = resp.playerId;
				if ( multiplayerSlotInUse[playerId] )
				{
					TString msgStr = multiplayerNames[playerId] + " ";
					msgStr = msgStr + STR(strQuit);
					msg->AddMessage( msgStr );
					multiplayerSlotInUse[playerId] = false;
					multiplayerNames[playerId] = "";
				}
				break;
			}

			case id_quit:
			{
				DataQuit resp;
				resp.Get( buf );
				nwStopNetwork();
				TString msgStr = STR(strServerQuit) + resp.response;
				msg->AddMessage( msgStr );

				// remove all other players visible
				for ( size_t i=0; i<kMaxPlayers; i++ )
				{
					if ( i!=myId )
					{
						multiplayerPlaying[i] = false;
						multiplayerSlotInUse[i] = false;
						multiplayerNames[i] = "";
					}
				}
				break;
			}

			case id_welcome:
			{
				if ( !clientReceivedWelcome )
				{
					clientReceivedWelcome = true;
					DataWelcome resp;
					resp.Get( buf );
					gameType = resp.gameType;
					maxPoints = resp.maxPoints;
					map = resp.map;
				}
				else
				{
					clientReceivedStart = true;

					DataWelcome resp;
					resp.Get( buf );
					gameType = resp.gameType;
					maxPoints = resp.maxPoints;
					map = resp.map;

					// the client deals with the adding of new
					// players after its loaded everything else
					// this sets the data and calls AddPlayer
					// in IDM_JOIN in tankapp_menu.cpp
					for ( size_t i=0; i<resp.cntr; i++ )
					{
						byte playerId = resp.playerId[i];

						team[playerId] = resp.team[i];
						character[playerId] = resp.character[i];
						multiplayerNames[playerId] = resp.name[i];
						multiplayerSlotInUse[playerId] = true;
						multiplayerPlaying[playerId] = true;
					}
				}
				break;
			}

			case id_globalcoords:
			{
				if ( clientGame )
				{
					DataGlobalCoords resp;
					resp.Get( buf );
					for ( size_t i=0; i<resp.cntr; i++ )
					{
						byte temp[kMaxSingleCoordSize]; // const defined in packet.h
						size_t vehicleType, vehicleId;
						size_t s;
						resp.Read( i, vehicleType, vehicleId, s, temp );

						switch ( vehicleType )
						{
						case vtNone:
							{
								if ( characters[vehicleId].Character()!=NULL )
								{
									characters[vehicleId].netGet(temp);
								}
								break;
							};
						case vtTank:
							{
								TTank& tank = aitanks[vehicleId].Tank();
								if ( tank.Strength() > 0 )
								{
									tank.netGet(temp);
								}
								break;
							};
						case vtPlane:
							{
								TPlane& plane = aiplanes[vehicleId].Plane();
								if ( plane.Strength() > 0 )
								{
									plane.netGet(temp);
								}
								break;
							};
						case vtMG42:
							{
								TMG42& mg42 = aimg42[vehicleId].MG42();
								if ( mg42.Strength() > 0 )
								{
									mg42.netGet(temp);
								}
								break;
							};
						case vtArtillery:
							{
								TArtillery& artillery = aiartillery[vehicleId].Artillery();
								if ( artillery.Strength() > 0 )
								{
									artillery.netGet(temp);
								}
								break;
							};
						case vtFlak:
							{
								TFlak& flak = aiflak[vehicleId].Flak();
								if ( flak.Strength() > 0 )
								{
									flak.netGet(temp);
								};
								break;
							};
						case vtV2:
							{
								V2& _v2 = v2[vehicleId];
								if ( _v2.Strength() > 0 )
								{
									_v2.netGet(temp);
								};
								break;
							};
						}
					}

					// update scores?
					if ( resp.containsScores==1 )
					{
						for ( size_t i=0; i<kMaxPlayers; i++ )
							scores[i] = resp.scores.scores[i];
					}
				}
				break;
			}

			case id_projectile:
			{
				FireProjectile( buf );
				break;
			}

			case id_explosive:
			{
				DropExplosive( buf );
				break;
			}

			case id_strengthUpdate:
			{
				DataStrengthUpdate up;
				up.Get( buf );
				for ( size_t i=0; i<up.cntr; i++ )
				{
					size_t vehicleType = up.vehicleType[i];
					size_t vehicleId = up.vehicleId[i];
					switch ( vehicleType )
					{
					case vtNone:
					{
						characters[vehicleId].Strength( up.strength[i] );
						break;
					}
					case vtPlane:
					{
						planes[vehicleId].Strength( up.strength[i] );
						break;
					}
					case vtFlak:
					{
						flak[vehicleId].Strength( up.strength[i] );
						break;
					}
					case vtArtillery:
					{
						artillery[vehicleId].Strength( up.strength[i] );
						break;
					}
					case vtTank:
					{
						tanks[vehicleId].Strength( up.strength[i] );
						break;
					}
					case vtMG42:
					{
						mg42[vehicleId].Strength( up.strength[i] );
						break;
					}
					case vtV2:
					{
						v2[vehicleId].Strength( up.strength[i] );
						break;
					}
					};
				}
				break;
			}

			case id_buildingStrengthUpdate:
			{
				DataBuildingStrengthUpdate up;
				up.Get( buf );
				for ( size_t i=0; i<up.cntr; i++ )
				{
					if ( landObj!=NULL )
					{
						landObj->UpdateLandscapeObjectStrength( up.buildingId[i], up.strength[i] );
					}
				}
				break;
			}

		}
	}

	// send my data to server
	if ( clientGame )
	{
		byte temp[kMaxSingleCoordSize]; // const defined in packet.h
		size_t s = 0;
		
		size_t vehicleType = characters[myId].VehicleType();
		size_t vehicleId = characters[myId].VehicleId();

		switch ( vehicleType )
		{
		case vtNone:
			{
				s = characters[myId].netSet(myId,myId,temp);
				break;
			}
		case vtTank:
			{
				s = tanks[vehicleId].netSet(myId,vehicleId,temp);
				break;
			}
		case vtPlane:
			{
				s = planes[vehicleId].netSet(myId,vehicleId,temp);
				break;
			}
		case vtFlak:
			{
				s = flak[vehicleId].netSet(myId,vehicleId,temp);
				break;
			}
		case vtMG42:
			{
				s = mg42[vehicleId].netSet(myId,vehicleId,temp);
				break;
			}
		case vtArtillery:
			{
				s = artillery[vehicleId].netSet(myId,vehicleId,temp);
				break;
			}
		}
		if ( s>0 )
		{
			nwWriteNetworkBuffer( temp, s );
		}
	}
};


