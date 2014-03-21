#include <QtGlobal>
#ifdef Q_OS_WIN
#include "Mixer.h"

static const DWORD controlStep = 3277; // 64535/20, 64535 - max volume value, 20 - regulation steps.

Mixer::Mixer()
	: _nNumMixers(mixerGetNumDevs())
	, _hMixer(NULL)
{
	ZeroMemory(&_mxcaps, sizeof(MIXERCAPS));
	MIXERLINE mixerLine;
	mixerLine.cbStruct = sizeof(MIXERLINE);
	mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
    MIXERCONTROLA mixerControl;
    MIXERLINECONTROLSA mixerLineControl;
    MIXERCONTROLA mixerControlV;
    MIXERLINECONTROLSA mixerLineControlV;

	if (_nNumMixers != 0)
	{
		if (mixerOpen(&_hMixer, 0, 0, 0, MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
		{
			_errCode = GetLastError();
			return;
		}
		if (mixerGetDevCaps((UINT)_hMixer, &_mxcaps, sizeof(MIXERCAPS)) != MMSYSERR_NOERROR) 
		{
			_errCode = GetLastError();
			return;
		}
	}
	if (_hMixer == NULL)
	{
		_errCode = GetLastError();
		return;
	}

	if (mixerGetLineInfo((HMIXEROBJ)_hMixer, &mixerLine, MIXER_OBJECTF_HMIXER|
			MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR) 
	{
		_errCode = GetLastError();
		return;
	}

	mixerLineControl.cbStruct         = sizeof(MIXERLINECONTROLS);
	mixerLineControl.dwLineID         = mixerLine.dwLineID;
	mixerLineControl.dwControlType    = MIXERCONTROL_CONTROLTYPE_MUTE;
	mixerLineControl.cControls        = 1;
	mixerLineControl.cbmxctrl         = sizeof(MIXERCONTROL);
	mixerLineControl.pamxctrl         = &mixerControl;
	UINT res = mixerGetLineControlsA((HMIXEROBJ)_hMixer, &mixerLineControl, MIXER_OBJECTF_HMIXER|
			MIXER_GETLINECONTROLSF_ONEBYTYPE);
	
	if (res != MMSYSERR_NOERROR)
	{
		_errCode = GetLastError();
		return;
	}
	
	_controlIDM = mixerControl.dwControlID;

	mixerLineControlV.cbStruct         = sizeof(MIXERLINECONTROLS);
	mixerLineControlV.dwLineID         = mixerLine.dwLineID;
	mixerLineControlV.dwControlType    = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mixerLineControlV.cControls        = 1;
	mixerLineControlV.cbmxctrl         = sizeof(MIXERCONTROL);
	mixerLineControlV.pamxctrl         = &mixerControlV;
	res = mixerGetLineControlsA((HMIXEROBJ)_hMixer, &mixerLineControlV, MIXER_OBJECTF_HMIXER|
			MIXER_GETLINECONTROLSF_ONEBYTYPE);

	if(res != MMSYSERR_NOERROR)
	{
		_errCode = GetLastError();
		return;
	}

	_controlIDV = mixerControlV.dwControlID;
	_errCode = 0;
}

Mixer::~Mixer()
{
	mixerClose(_hMixer);
}

bool Mixer::ToggleMute() 
{
	MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
	MIXERCONTROLDETAILS mixerControlDetails;

	mixerControlDetails.cbStruct          = sizeof(MIXERCONTROLDETAILS);
	mixerControlDetails.dwControlID       = _controlIDM;
	mixerControlDetails.cChannels         = 1;
	mixerControlDetails.cMultipleItems    = 0;
	mixerControlDetails.cbDetails         = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	mixerControlDetails.paDetails         = &mxcdMute;

	if(mixerGetControlDetails((HMIXEROBJ)_hMixer, &mixerControlDetails, MIXER_OBJECTF_HMIXER
			|MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		_errCode = GetLastError();
		return FALSE;
	}

	bool state = false;
	if(mxcdMute.fValue)
    {
		state = FALSE;
    }
	else
    {
		state = TRUE;
    }
	MIXERCONTROLDETAILS_BOOLEAN Mute = { (LONG) state };
	MIXERCONTROLDETAILS Details;

	Details.cbStruct        = sizeof(MIXERCONTROLDETAILS);
	Details.dwControlID     = _controlIDM;
	Details.cChannels       = 1;
	Details.cMultipleItems  = 0;
	Details.cbDetails       = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	Details.paDetails       = &Mute;

	if(mixerSetControlDetails((HMIXEROBJ)_hMixer, &Details, MIXER_OBJECTF_HMIXER
			|MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR) 
	{
		_errCode = GetLastError();
		return FALSE;
	}

	_errCode = 0;
	return TRUE;
}

bool Mixer::IncreaseVolume()
{
    if (isErrored())
    {
        return false;
    }

    DWORD curVol = GetVolume();
    if (curVol == 65536)
    {
        return true;
    }
    DWORD newVol = 0;
    if ((65536-curVol) < controlStep)
    {
        newVol = 65536;
    }
    else
    {
        newVol = curVol + controlStep;
    }

    return setVol(newVol);
}

bool Mixer::DecreaseVolume()
{
    if (isErrored())
    {
        return false;
    }

    DWORD curVol = GetVolume();
    if (curVol == 0)
    {
        return true;
    }
    DWORD newVol = 0;
    if (curVol < controlStep)
    {
        newVol = 0;
    }
    else
    {
        newVol = curVol - controlStep;
    }

    return setVol(newVol);
}

DWORD Mixer::GetVolume()
{
	MIXERCONTROLDETAILS mcd = {0};
	MIXERCONTROLDETAILS_UNSIGNED mcdu = {0};
	
	mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mcd.hwndOwner = 0;
	mcd.dwControlID = _controlIDV;
	mcd.paDetails = &mcdu;
	mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mcd.cChannels = 1;
	mixerGetControlDetails((HMIXEROBJ)_hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE);

	return(mcdu.dwValue);
}

bool Mixer::setVol(DWORD newVol)
{
    MIXERCONTROLDETAILS mcd = {0};
    MIXERCONTROLDETAILS_UNSIGNED mcdu = {0};

    mcdu.dwValue = newVol;
    mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mcd.hwndOwner = 0;
    mcd.dwControlID = _controlIDV;
    mcd.paDetails = &mcdu;
    mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mcd.cChannels = 1;
    if(mixerSetControlDetails((HMIXEROBJ)_hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE)
        != MMSYSERR_NOERROR)
    {
        _errCode = GetLastError();
        return false;
    }

    _errCode = 0;
    return true;
}
#endif //Q_OS_WIN
