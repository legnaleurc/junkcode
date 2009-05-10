package org.assignment;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.Hashtable;
import java.util.Map.Entry;

public class Server {
	
	private ServerSocket server_;
	private Hashtable<Socket, PrintWriter> clients_;
	private Listener listener_;

	public Server( int port ) {
		try {
			this.server_ = new ServerSocket( port );
		} catch (IOException e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
		this.clients_ = new Hashtable< Socket, PrintWriter >();
		this.listener_ = new Listener(this.server_, this.clients_);
	}
	
	public void listen() {
		this.listener_.start();
	}
	
	public void close() {
		try {
			this.server_.close();
		} catch (IOException e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
	}
	
	private class Listener extends Thread {
		
		private ServerSocket server_;
		private Hashtable<Socket, PrintWriter> clients_;

		public Listener( ServerSocket server, Hashtable<Socket, PrintWriter> clients ) {
			this.server_ = server;
			this.clients_ = clients;
		}
		
		public void run() {
			Socket client = null;
			while( true ) {
				try {
					client = this.server_.accept();
				} catch (IOException e) {
					System.err.println(e.getMessage());
					e.printStackTrace();
				}
				
				synchronized (this.clients_) {
					PrintWriter out = null;
					try {
						out = new PrintWriter( client.getOutputStream() );
					} catch (IOException e) {
						System.err.println(e.getMessage());
						e.printStackTrace();
					}
					this.clients_.put(client, out);
				}
				
				ClientReader cr = new ClientReader( client, this.clients_ );
				cr.start();
			}
		}
		
	}
	
	private class ClientReader extends Thread {
		
		private Socket client_;
		private Hashtable<Socket, PrintWriter> clients_;

		public ClientReader( Socket client, Hashtable<Socket, PrintWriter> clients ) {
			this.client_ = client;
			this.clients_ = clients;
		}
		
		public void run() {
			BufferedReader sin = null;
			try {
				sin = new BufferedReader( new InputStreamReader( this.client_.getInputStream() ) );
			} catch (IOException e) {
				System.err.println(e.getMessage());
				e.printStackTrace();
			}
			
			String line = null;
			try {
				while( ( line = sin.readLine() ) != null ) {
					synchronized(this.clients_) {
						for( Entry<Socket, PrintWriter> e : this.clients_.entrySet() ) {
							e.getValue().println(line);
							e.getValue().flush();
						}
					}
				}
			} catch (SocketException e) {
				synchronized(this.clients_) {
					this.clients_.remove(this.client_);
				}
			} catch (IOException e) {
				System.err.println(e.getMessage());
				e.printStackTrace();
			}
		}
		
	}

}
