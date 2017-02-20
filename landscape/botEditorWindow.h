#ifndef _LANDSCAPE_BOTEDITORWINDOW_H_
#define _LANDSCAPE_BOTEDITORWINDOW_H_

//==========================================================================

class BotEditor
{
public:
	BotEditor( TApp& _app, HINSTANCE _instance, HWND _parent );
	~BotEditor( void );

	HWND Window( void ) const;

	void SetMapInfo( TDefFile& def );

	// return selected ai type
	size_t SelectedAI( void ) const;

	// add a road marker for bots - connected to its previous
	void AddBotMarker( void );
	void RemoveBotMarker( void );

	void Timer( void );

	// set window pos at x,y
	void SetPos( int x, int y );

	bool Changed( void ) const;
	void Changed( bool _changed );

private:
	void SetAICheck( size_t aiCheck );

private:
	friend LRESULT CALLBACK BotEditorProc(HWND, UINT, WPARAM, LPARAM);

	static BotEditor* ptr;

	TApp&	app;
	HWND	parent;
	HINSTANCE instance;

	size_t	selectedAI;

	bool	showMarkers;
	int		currentMarker;
	int		currentc1;
	int		currentc2;
	int		currentc3;
	int		currentc4;

	bool	changed;

	HWND	dialog;

	HWND	ddMarkers;

	HWND	ddConnection1;
	HWND	ddConnection2;
	HWND	ddConnection3;
	HWND	ddConnection4;

	HWND	cbShowMarkers;

	HWND	butAddMarker;
	HWND	butRemoveMarker;

	HWND	radioAI1;
	HWND	radioAI2;
	HWND	radioAI3;
};

//==========================================================================

#endif
