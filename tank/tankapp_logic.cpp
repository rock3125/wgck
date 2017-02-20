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
#include <systemDialogs/tipsDlg.h>

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

void TApp::Logic( void )
{
	TEvent::Logic(); // gets/sets shift & ctrl states

	showTarget	= shiftDown;

	// increase overall logic counter - used by AIs
	logicCounter++;

	if ( showMenu )
	{
		LogicGameMenu();

		if ( !tipsShown )
		{
			tipsShown = true;
			TipsDialog dlg(this);
			dlg.Execute(tips);
		}

		aboutAngle = aboutAngle + 5;
	}
	if ( singlePlayerMission || serverGame || clientGame )
	{
		LogicSinglePlayer();
	}

	// process network traffic
	if ( nwServerActive() )
	{
		ServerNetworkLogic();
	}
	if ( nwClientActive() )
	{
		ClientNetworkLogic();
	}

	if ( msg!=NULL )
		msg->ProcessDisplayMessages();
}


void TApp::LogicSinglePlayer( void )
{
	// just let the user have a play - single user mode, not yet connected
	if ( !gameOver && !missionAccomplished )
	{
		GameStatus(gameOver,missionAccomplished);
		if ( gameOver || missionAccomplished )
		{
			animCount = 0;
			StopMusic();
		}
	}
	if ( gameOver )
	{
		keys = 0;
		if ( animCount<30 )
		{
			animCount++;
		}
		else
		{
			::ReleaseMouse();
			showMenu = true;

			fog = false;
			SetupFog( false );
			StopSounds();
		}
	}
	else if ( missionAccomplished )
	{
		keys = 0;
		if ( animCount<30 )
		{
			animCount++;
		}
		else
		{
			::ReleaseMouse();
			showMenu = true;

			fog = false;
			SetupFog( false );
			StopSounds();
		}
	}
	if ( initialised && landObj!=NULL )
	{
		TSmoke::Logic();
		LogicSinglePlayer( *landObj, characters[myId], keys, ctrlDown, showTarget );
	}
};


// logic explosion
void TApp::ProjectileLogic( void )
{
	for ( size_t i=0; i<kMaxPlayers; i++ )
	{
		if ( projectiles[i].InUse() )
		{
			projectiles[i].Logic(landObj,params);
		}
	}
};


bool TApp::Occupied( size_t vehicleType, size_t vehicleId )
{
	if ( serverGame || clientGame )
	{
		for ( size_t i=0; i<kMaxPlayers; i++ )
		{
			if ( i!=myId )
			if ( multiplayerPlaying[i] )
			if ( characters[i].VehicleType()==vehicleType && 
				 characters[i].VehicleId()==vehicleId )
			{
				return true;
			}
		}
	}
	return false;
};


bool TApp::LogicSinglePlayer( TLandscape& landObj, TCharacter& player,
							  size_t& _keys, bool _ctrlDown, bool _showTarget )
{
	size_t i;

	if ( !landObj.Initialised() || showMenu )
		return false;

	if ( cameraType>=5 && showTarget )
		deltaYAngle = -deltaYAngle;

	float dxa = deltaXAngle;
	float dya = deltaYAngle;

	if ( showMap )
	{
		if ( dxa<0 && mapScale<maxMapScale )
		{
			mapScale += mapScaleIncrease;
		}
		else if ( dxa>0 && mapScale>minMapScale )
		{
			mapScale -= mapScaleIncrease;
		}
		dya = 0;
		dxa = 0;
	}

	bool fire = false;
	size_t vehicleId = player.VehicleId();
	size_t vehicleType = player.VehicleType();

	switch ( vehicleType )
	{
	case vtNone:
		{
			player.Move( _keys, landObj.WaterLevel(), showTarget, dxa, dya );
			player.Logic();
			size_t vehicleId = 0;
			if ( player.LegalCharacterMove( landObj, params, vehicleType, vehicleId ) )
			{
				switch ( vehicleType )
				{
				case vtNone:
					{
						player.CommitMove();
						player.VehicleType( vehicleType );
						player.VehicleId( vehicleId );
						break;
					}
				case vtTank:
					{
						if ( tanks[vehicleId].CanEnter() && !Occupied(vehicleType,vehicleId) )
						{
							player.CommitMove();
							player.JumpIntoVehicle();
							tanks[vehicleId].Team( player.Team() ); // force team
							player.VehicleType( vehicleType );
							player.VehicleId( vehicleId );
						}
						else
						{
							player.CommitAngles();
							if ( tanks[vehicleId].IsaAI() && 
								 tanks[vehicleId].Speed()>0 )
							{
								player.Strength(0);
							}
							else
							{
								vehicleType = vtNone;
							}
						}
						break;
					}
				case vtPlane:
					{
						if ( planes[vehicleId].CanEnter()  && !Occupied(vehicleType,vehicleId) )
						{
							player.CommitMove();
							player.JumpIntoVehicle();
							planes[vehicleId].Team( player.Team() ); // force team
							player.VehicleType( vehicleType );
							player.VehicleId( vehicleId );
						}
						else
						{
							player.CommitAngles();
							if ( planes[vehicleId].IsaAI() &&
								 planes[vehicleId].Speed()>0 )
							{
								player.Strength(0);
							}
							else
							{
								vehicleType = vtNone;
							}
						}
						break;
					}
				case vtFlak:
					{
						if ( flak[vehicleId].CanEnter()  && !Occupied(vehicleType,vehicleId) )
						{
							player.CommitMove();
							player.JumpIntoVehicle();
							flak[vehicleId].Team( player.Team() ); // force team
							player.VehicleType( vehicleType );
							player.VehicleId( vehicleId );
						}
						else
						{
							player.CommitAngles();
							if ( flak[vehicleId].IsaAI() )
							{
								player.Strength(0);
							}
							else
							{
								vehicleType = vtNone;
							}
						}
						break;
					}
				case vtArtillery:
					{
						if ( artillery[vehicleId].CanEnter()  && !Occupied(vehicleType,vehicleId) )
						{
							player.CommitMove();
							player.JumpIntoVehicle();
							artillery[vehicleId].SetSoldier( &player );
							player.SetupForMG42();
							artillery[vehicleId].Team( player.Team() ); // force team
							player.VehicleType( vehicleType );
							player.VehicleId( vehicleId );
						}
						else
						{
							player.CommitAngles();
							if ( artillery[vehicleId].IsaAI() &&
								 artillery[vehicleId].Speed()>0 )
							{
								player.Strength(0);
							}
							else
							{
								vehicleType = vtNone;
							}
						}
						break;
					}
				case vtMG42:
					{
						if ( mg42[vehicleId].CanEnter()  && !Occupied(vehicleType,vehicleId) )
						{
							player.CommitMove();
							player.JumpIntoVehicle();
							mg42[vehicleId].SetSoldier( &player );
							player.SetupForMG42();
							mg42[vehicleId].Team( player.Team() ); // force team
							player.VehicleType( vehicleType );
							player.VehicleId( vehicleId );
						}
						else
						{
							player.CommitAngles();
							if ( mg42[vehicleId].IsaAI() )
							{
								player.Strength(0);
							}
							else
							{
								vehicleType = vtNone;
							}
						}
						break;
					}
				}
			}
			else // not a legal move - allow rotation though
			{
				player.CommitAngles();
			}
			_keys = _keys & ~aJump;
			break;
		}
	case vtTank:
		{
			TTank& tank = tanks[vehicleId];
			if ( tank.Strength() == 0 )
			{
				player.Strength(0);
			}
			else
			{
				player.X( tank.X() );
				player.Y( tank.Y() );
				player.Z( tank.Z() );
			}

			fire = tank.Move( landObj, _keys, showTarget, dxa, dya, &cam );
			if ( tank.LegalVehicleMove( &landObj, params ) )
			{
				tank.CommitMove();
			}
			else
				tank.AbortMove();

			if ( (_keys&aJump)>0)
			{
				player.VehicleType( vtNone );
				player.VehicleId( 0 );
				vehicleType = vtNone;
				vehicleId = 0;
				player.JumpOutOfVehicle( tank );
				tank.StopSounds();
				player.Logic();
				_keys = 0;
			}
			break;
		}
	case vtPlane:
		{
			TPlane& plane = planes[vehicleId];
			if ( plane.Strength()==0 )
			{
				player.Strength(0);
			}
			else
			{
				player.X( plane.X() );
				player.Y( plane.Y() );
				player.Z( plane.Z() );
			}

			plane.Move( _keys, showTarget, dxa, dya, &cam );
			if ( plane.LegalVehicleMove( &landObj, params ) )
			{
				plane.CommitMove();
			}
			else
				plane.AbortMove();

			if ( (_keys&aJump)>0)
			{
				player.VehicleType( vtNone );
				player.VehicleId( 0 );
				vehicleType = vtNone;
				vehicleId = 0;
				player.JumpOutOfVehicle( plane );
				player.Logic();
				_keys = 0;
			}
			break;
		}
	case vtFlak:
		{
			TFlak& flak1 = flak[vehicleId];
			if ( flak1.Strength()==0 )
			{
				player.Strength(0);
			}
			else
			{
				player.X( flak1.X() );
				player.Y( flak1.Y() );
				player.Z( flak1.Z() );
			}

			flak1.Move( _keys, showTarget, dxa, dya );
			if ( (_keys&aJump)>0)
			{
				player.VehicleType( vtNone );
				player.VehicleId( 0 );
				vehicleType = vtNone;
				vehicleId = 0;
				player.JumpOutOfVehicle( flak1 );
				player.Logic();
				_keys = 0;
			}
			break;
		}
	case vtArtillery:
		{
			TArtillery& artillery1 = artillery[vehicleId];
			if ( artillery1.Strength()==0 )
			{
				player.Strength(0);
			}
			else
			{
				player.X( artillery1.X() );
				player.Y( artillery1.Y() );
				player.Z( artillery1.Z() );
			}

			artillery1.Move( _keys, showTarget, dxa, dya );
			if ( artillery1.LegalVehicleMove( &landObj, params ) )
			{
				artillery1.CommitMove();
			}
			else
				artillery1.AbortMove();

			if ( (_keys&aJump)>0)
			{
				player.VehicleType( vtNone );
				player.VehicleId( 0 );
				vehicleType = vtNone;
				vehicleId = 0;
				artillery1.RemoveSoldier();
				player.JumpOutOfVehicle( artillery1 );
				player.Logic();
				_keys = 0;
			}
			break;
		}

	case vtMG42:
		{
			TMG42& mg42_1 = mg42[vehicleId];
			if ( mg42_1.Strength()==0 )
			{
				player.Strength(0);
			}
			else
			{
				player.X( mg42_1.X() );
				player.Y( mg42_1.Y() );
				player.Z( mg42_1.Z() );
			}

			mg42_1.Move( _keys, showTarget, dxa, dya );
			if ( (_keys&aJump)>0)
			{
				player.VehicleType( vtNone );
				player.VehicleId( 0 );
				vehicleType = vtNone;
				vehicleId = 0;
				player.JumpOutOfVehicle( mg42_1 );
				mg42_1.RemoveSoldier();
				player.Logic();
				_keys = 0;
			}
			break;
		}
	}

	// process logic for all other players' characters
	if ( !singlePlayerMission )
	{
		for ( i=0; i<kMaxPlayers; i++ )
		{
			if ( multiplayerSlotInUse[i] )
			if ( characters[i].VehicleType()==vtNone )
			{
				characters[i].Logic();
			}
		}
	}

	// process logic for other planes
	if ( (!gameOver && !missionAccomplished) || serverGame || clientGame )
	{
		for ( i=0; i<numPlanes; i++ )
		{
			if (!(vehicleType==vtPlane && vehicleId==i  || planes[i].IsaAI()))
			{
				size_t key = 0;
				planes[i].Move( key, false, 0, 0, &cam );
				{
					if ( planes[i].LegalVehicleMove( &landObj, params ) )
						planes[i].CommitMove();
					else
						planes[i].AbortMove();
				}
			}

			// check resources for replenishment
			for ( size_t j=0; j<numResources; j++ )
			{
				if ( planes[i].Strength()>0 && planes[i].Speed()==0 )
				if ( resources[j]->Strength()>0 )
				{
					float dx = resources[j]->TX() - planes[i].X();
					float dy = resources[j]->TY() - planes[i].Y();
					float dz = resources[j]->TZ() - planes[i].Z();
					float d =  dx*dx + dy*dy + dz*dz;
					if ( d < 100 )
					{
						if ( resources[j]->Armory() )
						{
							planes[i].Rearm();
						}
						if ( resources[j]->RefuelStation() )
						{
							planes[i].Refuel();
						}
						if ( resources[j]->Repairs() )
						{
							planes[i].Repair();
						}
					}
				}
			}
		}

		// process logic for other tanks
		for ( i=0; i<numTanks; i++ )
		{
			if (!(vehicleType==vtTank && vehicleId==i || tanks[i].IsaAI()))
			{
				size_t keys = 0;
				tanks[i].Move( landObj, keys, false, 0, 0, &cam );
				{
					if ( tanks[i].LegalVehicleMove( &landObj, params ) )
						tanks[i].CommitMove();
					else
						tanks[i].AbortMove();
				}
			}

			// check resources for replenishment
			for ( size_t j=0; j<numResources; j++ )
			{
				if ( tanks[i].Strength()>0 )
				if ( resources[j]->Strength()>0 )
				{
					float dx = resources[j]->TX() - tanks[i].X();
					float dy = resources[j]->TY() - tanks[i].Y();
					float dz = resources[j]->TZ() - tanks[i].Z();
					float d =  dx*dx + dy*dy + dz*dz;
					if ( d < 100 )
					{
						if ( resources[j]->Armory() )
						{
							tanks[i].Rearm();
						}
						if ( resources[j]->RefuelStation() )
						{
							tanks[i].Refuel();
						}
						if ( resources[j]->Repairs() )
						{
							tanks[i].Repair();
						}
					}
				}
			}
		}
	}

	// check resources for player replenishment
	for ( size_t j=0; j<numResources; j++ )
	{
		if ( resources[j]->Strength()>0 )
		{
			float dx = resources[j]->TX() - player.X();
			float dy = resources[j]->TY() - player.Y();
			float dz = resources[j]->TZ() - player.Z();
			float d =  dx*dx + dy*dy + dz*dz;
			if ( d < 100 )
			{
				if ( resources[j]->Armory() )
				{
					player.Rearm();
				}
			}
		}
	}

	// process AI logic
	if ( (!gameOver && !missionAccomplished && !clientGame) || serverGame )
	{
		for ( i=0; i<numAITanks; i++ )
		{
			aitanks[i].Logic( this,landObj,landscapeAI,params);
		}

		for ( i=0; i<numAIPlanes; i++ )
		{
			aiplanes[i].Logic( this,landObj,landscapeAI,params);
		}

		for ( i=0; i<numAIFlak; i++ )
		{
			aiflak[i].Logic( this,landObj,landscapeAI,params);
		}

		for ( i=0; i<numAIArtillery; i++ )
		{
			aiartillery[i].Logic( this,landObj,landscapeAI,params);
		}

		for ( i=0; i<numAIMG42; i++ )
		{
			aimg42[i].Logic( this,landObj,landscapeAI,params);
		}
		for ( i=0; i<numV2; i++ )
		{
			v2[i].Logic(this,landObj,landscapeAI,params);
		}
	}

	switch ( vehicleType )
	{
	case vtNone:
		{
			cam.SetPersonCamera( landObj, player, params, !_ctrlDown, 
								 deltaXAngle, deltaYAngle, showMap );
			break;
		}
	case vtTank:
		{
			cam.SetVehicleCamera( landObj, tanks[vehicleId],params, !_ctrlDown, 
								  deltaXAngle, deltaYAngle, showMap );
			break;
		};
	case vtPlane:
		{
			cam.SetVehicleCamera( landObj, planes[vehicleId],params, !_ctrlDown, 
								  deltaXAngle, deltaYAngle, showMap );
			break;
		}
	case vtFlak:
		{
			cam.SetVehicleCamera( landObj, flak[vehicleId],params, !_ctrlDown, 
								  deltaXAngle, deltaYAngle, showMap );
			break;
		}
	case vtArtillery:
		{
			cam.SetVehicleCamera( landObj, artillery[vehicleId],params, !_ctrlDown, 
								  deltaXAngle, deltaYAngle, showMap );
			break;
		}
	case vtMG42:
		{
			cam.SetVehicleCamera( landObj, mg42[vehicleId],params, !_ctrlDown, 
								  deltaXAngle, deltaYAngle, showMap );
			break;
		}
	}
	deltaXAngle = 0;
	deltaYAngle = 0;

	// logic for projectiles & explosions
	ProjectileLogic();

	// logic for explosives
	for ( i=0; i<kMaxPlayers; i++ )
	{
		explosives[i].Logic();
	}

	// set listener sound location
	if ( soundSystem!=NULL )
	{
		TPoint camPos = TPoint( cam.ListenX(),cam.ListenY(),cam.ListenZ() );
		TPoint fwd;
		TPoint up = TPoint(0,1,0);

		float ya = cam.YAngle() * Deg2Rad;
		float v1fcos = (float)cos(ya);
		float v1fsin = (float)sin(ya);

		fwd.x = -v1fsin;
		fwd.y = 0;
		fwd.z = v1fcos;

		soundSystem->SetCameraLocation( camPos, fwd, up );

		// background sound in action?
		if ( !backgroundSoundPlaying )
		{
			if ( backgroundSound>0 )
			{
				backgroundSoundPlaying = true;
				backgroundSoundChannel = soundSystem->Play( backgroundSound, camPos );
			}
		}
		else
		{
			soundSystem->SetLocation( backgroundSoundChannel, camPos, camPos );
		}
	}

	// have we visited any more of the sequence locations?
	bool done = false;
	for ( i=0; i<numSequenceLocations && !done; i++ )
	{
		if ( !sequenceLocations[i].visited )
		{
			float dx = (player.X() - sequenceLocations[i].x);
			float dy = (player.Y() - sequenceLocations[i].y);
			float dz = (player.Z() - sequenceLocations[i].z);
			float dist = dx*dx + dy*dy + dz*dz;
			done = true;
			if ( dist < 10 )
			{
				sequenceLocations[i].visited = true;
			}
		}
	}

	return fire;
};

//==========================================================================

