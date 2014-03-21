#ifndef END_POINT_VOLUME
#define END_POINT_VOLUME

#include <QtGlobal>
#if defined(Q_OS_WIN32)

#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include "AudioController.h"

class EndPointVolume : public AudioController
{
public:
	EndPointVolume();
	~EndPointVolume();

	bool IncreaseVolume();
	bool DecreaseVolume();
	bool ToggleMute();
	inline bool isErrored() const;
	inline UINT errCode() const;

private:
	EndPointVolume(const EndPointVolume &) {}
	EndPointVolume &operator = (const EndPointVolume &) {}
	IMMDeviceEnumerator *_deviceEnumerator;
	IMMDevice *_defaultDevice;
	IAudioEndpointVolume *_endpointVolume;
	GUID _guidMyContext;
	bool _useEventContext;
	UINT _errCode;
};

inline bool EndPointVolume::isErrored() const
{
	return !(_errCode == S_OK);
}

inline UINT EndPointVolume::errCode() const
{
	return 	_errCode;
}

#endif //Q_OS_WIN32
#endif //END_POINT_VOLUME
