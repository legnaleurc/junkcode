package org.bg.client;

import java.io.InputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;

public class ClientReader extends Thread {
	
	public ClientReader( InputStream input ) {
		this.input = input;
	}
	
	public void run() {
		BufferedReader in = new BufferedReader( new InputStreamReader( input ) );
		String line;
		
		try {
			while( ( line = in.readLine() ) != null ) {
				System.out.println( line );
			} 
		} catch( Exception e ) {
			e.printStackTrace();
			System.exit( -1 );
		}
	}
	
	private InputStream input;

}
