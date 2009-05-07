package org.assignment;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Vector;

public class Listener extends Thread {
	
	private ServerSocket server_;
	private Vector<Socket> clients_;
	private boolean stop_;
	private MainWindow parent_;

	public Listener( int port, MainWindow parent ) {
		this.parent_ = parent;
		try {
			this.server_ = new ServerSocket( port );
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		this.clients_ = new Vector<Socket>();
		this.stop_ = false;
	}
	
	public Vector<Socket> getClients() {
		return this.clients_;
	}
	
	public void run() {
		Socket client = null;
		do {
			try {
				client = this.server_.accept();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			synchronized( this.clients_ ) {
				this.clients_.add(client);
			}
			
			ClientReader person = new ClientReader(client, this.parent_);
			person.start();
		} while( !this.stop_ );
	}

}
