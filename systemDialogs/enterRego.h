#ifndef _DIALOGS_ENTERREGO_H_
#define _DIALOGS_ENTERREGO_H_

//==========================================================================

#include <systemDialogs/gui.h>

class TEvent;

//==========================================================================

class _EXPORT EnterRego
{
public:
	EnterRego( TEvent* );

	~EnterRego( void );

	bool Execute( void );

private:
	void GetRegistrySettings( void );
	void CheckButtons( void );

private:
	TEvent*		app;

	size_t		languageId;
	float		enterRegoDepth;

	TGUI*	regoPage;
	TGUI*	nameEdit;
	TGUI*	codeEdit;

	TGUI*	registerButton;
	TGUI*	cancelButton;
};

//==========================================================================

#endif
