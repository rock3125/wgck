#include <precomp_header.h>

#include <windows.h>
#include <direct.h>
#include <process.h>
#include <ddraw.h>

#ifdef _USEGUI
#include <win32/events.h>
#endif

#include <systemDialogs/dialogAssist.h>
#include <dialogs/videoSettings.h>

#include <Win32/resource.h>

//==========================================================================

#ifndef _USEGUI
LRESULT CALLBACK VideoDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
VideoSettingsDialog* VideoSettingsDialog::ptr = NULL;
#endif

//==========================================================================

#ifdef _USEGUI
VideoSettingsDialog::VideoSettingsDialog( TEvent* _app, HINSTANCE _hInstance, 
										  HWND _parent, TSound* _soundSystem )
	: hInstance( _hInstance ),
	  parent( _parent ),
	  app( _app ),
	  soundSystem( _soundSystem ),
	  videoSettingsPage( NULL )
{
#else
VideoSettingsDialog::VideoSettingsDialog( HINSTANCE _hInstance, HWND _parent )
	: hInstance( _hInstance ),
	  parent( _parent ),
	  soundSystem( NULL )
{
	VideoSettingsDialog::ptr = this;
#endif
	initialised = false;
	shadowLevel = 0;
	languageId = 0;
};


VideoSettingsDialog::~VideoSettingsDialog( void )
{
#ifdef _USEGUI
	if ( videoSettingsPage!=NULL )
		delete videoSettingsPage;
	videoSettingsPage = NULL;
#endif
};


int VideoSettingsDialog::Execute( void )
{
#ifdef _USEGUI
	if ( videoSettingsPage!=NULL )
		delete videoSettingsPage;
	videoSettingsPage = NULL;

	GetRegistrySettings();

	AddItem( videoSettingsPage, new TGUI( app, guiWindow, 0,0,340,320, -3, STR(strSettings) ) );
	AddItem( videoSettingsPage, new TGUI( app, guiLabel, 20,20,99,14, -3, STR(strResolution), 255,255,255 ) );
	AddItem( videoSettingsPage, new TGUI( app, guiLabel, 200,20,99,14, -3, STR(strLanguage), 255,255,255 ) );
	AddItem( videoSettingsPage, new TGUI( app, guiLabel, 120,170,75,14, -3, STR(strMusicVolume), 255,255,255 ) );
	AddItem( videoSettingsPage, new TGUI( app, guiLabel, 120,190,75,14, -3, STR(strSoundVolume), 255,255,255 ) );

	okButton = new TGUI( app, guiButton, 110,280,60,25, -3, STR(strOK) );
	cancelButton = new TGUI( app, guiButton, 180,280,80,25, -3, STR(strCancel) );

	AddItem( videoSettingsPage, new TGUI( app, guiLabel, 20,220,75,14, -3, STR(strPerformance), 255,255,255 ) );
	AddItem( videoSettingsPage, new TGUI( app, guiLabel, 20,240,75,14, -3, STR(strLowSpec), 255,255,255 ) );
	AddItem( videoSettingsPage, new TGUI( app, guiLabel, 220,240,75,14, -3, STR(strHighSpec), 255,255,255 ) );
	playMusic = new TGUI( app, guiCheckBox, 20,170,90,14, -3, STR(strPlayMusic) );
	playSounds = new TGUI( app, guiCheckBox, 20,190,90,14, -3, STR(strPlaySounds) );
	AddItem( videoSettingsPage, new TGUI( app, guiLabel, 20,60,99,14, -3, STR(strVideo), 255,255,255 ) );
	AddItem( videoSettingsPage, new TGUI( app, guiLabel, 20,150,99,14, -3, STR(strAudio), 255,255,255 ) );
	runFullscreenCheckBox = new TGUI( app, guiCheckBox, 20,80,180,14, -3, STR(strFullscreen) );
//	limitFpsCheckBox = new TGUI( app, guiCheckBox, 20,100,180,14, -3, STR(strLimitFPS) );
	shadowDropDown = new TGUI( app, guiDropDown, 20,100,180,14, -3 );
	antiAliasCheckBox = new TGUI( app, guiCheckBox, 20,120,180,14, -3, STR(strAntiAlias) );

	musicVolumeSlider = new TGUI( app, guiSlider, 210,170,100,20, -3, "" );
	soundVolumeSlider = new TGUI( app, guiSlider, 210,190,100,20, -3, "" );
	renderDepthSlider = new TGUI( app, guiSlider, 100,240,100,20, -3, "" );

	resolutionDropDown = new TGUI( app, guiDropDown, 20,40,140,14, -2 );
	languageDropDown = new TGUI( app, guiDropDown, 200,40,120,14, -3 );

	size_t cntr = app->NumResolutions();
	size_t w,h,bd,zd,vfreq;
	for ( size_t i=0; i<cntr; i++ )
	{
		app->GetResolution( i, w,h,bd,zd,vfreq );
		TString msg = Int2Str(w) + " x " + Int2Str(h);
		msg = msg + " x " + Int2Str(bd);
		resolutionDropDown->AddDropDownItem( msg.c_str() );
	}
	AddItem( videoSettingsPage, resolutionDropDown );
	AddItem( videoSettingsPage, languageDropDown );

	AddItem( videoSettingsPage, runFullscreenCheckBox );
//	AddItem( videoSettingsPage, limitFpsCheckBox );
	AddItem( videoSettingsPage, shadowDropDown );
	AddItem( videoSettingsPage, antiAliasCheckBox );
	AddItem( videoSettingsPage, playMusic );
	AddItem( videoSettingsPage, playSounds );
	AddItem( videoSettingsPage, musicVolumeSlider );
	AddItem( videoSettingsPage, soundVolumeSlider );
	AddItem( videoSettingsPage, renderDepthSlider );

	AddItem( videoSettingsPage, okButton );
	AddItem( videoSettingsPage, cancelButton );
	okButton->ButtonKey( 13 );
	cancelButton->ButtonKey( VK_ESCAPE );

	videoSettingsPage->SetLocation( 100, 100 );

	languageDropDown->AddDropDownItem( STR(strEnglish) );
	languageDropDown->AddDropDownItem( STR(strGerman) );
	languageDropDown->AddDropDownItem( STR(strDutch) );
//	languageDropDown->AddDropDownItem( STR(strArabic) );

	shadowDropDown->AddDropDownItem( STR(strShadowNone) );
	shadowDropDown->AddDropDownItem( STR(strShadowGood) );
	shadowDropDown->AddDropDownItem( STR(strShadowBest) );

	resolutionDropDown->SelectedItem( currentRes );
	shadowDropDown->SelectedItem( shadowLevel );
	languageDropDown->SelectedItem( languageId );
	runFullscreenCheckBox->Checked( currentFullScreen==1 );
//	limitFpsCheckBox->Checked( limitFps==1 );
	antiAliasCheckBox->Checked( antiAlias==1 );
	playMusic->Checked( playMusicSetting==1 );
	playSounds->Checked( playSoundSetting==1 );

	musicVolumeSlider->SliderSetRange( 0, 255 );
	soundVolumeSlider->SliderSetRange( 0, 255 );
	renderDepthSlider->SliderSetRange( app->MinRenderDepth(), app->MaxRenderDepth() );

	musicVolumeSlider->SliderPos( musicVolume );
	soundVolumeSlider->SliderPos( effectsVolume );
	renderDepthSlider->SliderPos( renderDepth );

	initialised = true;
	TGUI* prevPage = app->CurrentPage();
	app->CurrentPage( videoSettingsPage );

	if ( soundSystem!=NULL )
	{
		TPoint fwd(0,0,1);
		TPoint up(0,1,0);
		soundSystem->SetCameraLocation( TPoint(0,0,0), fwd, up );
	}

	MSG msg;
	size_t fpsTimer = ::GetTickCount();
	bool closing = false;
	while ( !closing )
	{
		::ProcessMessages( app, msg, fpsTimer );
		if ( msg.message==WM_QUIT || msg.message==WM_CLOSE )
		{
			closing = true;
			app->CurrentPage( prevPage );
			::PostQuitMessage( 0 );
		}

		size_t tempEffectsVolume = soundVolumeSlider->SliderPos();
		if ( tempEffectsVolume!=effectsVolume )
		{
			effectsVolume = tempEffectsVolume;
			if ( soundSystem!=NULL )
			{
				soundSystem->Volume( effectsVolume );
				soundSystem->Play( scanon, TPoint(0,0,10) );
			}
		}
		size_t tempMusicVolume = musicVolumeSlider->SliderPos();
		if ( tempMusicVolume!=musicVolume )
		{
			musicVolume = tempMusicVolume;
			if ( soundSystem!=NULL )
			{
				soundSystem->StreamVolume( musicVolume );
			}
		}

		size_t tempRenderDepth = renderDepthSlider->SliderPos();
		if ( tempRenderDepth!=renderDepth )
		{
			renderDepth = tempRenderDepth;
			app->RenderDepth( renderDepth );
		}

		if ( okButton->Clicked() )
		{
			// write values back to registry
			currentRes = resolutionDropDown->SelectedItem();
			languageId = languageDropDown->SelectedItem();
			if ( runFullscreenCheckBox->Checked() )
				currentFullScreen = 1;
			else
				currentFullScreen = 0;
//			if ( limitFpsCheckBox->Checked() )
//				limitFps = 1;
//			else
//				limitFps = 0;
			shadowLevel = shadowDropDown->SelectedItem();

			if ( antiAliasCheckBox->Checked() )
			{
				antiAlias = 1;
				app->AntiAlias( true );
			}
			else
			{
				antiAlias = 0;
				app->AntiAlias( false );
			}

			if ( playSounds->Checked() )
				playSoundSetting = 1;
			else
				playSoundSetting = 0;

			if ( playMusic->Checked() )
				playMusicSetting = 1;
			else
				playMusicSetting = 0;

			effectsVolume = soundVolumeSlider->SliderPos();
			musicVolume = musicVolumeSlider->SliderPos();
			renderDepth = renderDepthSlider->SliderPos();

			app->CurrentPage( prevPage );
			app->RenderDepth( renderDepth );

			SetRegistryKey( "Software\\PDV\\Performance", "Resolution", currentRes );
			SetRegistryKey( "Software\\PDV\\Performance", "Language", languageId );
			SetRegistryKey( "Software\\PDV\\Performance", "FullScreen", currentFullScreen );
//			SetRegistryKey( "Software\\PDV\\Performance", "LimitFPS", limitFps );
			SetRegistryKey( "Software\\PDV\\Performance", "ShadowLevel", shadowLevel );
			SetRegistryKey( "Software\\PDV\\Performance", "AntiAliased", antiAlias );
			SetRegistryKey( "Software\\PDV\\Performance", "PlayMusic", playMusicSetting );
			SetRegistryKey( "Software\\PDV\\Performance", "PlayEffects", playSoundSetting );
			SetRegistryKey( "Software\\PDV\\Performance", "EffectsVolume", effectsVolume );
			SetRegistryKey( "Software\\PDV\\Performance", "MusicVolume", musicVolume );
			SetRegistryKey( "Software\\PDV\\Performance", "RenderDepth", renderDepth );

			// set level
			app->ShadowLevel( shadowLevel );

			return IDOK;
		}
		if ( cancelButton->Clicked() )
		{
			app->CurrentPage( prevPage );
			return IDCANCEL;
		}
	}
	return IDCANCEL;
#else
	return DialogBox( hInstance, 
                    MAKEINTRESOURCE(IDD_VIDEOSETUP), 
                    parent, (DLGPROC)VideoDlgProc);
#endif
}


void VideoSettingsDialog::UpdateButtons( void )
{
};


void VideoSettingsDialog::GetRegistrySettings( void )
{
	// get values and set them in the dialog
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Resolution", currentRes ) )
		currentRes = 0;
	prevRes = currentRes;

	if ( !GetRegistryKey( "Software\\PDV\\Performance", "Language", languageId ) )
		languageId = 0;
	prevLanguage = languageId;

	if ( !GetRegistryKey( "Software\\PDV\\Performance", "FullScreen", currentFullScreen ) )
	{
		currentFullScreen = 1;
	}
	else
	{
		// double check caption
		if ( (::GetWindowLong( app->Window(), GWL_STYLE ) & WS_CAPTION) > 0 && currentFullScreen==1 )
		{
			currentFullScreen = 0;
			::SetRegistryKey( "Software\\PDV\\Performance", "FullScreen", 0 );
		}
	}
	prevFullScreen = currentFullScreen;

//	if ( !GetRegistryKey( "Software\\PDV\\Performance", "LimitFPS", limitFps ) )
//		limitFps = 1;
//	prevLimitFps = limitFps;
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "ShadowLevel", shadowLevel ) )
	{
		shadowLevel = 0;
	}

	if ( !GetRegistryKey( "Software\\PDV\\Performance", "AntiAliased", antiAlias ) )
		antiAlias = 1;
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "PlayEffects", playSoundSetting ) )
		playSoundSetting = 1;
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "PlayMusic", playMusicSetting ) )
		playMusicSetting = 1;
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "EffectsVolume", effectsVolume ) )
		effectsVolume = app->EffectsVolume();
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "MusicVolume", musicVolume ) )
		musicVolume = app->MusicVolume();
	if ( !GetRegistryKey( "Software\\PDV\\Performance", "RenderDepth", renderDepth ) )
		renderDepth = app->RenderDepth();
};

bool VideoSettingsDialog::NeedsReload( void ) const
{
	return ( (prevRes!=currentRes) || (prevFullScreen!=currentFullScreen) );
}

bool VideoSettingsDialog::LanguageChanged( size_t& newLanguageId ) const
{
	newLanguageId = languageId;
	return ( (prevLanguage!=languageId) );
}

//==========================================================================
#ifndef _USEGUI
LRESULT CALLBACK VideoDlgProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
		
		case WM_INITDIALOG:
			{
				VideoSettingsDialog::ptr->GetRegistrySettings();

				VideoSettingsDialog::ptr->initialised = true;

				VideoSettingsDialog::ptr->hRes = GetDlgItem( hwnd, IDC_RESOLUTION );
//				VideoSettingsDialog::ptr->hLimitFPS = GetDlgItem( hwnd, IDC_LIMITFPS );
				VideoSettingsDialog::ptr->hFullScreen = GetDlgItem( hwnd, IDC_FULLSCREEN );

				VideoSettingsDialog::ptr->EnumResolutions();
				SetControlHeight( VideoSettingsDialog::ptr->hRes, 100 );
				size_t cntr = VideoSettingsDialog::ptr->NumResolutions();
				int w,h,bd,zd;
				for ( size_t i=0; i<cntr; i++ )
				{
					VideoSettingsDialog::ptr->GetResolution( i, w,h,bd,zd );
					TString msg = Int2Str(w) + " x " + Int2Str(h);
					msg = msg + " x " + Int2Str(bd);
					ComboBoxAddString( VideoSettingsDialog::ptr->hRes, msg.c_str() );
				}
				DropDownBoxSetSelection( VideoSettingsDialog::ptr->hRes, VideoSettingsDialog::ptr->currentRes );
				ButtonSetCheck( VideoSettingsDialog::ptr->hFullScreen, VideoSettingsDialog::ptr->currentFullScreen==1 );
//				ButtonSetCheck( VideoSettingsDialog::ptr->hLimitFPS, VideoSettingsDialog::ptr->limitFps==1 );
				return 0;
			}

		case WM_CLOSE:
			{
				::EndDialog(hwnd,IDCANCEL);
			}

		case WM_COMMAND:
			{
				if ( HIWORD(wParam)==BN_CLICKED )
				{
					if ( LOWORD(wParam)==IDOK )
					{
						int cres = DropDownBoxGetSelection( VideoSettingsDialog::ptr->hRes );
						bool fs = ButtonGetCheck( VideoSettingsDialog::ptr->hFullScreen );
//						bool limit = ButtonGetCheck( VideoSettingsDialog::ptr->hLimitFPS );
						if ( cres>=0 )
						{
							SetRegistryKey( "Software\\PDV\\Performance", "Resolution", cres );
							size_t fullScreen = 0;
							if ( fs )
								fullScreen = 1;
							SetRegistryKey( "Software\\PDV\\Performance", "FullScreen", fullScreen );
							size_t limitFPS = 0;
							if ( limit )
								limitFPS = 1;
//							SetRegistryKey( "Software\\PDV\\Performance", "LimitFPS", limitFPS );
							EndDialog(hwnd, IDOK);
						}
						return 0;
					}
					if ( LOWORD(wParam)==IDCANCEL )
					{
						EndDialog(hwnd, IDCANCEL);
						return 0;
					}
				}
				break;
			}

		case WM_SYSCOMMAND:
			{
				if (wParam==IDCANCEL)
				{
					EndDialog(hwnd, IDCANCEL);
					return 0;
				}
			}
			break;

		case WM_SYSKEYUP:
			{
				if ( wParam=='C' || wParam=='c' )
				{
				}
			}
			break;

		default:
			{
				return 0;
			}
	}
	return 0;
}

#endif

//==========================================================================

