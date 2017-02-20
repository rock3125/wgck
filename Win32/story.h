#ifndef _TANK_STORY_H_
#define _TANK_STORY_H_

//==========================================================================

#include <systemDialogs/gui.h>

//==========================================================================

class TTexture;
class TEvent;

#define kMaxPages	10

//==========================================================================

enum buttonSelection { slNone, slAccept, slReject };

class _EXPORT TStory
{
public:
	TStory( void );
	~TStory( void );

	TStory( const TStory& );
	const TStory& operator=( const TStory& );

	bool Load( TEvent* app, const TString& fname, TString& errStr );

	void Logic( void );

	void Draw( TBinObject* missionControlText, TEvent* app ) const;

	const TString&	Map( void ) const;
	const TString&	ShortTitle( void ) const;

	void NextPage( void );
	void PreviousPage( void );

	void AddItem( TGUI*& page, TGUI* item ) const;

	size_t	Selection( void ) const; // button selection
	TGUI*	Buttons( void ) const; // access button menu

	TGUI*	AcceptButton( void ) const;
	TGUI*	RejectButton( void ) const;
	TGUI*	PreviousButton( void ) const;
	TGUI*	NextButton( void ) const;

	bool ReloadTitleScreen( float width, TString& errStr );

	bool PrepareStory( TEvent* app, TString& errStr ); // setup start
	void FinishStory( void ) const; // setup finished

private:
	mutable TGUI*		buttons;
	mutable TGUI*		acceptButton;
	mutable TGUI*		rejectButton;
	mutable TGUI*		prevButton;
	mutable TGUI*		nextButton;

	mutable TTexture*	titleScreenText[kMaxPages];

	TString		shortTitle;
	TString		pictureText[kMaxPages];
	TString		map;

	float		imageWidth;
	float		imageHeight;

	size_t		selection;		// button selection, slNone, slAccept, slReject
	size_t		currentPage;
	size_t		numPages;
	size_t		anim; // for animation
};

//==========================================================================

class _EXPORT TStories
{
public:
	TStories( void );
	~TStories( void );

	TStories( const TStories& );
	const TStories& operator=( const TStories& );

	bool	Reload( TEvent* app, TString& errStr );

	bool	Load( TEvent* app, TString& errStr );
	void	Draw( TEvent* app ) const;

	bool	Execute( TEvent* app, int _selectedStory, TString& errStr );

	size_t	Count( void ) const;
	TStory*	Story( size_t ) const;

	size_t	SelectedStory( void ) const;
	void	SelectedStory( int );

	const TString& SelectedMap( void ) const;

	void SetBinaries( TBinObject* _missionControlText );

private:
	size_t		count;
	int			selectedStory;
	TStory		stories[64];
	TBinObject*	missionControlText;
	TString		selectedMap;
};

//==========================================================================

#endif
