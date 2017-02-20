#include <windows.h>
#include <stdio.h>

#include <objbase.h>
#include <initguid.h>
#include <mmreg.h>
#include <dsound.h>
#include "WavRead.h"

#include <common/compatability.h>
#include <common/string.h>
#include <common/font.h>

//-----------------------------------------------------------------------------
// File: Play3DSound.cpp
//
// Desc: DirectSound support for how to load a wave file and play it using a 
//       3D DirectSound buffer.
//
// Copyright (c) 1999 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------

class EnumResult
{
public:
	EnumResult( LPGUID _lpGuid,
				LPCSTR _lpcstrDescription,
				size_t score );
	~EnumResult( void );

	LPGUID lpGuid;
	char*  lpcstrDescription[256];
	size_t score;

	EnumResult* next;
};

EnumResult* resultList = NULL;

EnumResult::EnumResult( LPGUID _lpGuid,
						LPCSTR _lpcstrDescription,
						size_t _score )
	: next(NULL)
{
	lpGuid = _lpGuid;
	strcpy( (char*)lpcstrDescription, _lpcstrDescription );
	score = _score;
};

EnumResult::~EnumResult( void )
{
	if ( next!=NULL )
	{
		EnumResult* temp = next;
		next = NULL;
		delete temp;
	}
};

void AddEnumResult( LPGUID _lpGuid,
					LPCSTR _lpcstrDescription,
					size_t _score )
{
	EnumResult* er = new EnumResult( _lpGuid, _lpcstrDescription, _score );
	if ( resultList!=NULL )
	{
		EnumResult* temp = resultList;
		while ( temp->next!=NULL )
			temp = temp->next;
		temp->next =er;
	}
	else
	{
		resultList = er;
	}
};

bool GetBestResult( LPGUID& _lpGuid,
					char* _lpcstrDescription,
					size_t& _score )
{
	if ( resultList==NULL )
	{
		_lpGuid = NULL;
		return false;
	}
	EnumResult* best = resultList;
	EnumResult* temp = resultList;
	while ( temp!=NULL )
	{
		if ( temp->score > best->score )
		{
			best = temp;
		}
		temp = temp->next;
	}
	_lpGuid = best->lpGuid;
	strcpy(_lpcstrDescription, (char*)best->lpcstrDescription );
	_score = best->score;

	return true;
};

void ClearResults( )
{
	if ( resultList!=NULL )
		delete resultList;
	resultList = NULL;
};

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

//-----------------------------------------------------------------------------

HRESULT CreateStaticBuffer( TCHAR* strFileName, int index, void* ptr );
HRESULT FillBuffer( int index, void* ptr );
HRESULT RestoreBuffers( int index, void* ptr );

//-----------------------------------------------------------------------------

#define kMaxWaves 100

//-----------------------------------------------------------------------------

class TDirectSound
{
	const TDirectSound& operator=( const TDirectSound& );
	TDirectSound( const TDirectSound& );
public:
	TDirectSound( void );
	~TDirectSound( void );

public:
	LPDIRECTSOUND           g_pDS;

	LPDIRECTSOUNDBUFFER     g_pDSBuffer[kMaxWaves];
	LPDIRECTSOUND3DBUFFER   g_pDS3DBuffer[kMaxWaves];

	DWORD                   g_dwBufferBytes[kMaxWaves];
	CWaveSoundRead*         g_pWaveSoundRead[kMaxWaves];

	LPDIRECTSOUND3DLISTENER g_pDSListener;
	DS3DLISTENER            g_dsListenerParams;
	DS3DBUFFER              g_dsBufferParams;

	bool					g_bDeferSettings;
};

//-----------------------------------------------------------------------------

TDirectSound::TDirectSound( void )
{
	g_pDSListener = NULL;
	g_pDS = NULL;
	g_bDeferSettings = false;

	for ( int i=0; i<kMaxWaves; i++ )
	{
		g_pDSBuffer[i] = NULL;
		g_pDS3DBuffer[i] = NULL;
		g_pWaveSoundRead[i] = NULL;
	}
};


TDirectSound::~TDirectSound( void )
{
	for ( int i=0; i<kMaxWaves; i++ )
	{
		if ( g_pWaveSoundRead[i]!=NULL )
			SAFE_DELETE( g_pWaveSoundRead[i] );
		if ( g_pDS3DBuffer[i]!=NULL )
			SAFE_RELEASE( g_pDS3DBuffer[i] );
		if ( g_pDSBuffer[i]!=NULL )
			SAFE_RELEASE( g_pDSBuffer[i] );
	}

    // Release DirectSound interfaces
    SAFE_RELEASE( g_pDSListener );
    SAFE_RELEASE( g_pDS ); 
	g_pDS = NULL;
};

//-----------------------------------------------------------------------------

BOOL CALLBACK DSEnumCallback( LPGUID lpGuid,            
							  LPCSTR lpcstrDescription,  
							  LPCSTR lpcstrModule,       
							  LPVOID lpContext )
{
    HRESULT             hr;
	LPDIRECTSOUND       g_pDS = NULL;
	HWND				hWnd = (HWND)lpContext;

	WriteString( "\nDSEnumCallback: " );
	WriteString( lpcstrDescription );
    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate( NULL, &g_pDS, NULL ) ) )
	{
	    SAFE_RELEASE( g_pDS ); 
		g_pDS = NULL;
        return TRUE;
	}

    // Set coop level to DSSCL_PRIORITY
    if( FAILED( hr = g_pDS->SetCooperativeLevel( hWnd, DSSCL_PRIORITY ) ) )
	{
	    SAFE_RELEASE( g_pDS ); 
		g_pDS = NULL;
        return TRUE;
	}

	DSCAPS caps;
    ZeroMemory( &caps, sizeof(DSCAPS) );
	caps.dwSize = sizeof(DSCAPS);
	g_pDS->GetCaps(&caps);

	// write out caps of 3D sound device
	WriteString( "\nSound card capabilities:\n" );
	DWORD flag = caps.dwFlags;
	size_t score = 0;
	if ( (flag&DSBCAPS_CTRL3D)!=0 )
	{
		WriteString( "DSBCAPS_CTRL3D\n" );
		score += 10;
	}
	if ( (flag&DSBCAPS_LOCDEFER)!=0 )
	{
		WriteString( "DSBCAPS_LOCDEFER\n" );
		score++;
	}
	if ( (flag&DSBCAPS_CTRLPAN)!=0 )
	{
		WriteString( "DSBCAPS_CTRLPAN\n" );
		score++;
	}
	if ( (flag&DSBCAPS_CTRLVOLUME)!=0 )
	{
		WriteString( "DSBCAPS_CTRLVOLUME\n" );
		score++;
	}
	if ( (flag&DSBCAPS_PRIMARYBUFFER)!=0 )
	{
		WriteString( "DSBCAPS_PRIMARYBUFFER\n" );
		score += 5;
	}
	if ( (flag&DSBCAPS_CTRLFREQUENCY )!=0 )
	{
		WriteString( "DSBCAPS_CTRLFREQUENCY\n" );
		score++;
	}
	if ( (flag&DSBCAPS_CTRLPOSITIONNOTIFY )!=0 )
	{
		WriteString( "DSBCAPS_CTRLPOSITIONNOTIFY\n" );
		score++;
	}
	if ( (flag&DSBCAPS_GETCURRENTPOSITION2 )!=0 )
	{
		WriteString( "DSBCAPS_GETCURRENTPOSITION2\n" );
		score++;
	}
	if ( (flag&DSBCAPS_GLOBALFOCUS )!=0 )
	{
		WriteString( "DSBCAPS_GLOBALFOCUS\n" );
		score++;
	}
	if ( (flag&DSBCAPS_LOCHARDWARE )!=0 )
	{
		WriteString( "DSBCAPS_LOCHARDWARE\n" );
		score++;
	}
	if ( (flag&DSBCAPS_MUTE3DATMAXDISTANCE )!=0 )
	{
		WriteString( "DSBCAPS_MUTE3DATMAXDISTANCE\n" );
		score++;
	}
	if ( (flag&DSBCAPS_STATIC )!=0 )
	{
		WriteString( "DSBCAPS_STATIC\n" );
		score++;
	}
	if ( (flag&DSBCAPS_STICKYFOCUS )!=0 )
	{
		WriteString( "DSBCAPS_STICKYFOCUS\n" );
		score++;
	}
	if ( (flag&DSBCAPS_LOCSOFTWARE )!=0 )
	{
		WriteString( "DSBCAPS_LOCSOFTWARE\n" );
		score++;
	}

    SAFE_RELEASE( g_pDS ); 
	g_pDS = NULL;

	AddEnumResult( lpGuid, (char*)lpcstrDescription, score );

	return TRUE;
}

//-----------------------------------------------------------------------------
// Name: IniTDirectSound()
// Desc: Initilizes DirectSound
//-----------------------------------------------------------------------------
void* WINAPI InitDirectSound( HWND hDlg, int nChannels,
							  int samplesPerSec, int bitDepth )
{
	TDirectSound* directSound = new TDirectSound();

	// enumerate all possible drivers
	WriteString( "\nEnumerating Direct Sound drivers\n" );
	DirectSoundEnumerate( DSEnumCallback, hDlg );

	LPGUID lpGuid;
	char   lpcstrDescription[256];
	size_t score;

	GetBestResult( lpGuid, lpcstrDescription, score );
	WriteString( "Picked \"%s\" as sound driver\n", lpcstrDescription );
	ClearResults();

	HRESULT             hr;
    DSBUFFERDESC        dsbdesc;
    LPDIRECTSOUNDBUFFER pDSBPrimary;

    // Initialize COM
    if( hr = CoInitialize( NULL ) )
	{
		delete directSound;
        return NULL;
	}

    // Create IDirectSound using the primary sound device
    if( FAILED( hr = DirectSoundCreate( lpGuid, &directSound->g_pDS, NULL ) ) )
	{
		delete directSound;
        return NULL;
	}

    // Set coop level to DSSCL_PRIORITY
    if( FAILED( hr = directSound->g_pDS->SetCooperativeLevel( hDlg, DSSCL_PRIORITY ) ) )
	{
		delete directSound;
        return NULL;
	}

    // Obtain primary buffer, asking it for 3D control

    ZeroMemory( &dsbdesc, sizeof(DSBUFFERDESC) );
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	// no settings necessairy - static buffer will do it from here!
    dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	// DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER |  | 
	// DSBCAPS_LOCDEFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;

    if( FAILED( hr = directSound->g_pDS->CreateSoundBuffer( &dsbdesc, &pDSBPrimary, NULL ) ) )
	{
		delete directSound;
        return NULL;
	}

    if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, 
                                                  (VOID**)&directSound->g_pDSListener ) ) )
	{
		delete directSound;
		return NULL;
	}

    // Get listener parameters
    directSound->g_dsListenerParams.dwSize = sizeof(DS3DLISTENER);
    directSound->g_pDSListener->GetAllParameters( &directSound->g_dsListenerParams );

    // Set primary buffer format to 22kHz and 16-bit output.
    WAVEFORMATEX wfx;
    ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
    wfx.wFormatTag      = WAVE_FORMAT_PCM; 
    wfx.nChannels       = nChannels; 
    wfx.nSamplesPerSec  = samplesPerSec; 
    wfx.wBitsPerSample  = bitDepth; 
    wfx.nBlockAlign     = wfx.wBitsPerSample / 8 * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
	{
		delete directSound;
        return NULL;
	}

    // Release the primary buffer, since it is not need anymore
    SAFE_RELEASE( pDSBPrimary );

    return (void*)(directSound);
}




//-----------------------------------------------------------------------------
// Name: FreeDirectSound()
// Desc: Releases DirectSound 
//-----------------------------------------------------------------------------
bool WINAPI FreeDirectSound( void* ptr )
{
	delete ptr;

    // Release COM
    CoUninitialize();

    return true;
}




//-----------------------------------------------------------------------------
// Name: SetParameters()
// Desc: 
//-----------------------------------------------------------------------------
void WINAPI SetParameters(	float fDopplerFactor, float fRolloffFactor,
							float fMinDistance,   float fMaxDistance, 
							int index, void* ptr )
{
   	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

    // Every change to 3-D sound buffer and listener settings causes 
    // DirectSound to remix, at the expense of CPU cycles. 
    // To minimize the performance impact of changing 3-D settings, 
    // use the DS3D_DEFERRED flag in the dwApply parameter of any of 
    // the IDirectSound3DListener or IDirectSound3DBuffer methods that 
    // change 3-D settings. Then call the IDirectSound3DListener::CommitDeferredSettings 
    // method to execute all of the deferred commands at once.
//    DWORD dwApplyFlag = ( pds->g_bDeferSettings ) ? DS3D_DEFERRED : DS3D_IMMEDIATE;
    DWORD dwApplyFlag = DS3D_IMMEDIATE;

    pds->g_dsListenerParams.flDopplerFactor = fDopplerFactor;
    pds->g_dsListenerParams.flRolloffFactor = fRolloffFactor;

    if( pds->g_pDSListener )
        pds->g_pDSListener->SetAllParameters( &pds->g_dsListenerParams, dwApplyFlag );

	pds->g_dsBufferParams.flMinDistance = fMinDistance;
	pds->g_dsBufferParams.flMaxDistance = fMaxDistance;
	if( pds->g_pDS3DBuffer[index]!=NULL )
		pds->g_pDS3DBuffer[index]->SetAllParameters( &pds->g_dsBufferParams, dwApplyFlag );
}

//-----------------------------------------------------------------------------
// Name: SetObjectProperties()
// Desc: Sets the position and velocity on the 3D buffer
//-----------------------------------------------------------------------------
void WINAPI SetObjectProperties( float _x, float _y, float _z, 
								 float vx, float vy, float vz, 
								 int index, void*ptr )
{
   	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

	D3DVECTOR pvPosition;
	D3DVECTOR pvVelocity;

	pvPosition.x = _x;
	pvPosition.y = _y;
	pvPosition.z = _z;

	pvVelocity.x = vx;
	pvVelocity.y = vy;
	pvVelocity.z = vz;

    // Every change to 3-D sound buffer and listener settings causes 
    // DirectSound to remix, at the expense of CPU cycles. 
    // To minimize the performance impact of changing 3-D settings, 
    // use the DS3D_DEFERRED flag in the dwApply parameter of any of 
    // the IDirectSound3DListener or IDirectSound3DBuffer methods that 
    // change 3-D settings. Then call the IDirectSound3DListener::CommitDeferredSettings 
    // method to execute all of the deferred commands at once.
    DWORD dwApplyFlag = ( pds->g_bDeferSettings ) ? DS3D_DEFERRED : DS3D_IMMEDIATE;

	pds->g_dsBufferParams.dwSize = sizeof(DS3DBUFFER);
    memcpy( &pds->g_dsBufferParams.vPosition, &pvPosition, sizeof(D3DVECTOR) );
    memcpy( &pds->g_dsBufferParams.vVelocity, &pvVelocity, sizeof(D3DVECTOR) );
//	pds->g_dsBufferParams.flMinDistance = 0;
//	pds->g_dsBufferParams.flMaxDistance = 100;
	pds->g_dsBufferParams.dwMode = DS3DMODE_NORMAL; //DS3DMODE_HEADRELATIVE;
	if( pds->g_pDS3DBuffer[index]!=NULL )
	    pds->g_pDS3DBuffer[index]->SetAllParameters( &pds->g_dsBufferParams, dwApplyFlag );
}




//-----------------------------------------------------------------------------
// Name: LoadWaveFile()
// Desc: Loads the wave file into a secondary static DirectSound buffer
//-----------------------------------------------------------------------------
bool WINAPI LoadWaveFile( const char* strFileName, int index, void* ptr )
{
    // Create the sound buffer object from the wave file data
    if( FAILED( CreateStaticBuffer( const_cast<char*>(strFileName), index, ptr ) ) )
    {        
		return false;
    }
    else // The sound buffer was successfully created
    {
        // Fill the buffer with wav data
        FillBuffer(index,ptr);
    }
	return true;
}



//-----------------------------------------------------------------------------
// Name: CreateStaticBuffer()
// Desc: Creates a wave file, and sound buffer 
//-----------------------------------------------------------------------------
HRESULT CreateStaticBuffer( TCHAR* strFileName, int index, void* ptr )
{
   	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

    HRESULT hr; 

    // Free any previous globals 
    SAFE_DELETE( pds->g_pWaveSoundRead[index] );
    SAFE_RELEASE( pds->g_pDSBuffer[index] );

    // Create a new wave file class
    pds->g_pWaveSoundRead[index] = new CWaveSoundRead();

    // Load the wave file
    if( FAILED( hr = pds->g_pWaveSoundRead[index]->Open( strFileName ) ) )
    {
        return S_FALSE;
    }

    // Set up the direct sound buffer, only requesting the flags we need, 
    // since each require overhead.
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_CTRL3D | DSBCAPS_STATIC | 
						 DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = pds->g_pWaveSoundRead[index]->m_ckIn.cksize;
    dsbd.lpwfxFormat   = pds->g_pWaveSoundRead[index]->m_pwfx;
    
    // Set the software DirectSound3D emulation algorithm to use
    // so display the algorith dialog box.
//	dsbd.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
//	dsbd.guid3DAlgorithm = DS3DALG_HRTF_FULL;

	// wrong version???
	dsbd.guid3DAlgorithm = DS3DALG_HRTF_LIGHT;

    // Create the static DirectSound buffer 
    if( FAILED( hr = pds->g_pDS->CreateSoundBuffer( &dsbd, &pds->g_pDSBuffer[index], NULL ) ) )
        return hr;

    // Get the 3D buffer from the secondary buffer
    if( FAILED( hr = pds->g_pDSBuffer[index]->QueryInterface( IID_IDirectSound3DBuffer, 
                                                  (VOID**)&pds->g_pDS3DBuffer[index] ) ) )
        return hr;

    pds->g_dwBufferBytes[index] = dsbd.dwBufferBytes;

    // Get the 3D buffer parameters
    pds->g_dsBufferParams.dwSize = sizeof(DS3DBUFFER);
    pds->g_pDS3DBuffer[index]->GetAllParameters( &pds->g_dsBufferParams );

    // Set new 3D buffer parameters
    pds->g_dsBufferParams.dwMode = DS3DMODE_HEADRELATIVE;
    pds->g_pDS3DBuffer[index]->SetAllParameters( &pds->g_dsBufferParams, DS3D_IMMEDIATE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FillBuffer()
// Desc: Fill the DirectSound buffer with data from the wav file
//-----------------------------------------------------------------------------
HRESULT FillBuffer( int index, void* ptr )
{
    HRESULT hr; 
    BYTE*   pbWavData; // Pointer to actual wav data 
    UINT    cbWavSize; // Size of data
    VOID*   pbData  = NULL;
    VOID*   pbData2 = NULL;
    DWORD   dwLength;
    DWORD   dwLength2;

	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

    // The size of wave data is in pWaveFileSound->m_ckIn
    INT nWaveFileSize = pds->g_pWaveSoundRead[index]->m_ckIn.cksize;

    // Allocate that buffer.
    pbWavData = new BYTE[ nWaveFileSize ];
    if( NULL == pbWavData )
        return E_OUTOFMEMORY;

    if( FAILED( hr = pds->g_pWaveSoundRead[index]->Read( nWaveFileSize, 
                                           pbWavData, 
                                           &cbWavSize ) ) )           
        return hr;

    // Reset the file to the beginning 
    pds->g_pWaveSoundRead[index]->Reset();

    // Lock the buffer down
    if( FAILED( hr = pds->g_pDSBuffer[index]->Lock( 0, pds->g_dwBufferBytes[index], &pbData, &dwLength, 
                                   &pbData2, &dwLength2, 0L ) ) )
        return hr;

    // Copy the memory to it.
    memcpy( pbData, pbWavData, pds->g_dwBufferBytes[index] );

    // Unlock the buffer, we don't need it anymore.
    pds->g_pDSBuffer[index]->Unlock( pbData, pds->g_dwBufferBytes[index], NULL, 0 );
    pbData = NULL;

    // We dont need the wav file data buffer anymore, so delete it 
    SAFE_DELETE( pbWavData );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: PlayBuffer()
// Desc: User hit the "Play" button, so play the DirectSound buffer
//-----------------------------------------------------------------------------
bool WINAPI PlayBuffer( bool bLooped, int index, void* ptr )
{
    HRESULT hr;
    DWORD   dwStatus;

	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

    if( NULL == pds->g_pDSBuffer[index] )
        return false;

    // Restore the buffers if they are lost
    if( FAILED( hr = RestoreBuffers(index,ptr) ) )
        return false;

    if( FAILED( hr = pds->g_pDSBuffer[index]->GetStatus( &dwStatus ) ) )
        return false;

    if( dwStatus & DSBSTATUS_PLAYING )
    {
        // Don't bother playing, just restart
        pds->g_pDSBuffer[index]->SetCurrentPosition( 0 );
    }
    else
    {
        // Play buffer 
        DWORD dwLooped = bLooped ? DSBPLAY_LOOPING : 0L;
        if( FAILED( hr = pds->g_pDSBuffer[index]->Play( 0, 0, dwLooped ) ) )
            return false;
    }
    return true;
}




//-----------------------------------------------------------------------------
// Name: StopBuffer()
// Desc: Stop the DirectSound buffer from playing 
//-----------------------------------------------------------------------------
void WINAPI StopBuffer( bool bResetPosition, int index, void* ptr )
{
	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

    if( NULL == pds->g_pDSBuffer[index] )
        return;

    pds->g_pDSBuffer[index]->Stop();

    if( bResetPosition )
        pds->g_pDSBuffer[index]->SetCurrentPosition( 0L );    
}



//-----------------------------------------------------------------------------
// Name: RestoreBuffers()
// Desc: Restore lost buffers and fill them up with sound if possible
//-----------------------------------------------------------------------------
HRESULT RestoreBuffers( int index, void* ptr )
{
    HRESULT hr;

	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

    if( NULL == pds->g_pDSBuffer[index] )
        return S_OK;

    DWORD dwStatus;
    if( FAILED( hr = pds->g_pDSBuffer[index]->GetStatus( &dwStatus ) ) )
        return hr;

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
        // Since the app could have just been activated, then
        // DirectSound may not be giving us control yet, so 
        // the restoring the buffer may fail.  
        // If it does, sleep until DirectSound gives us control.
		int cntr = 0;
        do 
        {
            hr = pds->g_pDSBuffer[index]->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
			cntr++;
        }
        while( hr = pds->g_pDSBuffer[index]->Restore() && cntr<100 );

		if (hr==DSERR_BUFFERLOST)
			return hr;

        if( FAILED( hr = FillBuffer(index,ptr) ) )
            return hr;
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: IsBufferPlaying()
// Desc: flag is it playing?
//-----------------------------------------------------------------------------
bool WINAPI IsBufferPlaying( int index, void* ptr )
{
    DWORD dwStatus = 0;
	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

    if( NULL == pds->g_pDSBuffer[index] )
        return false;

    pds->g_pDSBuffer[index]->GetStatus( &dwStatus );

    if( dwStatus & DSBSTATUS_PLAYING )
        return true;
    else 
        return false;
}


//-----------------------------------------------------------------------------
// Name: SetPan( pan [0..1] )
// Desc: User sets panning [0=left,0.5=center,1=right] of sound
//-----------------------------------------------------------------------------
void WINAPI SetPan( float pan, int index, void* ptr )
{
   	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

	if ( pan<0 )
		pan = 0;
	if ( pan>1 )
		pan = 1;

	LONG lPan;
	if ( pan==0.5f )
		lPan = DSBPAN_CENTER;
	else
	{
		float span = DSBPAN_RIGHT - DSBPAN_LEFT;
		lPan = DSBPAN_LEFT + LONG(span*pan);
	}

	if( pds->g_pDSBuffer[index]!=NULL )
		pds->g_pDSBuffer[index]->SetPan( lPan );
};


//-----------------------------------------------------------------------------
// Name: SetVolume( vol [0..1] )
// Desc: User sets volume [0=none, 1=default] of sound
//-----------------------------------------------------------------------------
void WINAPI SetVolume( float vol, int index, void* ptr )
{
   	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

	if ( vol<0 )
		vol = 0;
	if ( vol>1 )
		vol = 1;

	LONG lVol;
	float span = DSBVOLUME_MAX - DSBVOLUME_MIN;
	lVol = DSBVOLUME_MIN + LONG(span*vol);

	if( pds->g_pDSBuffer[index]!=NULL )
		pds->g_pDSBuffer[index]->SetVolume( lVol );
};
