package org.bg.server;

public class TestServer {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		GameServer self = new GameServer( 2048 );
		
		self.listen();
	}

}
