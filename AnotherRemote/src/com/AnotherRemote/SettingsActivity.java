package com.AnotherRemote;

import android.app.Activity;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.widget.CheckBox;
import android.widget.SeekBar;
import android.util.Log;
import android.view.KeyEvent;

public class SettingsActivity extends Activity
{
	private CheckBox se, vibr, dyspon, wfon, wifistate, tbarIcon;
	private boolean OldSound, OldVibration, OldDisplay, OldWiFi, OldWiFiState, OldTBarIcon;
	private SeekBar brightness, mousesens;
	private int iBrig = 0, iSens = 0, OldSens = 0, OldBrightness = 0;
	private SharedPreferences settings;
	
	@Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.settings);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        se = (CheckBox)findViewById(R.id.SE);
        vibr = (CheckBox)findViewById(R.id.VIBR);
        dyspon = (CheckBox)findViewById(R.id.DO);
        wfon = (CheckBox)findViewById(R.id.KeepWiFiOn);
        brightness = (SeekBar)findViewById(R.id.BL);
        mousesens = (SeekBar)findViewById(R.id.MouseSesativity);
        wifistate = (CheckBox)findViewById(R.id.WiFiState);
        tbarIcon = (CheckBox)findViewById(R.id.STATUSBARICON);
        Log.d("d", "1");
        
        brightness.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
		{
			public void onStopTrackingTouch(SeekBar seekBar) {}
			public void onStartTrackingTouch(SeekBar seekBar) {}
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
			{
				if(progress < 5)
				{
					iBrig = 5;
				}
				else
				{
					iBrig = progress;
				}
			}
		});
        
        mousesens.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
		{
			public void onStopTrackingTouch(SeekBar seekBar) {}
			public void onStartTrackingTouch(SeekBar seekBar) {}
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
			{
				iSens = progress;
			}
		});
        Log.d("d", "2");
        settings = getSharedPreferences("AnotherRemote", 0);
        
        OldSound = settings.getBoolean("snd", true);
        OldVibration = settings.getBoolean("vibration", true);
        OldDisplay = settings.getBoolean("display", true);
        OldWiFi = settings.getBoolean("KeepWiFiOn", true);
        OldBrightness = settings.getInt("brightness", 100);
        OldSens = settings.getInt("MouseSensitivity", 10);
        OldWiFiState = settings.getBoolean("TurnWiFiOff", true);
        OldTBarIcon = settings.getBoolean("TaskBarIcon", true);
        
        Log.d("d", "3");
        
        se.setChecked(OldSound);
        vibr.setChecked(OldVibration);
        dyspon.setChecked(OldDisplay);
        wfon.setChecked(OldWiFi);
        brightness.setProgress(OldBrightness);
        mousesens.setProgress(OldSens);
        wifistate.setChecked(OldWiFiState);
        tbarIcon.setChecked(OldTBarIcon);
        Log.d("d", "4");
    }
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		if(keyCode == KeyEvent.KEYCODE_BACK)
		{
			SharedPreferences.Editor editor = settings.edit();
			int ExitCode = 0;
			
			if(OldSound != se.isChecked())
			{
				ExitCode |= (1<<0);
				editor.putBoolean("snd", se.isChecked());
			}
			if(OldVibration != vibr.isChecked())
			{
				ExitCode |= (1<<1);
				editor.putBoolean("vibration", vibr.isChecked());
			}
			if(OldDisplay != dyspon.isChecked())
			{
				ExitCode |= (1<<2);
				editor.putBoolean("display", dyspon.isChecked());
			}
			if(OldWiFi != wfon.isChecked())
			{
				ExitCode |= (1<<3);
				editor.putBoolean("KeepWiFiOn", wfon.isChecked());
			}
			if(OldBrightness != iBrig)
			{
				ExitCode |= (1<<4);
				editor.putInt("brightness", iBrig);
			}
			if(OldSens != iSens)
			{
				ExitCode |= (1<<5);
				editor.putInt("MouseSensitivity", iSens);
			}
			if (wifistate.isChecked() != OldWiFiState)
			{
				ExitCode |= (1<<6);
				editor.putBoolean("TurnWiFiOff", wifistate.isChecked());
			}
			if (tbarIcon.isChecked() != OldTBarIcon)
			{
				ExitCode |= (1<<7);
				editor.putBoolean("TakbarIcon", tbarIcon.isChecked());
			}
			if(ExitCode != 0)
			{
				editor.commit();
			}
			setResult(ExitCode);
			finish();
			return true;
		}
		return false;
	}
}
