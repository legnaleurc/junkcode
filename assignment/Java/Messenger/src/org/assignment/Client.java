package org.assignment;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

/**
 * Client 連結
 * @author legnaleurc
 */
public class Client {
	
	private Socket connection_;
	private PrintWriter out_;
	private BufferedReader in_;

	/**
	 * 建構子
	 * @param host 主機名稱
	 * @param port 連接埠
	 * @throws IOException 連接失敗
	 */
	public Client( String host, int port ) throws IOException {
		this.connection_ = new Socket( host, port );
		
		this.out_ = new PrintWriter( this.connection_.getOutputStream() );
		
		this.in_ = new BufferedReader( new InputStreamReader( this.connection_.getInputStream() ) );
	}
	
	/**
	 * 送訊息給 server
	 * @param msg 訊息
	 */
	public void send( String msg ) {
		this.out_.println(msg);
		this.out_.flush();
	}
	
	/**
	 * 接收 server 訊息
	 * @return 單行訊息
	 * @throws IOException server 斷線
	 * @note 此為同步I/O，因此會 block 住，直到讀到資料
	 */
	public String receive() throws IOException {
		return this.in_.readLine();
	}

}
