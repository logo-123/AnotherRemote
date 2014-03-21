package com.AnotherRemote;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Vibrator;
import android.text.Html;
import android.util.Log;
import android.view.GestureDetector;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.Toast;
import android.widget.LinearLayout.LayoutParams;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;

public class PadLayout extends Activity implements android.view.GestureDetector.OnGestureListener, 
	GestureDetector.OnDoubleTapListener
{
	private final static int ACT = 1655985;
	private final static int VIBRATION_TIME = 50;
	private final static int HELLO_ID = 121540;
	private int OldBrightnessLevel = 0;
	private static MediaPlayer mp;
	private TextView plugin, PluginMessage;
	private boolean snd=true, vibrate=true, display=true, 
			KeepWiFiOn=true, wifiOff=true, taskbar=true;
	private static Vibrator vibrator;
	private Button dismiss, one, two, three, four, 
		five, six, seven, eight;
	private PopupWindow Popup;
	private View popUpView;
	private NotificationManager mNotificationManager;
	private GestureDetector mmGestureDetector;
	private int _serverPort;
	private String _serverAddr;
	private UDPListener _netHandler = null;
	private boolean _netReady = false;
	private float MouseSensitivity = 0;
	
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		// Force portrait orientation
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		setContentView(R.layout.pad_layout);
		// Volume buttons control notification volume
		setVolumeControlStream(AudioManager.STREAM_MUSIC);
		// Initializing vibrator
		vibrator = (Vibrator)getSystemService(Context.VIBRATOR_SERVICE);
		// Initializing media player
		mp = MediaPlayer.create(getApplicationContext(), R.raw.button);
		// Initializing popup window for plug-in messages
		popUpView = getLayoutInflater().inflate(R.layout.popup_window, null);
    	Popup = new PopupWindow(popUpView, LayoutParams.WRAP_CONTENT, 
    			LayoutParams.WRAP_CONTENT, true);
		PluginMessage = (TextView)popUpView.findViewById(R.id.textView1);
		dismiss = (Button)popUpView.findViewById(R.id.dissmiss_button);
		dismiss.setOnClickListener(new Button.OnClickListener()
        {
        	public void onClick(View v)
        	{
        		SetBrightness(OldBrightnessLevel);
        		Popup.dismiss();
        	}
        });
		
		FrameLayout MousePad = (FrameLayout)findViewById(R.id.MousePad);
		MousePad.setOnTouchListener(new View.OnTouchListener()
		{
			public boolean onTouch(View v, MotionEvent event)
			{
				boolean res = mmGestureDetector.onTouchEvent(event);
				return res;
			}
		});

		// Get actions buttons control 
        one = (Button)findViewById(R.id.B1);
        two = (Button)findViewById(R.id.B2);
        three = (Button)findViewById(R.id.B3);
        four = (Button)findViewById(R.id.B4);
        five = (Button)findViewById(R.id.B5);
        six = (Button)findViewById(R.id.B6);
        seven = (Button)findViewById(R.id.B7);
        eight = (Button)findViewById(R.id.B8);
        plugin = (TextView)findViewById(R.id.PLUGIN);

        // Left and right buttons acts on up and down events
        ImageButton left = (ImageButton)findViewById(R.id.MouseLeft);
        left.setOnTouchListener(new OnTouchListener()
        {
        	public boolean onTouch(View v, MotionEvent event)
        	{
        		if (event.getAction() == MotionEvent.ACTION_DOWN)
        		{
        			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("LBUTTONDOWN"))));
        		}
        		if (event.getAction() == MotionEvent.ACTION_UP)
        		{
        			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("LBUTTONUP"))));
        		}
        		return false;
        	}
        });
        ImageButton right = (ImageButton)findViewById(R.id.MouseRight);
        right.setOnTouchListener(new OnTouchListener()
        {
        	public boolean onTouch(View v, MotionEvent event)
        	{
        		if (event.getAction() == MotionEvent.ACTION_DOWN)
        		{
        			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("RBUTTONDOWN"))));
        		}
        		if (event.getAction() == MotionEvent.ACTION_UP)
        		{
        			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("RBUTTONUP"))));
        		}
        		return false;
        	}
        });
        
        SharedPreferences ConfigStorage = getSharedPreferences("AnotherRemote", 0);
		_serverAddr = ConfigStorage.getString("address", "10.1.4.4");
		_serverPort = Integer.parseInt(ConfigStorage.getString("port", "4655"));
        _netHandler = new UDPListener(_serverPort, _handler, _serverAddr);
        _netHandler.start();
        
		mmGestureDetector = new GestureDetector(this, this);
		mmGestureDetector.setIsLongpressEnabled(false);
		mmGestureDetector.setOnDoubleTapListener(this);
		mNotificationManager = (NotificationManager)getApplicationContext().getSystemService(Context.NOTIFICATION_SERVICE);
		setNotificationIcon();
	}
	
	@SuppressLint("HandlerLeak")
	private Handler _handler = new Handler()
	{
		public void handleMessage(Message msg)
		{
			switch (msg.what)
			{
			case UDPListener.MESSAGE_LABELS:
				setButtonsLabels((String[])msg.obj);
				break;
			case UDPListener.MESSAGE_POPUP:
				createPopupWindow((String)msg.obj);
				break;
			case UDPListener.MESSAGE_SEND_FAILED:
				Toast.makeText(getApplicationContext(), "Execution failed", Toast.LENGTH_LONG).show();
				break;
			case UDPListener.MESSAGE_READY:
				_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("BUTTONS_LABEL"))));
				ReloadConfig(0xFF);
				_netReady = true;
				break;
			}
		}
	};

	PadLayout getMe()
	{
		return this;
	}
	
	void setNotificationIcon()
	{
		Context context = getApplicationContext();
		final Notification notification = new Notification(R.drawable.app, "AnotherRemote",
				System.currentTimeMillis());
		notification.flags |= Notification.FLAG_NO_CLEAR|Notification.FLAG_ONGOING_EVENT;
		
		Intent notificationIntent = new Intent(this, PadLayout.class);
		notificationIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		//notificationIntent.setAction("android.intent.action.MAIN");
		//notificationIntent.addCategory("android.intent.category.LAUNCHER");
		PendingIntent contentIntent = PendingIntent.getActivity(this, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);
		notification.setLatestEventInfo(context, "AnotherRemote", "", contentIntent);
		mNotificationManager.notify(HELLO_ID, notification);
	}
	
    private void createPopupWindow(String text)
    {
    	SetBrightness(100);
		PluginMessage.setText(Html.fromHtml(text));
		Popup.showAtLocation(findViewById(R.id.pad_layout), Gravity.CENTER, 0, 0);
    }
	
	// On +(volume up), -(volume down) and mute buttons press 
	public void WindowsCommand(View v)
	{
		SendNotification(vibrate, snd);

		if (false == _netReady)
		{
			return;
		}
		
		switch(v.getId())
		{
		case R.id.VU:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("VOLUME_UP"))));
			break;
		case R.id.M:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("MUTE"))));
			break;
		case R.id.VD:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("VOLUME_DOWN"))));
			break;
		}
	}
	
	// Create AlertDialog on Power off button
	public void CreateAlert(View v)
	{
		final CharSequence[] items = {"Shutdown", "Hibernate", "Sleep"};

		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle("Select type");
		builder.setItems(items, new DialogInterface.OnClickListener()
		{
			public void onClick(DialogInterface dialog, int item)
			{
				PowerOff(String.valueOf(items[item]));
			}
		});
		AlertDialog alert = builder.create();
		alert.show();
	}
	
	// Setting required brightness level 
	private void SetBrightness(int progress)
	{
		int brightness=0;
		
		if (progress < 4)
		{
			brightness = 4;
		}
		else
		{
			brightness = progress;
		}
		WindowManager.LayoutParams lp = getWindow().getAttributes();
		lp.screenBrightness = (float)brightness/100;
		getWindow().setAttributes(lp);
	}
	
	// Load or reload (if menu was called) configuration
	private void ReloadConfig(int ShouldLoad)
	{
		SharedPreferences ConfigStorage = getSharedPreferences("AnotherRemote", 0);
		
		if ((ShouldLoad&(1<<0))!=0)
		{
			snd = ConfigStorage.getBoolean("snd", true);
		}
		if ((ShouldLoad&(1<<1))!=0)
		{
			vibrate = ConfigStorage.getBoolean("vibration", true);
		}
		if ((ShouldLoad&(1<<2))!=0)
		{
			display = ConfigStorage.getBoolean("display", true);
			// If Keep screen on selected -> disable screen off
			if (display)
			{
				getWindow().addFlags(android.view.WindowManager.
						LayoutParams.FLAG_KEEP_SCREEN_ON);
			}
			else
			{
				getWindow().clearFlags(android.view.WindowManager.
						LayoutParams.FLAG_KEEP_SCREEN_ON);
			}
		}
		if ((ShouldLoad&(1<<3))!=0)
		{
			KeepWiFiOn = ConfigStorage.getBoolean("KeepWiFiOn", true);
			if (KeepWiFiOn == true)
			{
				_netHandler.getHandler().sendMessage(Message.obtain(null, UDPListener.MESSAGE_START_KEEPALIVE));
			}
			else
			{
				_netHandler.getHandler().sendMessage(Message.obtain(null, UDPListener.MESSAGE_STOP_KEEPALIVE));
			}
		}
		if ((ShouldLoad&(1<<4))!=0)
		{
			OldBrightnessLevel = ConfigStorage.getInt("brightness", 100);
			SetBrightness(OldBrightnessLevel);
		}
		if ((ShouldLoad&(1<<5))!=0)
		{
			MouseSensitivity = (ConfigStorage.getInt("MouseSensitivity", 10)/10);
			if (MouseSensitivity > 2)
			{
				MouseSensitivity = 2;
			}
			if (MouseSensitivity < 0.1)
			{
				MouseSensitivity = (float)0.1;
			}
		}
		if ((ShouldLoad&(1<<6))!=0)
		{
			wifiOff = ConfigStorage.getBoolean("TurnWiFiOff", true);
		}
		if ((ShouldLoad&(1<<7)) != 0)
		{
			taskbar = ConfigStorage.getBoolean("TakbarIcon", true);
		}
	}
	
	// On switch plug-in pressed
	public void onSP(View v)
	{
		SendNotification(vibrate, snd);
		// Send switch plug-in command
		_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("SWITCH_PLUGIN"))));
	}
	
	// Play stop previous forward next back buttons pressed
	public void onMedia(View v)
	{
		SendNotification(vibrate, snd);
		
		if (false == _netReady)
		{
			return;
		}
		
		switch(v.getId())
		{
		case R.id.NEXT:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("NEXT"))));
			break;
		case R.id.PLAY:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("PLAY"))));
			break;
		case R.id.PREV:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("PREVIOUS"))));
			break;
		case R.id.BACK:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("BACKWARD"))));
			break;
		case R.id.FORW:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("FORWARD"))));
			break;
		case R.id.STOP:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("STOP"))));
			break;
		}
	}
	
	// On action button pressed
	public void onAct(View v)
	{
		SendNotification(vibrate, snd);
		
		if (false == _netReady)
		{
			return;
		}
		
		switch(v.getId())
		{
		case R.id.B1:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("ACTION_1"))));
			break;
		case R.id.B2:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("ACTION_2"))));
			break;
		case R.id.B3:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("ACTION_3"))));
			break;
		case R.id.B4:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("ACTION_4"))));
			break;
		case R.id.B5:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("ACTION_5"))));
			break;
		case R.id.B6:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("ACTION_6"))));
			break;
		case R.id.B7:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("ACTION_7"))));
			break;
		case R.id.B8:
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("ACTION_8"))));
			break;
		}
	}
	
	// Send selected power off method
	private void PowerOff(String type)
	{
		SendNotification(vibrate, snd);
		
		if (false == _netReady)
		{
			return;
		}
		
		if (type.equals("Shutdown"))
		{
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("POWER_OFF"))));
		}
		else if (type.equals("Hibernate"))
		{
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("HIBERNATE"))));
		}
		else if (type.equals("Sleep"))
		{
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("SLEEP"))));
		}
		end(false);
	}
	
    // Open menu
    @Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		MenuInflater inflater = getMenuInflater();
	    inflater.inflate(R.menu.menu, menu);
	    return true;
	}
	
    // Menu item selected
    @Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
    	SendNotification(vibrate, snd);
    	switch(item.getItemId())
	  	{
	    case R.id.Quit:
	    	end(true);
	    	break;
	    case R.id.Settings:
	    	startActivityForResult(new Intent(this, SettingsActivity.class), ACT);
	    	break;
	    case R.id.GetPIString:
			if (false == _netReady)
			{
				return true;
			}
	    	_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("PLUGIN_STRING"))));
	    	break;
	    }
	    return true;
	}
	
	public void end(boolean isBackPressed)
	{
		((NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE)).cancel(HELLO_ID);
    	mp.release();
    	_netHandler.destroy();
    	if (false == isBackPressed)
    	{
	    	if (true == wifiOff)
	    	{
	    		WifiManager wifiManager = (WifiManager)getSystemService(Context.WIFI_SERVICE); 
	    	    wifiManager.setWifiEnabled(false);
	    	}
    	}
		finish();
		System.exit(0);
	}
	
	@Override  
	public void onBackPressed()
	{
	    super.onBackPressed();
	    end(true);
	}
	
	// Trackball controls mouse so we need to capture events  
	@Override
	public boolean onTrackballEvent(MotionEvent event)
	{
		// If action move
		if (event.getAction() == MotionEvent.ACTION_MOVE)
		{
			float ix = event.getX(), iy = event.getY();
			
			if (false == _netReady)
			{
				return true;
			}
			
			// Moving along x axis
			if (ix != 0)
			{
				// If positive move to right, negative to left
				if (ix > 0)
				{
					_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("MSTEPRIGHT"))));
				}
				else
				{
					_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("MSTEPLEFT"))));
				}
			}
			else // Moving along y axis
			{
				if (iy > 0)
				{
					_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("MSTEPUP"))));
				}
				else
				{
					_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("MSTEPDOWN"))));
				}
			}
		}
		// Trackball press is left mouse button
		if (event.getAction() == MotionEvent.ACTION_UP)
		{
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("LBUTTONCLICK"))));
		}
		return true;
	}

	// After settings activity closed reload config
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		if (requestCode == ACT)
		{
			ReloadConfig(resultCode);
		}
	}

	private void setButtonsLabels(String [] array)
	{
		plugin.setText(array[1]);
		one.setText(array[2]);
		two.setText(array[3]);
		three.setText(array[4]);
		four.setText(array[5]);
		five.setText(array[6]);
		six.setText(array[7]);
		seven.setText(array[8]);
		eight.setText(array[9]);
	}
	
	// Plays sound and vibrates
	private static void SendNotification(boolean vibrate, boolean sound)
	{
		// All notifications running in thread
		if (vibrate)
		{
			new Thread(new Runnable()
		    {
		      	public void run()
		       	{
	  		  		vibrator.vibrate(VIBRATION_TIME);
	  		   	}
		    }).start();
		}
		if (sound)
		{
			new Thread(new Runnable()
			{
				public void run()
				{
			       	mp.seekTo(0);
	       			mp.start();
				}
			}).start();
		}
	}

	//@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		return false;
	}

	float lastX = 0;
	float lastY = 0;
	
	//@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		lastX = e2.getRawX();
		lastX = e2.getRawX();
		
		if ((Math.abs(lastX-distanceX) < 20) || (Math.abs(lastY-distanceY) < 20))
		{
			if (false == _netReady)
			{
				return true;
			}
			
			int valueX = (int)(MouseSensitivity*distanceX); 
			int valueY = (int)(MouseSensitivity*distanceY);
			
			valueX = valueX*(-1);
			valueY = valueY*(-1);
			
			String data = "MMOVE: "+String.valueOf(valueX)+" "+String.valueOf(valueY);
			
			//Log.d("Scroll", data);
			
			_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String(data))));
		}
		
		return true;
	}

	//@Override
	public boolean onDoubleTap(MotionEvent arg0)
	{
		return true;
	}
	
	//@Override
	public boolean onSingleTapConfirmed(MotionEvent arg0)
	{
		return false;
	}
	
	//@Override
	public void onShowPress(MotionEvent arg0)
	{
	}

	//@Override
	public boolean onSingleTapUp(MotionEvent arg0)
	{
		Log.d("Motion", "Single tap");
		_netHandler.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("LBUTTONCLICK"))));
		return false;
	}
	
	//@Override
	public boolean onDown(MotionEvent arg0)
	{
		return true;
	}

	//@Override
	public boolean onFling(MotionEvent arg0, MotionEvent arg1, float distanceX,
			float distanceY)
	{
     	return false;
	}

	//@Override
	public void onLongPress(MotionEvent arg0)
	{
	}
}
