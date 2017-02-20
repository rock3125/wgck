#ifndef _DIALOGS_JOINDIALOG_H_
#define _DIALOGS_JOINDIALOG_H_

//==========================================================================

#include <object/compoundobject.h>

#include <dialogs/hostDialog.h>
#include <systemDialogs/gui.h>

class TApp;

//==========================================================================

class _EXPORT JoinDialog
{
public:
	JoinDialog( TApp* );

	~JoinDialog( void );

	bool Execute( TGameParameters& gp );

	void SetupGuest( const TString&  _playerName, size_t port, const TString& _hostName );

private:
	void UpdateJoinButtons( void );
	void GetRegistrySettings( void );

	size_t DropDownToTeamId( void );

private:
	TApp*		app;

	bool joined;		// has joined a server?
	bool startJoin;		// are we attempting to join?

	bool startConnect;	// same with connect
	bool connected;

	size_t			numModels;
	TCharacter		characters[kMaxModels];
	TString			modelNames[kMaxModels];

	size_t			numMaps;
	size_t			languageId;
	TDefFile*		maps;

	size_t timer;

	TGameParameters	gp;

	TGUI*	joinPage;
	TGUI*	nameEdit;
	TGUI*	hostNameEdit;
	TGUI*	portEdit;
	TGUI*	connectButton;
	TGUI*	characterDropDown;
	TGUI*	teamDropDown;
	TGUI*	joinButton;
	TGUI*	lbSearch;
	TGUI*	searchButton;
	TGUI*	cancelButton;
	TGUI*	charGraphic;

	TString	hostNames;
	TString	hostIPs;
	TString	hostPorts;
	TString	numPlayers;
	TString gameNames;

	float	joinDialogDepth;
	float	yAngle;

	bool	redraw;
};

//==========================================================================

#endif
