package com.AnotherRemote;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Timer;
import java.util.TimerTask;

import android.annotation.SuppressLint;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

final public class UDPListener extends Thread
{
	private DatagramSocket _sock = null;
	private int _port = 0;
	private Handler _parentHandler = null; 
	private String _server;
	private Timer _keepAliveTimer = null;
	private Handler _handler = null;
	private Thread _listenThread = null;
	private volatile boolean _run = true;
	
	public static final int MESSAGE_SEND = 0;
	public static final int MESSAGE_SERVER_ALIVE = 1;
	public static final int MESSAGE_SERVER_DEAD = 2;
	public static final int MESSAGE_START_KEEPALIVE = 3;
	public static final int MESSAGE_STOP_KEEPALIVE = 4;
	public static final int MESSAGE_LABELS = 5;
	public static final int MESSAGE_POPUP = 6;
	public static final int MESSAGE_SEND_FAILED = 7;
	public static final int MESSAGE_READY = 8;
	public static final int MESSAGE_NEW_DATA = 9;
	
	public Handler getHandler()
	{
		return _handler;
	}
	
	UDPListener(int p, Handler h, String s)
	{
		_port = p;
		_parentHandler = h;
		_server = s;
		
		_keepAliveTimer = new Timer();
		
		try
		{
			_sock = new DatagramSocket(0);
		}
		catch (SocketException e)
		{
			Log.e("UDPListener::run() SocketException - ", e.getMessage());
		}
	}
	
	public void setServer(String address, int port)
	{
		_server = address;
		_port = port;
	}
	
	private void sendData(String d)
	{
		byte[] buf = d.getBytes();
		DatagramPacket _packet = null;
		try 
		{
			_packet = new DatagramPacket(buf, buf.length, InetAddress.getByName(_server), _port);
			Log.d("UDPListener::sendData", "Sending data "+d+" to "+_server+":"+_port);
			_sock.send(_packet);
		} 
		catch (UnknownHostException e1)
		{
			Log.e("UDPSender::run() UnknownHostException - ", e1.getMessage());
		}
		catch (IOException e)
		{
			Log.e("UDPSender::run() IOException - ", e.getMessage());
		}
		_packet = null;
	}
	
	private void handleServerData(String data)
	{
        Log.d("UDP", "Received: '" + data + "'");
        
        String splited[] = data.split(";");
        if ((10 == splited.length) && (true == splited[0].equals("newLabels")))
        {
     		_parentHandler.sendMessage(Message.obtain(null, MESSAGE_LABELS, (Object)splited));
        }
        else if (true == data.equals("SERVER_OK"))
        {
        	_parentHandler.sendMessage(Message.obtain(null, MESSAGE_SERVER_ALIVE));
        }
        else
        {
        	if (false == data.equals("PONG"))
        	{
	    		_parentHandler.sendMessage(Message.obtain(null, MESSAGE_POPUP, (Object)data));
        	}
        }
	}
	
	@SuppressLint("HandlerLeak")
	public void run()
	{
		Looper.prepare();
		_handler = new Handler()
		{
			public void handleMessage(Message msg)
			{
				Log.d("Handler", "Message received "+msg.what);
				
				switch (msg.what)
				{
				case MESSAGE_SEND:
					sendData((String)msg.obj);
					break;
				case MESSAGE_START_KEEPALIVE:
					_keepAliveTimer.schedule(new TimerTask()
					{
						@Override
						public void run()
						{
							sendData("PING");
						}
					}, 10000, 10000);
					break;
				case MESSAGE_STOP_KEEPALIVE:
					_keepAliveTimer.cancel();
					break;
				case MESSAGE_NEW_DATA:
					handleServerData((String)msg.obj);
					break;
				default:
					super.handleMessage(msg);
					break;
				}
			}
		};
		
		_listenThread = new Thread()
		{
			@Override
			public void run()
			{
				while (true == _run)
				{
					byte[] buf = new byte[1500];
			        DatagramPacket packet = new DatagramPacket(buf, buf.length);
			        
			        try
			        {
						_sock.receive(packet);
					}
			        catch (IOException e)
			        {
			        	Log.e("UDPListener::run() IOException - ", e.getMessage());
					}
			        
					String data = new String(packet.getData()).trim();
					
					if (false == data.equals(""))
					{
						getHandler().sendMessage(Message.obtain(null, MESSAGE_NEW_DATA, (Object)data));
					}
				}
			}
		};
		_listenThread.start();
		
		_parentHandler.sendMessage(Message.obtain(null, MESSAGE_READY));
		
        Looper.loop();
	}
	
	@Override
	public void destroy()
	{
		_run = false;
        _sock.close();
	}
}
