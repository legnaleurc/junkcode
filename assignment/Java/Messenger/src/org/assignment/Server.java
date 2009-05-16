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

/**
 * 建立 server<br/>
 * 
 * 使用 listen() 方法可以進入聆聽狀態，使用一個 thread 等待 client 連接。<br/>
 * 連接進入之後再為每一個 client 建立一個 thread 來處理輸入輸出，當任何一
 * 個 client 發出訊息，就廣播給其他所有 client。<br/>當然有用 synchronized
 * 處理競速狀態。<br/>
 * @author legnaleurc
 */
public class Server {
	
	private ServerSocket server_;
	private Hashtable<Socket, PrintWriter> clients_;
	private Listener listener_;

	/**
	 * 建構子
	 * @param port 連接埠
	 * @throws IOException 建立失敗
	 */
	public Server( int port ) throws IOException {
		this.server_ = new ServerSocket( port );
		this.clients_ = new Hashtable< Socket, PrintWriter >();
		this.listener_ = new Listener(this.server_, this.clients_);
	}
	
	/**
	 * 聆聽加入請求
	 * @note 會開啟執行緒，因此不會 block 住
	 */
	public void listen() {
		this.listener_.start();
	}
	
	/**
	 * 關閉 server
	 */
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
