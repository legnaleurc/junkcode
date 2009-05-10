package org.assignment;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketException;

public class Client {
	
	private Socket connection_;
	private PrintWriter out_;
	private BufferedReader in_;

	public Client( String host, int port ) throws IOException {
		this.connection_ = new Socket( host, port );
		
		this.out_ = new PrintWriter( this.connection_.getOutputStream() );
		
		this.in_ = new BufferedReader( new InputStreamReader( this.connection_.getInputStream() ) );
	}
	
	public void send( String msg ) {
		this.out_.println(msg);
		this.out_.flush();
	}
	
	public String receive() throws SocketException {
		try {
			return this.in_.readLine();
		} catch (SocketException e) {
			throw e;
		} catch (IOException e) {
			System.err.println(e.getMessage());
			e.printStackTrace();
			return null;
		}
	}

}
