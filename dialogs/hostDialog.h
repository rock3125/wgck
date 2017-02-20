#ifndef _DIALOGS_HOSTDIALOG_H_
#define _DIALOGS_HOSTDIALOG_H_

//==========================================================================

#include <object/compoundobject.h>
#include <object/character.h>

class TCharacter;
class TGUI;
class TApp;
class TDefFile;
class TStories;

//==========================================================================

class _EXPORT TGameParameters
{
public:
	TGameParameters( void );
	TGameParameters( const TGameParameters& );
	~TGameParameters( void );

	const TGameParameters& operator=( const TGameParameters& );

	TString name;			// player's name
	TString gameName;		// name of game
	TString	hostName;		// name of host
	size_t	port;			// tcp/ip host port

	int		character;		// chosen character
	int		map;			// chosen game map
	int		team;			// host's team
	int		gameType;		// what sort of game
	
	size_t	maxPoints;		// game over condition (see gameType)
};


//==========================================================================

class _EXPORT HostDialog
{
public:
	HostDialog( TApp* );
	~HostDialog( void );

	bool Execute( TGameParameters& gp );

	bool Initialised( void ) const;
	void Initialised( bool i );

private:
	void UpdateHostButtons( void );
	void GetRegistrySettings( void );
	void DrawVehicle( void );

private:
	TApp*	app;

	bool	closing;

	TGUI*	hostPage;
	TGUI*	nameEdit;
	TGUI*	gameNameEdit;
	TGUI*	portEdit;
	TGUI*	characterDropDown;
	TGUI*	mapDropDown;
	TGUI*	typeDropDown;
	TGUI*	teamDropDown;
	TGUI*	pointsEdit;
	TGUI*	hostButton;
	TGUI*	cancelButton;
	TGUI*	charGraphic;

	size_t			numModels;
	TCharacter		characters[kMaxModels];
	TString			modelNames[kMaxModels];

	size_t			numMaps;
	size_t			languageId;
	TDefFile*		maps;

	float yAngle;
	float hostDialogDepth;

	TGameParameters gp;
};

//==========================================================================

#endif
