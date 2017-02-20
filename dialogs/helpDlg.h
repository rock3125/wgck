#ifndef _DIALOGS_HELPDIALOG_H_
#define _DIALOGS_HELPDIALOG_H_

//==========================================================================

class TGUI;
class TApp;

//==========================================================================

class _EXPORT HelpDialog
{
public:
	HelpDialog( TApp* );
	~HelpDialog( void );

	bool Execute( void );

private:
	void GetRegistrySettings( void );

private:
	TApp*	app;

	bool	closing;

	TGUI*	helpPage;
	TGUI*	manualButton;
	TGUI*	tutorialButton;
	TGUI*	onlineButton;

	TGUI*	cancelButton;

	float	helpDialogDepth;
	size_t	languageId;
};

//==========================================================================

#endif
