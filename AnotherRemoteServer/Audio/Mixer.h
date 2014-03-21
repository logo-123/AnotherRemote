#include <QtGlobal>
#ifdef Q_OS_WIN32
#ifndef MIXER_VOLUME
#define MIXER_VOLUME

#include <windows.h>
#include "Audio/AudioController.h"

class Mixer : public AudioController
{
public:
	Mixer();
	~Mixer();

	bool IncreaseVolume();
	bool DecreaseVolume();
	bool ToggleMute();
	inline bool isErrored() const;
	inline UINT errCode() const;

private:
	DWORD GetVolume();
	UINT _nNumMixers;
	HMIXER _hMixer;
	MIXERCAPS _mxcaps;
	DWORD _controlIDM;
	DWORD _controlIDV;
	UINT _errCode;
    bool setVol(DWORD newVol);
};

inline bool Mixer::isErrored() const
{
	return !(_errCode == S_OK);
}

inline UINT Mixer::errCode() const
{
	return 	_errCode;
}

#endif //MIXER_VOLUME
#endif //Q_OS_WIN32
