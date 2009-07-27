package org.bg.client;

import java.net.Socket;
import java.io.PrintWriter;
import java.io.BufferedReader;
import java.io.InputStreamReader;

public class TestClient {
	
	public static void main( String[] args ) {
		TestClient self = new TestClient( 2048 );
		
		self.perform();
	}
	
	public TestClient( int port ) {
		try {
			socket = new Socket( "localhost", port );
			cout = new PrintWriter( socket.getOutputStream(), true );
		} catch( Exception e ) {
			System.err.println( e.getMessage() );
			System.exit( -1 );
		}
	}
	
	public void perform() {
		BufferedReader cin = new BufferedReader( new InputStreamReader( System.in ) );
		String line;
		
		try {
			ClientReader cr = new ClientReader( socket.getInputStream() );
			cr.start();
		} catch( Exception e ) {
			e.printStackTrace();
			System.exit( -1 );
		}
		
		try {
			while( ( line = cin.readLine() ) != null ) {
				send( line );
			}
		} catch( Exception e ) {
			e.printStackTrace();
			System.exit( -1 );
		}
	}
	
	public void send( String message ) {
		cout.println( message );
	}
	
	private Socket socket;
	private PrintWriter cout;

}
