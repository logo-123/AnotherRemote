package com.AnotherRemote;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.util.Timer;
import java.util.TimerTask;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.net.ConnectivityManager;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Vibrator;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

public class AnotherRemote extends Activity
{
	private static final int VIBRATION_TIME = 50;
	private EditText ipField, portField, m1, m2, m3, m4, m5, m6, BrIP;
	private boolean snd, vibrate;
	private WifiManager wifi;
    private Vibrator vibrator;
	private String ip, port, mac, brip;
	private SharedPreferences.Editor editor;
	private SharedPreferences settings;
	private MediaPlayer mp;
	private DatagramPacket packet;
	private UDPListener ul = null;
	private Intent _padLayout = null;
	private boolean _netReady = false, _timerRunning=false;
	Timer _keepAliveTimer = null;
	
	@SuppressLint("HandlerLeak")
	Handler _handler = new Handler()
	{
		public void handleMessage(Message msg)
		{
			Log.d("handleMessage", "Received "+msg.what);
			switch (msg.what)
			{
			case UDPListener.MESSAGE_SERVER_ALIVE:
				_keepAliveTimer.cancel();
				startActivity(_padLayout);
		    	finish();
				break;
			case UDPListener.MESSAGE_READY:
				_netReady = true;
				break;
			case UDPListener.MESSAGE_SERVER_DEAD:
			case UDPListener.MESSAGE_SEND_FAILED:
				Toast.makeText(getApplicationContext(), "Server not responding", 
	   					Toast.LENGTH_LONG).show();
				break;
			}
		}
	};
	
	@Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        setVolumeControlStream(AudioManager.STREAM_MUSIC);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        ipField = (EditText)findViewById(R.id.SERV);
    	portField = (EditText)findViewById(R.id.PORT);
        m1 = (EditText)findViewById(R.id.M1);
        m2 = (EditText)findViewById(R.id.M2);
        m3 = (EditText)findViewById(R.id.M3);
        m4 = (EditText)findViewById(R.id.M4);
        m5 = (EditText)findViewById(R.id.M5);
        m6 = (EditText)findViewById(R.id.M6);
        BrIP = (EditText)findViewById(R.id.BIP);
     	
        settings = getSharedPreferences("AnotherRemote", 0);
        editor = settings.edit();
        
        vibrate = settings.getBoolean("vibration", true);
        if (vibrate)
        {
        	vibrator = (Vibrator)getSystemService(Context.VIBRATOR_SERVICE);
        }
        
        snd = settings.getBoolean("snd", true);
        if (snd)
        {
        	mp = MediaPlayer.create(getApplicationContext(), R.raw.button);
        }
        
        ip = settings.getString("address", "10.1.4.4");
        port = settings.getString("port", "4655");
        mac = settings.getString("mac", "1C:6F:65:D8:4E:24");
        brip = settings.getString("brip", "10.1.4.31");
       	ipField.setText(ip);
        portField.setText(port);
       	BrIP.setText(brip);
        String[] macarr = mac.split(":");
        m1.setText(macarr[0]);
        m2.setText(macarr[1]);
        m3.setText(macarr[2]);
        m4.setText(macarr[3]);
       	m5.setText(macarr[4]);
       	m6.setText(macarr[5]);
       	
       	_padLayout = new Intent(this, PadLayout.class);
       	ul = new UDPListener(Integer.parseInt(port), _handler, ip);
       	ul.start();
    }

	private Runnable Timer_Tick = new Runnable()
	{
	    public void run()
	    {
			Toast.makeText(getApplicationContext(), "Server not responding", Toast.LENGTH_LONG).show();
			_keepAliveTimer.cancel();
			_timerRunning = false;
	    }
	};
	
	private void TimerMethod()
	{
	    this.runOnUiThread(Timer_Tick);
	}
	
    public void onConnectButton(View v)
    {
    	SendNotification();
    	if (ipField.getText().toString().equals(""))
    	{
    		Toast.makeText(getApplicationContext(), "IP field empty!", Toast.LENGTH_LONG).show();
    	}
    	else
    	{
    		if (ipField.getText().toString().matches("^[0-9]{1,4}\\.[0-9]{1,4}\\.[0-9]{1,4}\\.[0-9]{1,4}$"))
    		{
    			if (portField.getText().toString().equals(""))
    			{
    				Toast.makeText(getApplicationContext(), "Port field empty!", 
    						Toast.LENGTH_LONG).show();
    			}
    			else
    			{
    				int iPort = Integer.parseInt(portField.getText().toString());
    				if ((iPort>65535)||(iPort<=0))
    				{
    					Toast.makeText(getApplicationContext(), "Illegal port value!", 
    							Toast.LENGTH_LONG).show();
    				} 
    				else
    				{
    					wifi = (WifiManager)getSystemService(Context.WIFI_SERVICE);
    					if (wifi.getWifiState() == WifiManager.WIFI_STATE_ENABLED)
    			        {
    			           	if (!ipField.getText().toString().matches(ip))
    			           	{
    			           		ip = ipField.getText().toString();
       			        		editor.putString("address", ip);
       			        		editor.commit();
    			           	}
       			        	if (!portField.getText().toString().matches(port))
       			        	{
       			        		port = portField.getText().toString();
       			        		editor.putString("port", port);
       			        		editor.commit();
       			        	}
       			        	ConnectivityManager connManager = (ConnectivityManager) getSystemService(CONNECTIVITY_SERVICE);
       			        	if (false == connManager.getNetworkInfo(ConnectivityManager.TYPE_WIFI).isConnected())
       			        	{
       			        		Toast.makeText(getApplicationContext(), "Not connected to any of WiFi networks", 
       	    							Toast.LENGTH_LONG).show();
       			        		return;
       			        	}
       			        	if (true == _netReady)
       			        	{
       			        		if (true == _timerRunning)
       			        		{
       			        			return;
       			        		}
       			        		ul.getHandler().sendMessage(Message.obtain(null, 0, (Object)(new String("CHECK_SERVER"))));
       			        		_keepAliveTimer = new Timer();
       			        		_timerRunning = true;
       			        		_keepAliveTimer.schedule(new TimerTask()
       			        		{
	       			     			@Override
	       			     			public void run()
	       			     			{
	       			     				TimerMethod();
	       			     			}
	       			     		}, 3000, 100000);
       			        	}
       			        	else
       			        	{
       			        		Toast.makeText(getApplicationContext(), "Unable to open socket or run thread", 
       	    							Toast.LENGTH_LONG).show();
       			        	}
      			        }
    			        else
    			        {
    			        	WiFiAlert();
    			        }
     				}
    			}
    		}
    		else
    		{
    			Toast.makeText(getApplicationContext(), "Invalid IP address", 
    					Toast.LENGTH_LONG).show();
    		}
    	}
    }
    
    public void onWoLButton(View v)
    {
    	SendNotification();
    	
    	String macinp = new String();
    	macinp = m1.getText()+":"+m2.getText()+":"+m3.getText()+":"+m4.getText()+":"
			+m5.getText()+":"+m6.getText();
    	        
    	if (BrIP.getText().toString().
    			matches("^[0-9]{1,4}\\.[0-9]{1,4}\\.[0-9]{1,4}\\.[0-9]{1,4}$"))
		{
    		try
    		{
    			wifi = (WifiManager)getSystemService(Context.WIFI_SERVICE);
				if (wifi.getWifiState() == WifiManager.WIFI_STATE_ENABLED)
		        {
					byte[] macBytes = getMacBytes(macinp);
					byte[] bytes = new byte[6 + 16 * macBytes.length];
					for(int i = 0; i < 6; i++)
						bytes[i] = (byte) 0xff;
					for(int i = 6; i < bytes.length; i += macBytes.length)
						System.arraycopy(macBytes, 0, bytes, i, macBytes.length);
					InetAddress address = InetAddress.getByName(BrIP.getText().toString());
					packet = new DatagramPacket(bytes, bytes.length, address, 4655);
					SendMagicPacket();
					if (!macinp.matches(mac))
					{
						mac = macinp;
						editor.putString("mac", mac);
						editor.commit();
					}
					if (!BrIP.getText().toString().matches(brip))
					{
						brip = BrIP.getText().toString();
						editor.putString("brip", brip);
						editor.commit();
					}
		        }
				else
				{
					WiFiAlert();
				}
    		}
    		catch (Exception e)
    		{
    			Log.e("AnotherRemote::onWoLButton ", "Exception "+e.toString());
    		}
		} 
    	else
    	{
    		Toast.makeText(getApplicationContext(), "Invalid broadcast address",
    				Toast.LENGTH_LONG).show();
    	}
    }
    
    void SendMagicPacket()
    {
		Thread thr = new Thread()
		{
		   public void run()
		   {
			   DatagramSocket socket = null;
			   try
			   {
				   socket = new DatagramSocket();
				   socket.send(packet);
			   }
			   catch (SocketException e)
			   {
				   e.printStackTrace();
			   }
			   catch (IOException e)
			   {
				   Log.e("AnotherRemote::SendMagicPacket ", "IOException "+e.toString());
			   }
			   socket.close();
		   }
		};
		thr.start();
		try
		{
			thr.join();
		}
		catch (InterruptedException e)
		{
			Log.e("AnotherRemote::SendMagicPacket ", "InterruptedException "+e.toString());
		}
    }

	private void WiFiAlert()
    {
    	AlertDialog alertDialog = new AlertDialog.Builder(this).create();
		alertDialog.setTitle("AnotherRemote");
		alertDialog.setMessage("WI-FI is off. Would you like to activate it?");
		alertDialog.setButton("OK", new DialogInterface.OnClickListener()
		{
			public void onClick(DialogInterface dialog, int which)
			{
				WifiManager wifiManager = (WifiManager)getSystemService(Context.WIFI_SERVICE); 
	    	    wifiManager.setWifiEnabled(true);
			}
		});
		alertDialog.setButton2("Exit", new DialogInterface.OnClickListener()
		{
			public void onClick(DialogInterface dialog, int which)
			{
				try
				{
					exit();
				}
				catch (Throwable e)
				{
					Log.e("AnotherRemote::WiFiAlert ", "Throwable "+e.toString());
				}
			}
		});
		alertDialog.setButton3("Cancel", new DialogInterface.OnClickListener()
		{
			public void onClick(DialogInterface dialog, int which)
			{
				return;
			}
		});
		alertDialog.show();
    }

    private static byte[] getMacBytes(String macStr) throws IllegalArgumentException
    {
    	byte[] bytes = new byte[6];
    	String[] hex = macStr.split("(\\:|\\-)");
    	if (hex.length != 6)
    	{
            throw new IllegalArgumentException("Invalid MAC address.");
    	}
        try
        {
            for(int i = 0; i < 6; i++)
                bytes[i] = (byte) Integer.parseInt(hex[i], 16);
        }
        catch (NumberFormatException e)
        {
        	throw new IllegalArgumentException("Invalid hex digit in MAC address.");
        }
        return bytes;
    }
    
	@Override
	public void onDestroy()
	{
		if (snd)
		{
			mp.release();
		}
		ul.destroy();
		super.onDestroy();
	}
    
    protected void exit()
    {
    	finish();
    }
    
	// Plays sound and vibrates
	void SendNotification()
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
		if (snd)
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
}