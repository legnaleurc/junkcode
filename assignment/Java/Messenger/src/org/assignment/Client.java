package org.assignment;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class Client {
	
	private Socket connection_;
	private PrintWriter out_;
	private BufferedReader in_;

	public Client( String host, int port ) {
		try {
			this.connection_ = new Socket( host, port );
		} catch (UnknownHostException e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		} catch (IOException e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
		
		try {
			this.out_ = new PrintWriter( this.connection_.getOutputStream() );
		} catch (IOException e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
		
		try {
			this.in_ = new BufferedReader( new InputStreamReader( this.connection_.getInputStream() ) );
		} catch (IOException e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
	}
	
	public void send( String msg ) {
		this.out_.println(msg);
		this.out_.flush();
	}
	
	public String receive() {
		try {
			return this.in_.readLine();
		} catch (IOException e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
			return null;
		}
	}

}
