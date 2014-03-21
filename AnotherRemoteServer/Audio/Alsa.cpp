#include <QtGlobal>
#ifdef Q_OS_LINUX
#include "Audio/Alsa.h"
#include <QDebug>

Alsa::Alsa()
{
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Master");

    _errCode = snd_mixer_open(&handle, 0);
    if (_errCode < 0)
    {
        qDebug() << "snd_mixer_open - error";
        return;
    }

    _errCode = snd_mixer_attach(handle, "default");
    if (_errCode < 0)
    {
        qDebug() << "snd_mixer_attach - error";
        snd_mixer_close(handle);
        return;
    }

    _errCode = snd_mixer_selem_register(handle, NULL, NULL);
    if (_errCode < 0)
    {
        qDebug() << "snd_mixer_selem_register - error";
        snd_mixer_close(handle);
        return;
    }

    _errCode = snd_mixer_load(handle);
    if (_errCode < 0)
    {
        qDebug() << "snd_mixer_load - error";
        snd_mixer_close(handle);
        return;
    }

    elem = snd_mixer_find_selem(handle, sid);
    if (!elem)
    {
        qDebug() << "snd_mixer_find_selem - error";
        snd_mixer_close(handle);
        _errCode = -1;
        return;
    }

    _controlStep = 3276; // 65536/20 = 3276, 20 - steps
}

Alsa::~Alsa()
{
    snd_mixer_close(handle);
}

bool Alsa::IncreaseVolume()
{
    if (isErrored())
    {
        return false;
    }

    long curVol = getVolume();
    if (curVol == 65536)
    {
        return true;
    }

    long newVol = 0;
    if ((65536-curVol) < _controlStep)
    {
        newVol = 65536;
    }
    else
    {
        newVol = curVol+_controlStep;
    }

    if (snd_mixer_selem_set_playback_volume_all(elem, newVol) < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Alsa::DecreaseVolume()
{
    if (isErrored())
    {
        return false;
    }

    long curVol = getVolume();
    if (curVol == 0)
    {
        return true;
    }
    qDebug() << "Alsa::DecreaseVolume current value: "+QString::number(curVol);
    long newVol = 0;
    if (curVol < _controlStep)
    {
        newVol = 0;
    }
    else
    {
        newVol = curVol-_controlStep;
    }
    qDebug() << "Alsa::IncreaseVolume new value: "+QString::number(newVol);
    if (snd_mixer_selem_set_playback_volume_all(elem, newVol) < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Alsa::ToggleMute()
{
    int curState = 0;
    if (snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_MONO, &curState) < 0)
    {
        return false;
    }

    qDebug() << "Alsa::ToggleMute current value: "+QString::number(curState);
    if (curState)
    {
        curState = 0;
    }
    else
    {
        curState = 1;
    }

    qDebug() << "Alsa::ToggleMute new value: "+QString::number(curState);

    if (snd_mixer_selem_set_playback_switch_all(elem, curState) < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

long Alsa::getVolume()
{
    long outvol;

    snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &outvol);

    return outvol;
}

#endif //Q_OS_LINUX
