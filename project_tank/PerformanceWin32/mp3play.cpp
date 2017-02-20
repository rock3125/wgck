#include <windows.h>
#include <mmsystem.h>
#include <amstream.h>

#define DATA_SIZE 5000

/********************************************************************

   Trivial wave player stuff

 ********************************************************************/

class CWaveBuffer;

class CWaveBuffer {
    public:
        CWaveBuffer();
        ~CWaveBuffer();
        BOOL Init(HWAVEOUT hWave, int Size);
        void Done();
        BOOL Write(PBYTE pData, int nBytes, int& BytesWritten);
        void Flush();

    private:
        WAVEHDR      m_Hdr;
        HWAVEOUT     m_hWave;
        int          m_nBytes;
};

class CWaveOut {
    public:
        CWaveOut(LPCWAVEFORMATEX Format, int nBuffers, int BufferSize);
        ~CWaveOut();
        void Write(PBYTE Data, int nBytes);
        void Flush();
        void Wait();
        void Reset();
    private:
        const HANDLE       m_hSem;
        const int          m_nBuffers;
        int          m_CurrentBuffer;
        BOOL         m_NoBuffer;
        CWaveBuffer *m_Hdrs;
        HWAVEOUT     m_hWave;
};

/*
    CWaveBuffer
*/

CWaveBuffer::CWaveBuffer()
{
}

BOOL CWaveBuffer::Init(HWAVEOUT hWave, int Size)
{
    m_hWave  = hWave;
    m_nBytes = 0;

    /*  Allocate a buffer and initialize the header */
    m_Hdr.lpData = (LPSTR)LocalAlloc(LMEM_FIXED, Size);
    if (m_Hdr.lpData == NULL) {
        return FALSE;
    }
    m_Hdr.dwBufferLength  = Size;
    m_Hdr.dwBytesRecorded = 0;
    m_Hdr.dwUser = 0;
    m_Hdr.dwFlags = 0;
    m_Hdr.dwLoops = 0;
    m_Hdr.lpNext = 0;
    m_Hdr.reserved = 0;

    /*  Prepare it */
    waveOutPrepareHeader(hWave, &m_Hdr, sizeof(WAVEHDR));

    return TRUE;
}

CWaveBuffer::~CWaveBuffer() {
    if (m_Hdr.lpData) {
        waveOutUnprepareHeader(m_hWave, &m_Hdr, sizeof(WAVEHDR));
        LocalFree(m_Hdr.lpData);
    }
}

void CWaveBuffer::Flush()
{
    //ASSERT(m_nBytes != 0);
    m_nBytes = 0;
    waveOutWrite(m_hWave, &m_Hdr, sizeof(WAVEHDR));
}

BOOL CWaveBuffer::Write(PBYTE pData, int nBytes, int& BytesWritten)
{
    //ASSERT((DWORD)m_nBytes != m_Hdr.dwBufferLength);
    BytesWritten = min((int)m_Hdr.dwBufferLength - m_nBytes, nBytes);
    CopyMemory((PVOID)(m_Hdr.lpData + m_nBytes), (PVOID)pData, BytesWritten);
    m_nBytes += BytesWritten;
    if (m_nBytes == (int)m_Hdr.dwBufferLength) {
        /*  Write it! */
        m_nBytes = 0;
        waveOutWrite(m_hWave, &m_Hdr, sizeof(WAVEHDR));
        return TRUE;
    }
    return FALSE;
}

void CALLBACK WaveCallback(HWAVEOUT hWave, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
    if (uMsg == WOM_DONE) {
        ReleaseSemaphore((HANDLE)dwUser, 1, NULL);
    }
}

/*
    CWaveOut
*/

CWaveOut::CWaveOut(LPCWAVEFORMATEX Format, int nBuffers, int BufferSize) :
    m_nBuffers(nBuffers),
    m_CurrentBuffer(0),
    m_NoBuffer(TRUE),
    m_hSem(CreateSemaphore(NULL, nBuffers, nBuffers, NULL)),
    m_Hdrs(new CWaveBuffer[nBuffers]),
    m_hWave(NULL)
{
    /*  Create wave device */
    waveOutOpen(&m_hWave,
                WAVE_MAPPER,
                Format,
                (DWORD)WaveCallback,
                (DWORD)m_hSem,
                CALLBACK_FUNCTION);

    /*  Initialize the wave buffers */
    for (int i = 0; i < nBuffers; i++) {
        m_Hdrs[i].Init(m_hWave, BufferSize);
    }
}

CWaveOut::~CWaveOut()
{
    /*  First get our buffers back */
    waveOutReset(m_hWave);

    /*  Free the buffers */
    delete [] m_Hdrs;

    /*  Close the wave device */
    waveOutClose(m_hWave);

    /*  Free our semaphore */
    CloseHandle(m_hSem);
}

void CWaveOut::Flush()
{
    if (!m_NoBuffer) {
        m_Hdrs[m_CurrentBuffer].Flush();
        m_NoBuffer = TRUE;
        m_CurrentBuffer = (m_CurrentBuffer + 1) % m_nBuffers;
    }
}

void CWaveOut::Reset()
{
    waveOutReset(m_hWave);
}


void CWaveOut::Write(PBYTE pData, int nBytes)
{
    while (nBytes != 0) {
        /*  Get a buffer if necessary */
        if (m_NoBuffer) {
            WaitForSingleObject(m_hSem, INFINITE);
            m_NoBuffer = FALSE;
        }

        /*  Write into a buffer */
        int nWritten;
        if (m_Hdrs[m_CurrentBuffer].Write(pData, nBytes, nWritten)) {
            m_NoBuffer = TRUE;
            m_CurrentBuffer = (m_CurrentBuffer + 1) % m_nBuffers;
            nBytes -= nWritten;
            pData += nWritten;
        } else {
            //ASSERT(nWritten == nBytes);
            break;
        }
    }
}

void CWaveOut::Wait()
{
    /*  Send any remaining buffers */
    Flush();

    /*  Wait for our buffers back */
    for (int i = 0; i < m_nBuffers; i++) {
        WaitForSingleObject(m_hSem, INFINITE);
    }
    LONG lPrevCount;
    ReleaseSemaphore(m_hSem, m_nBuffers, &lPrevCount);
}

/**************************************************************************

  End of wave player stuff

 **************************************************************************/

CWaveOut* WaveOut;
IMediaStream *pStream;
IAudioStreamSample *pSample;
IAudioMediaStream *pAudioStream;
IAudioData *pAudioData;
PBYTE pBuffer;

HRESULT Start_RenderStreamToDevice( IMultiMediaStream *pMMStream )
{
    WAVEFORMATEX wfx;
    pBuffer = (PBYTE)LocalAlloc(LMEM_FIXED, DATA_SIZE);

    pMMStream->GetMediaStream(MSPID_PrimaryAudio, &pStream);
    pStream->QueryInterface(IID_IAudioMediaStream, (void **)&pAudioStream);
    pAudioStream->GetFormat(&wfx);
    CoCreateInstance(CLSID_AMAudioData, NULL, CLSCTX_INPROC_SERVER,
                                    IID_IAudioData, (void **)&pAudioData);
    pAudioData->SetBuffer(DATA_SIZE, pBuffer, 0);
    pAudioData->SetFormat(&wfx);
    pAudioStream->CreateSample(pAudioData, 0, &pSample);
    WaveOut = new CWaveOut(&wfx, 4, 65536);

    return S_OK;
}


void Do_RenderStreamToDevice( IMultiMediaStream *pMMStream )
{
	HRESULT hr = pSample->Update(0, NULL, NULL, 0);
    if (FAILED(hr) || MS_S_ENDOFSTREAM == hr)
		return;

	DWORD dwLength;
	pAudioData->GetInfo(NULL, NULL, &dwLength);
    WaveOut->Write(pBuffer, dwLength);
//    pMMStream->Seek(0);
}


void Finish_RenderStreamToDevice( void )
{
    pAudioData->Release();
    pSample->Release();
    pStream->Release();
    pAudioStream->Release();
    LocalFree((HLOCAL)pBuffer);
}


HRESULT RenderFileToMMStream(WCHAR * pszFileName, IMultiMediaStream **ppMMStream)
{
    IAMMultiMediaStream *pAMStream;
    CoCreateInstance(CLSID_AMMultiMediaStream, NULL, CLSCTX_INPROC_SERVER,
 		     IID_IAMMultiMediaStream, (void **)&pAMStream);
    pAMStream->Initialize(STREAMTYPE_READ, AMMSF_NOGRAPHTHREAD, NULL);
    pAMStream->AddMediaStream(NULL, &MSPID_PrimaryAudio, 0, NULL);
    pAMStream->OpenFile(pszFileName, AMMSF_RUN);
    *ppMMStream = pAMStream;
    return S_OK;
}


void PlayWaveFile( const char* fname, IMultiMediaStream*& pMMStream )
{
    CoInitialize(NULL);
    WCHAR wszName[1000];
    MultiByteToWideChar(CP_ACP, 0, fname, -1, wszName,
                        sizeof(wszName) / sizeof(wszName[0]));
    RenderFileToMMStream(wszName, &pMMStream);
    Start_RenderStreamToDevice(pMMStream);
}


void StopWaveFile( IMultiMediaStream* pMMStream )
{
    pMMStream->Release();
    CoUninitialize();
};

