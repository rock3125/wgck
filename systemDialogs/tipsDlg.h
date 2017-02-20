#ifndef _DIALOGS_TIPSDIALOG_H_
#define _DIALOGS_TIPSDIALOG_H_

//==========================================================================

#include <win32/tips.h>

class TGUI;
class TApp;

//==========================================================================

class _EXPORT TipsDialog
{
public:
	TipsDialog( TApp* );
	~TipsDialog( void );

	bool Execute( TTips& tips, bool forceShow = false );

private:
	void GetRegistrySettings( void );
	void SetText( TTips& tips );

private:
	TApp*	app;

	bool	closing;

	TGUI*	tipPage;

	TGUI*	nextButton;
	TGUI*	previousButton;
	TGUI*	showAtStartupButton;

	TGUI*	l1;
	TGUI*	l2;
	TGUI*	l3;
	TGUI*	info;

	TGUI*	closeButton;
	TGUI*	showTipsCheck;

	float	tipDialogDepth;
	size_t	languageId;
	bool	showTips;
	size_t	currentIndex;
};

//==========================================================================

#endif
