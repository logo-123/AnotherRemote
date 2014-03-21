#include <QtGlobal>
#ifdef Q_OS_LINUX
#ifndef ALSA_SOUND
#define ALSA_SOUND

#include <unistd.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>
#include "Audio/AudioController.h"

class Alsa : public AudioController
{
public:
    Alsa();
    ~Alsa();

    bool IncreaseVolume();
    bool DecreaseVolume();
    bool ToggleMute();
    inline bool isErrored() const;
    inline unsigned int errCode() const;

private:
    int _errCode;
    long _controlStep;
    snd_mixer_t *handle;
    snd_mixer_elem_t *elem;
    snd_mixer_selem_id_t *sid;

    long getVolume();
};

inline bool Alsa::isErrored() const
{
    return (_errCode < 0);
}

inline unsigned int Alsa::errCode() const
{
    return _errCode;
}

#endif //ALSA_SOUND
#endif //Q_OS_LINUX
