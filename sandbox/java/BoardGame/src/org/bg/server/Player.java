package org.bg.server;

import java.net.Socket;
import java.util.Hashtable;
import java.io.OutputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Enumeration;
import java.io.PrintWriter;

public class Player extends Thread {

	public Player( Socket socket, Hashtable< Socket, OutputStream > clients ) {
		this.socket = socket;
		this.clients = clients;
	}
	
	public void run() {
		try {
			BufferedReader cin = new BufferedReader( new InputStreamReader( socket.getInputStream() ) );
			String line;
			
			while( ( line = cin.readLine() ) != null ) {
				synchronized( clients ) {
					for( Enumeration< OutputStream > e = clients.elements(); e.hasMoreElements(); ) {
						PrintWriter cout = new PrintWriter( e.nextElement() );
						System.out.println( line );
						cout.println( line );
					}
				}
			}
		} catch( Exception e ) {
			System.err.println( e.getMessage() );
			System.exit( -1 );
		} finally {
			synchronized( clients ) {
				clients.remove( socket );
				
				try {
					socket.close();
				} catch( Exception e ) {
					e.printStackTrace();
					System.exit( -1 );
				}
			}
		}
	}
	
	private Socket socket;
	private Hashtable< Socket, OutputStream > clients;

}
