#ifndef SOUND_MANAGER
#define SOUND_MANAGER

#include <QtGlobal>
#include "AudioController.h"
#include "Mixer.h"
#include "EndPointVolume.h"

class SoundManager
{
public:
	SoundManager();
	~SoundManager();
	bool ToggleMute();
	bool IncreaseVolume();
	bool DecreaseVolume();

private:
#if defined(Q_OS_WIN32)
	enum WinVer {WinVista7, Win2000XP, WinUnsupported};
	WinVer GetWindowsVersion();
#endif // Q_OS_WIN32
	AudioController *_controller;
};

#endif //SOUND_MANAGER
