#include <QtGlobal>
#ifdef Q_OS_WIN
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include "EndPointVolume.h"

EndPointVolume::EndPointVolume()
	: _deviceEnumerator(0)
	, _defaultDevice(0)
	, _endpointVolume(0)
	, _guidMyContext(GUID_NULL)
	, _useEventContext(FALSE)
	, _errCode(S_OK)
{
	CoInitialize(NULL);
    if(CoCreateGuid(&_guidMyContext) != S_OK)
	{
		_errCode = GetLastError();
		return;
	}
		
	if(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, 
            __uuidof(IMMDeviceEnumerator), (LPVOID *)&_deviceEnumerator) != S_OK)
	{
		_errCode = GetLastError();
		return;
	}
    
    if(_deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &_defaultDevice) != S_OK)
	{
		_errCode = GetLastError();
		return;
	} 
    if(_defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
            CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&_endpointVolume) != S_OK)
	{
		_errCode = GetLastError();
		return;
	}

	_errCode = 0;
}

EndPointVolume::~EndPointVolume()
{
    _deviceEnumerator->Release();
	CoUninitialize();
}

bool EndPointVolume::IncreaseVolume()
{
    bool ret = (_endpointVolume->VolumeStepUp(&_guidMyContext) == S_OK);

	if (!ret)
	{
		_errCode = GetLastError();
	}
	else
	{
		_errCode = 0;
	}
	
	return ret;
}

bool EndPointVolume::DecreaseVolume()
{
    bool ret = (_endpointVolume->VolumeStepDown(&_guidMyContext) == S_OK);
	
	if (!ret)
	{
		_errCode = GetLastError();
	}
	else
	{
		_errCode = 0;
	}

	return ret;
}
	
bool EndPointVolume::ToggleMute()
{
    BOOL CurrentMute;
	
    if(_endpointVolume->GetMute(&CurrentMute) != S_OK)
	{
		_errCode = GetLastError();
		return FALSE;
	}

    bool ret = (_endpointVolume->SetMute(!(CurrentMute == TRUE), &_guidMyContext) == S_OK);
	
	if (!ret)
	{
		_errCode = GetLastError();
	}
	else
	{
		_errCode = 0;
	}

	return ret;
}

#endif //Q_OS_WIN
