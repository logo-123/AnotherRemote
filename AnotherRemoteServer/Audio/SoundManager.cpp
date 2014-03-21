#include <QtGlobal>
#include <Audio/SoundManager.h>

#if defined(Q_OS_WIN32)
#define _WIN32_WINNT 0x0600
#include <windows.h>
#elif defined(Q_OS_LINUX)
#include "Audio/Alsa.h"
#else
#   error "Unsupported OS detected. Windows or Linux valid only"
#endif

#if defined(Q_OS_WIN32)
// Findout windows version (requerd to use correct volume control interface)
SoundManager::WinVer SoundManager::GetWindowsVersion()
{
	OSVERSIONINFO osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

    SoundManager::WinVer ver = WinUnsupported;

	switch (osvi.dwMajorVersion)
	{
	case 5:
        return SoundManager::Win2000XP;
		break;
	case 6:
        return SoundManager::WinVista7;
	default:
		break;
	}

    return ver;
}
#endif

SoundManager::SoundManager()
	: _controller(0)
{
#if defined(Q_OS_WIN32)
	switch(GetWindowsVersion())
	{
    case SoundManager::Win2000XP:
		// Init mixer for windows 2000/XP
		_controller = new Mixer();
		break;
    case SoundManager::WinVista7:
		// Init EndPointVolume for windows Vista/7
		_controller = new EndPointVolume();
		break;
    default:
        _controller = 0;
        break;
	}
#elif defined(Q_OS_LINUX)
    _controller = new Alsa();
#endif
}

SoundManager::~SoundManager()
{
	if (_controller)
	{
		delete _controller;
		_controller = 0;
	}
}

// Toggle mute state
bool SoundManager::ToggleMute()
{
	if (_controller)
	{
		return _controller->ToggleMute();
	}
	else
	{
		return false;
	}
}

// Volume step up
bool SoundManager::IncreaseVolume()
{
	if (_controller)
	{
		return _controller->IncreaseVolume();
	}
	else
	{
		return false;
	}
}

// Volume step down
bool SoundManager::DecreaseVolume()
{
	if (_controller)
	{
		return _controller->DecreaseVolume();
	}
	else
	{
		return false;
	}
}
