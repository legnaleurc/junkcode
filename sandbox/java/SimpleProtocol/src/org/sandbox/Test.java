package org.sandbox;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;

public class Test {

	/**
	 * @param args
	 */
	public static void main( String[] args ) {
		Test test = new Test();
		/*
		BufferedReader reader = new BufferedReader( new InputStreamReader( System.in ) );
		while( true ) {
			try {
				String line = reader.readLine();
				test.write( line );
			} catch( IOException e ) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		*/
		test.write( "CAFEBABE" );
	}
	
	public Test() {
		SimpleServer server = new SimpleServer();
		if( !server.listen( new InetSocketAddress( ( InetAddress )null, 6000 ) ) ) {
			System.err.println( "Server Down" );
		}
		
		this.socket_ = new SimpleSocket();
		try {
			this.socket_.connectToServer( new InetSocketAddress( InetAddress.getLocalHost(), 6000 ) );
		} catch( UnknownHostException e ) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		this.clients_ = new ArrayList< SimpleSocket >();
		
		server.newConnection().connect( new Signal.Slot() {
			@Override
			public void call( Object sender, Object... args ) {
				SimpleSocket client = ( SimpleSocket )args[0];
				client.readyRead().connect( new Signal.Slot() {
					@Override
					public void call( Object sender, Object... args ) {
						Test.this.onClientReadyRead( ( byte[] )args[0] );
					}
				} );
				Test.this.clients_.add( client );
			}
		} );
	}
	
	public void write( String message ) {
		try {
			this.socket_.write( message.getBytes( "UTF8" ) );
		} catch( UnsupportedEncodingException e ) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private void onClientReadyRead( byte[] message ) {
		try {
			System.out.println( new String( message, "UTF8" ) );
		} catch( UnsupportedEncodingException e ) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private ArrayList< SimpleSocket > clients_;
	private SimpleSocket socket_;

}
