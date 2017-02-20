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

#include <network/packet.h>

#include <io.h>

//==========================================================================

#include <mmsystem.h>

//==========================================================================

const float Deg2Rad = 0.01745329252f;
const float Rad2Deg = 57.295779511273f;

//==========================================================================
/*
void TApp::DrawScores( void )
{
	size_t i;

	float y = 400;
	float x = 110;
	float ypos = y;
	if ( gameType==0 )
	{
		for ( i=0; i<kMaxPlayers; i++ )
		{
			if ( net->Used(i) )
				ypos -= 12;
		}

		glEnable( GL_COLOR_MATERIAL );
		glEnable( GL_BLEND );
		glColor4ub( 0,0,0, 100 );
			glBegin( GL_QUADS );
			glVertex3f( x,y+34,-11);
			glVertex3f( x,ypos+6,-11 );
			glVertex3f( x+310,ypos+6,-11);
			glVertex3f( x+310,y+34,-11);
			glEnd();
		glDisable( GL_BLEND );

		glColor3ub( 0,0,0 );
		glBegin( GL_LINES );
			glVertex3f( x, y+16, -10 );
			glVertex3f( x+310, y+16, -10 );
			glVertex3f( x, y+34, -10 );
			glVertex3f( x+310, y+34, -10 );
		glEnd();

		ypos = y;
		glColor3ub(255,255,255);
		Write( x+10, y+20, -10, "Name" );
		Write( x+210, y+20, -10, "Score" );
		for ( i=0; i<kMaxPlayers; i++ )
		{
			if ( net->Used(i) )
			{
				Write( x+10,ypos,-10, net->Names(i) );
				Write( x+210,ypos,-10, Int2Str( scores[i] ) );
				ypos -= 12;
			}
		}

		glColor3ub(0,0,0);
		glBegin( GL_LINES );
			glVertex3f( x, ypos+6, -10 );
			glVertex3f( x+310, ypos+6, -10 );
			glVertex3f( x+310, y+34, -10 );
			glVertex3f( x+310, ypos+6, -10 );
			glVertex3f( x, y+34, -10 );
			glVertex3f( x, ypos+6, -10 );
			glVertex3f( x+206, y+34, -10 );
			glVertex3f( x+206, ypos+6, -10 );
		glEnd();
	}
	else
	{
		// calculate team scores
		size_t teamScore[kMaxTeams];
		for ( i=0; i<kMaxTeams; i++ )
			teamScore[i] = 0;
		for ( i=0; i<kMaxPlayers; i++ )
		{
			teamScore[team[i]] += scores[i];
		}
		size_t winningTeam = kMaxTeams;
		size_t winningTeamScore = 0;
		for ( i=0; i<kMaxTeams; i++ )
		{
			if ( teamScore[i] > winningTeamScore )
			{
				winningTeamScore = teamScore[i];
				winningTeam = i;
			}
			if ( teamScore[i] >= maxPoints )
				gameOver = true;
		}

		x = 90;
		for ( i=0; i<kMaxPlayers; i++ )
		{
			if ( net->Used(i) )
				ypos -= 12;
		}

		glEnable( GL_COLOR_MATERIAL );
		glEnable( GL_BLEND );
		glColor4ub( 0,0,0, 100 );
			glBegin( GL_QUADS );
			glVertex3f( x,y+34,-11);
			glVertex3f( x,ypos+6,-11 );
			glVertex3f( x+350,ypos+6,-11);
			glVertex3f( x+350,y+34,-11);
			glEnd();
		glDisable( GL_BLEND );

		glColor3ub( 0,0,0 );
		glBegin( GL_LINES );
			glVertex3f( x, y+16, -10 );
			glVertex3f( x+350, y+16, -10 );
			glVertex3f( x, y+34, -10 );
			glVertex3f( x+350, y+34, -10 );
		glEnd();

		float ypos = y;
		glColor3ub( 255,255,255 );
		Write( x+10, y+20, -10, "Name" );
		Write( x+210, y+20, -10, "Team" );
		Write( x+250, y+20, -10, "Score" );
		for ( i=0; i<kMaxPlayers; i++ )
		{
			if ( net->Used(i) )
			{
				Write( x+10,ypos,-10, net->Names(i) );
				Write( x+210,ypos,-10, Int2Str( team[i]+1 ) );

				TString scoreStr = "team:" + Int2Str( teamScore[team[i]] ) + " (me:" + Int2Str( scores[i] ) + ")";
				Write( x+250,ypos,-10, scoreStr );
				ypos -= 12;
			}
		}

		glColor3ub( 0,0,0 );
		glBegin( GL_LINES );
			glVertex3f( x, ypos+6, -10 );
			glVertex3f( x+350, ypos+6, -10 );
			glVertex3f( x+350, y+34, -10 );
			glVertex3f( x+350, ypos+6, -10 );
			glVertex3f( x, y+34, -10 );
			glVertex3f( x, ypos+6, -10 );
			glVertex3f( x+206, y+34, -10 );
			glVertex3f( x+206, ypos+6, -10 );
			glVertex3f( x+246, y+34, -10 );
			glVertex3f( x+246, ypos+6, -10 );
		glEnd();

		if ( gameOver )
		{
			if ( winningTeam < kMaxTeams )
			{
				TString wonStr = "T E A M  " + Int2Str(winningTeam) + "  W O N";
				glColor3ub( 0,0,0 );
				Write( x + 150,  ypos - 40, -10, wonStr );
			}
		}
	}
	if ( gameOver )
	{
		glColor3ub( 0,0,0 );
		Write( x + 150,  ypos - 20, -10, "G A M E   O V E R" );
	}
};

void TApp::RenderClientServer( size_t playerId )
{
	// just let the user have a play - single user mode
	DataParticulars& dud = players[playerId];
	if ( landObj!=NULL )
		RenderClientServer( *landObj, dud );

	SetupGL2d( vdepth, vwidth, vheight );
	glLoadIdentity();
	glEnable( GL_COLOR_MATERIAL );

	// draw display of ammo or something
	TTexture* weaponsIcon = NULL;
	TTexture* vehicleIcon = NULL;

	bool	drawFuel = false;
	size_t	amountAmmo;
	size_t	strengthAmount;
	float	fuelAmount;

	switch ( dud.vehicleType )
	{
	case vtNone:
		{
			vehicleIcon = iconPerson[character[0]];
			weaponsIcon = iconWeapon[characters[0]->SelectedGun()];
			
			amountAmmo = characters[0]->Ammo();
			strengthAmount = characters[0]->Strength();
			fuelAmount = 0;

			break;
		}
	case vtTank:
		{
			vehicleIcon = iconVehicle[tanks[dud.vehicleId].IconId()];
			weaponsIcon = iconWeapon[4]; // shell
			drawFuel = true;

			amountAmmo = tanks[dud.vehicleId].Ammo();
			strengthAmount = size_t(float(tanks[dud.vehicleId].Strength() - 33) * 1.33f);
			if ( strengthAmount>100 )
				strengthAmount = 0;
			fuelAmount = tanks[dud.vehicleId].Fuel();
			break;
		}
	case vtPlane:
		{
			vehicleIcon = iconVehicle[planes[dud.vehicleId].IconId()];
			switch ( planes[dud.vehicleId].SelectedGun() )
			{
			case 0:
				{
					weaponsIcon = iconWeapon[6]; // machine gun
					break;
				}
			case 1:
				{
					weaponsIcon = iconWeapon[5]; // bomb
					break;
				}
			case 2:
				{
					weaponsIcon = iconWeapon[4]; // shell
					break;
				}
			};
			drawFuel = true;
			TPlane& plane = planes[dud.vehicleId];
			amountAmmo = plane.Ammo();
			strengthAmount = plane.Strength();
			fuelAmount = plane.Fuel();
			break;
		}
	case vtFlak:
		{
			vehicleIcon = iconVehicle[flak[dud.vehicleId].IconId()];
			weaponsIcon = iconWeapon[4]; // shell
			TFlak& flak1 = flak[dud.vehicleId];
			amountAmmo = flak1.Ammo();
			strengthAmount = flak1.Strength();
			fuelAmount = 0;
			break;
		}
	case vtArtillery:
		{
			vehicleIcon = iconVehicle[artillery[dud.vehicleId].IconId()];
			weaponsIcon = iconWeapon[4]; // shell
			TArtillery& artillery1 = artillery[dud.vehicleId];
			amountAmmo = artillery1.Ammo();
			strengthAmount = artillery1.Strength();
			fuelAmount = artillery1.Fuel();
			break;
		}
	case vtMG42:
		{
			vehicleIcon = iconVehicle[mg42[dud.vehicleId].IconId()];
			weaponsIcon = iconWeapon[6]; // mg42
			TMG42& mg42_1 = mg42[dud.vehicleId];
			amountAmmo = mg42_1.Ammo();
			strengthAmount = mg42_1.Strength();
			fuelAmount = 0;
			break;
		}
	}

	glPushMatrix();
		glTranslatef( 0,0, -11 );
		if ( vehicleIcon!=NULL )
		{
			vehicleIcon->Draw( 74,10,10,74 );
		}
		if ( weaponsIcon!=NULL )
		{
			weaponsIcon->Draw( 138,10,74,74 );
		}
	glPopMatrix();


	if ( drawFuel )
	{
		DrawBar( 12,10,-10, strengthAmount, 60 );
		DrawBar( 12,14,-10, size_t(fuelAmount), 60 );
	}
	else
	{
		DrawBar( 12,14,-10, strengthAmount, 60 );
	}

	glColor3ub( 255,255,255 );
	Write( 110, 10, -10, Int2Str( amountAmmo) );

	msg->DisplayMessages();

	if ( gameOver )
	{
		SetupGL2d( vdepth, vwidth, vheight );
		glLoadIdentity();
		glEnable( GL_COLOR_MATERIAL );
		glColor3ub( 255,255,255 );

		glTranslatef( vwidth*0.5f,vheight*0.85f, -3 );

		float scale = animCount*10;
		glScalef( scale,scale,scale );

		if ( gameOverText!=NULL )
			gameOverText->Draw();
	}
	else if ( missionAccomplished )
	{
		SetupGL2d( vdepth, vwidth, vheight );
		glLoadIdentity();
		glEnable( GL_COLOR_MATERIAL );
		glColor3ub( 255,255,255 );

		glTranslatef( vwidth*0.5f,vheight*0.85f, -3 );

		float scale = animCount*10;
		glScalef( scale,scale,scale );

		if ( missionAccomplishedText!=NULL )
			missionAccomplishedText->Draw();
	}

	// set countdown message
	size_t cd = 10000;
	bool found = false;
	for ( size_t i=0; i<numV2; i++ )
	{
		if ( v2[i].Strength() > 0 )
		if ( v2[i].CountDown()>0 && v2[i].CountDown() < cd )
		{
			cd = v2[i].CountDown();
			found = true;
		}
	}
	if ( found )
	{
		size_t mins = cd / 60;
		size_t secs = cd % 60;
		TString secStr = Int2Str( secs );
		if ( secStr.length()==1 )
		{
			secStr = "0" + secStr;
		}
		TString msg = Int2Str( mins ) + ":" + secStr;
		SetCountDown( msg );
	}
	else
	{
		SetCountDown( "" );
	}
};
*/

/*
void TApp::RenderClientServer( TLandscape& landObj, const DataParticulars& player )
{
	SetupGL3d( vdepth, vwidth, vheight );

	if ( showMap )
	{
		glEnable( GL_NORMALIZE );
		glLoadIdentity();
		glTranslatef( 0, 0, -10 );
	    glRotatef( cam.XAngle(), 1,0,0 );
		glRotatef( cam.YAngle(), 0,1,0 );

		glScalef( mapScale, mapScale, mapScale );

		TBinObject* smallMap = landObj.SmallMap();
		smallMap->Draw();

		// draw all vehicles on the map
		glDisable( GL_BLEND );
		glEnable( GL_COLOR_MATERIAL );
		glColor3ub(255,255,255);

		size_t i;
		float x,y,z;
		for ( i=0; i<numTanks; i++ )
		{
			TTank& tank = tanks[i];
			byte r = 100;
			byte g = 200;
			byte b = 100;
			if ( tank.IsaAI() )
			{
				if ( aitanks[tank.AIID()].AITeam()==0 )
				{
					r = 200;
					g = 100;
					b = 100;
				}
			}
			if ( tank.Strength()>0 )
			{
				x = tank.MX()*0.5f;
				y = tank.MY()*0.5f;
				z = tank.MZ()*0.5f;
				DrawFlag( x,y,z, r,g,b );
			}
		}
		for ( i=0; i<numPlanes; i++ )
		{
			TPlane& plane = planes[i];
			byte r = 100;
			byte g = 200;
			byte b = 100;
			if ( plane.IsaAI() )
			{
				if ( aiplanes[plane.AIID()].AITeam()==0 )
				{
					r = 200;
					g = 100;
					b = 100;
				}
			}
			if ( plane.Strength()>0 )
			{
				x = plane.X()*0.5f;
				y = plane.Y()*0.5f;
				z = plane.Z()*0.5f;
				DrawFlag( x,y,z, r,g,b );
			}
		}
		for ( i=0; i<numFlak; i++ )
		{
			TFlak& flak1 = flak[i];
			byte r = 100;
			byte g = 200;
			byte b = 100;
			if ( flak1.IsaAI() )
			{
				if ( aiflak[flak1.AIID()].AITeam()==0 )
				{
					r = 200;
					g = 100;
					b = 100;
				}
			}
			if ( flak1.Strength()>0 )
			{
				x = flak1.X()*0.5f;
				y = flak1.Y()*0.5f;
				z = flak1.Z()*0.5f;
				DrawFlag( x,y,z, r,g,b );
			}
		}
		for ( i=0; i<numArtillery; i++ )
		{
			TArtillery& art = artillery[i];
			byte r = 100;
			byte g = 200;
			byte b = 100;
			if ( art.IsaAI() )
			{
				if ( aiartillery[art.AIID()].AITeam()==0 )
				{
					r = 200;
					g = 100;
					b = 100;
				}
			}
			if ( art.Strength()>0 )
			{
				x = art.X()*0.5f;
				y = art.Y()*0.5f;
				z = art.Z()*0.5f;
				DrawFlag( x,y,z, r,g,b );
			}
		}

		for ( i=0; i<numMG42s; i++ )
		{
			TMG42& mg42_1 = mg42[i];
			byte r = 100;
			byte g = 200;
			byte b = 100;
			if ( mg42_1.IsaAI() )
			{
				if ( aimg42[mg42_1.AIID()].AITeam()==0 )
				{
					r = 200;
					g = 100;
					b = 100;
				}
			}
			if ( mg42_1.Strength()>0 )
			{
				x = mg42_1.X()*0.5f;
				y = mg42_1.Y()*0.5f;
				z = mg42_1.Z()*0.5f;
				DrawFlag( x,y,z, r,g,b );
			}
		}

		for ( i=0; i<numV2; i++ )
		{
			V2& v2_1 = v2[i];
			byte r = 200;
			byte g = 200;
			byte b = 200;
			if ( v2_1.Strength()>0 )
			{
				x = v2_1.X()*0.5f;
				y = v2_1.Y()*0.5f;
				z = v2_1.Z()*0.5f;
				DrawFlag( x,y,z, r,g,b );
			}
		}

		if ( players[0].vehicleType==vtNone )
		{
			x = players[0].mx*0.5f;
			y = players[0].my*0.5f;
			z = players[0].mz*0.5f;
			DrawFlag( x,y,z, 200,200,100 );
		}

		// show next sequencer
		bool done = false;
		for ( i=0; i<numSequenceLocations && !done; i++ )
		{
			if ( !sequenceLocations[i].visited )
			{
				done = true;
				DrawArrow( sequenceLocations[i].x * 0.5f,
						   sequenceLocations[i].y * 0.5f,
						   sequenceLocations[i].z * 0.5f,
						   255,255,255 );
			}
		}

		glDisable( GL_NORMALIZE );
	}
	else
	{
		if ( sky!=NULL && !fog )
		{
			glEnable( GL_COLOR_MATERIAL );
			glEnable( GL_NORMALIZE );
			glDisable( GL_BLEND );
			glColor3ub( 255,255,255 );

			glLoadIdentity();
			if ( cam.Type()>=5 )
			{
				glRotatef( cam.ZAngle(), 0,0,1 );
				glRotatef( cam.XAngle(), 1,0,0 );
				glRotatef( cam.YAngle(), 0,1,0 );
			}
			else
			{
				glRotatef( cam.XAngle(), 1,0,0 );
				glRotatef( cam.YAngle(), 0,1,0 );
			}
			float cloudScale = float((renderDepth)-0.5f)*landObj.DivSizeX();
			// -cam.CameraY() * 0.01f
			glTranslatef( 0, sky->SizeY()*cloudScale-landObj.WaterLevel()+cam.CameraY(), 0 );
			glScalef( cloudScale,cloudScale,cloudScale );
			glNormal3f( 0,0,1 );
			sky->Draw();
			glDisable( GL_NORMALIZE );
		}

		if ( landObj.Initialised() )
		{
			glLoadIdentity();
			glRotatef( cam.ZAngle(), 0,0,1 );
		    glRotatef( cam.XAngle(), 1,0,0 );
			glRotatef( cam.YAngle(), 0,1,0 );
			glTranslatef( cam.CameraX(),
						  cam.CameraY(),
						  cam.CameraZ() );

			if ( !lazy )
				landObj.Draw( cam, player.mx, player.mz,false);

			TSmoke::Draw();

			size_t i;

			DrawProjectiles( cam, false );

			size_t vehicleId = player.vehicleId;
			size_t vehicleType = player.vehicleType;

			// draw dropped explosives
			for ( i=0; i<kMaxPlayers; i++ )
			{
				explosives[i].Draw();
			}

			// draw AIs
			for ( i=0; i<numAITanks; i++ )
			{
				if ( InRange( aitanks[i].X(), aitanks[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					aitanks[i].Draw(false);
				}
			}

			for ( i=0; i<numAIPlanes; i++ )
			{
				if ( InRange( aiplanes[i].X(), aiplanes[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					aiplanes[i].Draw(false);
				}
			}
			for ( i=0; i<numAIFlak; i++ )
			{
				if ( InRange( aiflak[i].X(), aiflak[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					aiflak[i].Draw(false);
				}
			}
			for ( i=0; i<numAIArtillery; i++ )
			{
				if ( InRange( aiartillery[i].X(), aiartillery[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					aiartillery[i].Draw(false);
				}
			}

			for ( i=0; i<numAIMG42; i++ )
			{
				if ( InRange( aimg42[i].X(), aimg42[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					aimg42[i].Draw(false);
				}
			}
			for ( i=0; i<numV2; i++ )
			{
				if ( InRange( v2[i].X(), v2[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					v2[i].Draw(false);
				}
			}

			// draw all available tanks
			for ( i=0; i<numTanks; i++ )
			{
				if ( !tanks[i].IsaAI() )
				if ( InRange( tanks[i].X(), tanks[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					if ( vehicleType==vtTank && vehicleId==i && cameraType>=5 )
					{
						tanks[i].Draw1stPerson();
					}
					else
					{
						tanks[i].Draw( false );
					}
				}
			}

			// draw all available planes
			for ( i=0; i<numPlanes; i++ )
			{
				if ( !planes[i].IsaAI() )
				if ( InRange( planes[i].X(), planes[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					if ( vehicleType==vtPlane && vehicleId==i && cameraType>=5 )
					{
						planes[i].Draw1stPerson();
					}
					else
					{
						planes[i].Draw(false);
					}
				}
			}

			// draw all available flak
			for ( i=0; i<numFlak; i++ )
			{
				if ( !flak[i].IsaAI() )
				if ( InRange( flak[i].X(), flak[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					if ( vehicleType==vtFlak && vehicleId==i && cameraType>=5 )
					{
						flak[i].Draw1stPerson();
					}
					else
					{
						flak[i].Draw(false);
					}
				}
			}

			// draw all available artillery
			for ( i=0; i<numArtillery; i++ )
			{
				if ( !artillery[i].IsaAI() )
				if ( InRange( artillery[i].X(), artillery[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					if ( vehicleType==vtArtillery && vehicleId==i && cameraType>=5 )
					{
						artillery[i].Draw1stPerson();
					}
					else
					{
						artillery[i].Draw(false);
					}
				}
			}

			for ( i=0; i<numMG42s; i++ )
			{
				if ( !mg42[i].IsaAI() )
				if ( InRange( mg42[i].X(), mg42[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					if ( vehicleType==vtMG42 && vehicleId==i && cameraType>=5 )
					{
						mg42[i].Draw1stPerson();
					}
					else
					{
						mg42[i].Draw(false);
					}
				}
			}

			// draw characters
			if ( vehicleType==vtNone )
			{
				TCharacter& ch = *characters[0];
				if ( cameraType>=5 )
				{
					ch.Draw1stPerson();
				}
				else
				{
					ch.Draw();
				}


			}
		
			if ( !lazy )
				landObj.Draw( cam, player.mx, player.mz,true);

			//
			// Draw transparencies
			//

			// draw AIs with transparencies
			for ( i=0; i<numAIPlanes; i++ )
			{
				if ( InRange( aiplanes[i].X(), aiplanes[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
					aiplanes[i].Draw(true);
			}

			// draw all available planes with transparencies
			for ( i=0; i<numPlanes; i++ )
			{
				if ( !planes[i].IsaAI() )
				if ( InRange( planes[i].X(), planes[i].Z(),
							  cam.CameraX(), cam.CameraZ() ) )
				{
					if (!( vehicleType==vtPlane && vehicleId==i && cameraType>=5 ))
					{
						planes[i].Draw(true);
					}
				}
			}

			DrawProjectiles( cam, true );
		}
	}

	// adjust render depth?
	if ( landObj.Initialised() )
	if ( currentRenderDepth!=renderDepth )
	{
		WriteString( "Changing render depth from %d to %d\n", 
					 currentRenderDepth, renderDepth );
		currentRenderDepth = renderDepth;
		landObj.RenderDepth( renderDepth );
	}

	if ( showMenu )
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable( GL_COLOR_MATERIAL );
		glColor3ub( 255,255,255 );
		RenderMenu();
	}
};
*/

//==========================================================================

/*
	if ( net==NULL )
		return;

	if ( net->serverStarted && net->MyId()>0 )
	{
		DataParticulars others[kMaxPlayers];
		size_t cntr = 0;
		for ( i=0; i<kMaxPlayers; i++ )
			if ( net->Used(i) && byte(i+1)!=net->MyId() )
			{
				others[cntr] = GetData(i+1);
				cntr++;
			}

		DataParticulars& player = players[net->MyId()-1];
		if ( landObj!=NULL )
			Render( *landObj, player, cntr, others );

		SetupGL2d( vdepth, vwidth, vheight );
		glLoadIdentity();

		glColor3ub( 200,200,55 );
		if ( showScores || gameOver )
		{
			DrawScores();
		}
		msg->DisplayMessages();
	}
	else if ( net->clientConnected && net->MyId()>0 )
	{
		DataParticulars others[kMaxPlayers];
		size_t cntr = 0;
		for ( i=0; i<kMaxPlayers; i++ )
			if ( net->Used(i) && byte(i)!=(net->MyId()-1) )
			{
				others[cntr] = GetData(i+1);
				cntr++;
			}

		if ( landObj!=NULL )
			Render( *landObj, GetData(net->MyId()), cntr, others );

		SetupGL2d( vdepth, vwidth, vheight );
		glLoadIdentity();

		glColor3ub( 200,200,55 );
		if ( showScores || gameOver )
		{
			DrawScores();
		}
		msg->DisplayMessages();
	}
*/

