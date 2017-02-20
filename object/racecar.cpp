#include <precomp_header.h>

#include <win32/win32.h>
#include <common/sounds.h>

#include <object/binobject.h>
#include <object/racecar.h>
#include <object/geometry.h>
#include <object/smoke.h>

//==========================================================================

const float kPI = 3.1415927f;
const float degToRad = 0.01745329252033f;
const float radToDeg = 57.2957795112730f;

//==========================================================================

TRaceCar::TRaceCar( const float _speed )
	: soundSystem(NULL),
	  recording(NULL),
	  TVehicle(0.1f, 1.5f )
{
	initialised = false;
	maxSpeed = _speed;
	speed = 0;
	wheelRotAngles = 0;
	wheelY = 0;
	prevMy = 0;
	jumping = false;
	jumpDuration = 1.0f;

	engineIdleIndex = 0;
	engineIndex = 0;
	enginePlaying = false;
	engineIdlePlaying = false;

	playback = false;
	record = false;
};


TRaceCar::~TRaceCar( void )
{
	soundSystem = NULL;

	if ( recording!=NULL )
		delete recording;
	recording = NULL;

	initialised = false;
};


bool TRaceCar::LoadBinary( TSound* _soundSystem,
						   size_t _engineIdleIndex,
						   size_t _engineIndex,
						   const TString& bodyFname, 
						   const TString& wheelFname,
					       TString& errStr, 
						   const TString& pathname,
						   const TString& texturePath )
{
	soundSystem = _soundSystem;
	engineIdleIndex = _engineIdleIndex;
	engineIndex = _engineIndex;

	bool success = TVehicle::LoadBinary(bodyFname,errStr,pathname,texturePath);
	if ( success )
		success = wheel.LoadBinary( wheelFname, errStr, pathname, texturePath );
	initialised = success;
	return success;
};


bool TRaceCar::PlaybackFinished( void ) const
{
	if ( recording!=NULL )
		return !playback;
	return false;
};


void TRaceCar::SetupRecording( const TString& filename )
{
	if ( recording!=NULL )
		delete recording;
	recording = new TCarRecord();
	PostCond( recording!=NULL );

	record = true;
	playback = false;

	recordingFname = filename;
};


bool TRaceCar::StopRecording( TString& errStr )
{
	PreCond( record==true );
	PreCond( recording!=NULL );
	record = false;
	playback = false;
	return recording->SaveBinary( recordingFname, errStr );
};


bool TRaceCar::SetupPlayback( const TString& filename, TString& errStr )
{
	if ( recording!=NULL )
		delete recording;
	recording = new TCarRecord();
	PostCond( recording!=NULL );

	playback = true;
	record = false;
	return recording->LoadBinary( filename, errStr );
};


void TRaceCar::StopPlayback( void )
{
	playback = false;
	record = false;
};


//
// adds new behaviours to the base class
// such as drifting at high speeds, steering with front wheels
// and lifting of the car
//
void TRaceCar::Move( TLandscape& landObj, size_t& key, bool showTarget, 
					 float dxa, float dya )
{
	if ( recording!=NULL )
	{
		if ( record )
		{
			recording->Add( this, key, showTarget, dxa, dya );
		}
		else if ( playback )
		{
			playback = recording->Get( this, key, showTarget, dxa, dya );
		}
	}

	size_t tempKey = 0;
	if ( speed<0 )
	{
		tempKey = tempKey | aDown;
	}
	else if ( speed>0 )
	{
		tempKey = tempKey | aUp;
	}

	if ( !jumping ) // can only get new prevMy if not already jumping
		prevMy = my;

	float absspeed = float(fabs(speed));
	TVehicle::Move(tempKey,absspeed);

	if ( jumping ) // jumping?
	{
		// process so we go down autmoatically
		prevMy = prevMy - 0.1f*jumpDuration;
		if ( my > prevMy ) // only if we've hit the ground we switch it off again
		{
			jumpDuration = -5.0f;
			jumping = false;
			prevMy = my;
		}
	}
	jumpDuration = jumpDuration + 1.0f;


	if ( (key&aLeft)>0 )
	{
		if ( wheelY < 35.0f )
			wheelY += 3.5f;

		// add drift effect
		float anglespeed = wheelY * 0.01f;
		float yangle = (Yangle()+90.0f)*degToRad;
		float v1fcos = (float)cos(yangle);
		float v1fsin = (float)sin(yangle);

		if ( speed >= (maxSpeed*0.75f) )
		{
			X( X() - v1fsin*anglespeed );
			Z( Z() + v1fcos*anglespeed );
			TSmoke::AddNewParticle( -0.5f, 0.15f, 0.8f, -0.25f,0.25f );
		}
	}
	else if ( (key&aRight)>0 )
	{
		if ( wheelY > -35.0f )
			wheelY -= 3.5f;

		// add drift effect
		float anglespeed = -wheelY * 0.01f;
		float yangle = (Yangle()-90.0f)*degToRad;
		float v1fcos = (float)cos(yangle);
		float v1fsin = (float)sin(yangle);

		if ( speed >= (maxSpeed*0.75f) )
		{
			X( X() - v1fsin*anglespeed );
			Z( Z() + v1fcos*anglespeed );
			TSmoke::AddNewParticle( 0.5f, 0.15f, 0.8f, 0.25f,0.25f );
		}
	}
	else
	{
		if ( wheelY!=0 )
			wheelY = wheelY * 0.90f;
		if ( wheelY > 0  && wheelY < 1.0f )
			wheelY = 0;
		if ( wheelY <0 && wheelY > -1.0f )
			wheelY = 0;
	}

	if ( (tempKey&aUp)>0 )
	{
		Yangle( Yangle()-wheelY*0.15f*absspeed );
		wheelRotAngles += 93.0f;
		if ( speed > (maxSpeed*0.5f) )
		{
			TSmoke::AddNewParticle( 0.5f, 0.15f, 0.8f, 0, 1 );
			TSmoke::AddNewParticle( -0.5f, 0.15f, 0.8f, 0, 1 );
		}
	}
	else if ( (tempKey&aDown)>0 )
	{
		Yangle( Yangle()-wheelY*0.15f*absspeed );
		wheelRotAngles -= 93.0f;
		if ( speed < (-maxSpeed*0.5f) )
		{
			TSmoke::AddNewParticle( 0.5f, 0.15f, 0.8f, 0, -1 );
			TSmoke::AddNewParticle( -0.5f, 0.15f, 0.8f, 0, -1 );
		}
	}

	// jump?
	if ( speed > (maxSpeed*0.8f) ) // at speed?
	if ( (prevMy-my) > 0.15f ) // going down fast?
	if ( !jumping && jumpDuration>0 ) // not already doing it?
	{
		jumping = true;
		jumpDuration = 1.0f;
	}
	if ( jumping )
	{
		xa = xa * 0.8f;
		za = za * 0.8f;
	}

	if ( (key&aUp)>0 )
	{
		if ( speed < maxSpeed )
			speed = speed + (0.02f-(speed/maxSpeed)*0.018f);
	}
	else if ( (key&aDown)>0 )
	{
		if ( speed > (-maxSpeed*0.5f) )
			speed = speed - 0.02f;
	}
	else
	{
		if ( speed!=0 )
			speed = speed * 0.95f;
		if ( speed > 0  && speed < 0.01f )
			speed = 0;
		if ( speed < 0 && speed > -0.01f )
			speed = 0;
	}

	// engine sounds
	if ( soundSystem!=NULL )
	{
		if ( speed!=0 && !enginePlaying )
		{
			enginePlaying = true;
			engineIdlePlaying = false;
			soundSystem->Play( engineIndex, true );
			soundSystem->Stop( engineIdleIndex );
		}
		else if ( speed==0 && !engineIdlePlaying )
		{
			engineIdlePlaying = true;
			enginePlaying = false;
			soundSystem->Play( engineIdleIndex, true );
			soundSystem->Stop( engineIndex );
		}
	}

	TSmoke::AnimateSmoke();

	// engine stuff, pause animations if we have no speed
	PauseAnimations( (speed==0) );
};


void TRaceCar::Draw( void )
{
	if ( !initialised )
		return;

	// do the jump thingy
	if ( jumping )
	{
		float jumpHeight = (prevMy-my);
		glTranslatef( 0, jumpHeight, 0 );
	}

	TVehicle::Draw();
	{

		// obj can be considered to be at 0,0 at this stage
		glPushMatrix();

			glTranslatef( (x3+x4)*0.5f, //mx,
						  my,
 						  mz );

			glRotatef( ya, 0,1,0);
			glRotatef( xa, 0,0,1 );
			glRotatef( za, 1,0,0 );

			TSmoke::DrawSmoke();

			// draw wheels
			glPushMatrix();
				glTranslatef( 0.5f, 0.15f, 0.8f );
				glRotatef( wheelRotAngles, 1,0,0 );
				wheel.Draw();
			glPopMatrix();

			glPushMatrix();
				glTranslatef( -0.5f, 0.15f, 0.8f );
				glRotatef( wheelRotAngles, 1,0,0 );
				wheel.Draw();
			glPopMatrix();

			glPushMatrix();
				glTranslatef( 0.5f, 0.15f, -0.8f );
				glRotatef( wheelY, 0,1,0 );
				glRotatef( wheelRotAngles, 1,0,0 );
				wheel.Draw();
			glPopMatrix();

			glPushMatrix();
				glTranslatef( -0.5f, 0.15f, -0.8f );
				glRotatef( wheelY, 0,1,0 );
				glRotatef( wheelRotAngles, 1,0,0 );
				wheel.Draw();
			glPopMatrix();

		glPopMatrix();

	}
};


float TRaceCar::Speed( void ) const
{
	return speed;
};


void TRaceCar::Speed( float _speed )
{
	speed = _speed;
};


float TRaceCar::MaxSpeed( void ) const
{
	return maxSpeed;
};

