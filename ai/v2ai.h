#ifndef _OBJECT_V2_H_
#define _OBJECT_V2_H_

//==========================================================================

#include <ai/tankai.h>

//==========================================================================

class V2
{
public:
	V2( void );
	V2( const V2& );
	~V2( void );

	const V2& operator=( const V2& );

	// initialise rocket vars
	void Init( void );

	// tick-tock start count down!
	void StartTimer( void );

	void Logic( TApp* app, TLandscape& landObj, TLandscapeAI& land, TParams& params );
	void Draw( bool transp );

	void operator=( TVehicleSettings& vs );
	const V2& operator = ( const TLandscapeObject& o );

	// sounds
	void EngineOn( void );
	void StopSounds( void );

	void SetTarget( float tx, float ty, float tz );

	float X( void ) const;
	float Y( void ) const;
	float Z( void ) const;

	float Xangle( void ) const;
	float Yangle( void ) const;
	float Zangle( void ) const;

	size_t GameImportance( void ) const;

	size_t	Strength( void ) const;
	void	Strength( size_t _strength );

	bool InsideVehicle( const TPoint& point ) const;
	bool InsideVehicle( float _x, float _y, float _z ) const;

	void CalculateDamage( const TPoint& point, float damage );
	void CalculateDamage( float expx, float expy, float expz, float damage );

	void SetSoundSystem( TSound* _soundSystem );

	size_t CountDown( void ) const;

	bool SendOverNetwork( void ) const;
	void SendOverNetwork( bool _sendOverNetwork );

	size_t AIID( void ) const;
	void AIID( size_t _aiId );

	bool	StrengthUpdated( void ) const;
	void	StrengthUpdated( bool _strengthUpdated );

	// get set net data
	size_t	netSet( size_t myId, size_t vid, byte* data );
	size_t	netSize( void ) const;
	size_t	netGet( const byte* data );

private:
	void V2AI( TApp* app, TLandscape& landObj, TLandscapeAI& land, 
			   TParams& params );
	bool TurnVehicle( float px, float pz );
	float Distance( float px, float py, float pz );

private:
	TCompoundObject* rocket;

	size_t			currentState;
	size_t			aiId;

	bool			strengthUpdated;

	// sound management
	size_t			engineSound;
	TSound*			soundSystem;
	bool			engineSoundOn;
	TPoint			prevEnginePos;
	int				engineChannel;

	bool			launched;
	bool			waitingForLaunch;
	bool			exploded;
	bool			sendOverNetwork;
	size_t			countDown;
	size_t			launchTime;

	TString			name;

	size_t			strength;
	size_t			gameImportance;

	float			hover;

	float			x;
	float			y;
	float			z;

	float			startHeight;
	float			speed;

	float			xangle;
	float			yangle;
	float			zangle;

	float			targetx;
	float			targety;
	float			targetz;

	float			scalex;
	float			scaley;
	float			scalez;
	float			scale;
};

//==========================================================================

#endif
