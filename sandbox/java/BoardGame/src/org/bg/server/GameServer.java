package org.bg.server;

import java.net.ServerSocket;
import java.util.Hashtable;
import java.net.Socket;
import java.io.OutputStream;

public class GameServer {
	
	public GameServer( int port ) {
		try {
			server = new ServerSocket( port );
		} catch( Exception e ) {
			System.err.println( e.getMessage() );
			System.exit( -1 );
		}
		
		clients = new Hashtable< Socket, OutputStream >();
	}
	
	public void listen() {
		try {
			while( true ) {
				System.out.println( "Waiting..." );
				Socket client = server.accept();
				System.out.println( "Accepted" );
				
				clients.put( client, client.getOutputStream() );
				
				Player player = new Player( client, clients );
				player.start();
			}
		} catch( Exception e ) {
			System.err.println( e.getMessage() );
			System.exit( -1 );
		}
	}
	
	private ServerSocket server;
	private Hashtable< Socket, OutputStream > clients;

}
