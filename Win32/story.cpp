#include <precomp_header.h>

#include <object/texture.h>
#include <win32/story.h>
#include <win32/events.h>

//==========================================================================

TStory::TStory( void )
	: buttons(NULL),
	  acceptButton(NULL),
	  rejectButton(NULL)
{
	selection = slNone;
	currentPage = 0;
	numPages = 0;
	anim = 0;

	imageWidth = 1024;
	imageHeight = 768;

	for ( size_t i=0; i<kMaxPages; i++ )
	{
		titleScreenText[i] = NULL;
	}
};

TStory::TStory( const TStory& s )
	: buttons(NULL),
	  acceptButton(NULL),
	  rejectButton(NULL)
{
	for ( size_t i=0; i<kMaxPages; i++ )
	{
		titleScreenText[i] = NULL;
	}

	operator=(s);
};

TStory::~TStory( void )
{
	for ( size_t i=0; i<kMaxPages; i++ )
	{
		if ( titleScreenText[i]!=NULL )
			delete titleScreenText[i];
		titleScreenText[i] = NULL;
	}

	if ( buttons!=NULL )
		delete buttons;
	buttons = NULL;
};

const TStory& TStory::operator=( const TStory& s )
{
	currentPage = s.currentPage;
	numPages = s.numPages;
	shortTitle = s.shortTitle;
	map = s.map;

	imageWidth = s.imageWidth;
	imageHeight = s.imageHeight;

	TString		pictureText[kMaxPages];

	selection = s.selection;
	anim = s.anim;

	for ( size_t i=0; i<kMaxPages; i++ )
	{
		pictureText[i] = s.pictureText[i];

		if ( titleScreenText[i]!=NULL )
			delete titleScreenText[i];
		titleScreenText[i] = NULL;

		if ( s.titleScreenText[i]!=NULL )
		{
			titleScreenText[i] = new TTexture();
			*titleScreenText[i] = *s.titleScreenText[i];
		}
	}
	return *this;
};


void TStory::NextPage( void )
{
	if ( numPages>0 )
	{
		currentPage++;
		if ( currentPage >= numPages )
		{
			currentPage = 0;
		}
	}
};


void TStory::PreviousPage( void )
{
	if ( numPages>0 )
	{
		if ( currentPage>0 )
		{
			currentPage--;
		}
		else
		{
			currentPage = numPages - 1;
		}
	}
};


void TStory::AddItem( TGUI*& page, TGUI* item ) const
{
	if ( page==NULL )
		page = item;
	else
	{
		if ( item->itemType==guiCompoundObject )
		{
			item->next = page;
			page = item;
		}
		else
		{
			TGUI* list = page;
			while ( list->next!=NULL )
				list = list->next;
			list->next = item;
		}
	}
};

bool TStory::ReloadTitleScreen( float width, TString& errStr )
{
	currentPage = 0;

	for ( size_t i=0; i<kMaxPages; i++ )
	{
		if ( titleScreenText[i]!=NULL )
			delete titleScreenText[i];
		titleScreenText[i] = NULL;
		if ( pictureText[i].length()>0 )
		{
			TString path = "data\\stories\\";
			if ( width==640 )
			{
				path = path + "640";
				imageWidth = 640;
				imageHeight = 480;
			}
			else if ( width==800 )
			{
				path = path + "800";
				imageWidth = 800;
				imageHeight = 600;
			}
			else
			{
				path = path + "1024";
				imageWidth = 1024;
				imageHeight = 768;
			}
			titleScreenText[i] = new TTexture();
			if ( !titleScreenText[i]->LoadBinary( pictureText[i], path, errStr ) )
			{
				return false;
			}
		}
	}
	return true;
};

bool TStory::Load( TEvent* app,
				   const TString& fname, 
				   TString& errStr )
{
	TString storyFname = "data\\stories\\" + fname;
	TPersist file(fileRead);
	if ( !file.FileOpen( storyFname ) )
	{
		errStr = file.ErrorString();
		return false;
	}

	file.ReadLine(shortTitle);
	shortTitle = shortTitle.GetItem('=',1);
	if ( shortTitle.length()==0 )
	{
		errStr = "no short title";
		return false;
	}

	TString temp;
	file.ReadLine(temp);
	temp = temp.GetItem('=',1);
	if ( temp.length()==0 )
	{
		errStr = "no picture text";
		return false;
	}

	numPages = 0;
	for ( size_t i=0; i<kMaxPages; i++ )
	{
		pictureText[i] = temp.GetItem(',',i);
		if ( pictureText[i].length() > 0 )
		{
			if ( app->LanguageId()==1 )
			{
				pictureText[i] = "ger_" + pictureText[i];
			}
			else
			{
				pictureText[i] = "eng_" + pictureText[i];
			}
			numPages++;
		}
	}

	file.ReadLine(map);
	map = map.GetItem('=',1);
	if ( map.length()==0 )
	{
		errStr = "no map";
		return false;
	}

	return true;
};

const TString& TStory::Map( void ) const
{
	return map;
};

const TString& TStory::ShortTitle( void ) const
{
	return shortTitle;
};

void TStory::Logic( void )
{
	if ( anim < 30 )
	{
		anim++;
	}
};

void TStory::Draw( TBinObject* missionControlText, TEvent* app ) const
{
	PreCond( titleScreenText!=NULL );
	PreCond( buttons!=NULL );
	PreCond( app!=NULL );

	float width = app->Width();
	float height = app->Height();

	glLoadIdentity();
	glEnable( GL_COLOR_MATERIAL );
	glColor3ub( 255,255,255 );
	glPushMatrix();
		glEnable( GL_COLOR_MATERIAL );
		glTranslatef( 0,0, -4 );
		glColor3ub(255,255,255);

		if ( titleScreenText[currentPage]!=NULL )
		{
			float ah = 0;
			float aw = 1;
			if ( imageWidth < 1024 )
			{
				ah = ((1024-imageHeight) / 768) * 0.5f;
				aw = (imageWidth / 1024);
			}
			titleScreenText[currentPage]->Draw( width, 0, 0, height,
												0, ah,
												aw, 
												1 );
		}

		if ( missionControlText!=NULL )
		{
			glPushMatrix();
				float scale = float(anim) * 10;
				glTranslatef( width*0.5f,height*0.85f, 1 );
				glScalef( scale,scale,scale );
				missionControlText->Draw();
			glPopMatrix();
		}

	glPopMatrix();

	buttons->Draw(false);
};

size_t TStory::Selection( void ) const
{
	return selection;
};

TGUI* TStory::Buttons( void ) const
{
	return buttons;
};

TGUI* TStory::AcceptButton( void ) const
{
	return acceptButton;
};

TGUI* TStory::RejectButton( void ) const
{
	return rejectButton;
};

TGUI* TStory::PreviousButton( void ) const
{
	return prevButton;
};

TGUI* TStory::NextButton( void ) const
{
	return nextButton;
};

bool TStory::PrepareStory( TEvent* app, TString& errStr ) 
{
	currentPage = 0;
	anim = 0;

	float width = app->Width();
	float height = app->Height();

	if ( titleScreenText[0]==NULL )
	{
		if ( !ReloadTitleScreen( width, errStr ) )
			return false;
	}
	if ( buttons==NULL )
	{
		size_t languageId = app->LanguageId();
		rejectButton = new TGUI( app, guiButton, width*0.12f,height-50,170,25, -3, STR(strReject) );
		acceptButton = new TGUI( app, guiButton, width*0.55f,height-50,170,25, -3, STR(strAccept) );

		if ( numPages>1 )
		{
			prevButton = new TGUI( app, guiButton, width*0.39f,height-50,50,25, -3, "<<" );
			nextButton = new TGUI( app, guiButton, width*0.47f,height-50,50,25, -3, ">>" );

			AddItem( buttons, rejectButton );
			AddItem( buttons, prevButton );
			AddItem( buttons, nextButton );
			AddItem( buttons, acceptButton );
		}
		else
		{
			nextButton = NULL;
			prevButton = NULL;
			AddItem( buttons, rejectButton );
			AddItem( buttons, acceptButton );
		}

	}
	return true;
};

void TStory::FinishStory( void ) const
{
	if ( buttons!=NULL )
		delete buttons;
	buttons = NULL;

	for ( size_t i=0; i<kMaxPages; i++ )
	{
		if ( titleScreenText[i]!=NULL )
			delete titleScreenText[i];
		titleScreenText[i] = NULL;
	}
};

//==========================================================================

TStories::TStories( void )
	: missionControlText(NULL)
{
	count = 0;
	selectedStory = -1;
};

TStories::~TStories( void )
{
	missionControlText = NULL;
};

TStories::TStories( const TStories& s )
{
	operator=(s);
};

const TStories& TStories::operator=( const TStories& s )
{
	count = s.count;

	if ( count>0 )
	{
		for ( size_t i=0; i<count; i++ )
			stories[i] = s.stories[i];
	}

	missionControlText = s.missionControlText;

	return *this;
};

bool TStories::Reload( TEvent* app, TString& errStr )
{
	count = 0;
	selectedStory = -1;

	return Load( app, errStr );
};

bool TStories::Load( TEvent* app, TString& errStr )
{
	bool eof = false;
	count = 0;
	while ( !eof && count<64 )
	{
		TPersist file(fileRead);
		TString fname = "story";
		size_t i = count+1;
		if ( i<10 )
		{
			fname = fname + "0" + Int2Str(i) + ".txt";
		}
		else
		{
			fname = fname + Int2Str(i) + ".txt";
		}
		TString fnameWithPath = "data\\stories\\" + fname;
		if ( file.FileOpen( fnameWithPath ) )
		{
			file.FileClose();
			if ( !stories[count].Load( app, fname, errStr ) )
				return false;
			count++;
		}
		else
		{
			eof = true;
		}
	};

	if ( count==0 )
	{
		errStr = "No story files found";
		return false;
	}
	return true;
};

size_t TStories::Count( void ) const
{
	return count;
};

TStory* TStories::Story( size_t index ) const
{
	PreCond( index<count );
	return const_cast<TStory*>(&stories[index]);
};

void TStories::Draw( TEvent* app ) const
{
	if ( selectedStory>=0 )
	{
		TStory* story = Story(selectedStory);
		story->Draw(missionControlText,app);
	}
}

const TString& TStories::SelectedMap( void ) const
{
	return selectedMap;
};

size_t TStories::SelectedStory( void ) const
{
	return selectedStory;
};

void TStories::SelectedStory( int _selectedStory )
{
	selectedStory = _selectedStory;
};

void TStories::SetBinaries( TBinObject* _missionControlText )
{
	missionControlText = _missionControlText;
};

bool TStories::Execute( TEvent* app, int _selectedStory, TString& errStr )
{
	selectedStory = _selectedStory;
	app->ShowStory( true );
	TGUI* prevPage = app->CurrentPage();

	TStory* story = Story(selectedStory);
	if ( !story->PrepareStory(app, errStr ) )
		return false;

	app->CurrentPage( story->Buttons() );
	TGUI* acceptButton = story->AcceptButton();
	TGUI* rejectButton = story->RejectButton();
	TGUI* prevButton = story->PreviousButton();
	TGUI* nextButton = story->NextButton();

	MSG msg;
	size_t fpsTimer = ::GetTickCount();
	bool closing = false;
	while ( !closing )
	{
		size_t time = ::GetTickCount();
		::ProcessMessages( app, msg, fpsTimer );
		if ( msg.message==WM_QUIT || msg.message==WM_CLOSE )
		{
			app->ShowStory( false );
			app->CurrentPage( prevPage );
			story->FinishStory();
			::PostQuitMessage( 0 );
			return false;
		}

		if ( acceptButton->Clicked() )
		{
			closing = true;
		}

		if ( prevButton!=NULL )
		if ( prevButton->Clicked() )
		{
			story->PreviousPage();
		}

		if ( nextButton!=NULL )
		if ( nextButton->Clicked() )
		{
			story->NextPage();
		}

		if ( rejectButton->Clicked() )
		{
			app->ShowStory( false );
			app->CurrentPage( prevPage );
			story->FinishStory();
			return false;
		}
		story->Logic();
	}
	app->CurrentPage( prevPage );
	app->ShowStory( false );
	story->FinishStory();
	selectedMap = story->Map();

	return true;
};

//==========================================================================
