package org.sandbox;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;
import java.util.Arrays;

public class SimpleSocket {

	private class Listener extends Thread {
		public Socket socket;

		public Listener( Socket socket ) {
			this.socket = socket;
		}

		@Override
		public void run() {
			InputStream is = null;
			try {
				is = this.socket.getInputStream();
			} catch( IOException e ) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return;
			}
			byte[] buffer = null;
			try {
				buffer = new byte[this.socket.getReceiveBufferSize()];
			} catch( SocketException e ) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return;
			}
			while( this.socket.isBound() ) {
				Integer size = -1;
				try {
					size = is.read( buffer );
					SimpleSocket.this.readyRead_.emit( Arrays.copyOf( buffer, size ) );
				} catch( IOException e ) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					break;
				}
			}
		}
	}
	
	public SimpleSocket() {
		this.listener_ = new Listener( new Socket() );
		this.readyRead_ = new Signal( this );
	}

	public SimpleSocket( Socket socket ) {
		this.listener_ = new Listener( socket );
		this.readyRead_ = new Signal( this );
	}

	public Boolean connectToServer( SocketAddress bindpoint ) {
		if( this.listener_.socket == null ) {
			return false;
		}
		try {
			this.listener_.socket.bind( bindpoint );
		} catch( IOException e ) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
		return true;
	}

	public void disconnectFromServer() {
		if( this.listener_.socket == null ) {
			return;
		}
		try {
			this.listener_.socket.close();
		} catch( IOException e ) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void write( byte[] message ) {
		OutputStream os = null;
		try {
			os = this.listener_.socket.getOutputStream();
			os.write( message );
		} catch( IOException e ) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public Signal readyRead() {
		return this.readyRead_;
	}

	private Listener listener_;
	private Signal readyRead_;

}
