#ifndef __CONSOLECLASS_H__
#define __CONSOLECLASS_H__

//==========================================================================

class DXConsole
{
public:
	DXConsole( void );
    ~DXConsole( void );

	bool 		ProcessInput( void );				// get user input from console
	bool		DoBatch( const TString& fname );	// execute a batch command file

private:
	TString		GetInput( void );
	void		Help( void );

private:
    char*	whoStr;
};

//==========================================================================

#endif

