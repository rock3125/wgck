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

	bool		SetupOpenGL( HINSTANCE );

private:
	TString		GetInput( void );
	void		Help( void );

	bool CreateLand( const TString& fname, const TString& tpath,
					 const TString& divx, const TString& divy,
					 TString& errStr );
	bool UpdateLand( const TString& fname, const TString& tpath,
					 TString& errStr );
	bool CreateBinMesh( const TString& fname, const TString& strip,
						const TString& seaStr, const TString& material,
						TString& errStr );

private:
    char*	whoStr;
};

//==========================================================================

#endif

