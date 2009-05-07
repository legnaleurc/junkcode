package org.assignment;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;

public class ClientReader extends Thread {
	
	public ClientReader(Socket self, MainWindow parent) {
		this.self_ = self;
		this.parent_ = parent;
	}
	
	public void run() {
		BufferedReader sin = null;
		try {
			sin = new BufferedReader( new InputStreamReader( this.self_.getInputStream() ) );
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		String line;
		try {
			while( ( line = sin.readLine() ) != null ) {
				synchronized( this.parent_ ) {
					this.parent_.appendMessage(line);
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private Socket self_;
	private MainWindow parent_;

}
