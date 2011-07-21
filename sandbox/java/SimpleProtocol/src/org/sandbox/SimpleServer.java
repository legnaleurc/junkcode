package org.sandbox;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketAddress;

public class SimpleServer {

	private class Listener extends Thread {

		public ServerSocket server;

		public Listener() {
			this.server = null;
			try {
				this.server = new ServerSocket();
			} catch( IOException e ) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

		@Override
		public void run() {
			while( this.server.isBound() ) {
				Socket socket = null;
				try {
					socket = this.server.accept();
					SimpleServer.this.newConnection_.emit( new SimpleSocket(
							socket ) );
				} catch( IOException e ) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}

	public SimpleServer() {
		this.newConnection_ = new Signal( this );
		this.listener_ = new Listener();
	}

	public Boolean listen( SocketAddress endpoint ) {
		if( this.listener_.server == null ) {
			return false;
		}
		try {
			this.listener_.server.bind( endpoint );
		} catch( IOException e ) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
		this.listener_.start();
		return true;
	}

	public void close() {
		try {
			this.listener_.server.close();
		} catch( IOException e ) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public Signal newConnection() {
		return this.newConnection_;
	}

	private Listener listener_;
	private Signal newConnection_;

}
