#include <precomp_header.h>

#include <win32/win32.h>
#include <3dsound/fmod_sound.h>

//==========================================================================

// Close complete system
void CloseFMOD( void )
{
	FSOUND_Close();
}


void StopSound( int channel )
{
	FSOUND_StopSound( channel );
};

bool SoundIsPlaying( int channel )
{
	return (FSOUND_IsPlaying(channel)==TRUE);
};

// init complete system
bool InitFMOD( TString& errStr )
{
	size_t i;

	// check fmod version
	if (FSOUND_GetVersion() < FMOD_VERSION)
	{
		errStr = "Error : You are using the wrong DLL version!";
		errStr = errStr + "You should be using FMOD ";
		errStr = errStr + FloatToString( 0,2, FMOD_VERSION );
		return false;
	}

	// select output type
	FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
	//FSOUND_SetOutput(FSOUND_OUTPUT_WINMM);
	//FSOUND_SetOutput(FSOUND_OUTPUT_A3D);

	// get best possible driver
	size_t driverCount = FSOUND_GetNumDrivers();
	if ( driverCount==0 )
	{
		errStr = "Error : no sound drivers found\n";
		return false;
	}

	size_t* scores = new size_t[driverCount];
	for ( i=0; i<driverCount; i++ )
	{
		scores[i] = 0;
	}

	WriteString( "Sound drivers:\n" );
	for ( i=0; i<driverCount; i++ )
	{
		WriteString( "%d - %s\n", i+1, FSOUND_GetDriverName(i));	// print driver names
		{
			unsigned int caps = 0;

			FSOUND_GetDriverCaps(i, &caps);

			size_t score = 0;
			if (!caps)
			{
				WriteString("  * This driver supports software mode only.\n");
			}
			if (caps & FSOUND_CAPS_HARDWARE)
			{
				score += 100;
				WriteString("  * Driver supports hardware 3D sound!\n");
			}
			if (caps & FSOUND_CAPS_EAX2)
			{
				score += 10;
				WriteString("  * Driver supports EAX 2 reverb!\n");
			}
			if (caps & FSOUND_CAPS_EAX3)
			{
				score += 10;
				WriteString("  * Driver supports EAX 3 reverb!\n");
			}
			if (caps & FSOUND_CAPS_GEOMETRY_OCCLUSIONS)
			{
				score += 10;
				WriteString("  * Driver supports hardware 3d geometry processing with occlusions!\n");
			}
			if (caps & FSOUND_CAPS_GEOMETRY_REFLECTIONS)
			{
				score += 10;
				WriteString("  * Driver supports hardware 3d geometry processing with reflections!\n");
			}
			scores[i] = score;
		}
	}

	size_t driverIndex = 0;
	for ( i=1; i<driverCount; i++ )
	{
		if ( scores[i] > scores[driverIndex] )
		{
			driverIndex = i;
		}
	}
	WriteString( "Selected driver %d\n", driverIndex+1 );

	// release mem
	delete []scores;

	FSOUND_SetDriver(driverIndex);	// Select sound card (0 = default)

	// init the sound system, 22KHz, 32 channels
	if (!FSOUND_Init(22050, 32, 0))
	{
		errStr = "FMOD Init: " + TString(FMOD_ErrorString(FSOUND_GetError()));
		errStr = errStr + "\n";
		return false;
	}
	return true;
};


// load a sound sample
FSOUND_SAMPLE* LoadSample( const TString& fname, bool looped, TString& errStr )
{
	FSOUND_SAMPLE* samp1 = FSOUND_Sample_Load( FSOUND_FREE, fname.c_str(), FSOUND_HW3D, 0 );
 	if ( samp1==NULL )
	{
		errStr = "samp1: " + TString(FMOD_ErrorString(FSOUND_GetError()));
		errStr = errStr + "\n";
		return NULL;
	}

	// increasing mindistnace makes it louder in 3d space
	FSOUND_Sample_SetMinMaxDistance(samp1, 4.0f, 1000.0f);	
	FSOUND_Sample_SetLoopMode(samp1, looped?FSOUND_LOOP_NORMAL:FSOUND_LOOP_OFF);

	return samp1;
};


// play the sample on the next available channel
int PlaySound( FSOUND_SAMPLE* sample, const TPoint& pos )
{
	float p[3];
	p[0] = pos.x;
	p[1] = pos.y;
	p[2] = pos.z;

	float v[3];
	v[0] = 0;
	v[1] = 0;
	v[2] = 0;

	int channel1 = FSOUND_PlaySoundEx(FSOUND_FREE, sample, NULL, TRUE); // start paused
	FSOUND_3D_SetAttributes( channel1, p, v );
	FSOUND_SetPaused(channel1, FALSE);
	return channel1;
};


void UpdateSound( int channel, const TPoint& pos, const TPoint& prevPos )
{
	float p[3];
	p[0] = pos.x;
	p[1] = pos.y;
	p[2] = pos.z;

	float v[3];
	v[0] = (pos.x-prevPos.x) * (1000/kLogicFPS);
	v[1] = (pos.y-prevPos.y) * (1000/kLogicFPS);
	v[2] = (pos.z-prevPos.z) * (1000/kLogicFPS);

	FSOUND_3D_SetAttributes( channel, p, v );
};


// set camera pos
void SetCamera( const TPoint& pos,
			    const TPoint& fwd,
				const TPoint& up )
{
	static float lastpos[3] = { 0,0,0 };

	// ********* NOTE ******* READ NEXT COMMENT!!!!!
	// vel = how far we moved last FRAME (m/f), 
	// then time compensate it to SECONDS (m/s).
	float vel[3];
	vel[0] = (pos.x-lastpos[0]) * (1000 / kLogicFPS);
	vel[1] = (pos.y-lastpos[1]) * (1000 / kLogicFPS);
	vel[2] = (pos.z-lastpos[2]) * (1000 / kLogicFPS);

	// limit velocities to around logic frame rate max
	// otherwise it'll sound strange (the fast movement)
	if ( vel[0] < -kLogicFPS )
	{
		vel[0] = -kLogicFPS;
	}
	else if ( vel[0] > kLogicFPS )
	{
		vel[0] = kLogicFPS;
	}

	if ( vel[1] < -kLogicFPS )
	{
		vel[1] = -kLogicFPS;
	}
	else if ( vel[1] > kLogicFPS )
	{
		vel[1] = kLogicFPS;
	}

	if ( vel[2] < -kLogicFPS )
	{
		vel[2] = -kLogicFPS;
	}
	else if ( vel[2] > kLogicFPS )
	{
		vel[2] = kLogicFPS;
	}

	// store pos for next time
	lastpos[0] = pos.x;
	lastpos[1] = pos.y;
	lastpos[2] = pos.z;

	FSOUND_3D_Listener_SetAttributes( &lastpos[0], &vel[0], 
									  fwd.x,fwd.y,fwd.z,
									  up.x,up.y,up.z );
	FSOUND_3D_Update();
};

void SetVolume( int channel, byte vol )
{
	FSOUND_SetVolume( channel, vol );
};

FSOUND_STREAM* OpenStream( const TString& fname )
{
	return FSOUND_Stream_OpenFile( fname.c_str(), FSOUND_2D|FSOUND_LOOP_NORMAL, 0 );
};

int PlayStream( FSOUND_STREAM* stream )
{
	return FSOUND_Stream_Play( FSOUND_FREE, stream );
};

bool StopStream( FSOUND_STREAM* stream )
{
	return (FSOUND_Stream_Stop( stream )==TRUE);
};

bool CloseStream( FSOUND_STREAM* stream )
{
	return (FSOUND_Stream_Close(stream)==TRUE);
};

//==========================================================================
