#ifndef _USE3DSOUND_

//-----------------------------------------------------------------------------
// File: PlaySound.cpp
//
// Desc: DirectSound support for how to load a wave file and play it using a 
//       static DirectSound buffer.
//
// Copyright (c) 1999 Microsoft Corp. All rights reserved.
//-----------------------------------------------------------------------------
#define STRICT
#include <objbase.h>
#include <initguid.h>
#include <mmreg.h>
#include <dsound.h>
#include "WavRead.h"


//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

bool DSError( HRESULT hr, char* str );

//-----------------------------------------------------------------------------

HRESULT CreateStaticBuffer( TCHAR* strFileName, int index, void* ptr );
HRESULT FillBuffer( int index, void* ptr );
HRESULT RestoreBuffers( int index, void* ptr );
bool WINAPI FreeDirectSound( void* ptr );

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
	LPDIRECTSOUND		g_pDS;

	LPDIRECTSOUNDBUFFER g_pDSBuffer[kMaxWaves];
	CWaveSoundRead*     g_pWaveSoundRead[kMaxWaves];
	DWORD               g_dwBufferBytes[kMaxWaves];
};

//-----------------------------------------------------------------------------

//LPDIRECTSOUND g_pDS = NULL;

//-----------------------------------------------------------------------------

TDirectSound::TDirectSound( void )
{
	for ( int i=0; i<kMaxWaves; i++ )
	{
		g_pDSBuffer[i] = NULL;
		g_pWaveSoundRead[i] = NULL;
	}
};


TDirectSound::~TDirectSound( void )
{
	::FreeDirectSound( this );
};

//-----------------------------------------------------------------------------
// Name: InitDirectSound()
// Desc: Initilizes DirectSound
//-----------------------------------------------------------------------------
void* WINAPI InitDirectSound( HWND hDlg, int nChannels, int samplesPerSec, int bitDepth )
{
	TDirectSound* directSound = new TDirectSound();

    HRESULT hr;
    LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

    // Initialize COM
    if( hr = CoInitialize( NULL ) )
		return NULL;

	// Create IDirectSound using the primary sound device
	if( FAILED( hr = DirectSoundCreate( NULL, &directSound->g_pDS, NULL ) ) )
        return NULL;

	// Set coop level to DSSCL_PRIORITY
	if( FAILED( hr = directSound->g_pDS->SetCooperativeLevel( hDlg, DSSCL_PRIORITY ) ) )
        return NULL;

	// Get the primary buffer 
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.dwFlags		= DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D | DSBCAPS_LOCDEFER | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
//	dsbd.dwFlags        = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN |
//						  DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_LOCHARDWARE;
	dsbd.dwBufferBytes  = 0;
	dsbd.lpwfxFormat    = NULL;

	if( FAILED( hr = directSound->g_pDS->CreateSoundBuffer( &dsbd, &pDSBPrimary, NULL ) ) )
	{
//		char str[256];
//		DSError( hr, str );
        return NULL;
	}

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
        return NULL;

    SAFE_RELEASE( pDSBPrimary );

	return (void*)directSound;
}




//-----------------------------------------------------------------------------
// Name: FreeDirectSound()
// Desc: Releases DirectSound 
//-----------------------------------------------------------------------------
bool WINAPI FreeDirectSound( void* ptr )
{
	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

	for ( int i=0; i<kMaxWaves; i++ )
	{
		if ( pds->g_pWaveSoundRead[i]!=NULL )
			SAFE_DELETE( pds->g_pWaveSoundRead[i] );
		if ( pds->g_pDSBuffer[i]!=NULL )
			SAFE_RELEASE( pds->g_pDSBuffer[i] );
	}

    // Release DirectSound interfaces
    SAFE_RELEASE( pds->g_pDS ); 

	pds->g_pDS = NULL;

    // Release COM
    CoUninitialize();

    return true;
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
// Desc: Creates a wave file, sound buffer and notification events 
//-----------------------------------------------------------------------------
HRESULT CreateStaticBuffer( TCHAR* strFileName, int index, void* ptr )
{
    HRESULT hr; 

	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

    // Free any previous globals 
    SAFE_DELETE( pds->g_pWaveSoundRead[index] );
    SAFE_RELEASE( pds->g_pDSBuffer[index] );

    // Create a new wave file class
    pds->g_pWaveSoundRead[index] = new CWaveSoundRead();

    // Load the wave file
    if( FAILED( pds->g_pWaveSoundRead[index]->Open( strFileName ) ) )
    {
		return false;
    }

    // Set up the direct sound buffer, and only request the flags needed
    // since each requires some overhead and limits if the buffer can 
    // be hardware accelerated
    DSBUFFERDESC dsbd;
    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
    dsbd.dwSize        = sizeof(DSBUFFERDESC);
    dsbd.dwFlags       = DSBCAPS_STATIC;
    dsbd.dwBufferBytes = pds->g_pWaveSoundRead[index]->m_ckIn.cksize;
    dsbd.lpwfxFormat   = pds->g_pWaveSoundRead[index]->m_pwfx;

    // Create the static DirectSound buffer 
    if( FAILED( hr = pds->g_pDS->CreateSoundBuffer( &dsbd, &pds->g_pDSBuffer[index], NULL ) ) )
        return hr;

    // Remember how big the buffer is
    pds->g_dwBufferBytes[index] = dsbd.dwBufferBytes;

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

/*
//-----------------------------------------------------------------------------
// Name: SetFrequency( freq [0..1] )
// Desc: User sets frequency of sound [0=1=normal, <0..1> is min freq to max freq]
//-----------------------------------------------------------------------------
void WINAPI SetFrequency( float freq, int index, void* ptr )
{
   	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

	if ( freq<0 )
		freq = 0;
	if ( freq>1 )
		freq = 1;

	DWORD dwFrequency;
	if ( freq==0 || freq==1 )
		dwFrequency = DSBFREQUENCY_ORIGINAL;
	else
		dwFrequency = DSBFREQUENCY_MIN + DWORD(float(DSBFREQUENCY_MAX) * freq);

	if( pds->g_pDSBuffer[index]!=NULL )
	{
		HRESULT ret = pds->g_pDSBuffer[index]->SetFrequency( dwFrequency );
		char str[256];
		if ( ret!=DS_OK )
		{
			switch (ret)
			{
			case DSERR_CONTROLUNAVAIL:
				{
					strcpy( str, "DSERR_CONTROLUNAVAIL" );
					break;
				}
			case DSERR_GENERIC:
				{
					strcpy( str, "DSERR_GENERIC" );
					break;
				}
			case DSERR_INVALIDPARAM:
				{
					strcpy( str, "DSERR_INVALIDPARAM" );
					break;
				}
			case DSERR_PRIOLEVELNEEDED:
				{
					strcpy( str, "DSERR_PRIOLEVELNEEDED" );
					break;
				}
			}
		}
	}
};
*/

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

//-----------------------------------------------------------------------------
// Name: PlayBuffer()
// Desc: User hit the "Play" button, so play the DirectSound buffer
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
// Name: PlayBuffer()
// Desc: User hit the "Play" button, so play the DirectSound buffer
//-----------------------------------------------------------------------------
bool WINAPI PlayBuffer( bool bLooped, int index, void* ptr )
{
    HRESULT hr;
	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

    if( NULL == pds->g_pDSBuffer[index] )
        return false;

    // Restore the buffers if they are lost
    if( FAILED( hr = RestoreBuffers(index,ptr) ) )
        return false;

    // Play buffer 
    DWORD dwLooped = bLooped ? DSBPLAY_LOOPING : 0L;
    if( FAILED( hr = pds->g_pDSBuffer[index]->Play( 0, 0, dwLooped ) ) )
        return false;

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
// Name: IsSoundPlaying()
// Desc: Checks to see if a sound is playing and returns TRUE if it is.
//-----------------------------------------------------------------------------
bool WINAPI IsSoundPlaying( int index, void* ptr )
{
	TDirectSound* pds = static_cast<TDirectSound*>(ptr);

    if( pds->g_pDSBuffer[index] )
    {  
        DWORD dwStatus = 0;
        pds->g_pDSBuffer[index]->GetStatus( &dwStatus );
        return( ( dwStatus & DSBSTATUS_PLAYING ) != 0 );
    }
    else
    {
        return false;
    }
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
        do 
        {
            hr = pds->g_pDSBuffer[index]->Restore();
            if( hr == DSERR_BUFFERLOST )
                Sleep( 10 );
        }
        while( hr = pds->g_pDSBuffer[index]->Restore() );

        if( FAILED( hr = FillBuffer(index,ptr) ) )
            return hr;
    }

    return S_OK;
}


bool DSError( HRESULT hr, char* str )
{
	switch (hr)
	{
	case DS_OK:
		{
			strcpy( str, "ok" );
			return false;
			break;
		}

// The function completed successfully, but we had to substitute the 3D algorithm
	case DS_NO_VIRTUALIZATION:
		{
			strcpy( str, "DS_NO_VIRTUALIZATION" );
			return false;
			break;
		}

// The call failed because resources (such as a priority level)
// were already being used by another caller
	case DSERR_ALLOCATED:
		{
			strcpy( str, "DSERR_ALLOCATED" );
			return false;
			break;
		}

// The control (vol,pan,etc.) requested by the caller is not available
	case DSERR_CONTROLUNAVAIL:
		{
			strcpy( str, "DSERR_CONTROLUNAVAIL" );
			return false;
			break;
		}

// An invalid parameter was passed to the returning function
	case DSERR_INVALIDPARAM:
		{
			strcpy( str, "DSERR_INVALIDPARAM" );
			return false;
			break;
		}

// This call is not valid for the current state of this object
	case DSERR_INVALIDCALL:
		{
			strcpy( str, "DSERR_INVALIDCALL" );
			return false;
			break;
		}

// An undetermined error occured inside the DirectSound subsystem
	case DSERR_GENERIC:
		{
			strcpy( str, "DSERR_GENERIC" );
			return false;
			break;
		}

// The caller does not have the priority level required for the function to
// succeed
	case DSERR_PRIOLEVELNEEDED:
		{
			strcpy( str, "DSERR_PRIOLEVELNEEDED" );
			return false;
			break;
		}

// Not enough free memory is available to complete the operation
	case DSERR_OUTOFMEMORY:
		{
			strcpy( str, "DSERR_OUTOFMEMORY" );
			return false;
		}

// The specified WAVE format is not supported
	case DSERR_BADFORMAT:
		{
			strcpy( str, "DSERR_BADFORMAT" );
			return true;
		}

// The function called is not supported at this time
	case DSERR_UNSUPPORTED:
		{
			strcpy( str, "DSERR_UNSUPPORTED" );
			return true;
		}

// No sound driver is available for use
	case DSERR_NODRIVER:
		{
			strcpy( str, "DSERR_NODRIVER" );
			return true;
		}

// This object is already initialized
	case DSERR_ALREADYINITIALIZED:
		{
			strcpy( str, "DSERR_ALREADYINITIALIZED" );
			return true;
		}

// This object does not support aggregation
	case DSERR_NOAGGREGATION:
		{
			strcpy( str, "DSERR_NOAGGREGATION" );
			return true;
		}

// The buffer memory has been lost, and must be restored
	case DSERR_BUFFERLOST:
		{
			strcpy( str, "DSERR_BUFFERLOST" );
			return true;
		}

// Another app has a higher priority level, preventing this call from
// succeeding
	case DSERR_OTHERAPPHASPRIO:
		{
			strcpy( str, "DSERR_OTHERAPPHASPRIO" );
			return true;
		}

// This object has not been initialized
	case DSERR_UNINITIALIZED:
		{
			strcpy( str, "DSERR_UNINITIALIZED" );
			return true;
		}

// The requested COM interface is not available
	case DSERR_NOINTERFACE:
		{
			strcpy( str, "DSERR_NOINTERFACE" );
			return true;
		}

// Access is denied
	case DSERR_ACCESSDENIED:
		{
			strcpy( str, "DSERR_ACCESSDENIED" );
			return true;
		}
	default:
		{
			strcpy( str, "unknown error" );
			break;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------

#endif
